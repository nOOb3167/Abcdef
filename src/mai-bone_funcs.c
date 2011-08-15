#include <src/ai_example.h>
#include <src/mai-bone_funcs.h>

XPTR_ARRAY_MACRO_TYPE_DEFINE (mai_bone, MaiBone);

MaiBone *
mai_bone_new_from (struct aiBone *bone)
{
  MaiBone *self = GET_NEW;

  self->name = g_strdup (bone->mName.data);
  self->offset_matrix = g_new0 (CoglMatrix, 1);
  ai_matrix_to_cogl_matrix (&bone->mOffsetMatrix, self->offset_matrix);

  self->weights = g_nx_vertex_weight_array_new ();
  int cnt;
  for (cnt=0; cnt<bone->mNumWeights; ++cnt)
    {
      NxVertexWeight nvw;
      nvw.vertex_id = bone->mWeights[cnt].mVertexId;
      nvw.weight = bone->mWeights[cnt].mWeight;
      g_nx_vertex_weight_array_append_val (self->weights, &nvw);
    }

  return self;
}
