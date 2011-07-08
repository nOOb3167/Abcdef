#include <stdio.h>
#include <string.h>
#include <error.h>
#include <assimp.h>
#include <aiPostProcess.h>
#include <aiScene.h>

#include <src/mai-node.h>
#include <Nxt.h>
#include <ai_example.h>

void
_stuff (struct aiScene *scene);


void
ai_import_file (const char *file_name)
{
  const struct aiScene *scene;
  struct aiScene *nonconst_scene;
  /**
   * Warning: PreTransformVertices, flatten scene essentially.
   * See LimitBoneWeights for shared skinning.
   */
  scene = aiImportFile (file_name,
      aiProcess_Triangulate | aiProcess_SortByPType |
      aiProcess_JoinIdenticalVertices);
  g_xassert (scene);
  nonconst_scene = (struct aiScene *)scene;

  _stuff (nonconst_scene);
}

void
_stuff (struct aiScene *scene)
{
  g_xassert (scene->mNumMeshes >= 1);
  struct aiMesh *mesh;
  mesh = scene->mMeshes[0];
  printf ("Total number of meshes: %d\n", scene->mNumMeshes);
  printf ("Number of faces/vertices in mesh zero: %d/%d\n", mesh->mNumFaces, mesh->mNumVertices);
  g_xassert (mesh->mPrimitiveTypes == aiPrimitiveType_TRIANGLE);

  struct xvtx *verts;
  verts = g_new0 (struct xvtx, mesh->mNumVertices);
  int cnt;
  for (cnt=0; cnt<mesh->mNumVertices; ++cnt)
    {
      verts[cnt].x = mesh->mVertices[cnt].x;
      verts[cnt].y = mesh->mVertices[cnt].y;
      verts[cnt].z = mesh->mVertices[cnt].z;
    }

  unsigned int *indices;
  indices = g_new0 (unsigned int, mesh->mNumFaces * 3);
  int idx;
  for (cnt=0,idx=0; cnt<mesh->mNumFaces; ++cnt)
    {
      indices[idx++] = mesh->mFaces[cnt].mIndices[0];
      indices[idx++] = mesh->mFaces[cnt].mIndices[1];
      indices[idx++] = mesh->mFaces[cnt].mIndices[2];
    }

  nxt_draw_array (verts, mesh->mNumVertices, indices, mesh->mNumFaces * 3);

  g_xassert (scene->mRootNode);
  struct aiNode *root_node;
  root_node = scene->mRootNode;
  g_xassert (!strcmp (root_node->mName.data, "Scene"));

  struct _MaiNode *mn;
  mn = mai_node_new_from (scene, root_node, NULL);
  printf ("Nodename '%s'\n", mn->name);
  int tmp1;
  for (tmp1=0; tmp1<mn->children->len; ++tmp1)
    {
      printf ("Subname '%s'\n", ((struct _MaiNode*)(g_ptr_array_index(mn->children, tmp1)))->name);
    }
  return;
}

void
ai_matrix_to_cogl_matrix (struct aiMatrix4x4 *ai_matrix, CoglMatrix *cogl_matrix)
{
  g_xassert (ai_matrix);
  g_xassert (cogl_matrix);

  /* coglRC aiRC */
  cogl_matrix->xx = ai_matrix->a1;
  cogl_matrix->yx = ai_matrix->b1;
  cogl_matrix->zx = ai_matrix->c1;
  cogl_matrix->wx = ai_matrix->d1;

  cogl_matrix->xy = ai_matrix->a2;
  cogl_matrix->yy = ai_matrix->b2;
  cogl_matrix->zy = ai_matrix->c2;
  cogl_matrix->wy = ai_matrix->d2;

  cogl_matrix->xz = ai_matrix->a3;
  cogl_matrix->yz = ai_matrix->b3;
  cogl_matrix->zz = ai_matrix->c3;
  cogl_matrix->wz = ai_matrix->d3;

  cogl_matrix->xw = ai_matrix->a4;
  cogl_matrix->yw = ai_matrix->b4;
  cogl_matrix->zw = ai_matrix->c4;
  cogl_matrix->ww = ai_matrix->d4;
}

CoglPrimitive *
nx_cogl_primitive_new (GArray *verts, GArray *indices)
{
  CoglAttributeBuffer *bfr;
  g_xassert (verts->len > 0);
  bfr = cogl_attribute_buffer_new (sizeof (struct xvtx) * verts->len, verts->data);
  g_xassert (bfr);

  CoglAttribute *attr;
  attr = cogl_attribute_new (bfr, "cogl_position_in",
                             sizeof (struct xvtx), offsetof (struct xvtx, x),
                             3, COGL_ATTRIBUTE_TYPE_FLOAT);
  g_xassert (attr);

  CoglIndices *idx;
  g_xassert (indices->len > 0);
  idx = cogl_indices_new (COGL_INDICES_TYPE_UNSIGNED_INT, indices->data, indices->len);
  g_xassert (idx);

  CoglPrimitive *prim;
  prim = cogl_primitive_new (COGL_VERTICES_MODE_TRIANGLES, verts->len, attr, NULL);
  g_xassert (prim);

  cogl_primitive_set_indices (prim, idx);

  cogl_object_unref (idx);
  cogl_object_unref (bfr);
  cogl_object_unref (attr);

  return prim;
}
