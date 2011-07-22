#ifndef MAI_ANIM_INSTANCE_FUNCS_H_
#define MAI_ANIM_INSTANCE_FUNCS_H_

MaiAnimInstance *
mai_anim_instance_new_from_anim (MaiAnim *anim, GHashTable *name_node_map, MaiNode *nodes);

void
mai_anim_instance_draw (MaiAnimInstance * self);

#endif /* MAI_ANIM_INSTANCE_FUNCS_H_ */
