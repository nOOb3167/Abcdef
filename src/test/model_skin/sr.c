#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_windows.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_primitives.h>

#include <glib.h>
#include <src/error.h>
#include <src/mai-model.h>
#include <src/mai-model_funcs.h>
#include <src/mai-node-anim.h>
#include <src/mai-node-anim_funcs.h>
#include <nx_mat.h>
#include <sr.h>

struct NxState *g_state;

void
sr_project_one (NxMat *mst, NxVec4 *vec_inout)
{
  NxVec4 temp;
  temp = *vec_inout;
  nx_mat_transform (mst, &temp);
  temp.vals[0] /= temp.vals[3];
  temp.vals[1] /= temp.vals[3];
  temp.vals[2] /= temp.vals[3];
  temp.vals[3] = 1.0f;
  *vec_inout = temp;
}

void
sr_draw_unit_vec_at (NxMat *mst, NxVec4 *pos, NxVec4 *dir)
{
  ALLEGRO_COLOR clr;
  clr = al_map_rgb (0, 255, 0);
  ALLEGRO_COLOR clr2;
  clr2 = al_map_rgb (0, 255, 0);


  float sf;
  sf = 20.0f;
  float ofx, ofy;
  ofx = 50.0f; ofy = 50.0f;

  NxVec4 start, end;
  start = *pos;
  nx_vec_add (&end, &start, dir);

  NxVec4 projs[2];
  projs[0] = start;
  projs[1] = end;
  sr_project_one (mst, &projs[0]);
  sr_project_one (mst, &projs[1]);

  NxVec4 s_pos;
  NxVec4 s_dir;
  s_pos = projs[0];
  nx_vec_sub (&s_dir, &projs[1], &projs[0]);
  NxVec4 z_dir = {0.0f, 0.0f, 1.0f, 1.0f};
  NxVec4 orth_f;
  nx_vec_cross_product (&orth_f, &s_dir, &z_dir);
  nx_vec_normalize3 (&orth_f, &orth_f);
  nx_vec_scale (&orth_f, &orth_f, 0.15f);

  NxVec4 ivec;
  nx_vec_scale (&ivec, &s_dir, 0.85f);
  nx_vec_add (&ivec, &s_pos, &ivec);

  al_draw_line (projs[0].vals[0]*sf+ofx, projs[0].vals[1]*sf+ofx,
                projs[1].vals[0]*sf+ofx, projs[1].vals[1]*sf+ofx,
                clr, 1.0f);

  NxVec4 projs2[2];

  NxVec4 upper, lower;
  nx_vec_add (&upper, &ivec, &orth_f);
  projs2[0] = projs[1];
  projs2[1] = upper;
  al_draw_line (projs2[0].vals[0]*sf+ofx, projs2[0].vals[1]*sf+ofx,
                projs2[1].vals[0]*sf+ofx, projs2[1].vals[1]*sf+ofx,
                clr2, 1.0f);
  nx_vec_negate (&orth_f, &orth_f);
  nx_vec_add (&lower, &ivec, &orth_f);
  projs2[0] = projs[1];
  projs2[1] = lower;
  al_draw_line (projs2[0].vals[0]*sf+ofx, projs2[0].vals[1]*sf+ofx,
                projs2[1].vals[0]*sf+ofx, projs2[1].vals[1]*sf+ofx,
                clr2, 1.0f);
}

void
sr_draw_tri (NxMat *mst, NxVec4 pts[3])
{
  ALLEGRO_COLOR clr;
  clr = al_map_rgb (255, 0, 0);

  float sf;
  sf = 20.0f;
  float ofx, ofy;
  ofx = 50.0f; ofy = 50.0f;

  NxVec4 projs[3];
  projs[0] = pts[0]; projs[1] = pts[1]; projs[2] = pts[2];
  sr_project_one (mst, &projs[0]);
  sr_project_one (mst, &projs[1]);
  sr_project_one (mst, &projs[2]);

  int cnt;
  for (cnt=0; cnt<3; ++cnt)
    {
      int cur, nxt;
      cur = cnt; nxt = (cnt + 1) % 3;
      al_draw_line (projs[cur].vals[0]*sf+ofx, projs[cur].vals[1]*sf+ofx,
                    projs[nxt].vals[0]*sf+ofx, projs[nxt].vals[1]*sf+ofx,
                    clr, 1.0f);
    }
}

void
sr_draw_node (NxMat *mst, GArray *verts, GArray *indices, GArray *uvs)
{
  g_xassert (indices->len > 0);
  g_xassert ((indices->len % 3) == 0);
  int cnt;
  for (cnt=0; cnt<indices->len; cnt+=3)
    {
      unsigned int idx;
      struct xvtx vert;
      NxVec4 xv[3];
      int cnt2;
      for (cnt2=0; cnt2<3; ++cnt2)
        {
          idx = g_array_index (indices, unsigned int, cnt+cnt2);
          g_xassert (idx < verts->len);
          vert = g_array_index (verts, struct xvtx, idx);
          xv[cnt2].vals[0] = vert.x; xv[cnt2].vals[1] = vert.y; xv[cnt2].vals[2] = vert.z; xv[cnt2].vals[3] = 1.0f;
        }
      sr_draw_tri (mst, xv);
    }
}

void
sr_update_global_ypr (ALLEGRO_KEYBOARD_STATE *aks)
{
  int left, right, up, down;
  left = al_key_down (aks, ALLEGRO_KEY_A);
  left = !!left;
  right = al_key_down (aks, ALLEGRO_KEY_D);
  right = !!right;
  up = al_key_down (aks, ALLEGRO_KEY_W);
  up = !!up;
  down= al_key_down (aks, ALLEGRO_KEY_S);
  down= !!down;

  if (al_key_down (aks, ALLEGRO_KEY_ESCAPE))
    {
      printf ("Escape\n");
      exit (0);
    }

  NxMat w_mat;
  w_mat = g_state->p_mat;
  if (left)
    g_state->yaw += 2.0f;
  if (right)
    g_state->yaw -= 2.0f;
  if (up)
    g_state->pitch += 2.0f;
  if (down)
    g_state->pitch -= 2.0f;

  nx_mat_rotate (&w_mat, g_state->yaw,
      0.0f, 1.0f, 0.0f);
  nx_mat_rotate (&w_mat, g_state->pitch,
      1.0f, 0.0f, 0.0f);

  g_state->w_mat = w_mat;
}

struct SrNode *
_sr_node_walk (struct SrNodeGraph *mdl, MaiNode *nde_foreign)
{
  struct SrNode *ret;

  ret = g_malloc0 (sizeof (*ret));

  g_hash_table_insert (mdl->name_node_map, nde_foreign->name, ret);

  ret->name = nde_foreign->name;
  ret->parent_name = nde_foreign->parent ? nde_foreign->parent->name : NULL;
  nx_mat_from_cogl_matrix (&ret->transformation, nde_foreign->transformation);

  ret->child_names_len = nde_foreign->children->len;
  ret->child_names = g_malloc0_n (nde_foreign->children->len, sizeof (*ret->child_names));

  int cnt;
  for (cnt=0; cnt<nde_foreign->children->len; ++cnt)
    {
      MaiNode *nnde;
      struct SrNode *xnnde;

      nnde = g_mai_node_ptr_array_index (nde_foreign->children, cnt);
      g_xassert (nnde);

      xnnde = _sr_node_walk (mdl, nnde);
      ret->child_names[cnt] = xnnde->name;
    }

  return ret;
}

void
sr_model_from_mai_model (struct SrNodeGraph **result, MaiModel *model)
{
  struct SrNodeGraph *ret;

  g_xassert (model->nodes);

  ret = g_malloc0 (sizeof (*ret));

  ret->name = "Err No Such Thing";
  ret->name_node_map = g_hash_table_new (g_str_hash, g_str_equal);
  ret->nodes_len = 1;
  ret->nodes = _sr_node_walk (ret, model->nodes);

  *result = ret;
}

void
sr_weight_dump (MaiModel *model)
{
  MaiNode *mesh_node;
  mesh_node = g_hash_table_lookup (model->name_node_map, "Cube");
  g_xassert (mesh_node);
  g_xassert (mesh_node->bones->len == 2);

  int cnt;
  for (cnt=0; cnt<mesh_node->bones->len; ++cnt)
    {
      MaiBone *bone;
      bone = g_mai_bone_ptr_array_index (mesh_node->bones, cnt);
      printf ("Bone %s\n", bone->name);
      int idx;
      for (idx=0; idx<bone->weights->len; ++idx)
        {
          NxVertexWeight wt;
          wt = g_nx_vertex_weight_array_index (bone->weights, idx);
          printf ("%d: %f\n", wt.vertex_id, wt.weight);
        }
    }
}

void
sr_update_node_graph (MaiAnimInstance *mai, struct SrNodeGraph *graph)
{
  void
  sr_update_channel (MaiNodeAnim *man)
  {
    void
    sr_compose_mtx (NxMat *result)
    {
      /**
       * Stick to AnimEvaluator.cpp AnimEvaluator::Evaluate order.
       * Namely:
       *   Start with Rotation, apply 3x3 scale, set translation
       */
      struct NxAnimKey pos, rot, sca;
      pos = g_array_index (man->position_keys, struct NxAnimKey, mai->current_frame);
      rot = g_array_index (man->rotation_keys, struct NxAnimKey, mai->current_frame);
      sca = g_array_index (man->scaling_keys, struct NxAnimKey, mai->current_frame);

      NxMat mat;
      nx_mat_init_identity (&mat);

      float angle;
      struct xvtx axis;
      CoglQuaternion quat;
      quat.w = rot.val.rot.w;
      quat.x = rot.val.rot.x;
      quat.y = rot.val.rot.y;
      quat.z = rot.val.rot.z;
      nx_cogl_quaternion_to_rotation_axis_and_angle (&quat, &angle, &axis);
      nx_mat_rotate (&mat, angle, axis.x, axis.y, axis.z);

      NX_MAT_ELT (&mat, 0, 0) *= sca.val.vec.x;
      NX_MAT_ELT (&mat, 1, 0) *= sca.val.vec.x;
      NX_MAT_ELT (&mat, 2, 0) *= sca.val.vec.x;
      NX_MAT_ELT (&mat, 0, 1) *= sca.val.vec.y;
      NX_MAT_ELT (&mat, 1, 1) *= sca.val.vec.y;
      NX_MAT_ELT (&mat, 2, 1) *= sca.val.vec.y;
      NX_MAT_ELT (&mat, 0, 2) *= sca.val.vec.z;
      NX_MAT_ELT (&mat, 1, 2) *= sca.val.vec.z;
      NX_MAT_ELT (&mat, 2, 2) *= sca.val.vec.z;

      NX_MAT_ELT (&mat, 0, 3) *= pos.val.vec.x;
      NX_MAT_ELT (&mat, 1, 3) *= pos.val.vec.y;
      NX_MAT_ELT (&mat, 2, 3) *= pos.val.vec.z;

      *result = mat;
    }

    MaiNode *mn;
    struct SrNode *sn;

    sn = g_hash_table_lookup (graph->name_node_map, man->node_name);
    g_xassert (sn);

    NxMat current_mat;
    sr_compose_mtx (&current_mat);

    sn->transformation = current_mat;
  }

  MaiAnim *ma;
  ma = mai->anim;

  for (int cnt = 0; cnt < ma->channels->len; ++cnt)
    {
      sr_update_channel (g_mai_node_anim_ptr_array_index (
          ma->channels, cnt));
    }
}

void
sr_skeletal (MaiModel *model)
{
  MaiAnimInstance *mai;

  g_xassert (model->anims->len == 1);
  mai = mai_anim_instance_new_from_anim (
                                         g_mai_anim_ptr_array_index (model->anims, 0),
                                         model->name_node_map,
                                         model->nodes);

  struct SrNodeGraph *sr_model;
  sr_model_from_mai_model (&sr_model, model);
  struct SrNode *cube_node;
  cube_node = g_hash_table_lookup (sr_model->name_node_map, "Cube");
  g_xassert (cube_node);

  struct SrNodeGraph *sr_model_copy;
  sr_node_graph_copy (&sr_model_copy, sr_model);
  g_xassert (sr_model_copy);

  sr_update_node_graph (mai, sr_model_copy);

  void
  sr_bone_mtx (NxMat *result, MaiBone *bone)
  {
    void
    sr_node_accumulate (NxMat *result, struct SrNode *src)
    {
      NxMat
      _sr_node_accumulate (struct SrNode *cur_src)
      {
        NxMat par_mtx;
        struct SrNode par_node;

        nx_mat_init_identity (&par_mtx);

        par_node = g_hash_table_lookup (cur_src->parent_name, "Cube");
        if (0 != par_node)
          return par_mtx;

        NxMat result;
        par_mtx = _sr_node_accumulate (par_node);
        nx_mat_multiply(&result, par_mtx, cur_src->transformation);
        return result;
      }

      *result = _sr_node_accumulate (src);
    }

    NxMat offset_mtx;
    nx_mat_from_cogl_matrix (&offset_mtx, bone->offset_matrix);

    struct SrNode *tmp;
    tmp = g_hash_table_lookup (sr_model->name_node_map, "Cube");
    g_xassert (tmp);

    NxMat bone_ws;
    bone_ws = sr_node_accumulate (tmp);

    /**
     * Probably also want inverse mesh node but whatever
     */
    NxMat bone_mtx;
    nx_mat_multiply (&bone_mtx, &bone_ws, &offset_mtx);

    *result = bone_mtx;
  }

  /**
   * Warning implicit mesh selection
   */
  MaiNode *mn;
  mn = g_hash_table_lookup (model->name_node_map, "Cube");
  g_xassert (mn);
  for (int cnt = 0; cnt < mn->bones; ++cnt)
    {
      MaiBone *bone;
      bone = g_mai_bone_ptr_array_index (mn->bones, cnt);
      NxMat bone_mtx;
      sr_bone_mtx (&bone_mtx, bone);
    }
}

int
main (int argc, char **argv)
{
  ALLEGRO_DISPLAY *display;

  g_type_init ();

  /**
   * Initialize g_state
   */
  struct NxState state = {0};
  g_state = g_new0 (struct NxState, 1);
  *g_state = state;

  NxMat z_mat;
  nx_mat_projection (&z_mat, -1.0f);
  //nx_mat_ortho (&z_mat);
  nx_mat_translation (&z_mat, 0.0f, 0.0f, -3.0f);
  g_state->p_mat = z_mat;
  g_state->w_mat = z_mat;

  al_init ();
  al_init_primitives_addon ();

  al_install_keyboard ();
  al_install_mouse ();

  display = al_create_display (100, 100);
  g_xassert (display);

  al_set_target_backbuffer (display);

  MaiModel *model;
  model = mai_model_new_from_file ("c_sr_weight.dae");
  MaiNode *mesh_node;
  mesh_node = g_hash_table_lookup (model->name_node_map, "Cube");
  g_xassert (mesh_node);

  sr_weight_dump (model);

  /**
   * Plan
   *   Go make a node drawing function.
   *   Draws meshes and node direction vectors.
   */

  ALLEGRO_KEYBOARD_STATE aks;

  int frame;
  for (frame=0; frame<60; ++frame)
    {
      al_get_keyboard_state (&aks);
      sr_update_global_ypr (&aks);
      al_clear_to_color (al_map_rgb (0, 0, 0));
      sr_draw_node (&g_state->w_mat, mesh_node->mesh_verts, mesh_node->mesh_indices, mesh_node->mesh_uvs);
      NxVec4 uvecs[2] = {{0.0f, 0.0f, 0.0f, 1.0f}, {3.0f, 3.0f, 0.0f, 1.0f}};
      sr_draw_unit_vec_at (&g_state->w_mat, &uvecs[0], &uvecs[1]);

      al_flip_display ();
      al_rest (0.05f);
    }

  return EXIT_SUCCESS;
}

struct SrNode *
_sr_copy_node_walk (struct SrNodeGraph *mdl, struct SrNode *nde)
{
  struct SrNode *ret;

  ret = g_malloc0 (sizeof (*ret));

  /**
   * Lol not ref counted or actually copied
   */
  ret->child_names = nde->child_names;
  ret->child_names_len = nde->child_names_len;
  ret->name = g_strdup (nde->name);
  ret->parent_name = g_strdup (nde->parent_name);
  ret->transformation = nde->transformation;

  return ret;
}

void
sr_node_graph_copy (struct SrNodeGraph **result, struct SrNodeGraph *what)
{
  struct SrNodeGraph *ret;

  ret = g_malloc0 (sizeof (*ret));
  ret->name = what->name;
  ret->name_node_map = g_hash_table_ref (what->name_node_map);
  ret->nodes_len = what->nodes_len;
  ret->nodes = _sr_copy_node_walk (what, what->nodes);

  *result = ret;
}

void
nx_mat_from_cogl_matrix (NxMat *mat, CoglMatrix *cogl_matrix)
{
  g_xassert (mat);
  g_xassert (cogl_matrix);

  float tmp_vals[16] = {
      cogl_matrix->xx, cogl_matrix->yx, cogl_matrix->zx, cogl_matrix->wx,
      cogl_matrix->xy, cogl_matrix->yy, cogl_matrix->zy, cogl_matrix->wy,
      cogl_matrix->xz, cogl_matrix->yz, cogl_matrix->zz, cogl_matrix->wz,
      cogl_matrix->xw, cogl_matrix->yw, cogl_matrix->zw, cogl_matrix->ww,
  };

  nx_mat_init_from_array (mat, tmp_vals);
}
