#include <src/mai-node-anim.h>

#define GET_NEW ((MaiNodeAnim *)g_object_new(mai_node_anim_get_type(), NULL))

XPTR_ARRAY_MACRO_TYPE_DEFINE (mai_node_anim, MaiNodeAnim);

MaiNodeAnim*
mai_node_anim_new_from (struct aiScene *scene, struct aiNodeAnim *from)
{
  MaiNodeAnim * self = GET_NEW;

  self->node_name= g_strdup (from->mNodeName.data);
  self->position_keys = g_array_new (FALSE, TRUE, sizeof (struct NxAnimKey));
  self->rotation_keys = g_array_new (FALSE, TRUE, sizeof (struct NxAnimKey));
  self->scaling_keys = g_array_new (FALSE, TRUE, sizeof (struct NxAnimKey));

  for (gint i = 0; i<from->mNumPositionKeys; ++i)
    {
      struct NxAnimKey toadd;
      toadd.time = from->mPositionKeys[i].mTime;
      toadd.val.vec.x = from->mPositionKeys[i].mValue.x;
      toadd.val.vec.y = from->mPositionKeys[i].mValue.y;
      toadd.val.vec.z = from->mPositionKeys[i].mValue.z;
      g_array_append_vals (self->position_keys, &toadd, 1);
    }

  for (gint i=0; i<from->mNumRotationKeys; ++i)
    {
      struct NxAnimKey toadd;
      toadd.time = from->mRotationKeys[i].mTime;
      toadd.val.rot.w = from->mRotationKeys[i].mValue.w;
      toadd.val.rot.x = from->mRotationKeys[i].mValue.x;
      toadd.val.rot.y = from->mRotationKeys[i].mValue.y;
      toadd.val.rot.z = from->mRotationKeys[i].mValue.z;
      g_array_append_vals (self->rotation_keys, &toadd, 1);
    }

  for (gint i=0; i<from->mNumScalingKeys; ++i)
    {
      struct NxAnimKey toadd;
      toadd.time = from->mScalingKeys[i].mTime;
      toadd.val.vec.x = from->mScalingKeys[i].mValue.x;
      toadd.val.vec.y = from->mScalingKeys[i].mValue.y;
      toadd.val.vec.z = from->mScalingKeys[i].mValue.z;
      g_array_append_vals (self->scaling_keys, &toadd, 1);
    }

  return self;
}
