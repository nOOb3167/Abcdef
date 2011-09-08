#include <src/ai_example.h>
#include <src/array_macro.h>
#include <src/mai-bone.h>

XPTR_ARRAY_MACRO_TYPE_DEFINE (mai_bone, MaiBone);

NX_GET_NEW (mai_bone);

MaiBone *
mai_bone_new_from (struct aiBone *bone)
{
  MaiBone *self;

  self = MAI_BONE (nx_get_new ());
  self->name = g_strdup (bone->mName.data);
  self->offset_matrix = g_malloc0 (sizeof (*self->offset_matrix));
  ai_matrix_to_cogl_matrix (&bone->mOffsetMatrix, self->offset_matrix);

  self->weights = g_nx_vertex_weight_array_new ();

  for (gint i = 0; i < bone->mNumWeights; ++i)
    {
      NxVertexWeight nvw;
      nvw.vertex_id = bone->mWeights[i].mVertexId;
      nvw.weight = bone->mWeights[i].mWeight;
      g_nx_vertex_weight_array_append_val (self->weights, &nvw);
    }

  self->id_wval_map = g_hash_table_new_full (g_int_hash, g_int_equal,
      g_free, g_free);

  for (gint i = 0; i < bone->mNumWeights; ++i)
    {
      int *id;
      float *weight;
      NxVertexWeight nvw;

      id = g_malloc (sizeof (*id));
      weight = g_malloc (sizeof (*weight));
      nvw = g_nx_vertex_weight_array_index (self->weights, i);

      *id = nvw.vertex_id;
      *weight = nvw.weight;

      g_hash_table_insert (self->id_wval_map, id, weight);
    }

  return self;
}
