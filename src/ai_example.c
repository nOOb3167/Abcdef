#include <stdio.h>
#include <error.h>
#include <assimp.h>
#include <aiPostProcess.h>
#include <aiScene.h>

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
      aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_PreTransformVertices);
  g_xassert (scene);

  _stuff (scene);
}

void
_stuff (const struct aiScene *scene)
{
  g_xassert (scene->mNumMeshes == 1);
  struct aiMesh *mesh;
  mesh = scene->mMeshes[0];
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
  nxt_draw_array (verts, mesh->mNumVertices);
}
