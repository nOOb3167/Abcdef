#include <src/mai-node-anim.h>
#include <src/mai-anim_funcs.h>

#define GET_NEW ((MaiAnim *)g_object_new(mai_anim_get_type(), NULL))

MaiAnim*
mai_anim_new_from (struct aiScene *scene, struct aiAnimation *from)
{
  MaiAnim * self = GET_NEW;

  self->name = g_strdup (from->mName.data);
  self->duration = from->mDuration;

  self->channels = g_array_new (FALSE, TRUE, sizeof (MaiNodeAnim));

  return self;
}
