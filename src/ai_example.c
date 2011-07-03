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
_stuff (const struct aiScene *scene);


void
ai_import_file (const char *file_name)
{
  const struct aiScene *scene;
  /**
   * Warning: PreTransformVertices, flatten scene essentially.
   * See LimitBoneWeights for shared skinning.
   */
  scene = aiImportFile (file_name,
      aiProcess_Triangulate | aiProcess_SortByPType |
      aiProcess_JoinIdenticalVertices);
  g_xassert (scene);

  _stuff (scene);
}

void
_stuff (const struct aiScene *scene)
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
  mn = mai_node_new_from (root_node, NULL);
  printf ("MaiNode name '%s'\n", mn->name);
  return;
}
