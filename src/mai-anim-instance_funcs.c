#include <src/mai-anim-instance_funcs.h>

MaiAnimInstance *
mai_anim_instance_new_from_anim (MaiAnim *anim, GHashTable *name_node_map, MaiNode *nodes)
{
  MaiAnimInstance *self = GET_NEW;

  self->anim = anim;
  self->name_node_map = name_node_map;
  self->nodes = nodes;

  return self;
}
