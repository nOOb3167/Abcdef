#include <stdio.h>
#include <string.h>
#include <math.h>
#include <error.h>
#include <assimp.h>
#include <aiPostProcess.h>
#include <aiScene.h>
#include <src/cogl_include.h>

#include <src/mai-node.h>
#include <src/mai-node_funcs.h>
#include <src/mai-anim.h>
#include <src/mai-anim_funcs.h>
#include <src/mai-model.h>
#include <src/mai-model_funcs.h>
#include <src/mai-anim-instance.h>
#include <src/mai-anim-instance_funcs.h>
#include <Nxt.h>
#include <ai_example.h>

#include <src/array_macro.h>

void
_stuff (struct aiScene *scene);


struct aiScene *
ai_scene_from_file (const char *file_name)
{
  const struct aiScene *scene;
  struct aiScene *nonconst_scene;
  /**
   * Warning: PreTransformVertices, flatten scene essentially.
   * See LimitBoneWeights for shader skinning.
   */
  scene = aiImportFile (file_name,
      aiProcess_Triangulate | aiProcess_SortByPType |
      aiProcess_JoinIdenticalVertices);
  g_xassert (scene);
  nonconst_scene = (struct aiScene *)scene;

  return nonconst_scene;
}

void
ai_import_file (const char *file_name)
{
  struct aiScene *scene;
  scene = ai_scene_from_file (file_name);

  _stuff (scene);
}

void
_stuff (struct aiScene *scene)
{
  g_xassert (scene->mRootNode);
  g_xassert (!strcmp (scene->mRootNode->mName.data, "Scene"));

  HaHa haha = {1234, 5678};
  GHaHaArray *ghha;
  ghha = g_ha_ha_array_new ();
  g_ha_ha_array_append_val (ghha, &haha);
  g_ha_ha_array_index (ghha, 0);
  g_ha_ha_array_free (ghha, 1);
  GHaHaPtrArray *gppa;
  gppa = g_ha_ha_ptr_array_new ();
  g_ha_ha_ptr_array_add (gppa, &haha);
  g_ha_ha_ptr_array_index (gppa, 0);
  g_ha_ha_ptr_array_free (gppa, 1);

  MaiModel *mm;
  mm = mai_model_new_from (scene);

  MaiAnimInstance *mai;
  MaiAnim *tmp_an;
  tmp_an = g_mai_anim_ptr_array_index (mm->anims, 0);
  mai = mai_model_get_anim_by_name (mm, tmp_an->name);

  mai_anim_instance_draw (mai);
  g_mai = mai;

  MaiNode *mn;
  mn = mm->nodes;

  //mai_node_draw_recursive ((MaiNode*)(g_ptr_array_index(mn->children, 0)));
}

void
ai_matrix_to_cogl_matrix (struct aiMatrix4x4 *ai_matrix, CoglMatrix *cogl_matrix)
{
  g_xassert (ai_matrix);
  g_xassert (cogl_matrix);

  /* coglRC aiRC */
  float tmp_vals[16] = {
      ai_matrix->a1, ai_matrix->b1, ai_matrix->c1, ai_matrix->d1,
      ai_matrix->a2, ai_matrix->b2, ai_matrix->c2, ai_matrix->d2,
      ai_matrix->a3, ai_matrix->b3, ai_matrix->c3, ai_matrix->d3,
      ai_matrix->a4, ai_matrix->b4, ai_matrix->c4, ai_matrix->d4,
  };

  cogl_matrix_init_from_array (cogl_matrix, tmp_vals);
}

void
nx_cogl_quaternion_to_rotation_axis_and_angle (CoglQuaternion *quat, float *angle_out, struct xvtx *axis_out)
{
  /* http://www.j3d.org/matrix_faq/matrfaq_latest.html#Q57 */
  float xcos_a;
  float xangle;
  float xsin_a;

  g_xassert (angle_out);
  g_xassert (axis_out);

  xcos_a = quat->w;
  xangle = acosf (xcos_a) * 2;
  xsin_a = sqrtf ( 1.0f - xcos_a * xcos_a);
  if (fabsf (xsin_a) < 0.0005f)
    xsin_a = 1.0f;
  *angle_out = xangle * NX_RADIANS_TO_DEGREES;
  axis_out->x = quat->x / xsin_a;
  axis_out->y = quat->y / xsin_a;
  axis_out->z = quat->z / xsin_a;
}

gboolean
_nx_invert_matrix_general (CoglMatrix *matrix, float matrix_inv[16])
{
  const float *m = (float *)matrix;
  float *out = matrix_inv;
  float wtmp[4][8];
  float m0, m1, m2, m3, s;
  float *r0, *r1, *r2, *r3;

  r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

  r0[0] = NX_MAT (m, 0, 0), r0[1] = NX_MAT (m, 0, 1),
    r0[2] = NX_MAT (m, 0, 2), r0[3] = NX_MAT (m, 0, 3),
    r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,

    r1[0] = NX_MAT (m, 1, 0), r1[1] = NX_MAT (m, 1, 1),
    r1[2] = NX_MAT (m, 1, 2), r1[3] = NX_MAT (m, 1, 3),
    r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,

    r2[0] = NX_MAT (m, 2, 0), r2[1] = NX_MAT (m, 2, 1),
    r2[2] = NX_MAT (m, 2, 2), r2[3] = NX_MAT (m, 2, 3),
    r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,

    r3[0] = NX_MAT (m, 3, 0), r3[1] = NX_MAT (m, 3, 1),
    r3[2] = NX_MAT (m, 3, 2), r3[3] = NX_MAT (m, 3, 3),
    r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;

  /* choose pivot - or die */
  if (fabsf (r3[0]) > fabsf (r2[0]))
    NX_SWAP_ROWS (r3, r2);
  if (fabsf (r2[0]) > fabsf (r1[0]))
    NX_SWAP_ROWS (r2, r1);
  if (fabsf (r1[0]) > fabsf (r0[0]))
    NX_SWAP_ROWS (r1, r0);
  if (0.0 == r0[0])
    return FALSE;

  /* eliminate first variable     */
  m1 = r1[0]/r0[0]; m2 = r2[0]/r0[0]; m3 = r3[0]/r0[0];
  s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
  s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
  s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
  s = r0[4];
  if (s != 0.0) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
  s = r0[5];
  if (s != 0.0) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
  s = r0[6];
  if (s != 0.0) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
  s = r0[7];
  if (s != 0.0) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

  /* choose pivot - or die */
  if (fabsf (r3[1]) > fabsf (r2[1]))
    NX_SWAP_ROWS (r3, r2);
  if (fabsf (r2[1]) > fabsf (r1[1]))
    NX_SWAP_ROWS (r2, r1);
  if (0.0 == r1[1])
    return FALSE;

  /* eliminate second variable */
  m2 = r2[1] / r1[1]; m3 = r3[1] / r1[1];
  r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
  r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
  s = r1[4]; if (0.0 != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
  s = r1[5]; if (0.0 != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
  s = r1[6]; if (0.0 != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
  s = r1[7]; if (0.0 != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

  /* choose pivot - or die */
  if (fabsf (r3[2]) > fabsf (r2[2]))
    NX_SWAP_ROWS (r3, r2);
  if (0.0 == r2[2])
    return FALSE;

  /* eliminate third variable */
  m3 = r3[2] / r2[2];
  r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
    r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6],
    r3[7] -= m3 * r2[7];

  /* last check */
  if (0.0 == r3[3])
    return FALSE;

  s = 1.0f / r3[3];             /* now back substitute row 3 */
  r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

  m2 = r2[3];                 /* now back substitute row 2 */
  s  = 1.0f / r2[2];
  r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
    r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
  m1 = r1[3];
  r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
    r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
  m0 = r0[3];
  r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
    r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

  m1 = r1[2];                 /* now back substitute row 1 */
  s  = 1.0f / r1[1];
  r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
    r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
  m0 = r0[2];
  r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
    r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

  m0 = r0[1];                 /* now back substitute row 0 */
  s  = 1.0f / r0[0];
  r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
    r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

  NX_MAT (out, 0, 0) = r0[4]; NX_MAT (out, 0, 1) = r0[5],
    NX_MAT (out, 0, 2) = r0[6]; NX_MAT (out, 0, 3) = r0[7],
    NX_MAT (out, 1, 0) = r1[4]; NX_MAT (out, 1, 1) = r1[5],
    NX_MAT (out, 1, 2) = r1[6]; NX_MAT (out, 1, 3) = r1[7],
    NX_MAT (out, 2, 0) = r2[4]; NX_MAT (out, 2, 1) = r2[5],
    NX_MAT (out, 2, 2) = r2[6]; NX_MAT (out, 2, 3) = r2[7],
    NX_MAT (out, 3, 0) = r3[4]; NX_MAT (out, 3, 1) = r3[5],
    NX_MAT (out, 3, 2) = r3[6]; NX_MAT (out, 3, 3) = r3[7];

  return TRUE;
}

gboolean
nx_cogl_matrix_get_inverse (CoglMatrix *matrix, CoglMatrix *inverse)
{
  gboolean ret;
  float inv[16];
  ret = _nx_invert_matrix_general (matrix, inv);
  cogl_matrix_init_from_array (inverse, inv);
  return ret;
}

CoglPrimitive *
nx_cogl_primitive_new (GArray *verts, GArray *indices, GArray *uvs)
{
  context_switch_cogl ();

  CoglAttributeBuffer *bfr;
  g_xassert (verts->len > 0);
  bfr = cogl_attribute_buffer_new (sizeof (struct xvtx) * verts->len, verts->data);
  g_xassert (bfr);
  CoglAttribute *attr;
  attr = cogl_attribute_new (bfr, "cogl_position_in",
                             sizeof (struct xvtx), offsetof (struct xvtx, x),
                             3, COGL_ATTRIBUTE_TYPE_FLOAT);
  g_xassert (attr);

  CoglAttributeBuffer *bfr_uv;
  g_xassert (uvs->len > 0);
  bfr_uv = cogl_attribute_buffer_new (sizeof (struct xvtx) * uvs->len, uvs->data);
  g_xassert (bfr_uv);

  CoglAttribute *attr_uv;
  attr_uv = cogl_attribute_new (bfr_uv, "cogl_tex_coord0_in",
                             sizeof (struct xvtx), offsetof (struct xvtx, x),
                             3, COGL_ATTRIBUTE_TYPE_FLOAT);
  g_xassert (attr_uv);

  CoglIndices *idx;
  g_xassert (indices->len > 0);
  idx = cogl_indices_new (COGL_INDICES_TYPE_UNSIGNED_INT, indices->data, indices->len);
  g_xassert (idx);

  CoglPrimitive *prim;
  prim = cogl_primitive_new (COGL_VERTICES_MODE_TRIANGLES, verts->len,                             attr, attr_uv, NULL);
//  CoglAttribute *attrs_arry[2];
//  attrs_arry[0] = attr;
//  attrs_arry[1] = attr_uv;
//  prim = cogl_primitive_new_with_attributes (COGL_VERTICES_MODE_TRIANGLES, verts->len, attrs_arry, 2);
  g_xassert (prim);

  cogl_primitive_set_indices (prim, idx);

  cogl_object_unref (idx);
  cogl_object_unref (bfr);
  cogl_object_unref (attr);

  return prim;
}

void
nx_cogl_primitive_draw (CoglPrimitive* prim)
{
  cogl_primitive_draw (prim);
}
