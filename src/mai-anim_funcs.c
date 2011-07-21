#include <src/mai-node-anim.h>
#include <src/mai-node-anim_funcs.h>
#include <src/mai-anim_funcs.h>

#define GET_NEW ((MaiAnim *)g_object_new(mai_anim_get_type(), NULL))

XARRAY_MACRO_TYPE_DEFINE (mai_anim, MaiAnim);

MaiAnim*
mai_anim_new_from (struct aiScene *scene, struct aiAnimation *from)
{
  MaiAnim * self = GET_NEW;

  self->name = g_strdup (from->mName.data);
  self->duration = from->mDuration;

  self->channels = g_array_new (FALSE, TRUE, sizeof (MaiNodeAnim));
  int cnt;
  for (cnt=0; cnt<from->mNumChannels; ++cnt)
    {
      MaiNodeAnim *na;
      na = mai_node_anim_new_from (scene, from->mChannels[cnt]);
      g_array_append_vals (self->channels, na, 1);
    }

  return self;
}
