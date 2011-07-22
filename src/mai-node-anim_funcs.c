#include <src/mai-node-anim_funcs.h>

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

  int cnt;

  for (cnt=0; cnt<from->mNumPositionKeys; ++cnt)
    {
      struct NxAnimKey toadd;
      toadd.time = from->mPositionKeys[cnt].mTime;
      toadd.val.vec.x = from->mPositionKeys[cnt].mValue.x;
      toadd.val.vec.y = from->mPositionKeys[cnt].mValue.y;
      toadd.val.vec.z = from->mPositionKeys[cnt].mValue.z;
      g_array_append_vals (self->position_keys, &toadd, 1);
    }

  for (cnt=0; cnt<from->mNumRotationKeys; ++cnt)
    {
      struct NxAnimKey toadd;
      toadd.time = from->mRotationKeys[cnt].mTime;
      toadd.val.rot.w = from->mRotationKeys[cnt].mValue.w;
      toadd.val.rot.x = from->mRotationKeys[cnt].mValue.x;
      toadd.val.rot.y = from->mRotationKeys[cnt].mValue.y;
      toadd.val.rot.z = from->mRotationKeys[cnt].mValue.z;
      g_array_append_vals (self->rotation_keys, &toadd, 1);
    }

  for (cnt=0; cnt<from->mNumScalingKeys; ++cnt)
    {
      struct NxAnimKey toadd;
      toadd.time = from->mScalingKeys[cnt].mTime;
      toadd.val.vec.x = from->mScalingKeys[cnt].mValue.x;
      toadd.val.vec.y = from->mScalingKeys[cnt].mValue.y;
      toadd.val.vec.z = from->mScalingKeys[cnt].mValue.z;
      g_array_append_vals (self->scaling_keys, &toadd, 1);
    }

  return self;
}
