#include <error.h>
#include <assimp.h>
#include <aiPostProcess.h>
#include <aiScene.h>

#include <ai_example.h>

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
}
