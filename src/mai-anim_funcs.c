#include <src/mai-node-anim.h>
#include <src/mai-anim.h>

#define GET_NEW ((MaiAnim *)g_object_new(mai_anim_get_type(), NULL))

XARRAY_MACRO_TYPE_DEFINE (mai_anim, MaiAnim);
XPTR_ARRAY_MACRO_TYPE_DEFINE (mai_anim, MaiAnim);

MaiAnim*
mai_anim_new_from (struct aiScene *scene, struct aiAnimation *from)
{
  MaiAnim * self = GET_NEW;

  self->name = g_strdup (from->mName.data);
  self->duration = from->mDuration;

  self->channels = g_mai_node_anim_ptr_array_new ();
  int cnt;
  for (cnt=0; cnt<from->mNumChannels; ++cnt)
    {
      MaiNodeAnim *na;
      na = mai_node_anim_new_from (scene, from->mChannels[cnt]);
      g_mai_node_anim_ptr_array_add (self->channels, na);
    }

  return self;
}
