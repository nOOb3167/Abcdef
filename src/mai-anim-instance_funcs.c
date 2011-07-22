#include <stdio.h>
#include <src/ai_example.h>
#include <src/mai-anim-instance_funcs.h>

void
_mai_anim_draw_recursive (MaiAnimInstance *self, MaiNode *node, CoglMatrix *acc_mtx);


MaiAnimInstance *
mai_anim_instance_new_from_anim (MaiAnim *anim, GHashTable *name_node_map, MaiNode *nodes)
{
  MaiAnimInstance *self = GET_NEW;

  self->anim = anim;
  self->name_node_map = name_node_map;
  self->nodes = nodes;
  self->current_frame = 0;

  return self;
}

void
mai_anim_instance_draw (MaiAnimInstance * self)
{
  CoglMatrix initial_mtx;
  cogl_matrix_init_identity (&initial_mtx);
  cogl_set_modelview_matrix (&initial_mtx);

  cogl_set_source_color4ub ('\x1', '\xFF', '\x1', 255);
  cogl_set_source_texture (g_testtex);
  cogl_ortho (0, 64, 0, 64, -1, 1);

  cogl_matrix_translate (&initial_mtx, 20.0f, 20.0f, 0.0f);
  cogl_matrix_scale (&initial_mtx, 5.0f, 5.0f, 1.0f);

  _mai_anim_draw_recursive (self, self->nodes, &initial_mtx);

  cogl_flush ();
}

void
_mai_anim_draw_recursive (MaiAnimInstance *self, MaiNode *node, CoglMatrix *acc_mtx)
{
  CoglPrimitive *to_draw;
  if (node->mesh_verts->len > 0)
    to_draw = nx_cogl_primitive_new (node->mesh_verts, node->mesh_indices, node->mesh_uvs);

  CoglMatrix cur_mtx;
  cogl_matrix_init_identity (&cur_mtx);
  cogl_matrix_multiply (&cur_mtx, &cur_mtx, acc_mtx);

  CoglMatrix anim_trans;
  cogl_matrix_init_identity (&anim_trans);
  cogl_matrix_multiply (&anim_trans, &anim_trans, node->transformation);
  int cnt;
  for (cnt=0; cnt<self->anim->channels->len; ++cnt)
    {
      MaiNodeAnim *mni;
      mni = g_mai_node_anim_ptr_array_index (self->anim->channels, cnt);
      if (0 == g_strcmp0 (mni->node_name, node->name))
        {
          struct NxAnimKey pos;
          struct NxAnimKey rot;
          struct NxAnimKey sca;
          pos = g_array_index (mni->position_keys, struct NxAnimKey, self->current_frame);
          rot = g_array_index (mni->rotation_keys, struct NxAnimKey, self->current_frame);
          sca = g_array_index (mni->scaling_keys, struct NxAnimKey, self->current_frame);
          cogl_matrix_init_identity (&anim_trans);
          cogl_matrix_translate (&anim_trans, pos.val.vec.x, pos.val.vec.y, pos.val.vec.z);
          CoglQuaternion quat;
          float rotang;
          float vals[4] = {rot.val.rot.w, rot.val.rot.x, rot.val.rot.y, rot.val.rot.z};
          cogl_quaternion_init_from_array (&quat, vals);
          struct xvtx vec3;
          nx_cogl_quaternion_to_rotation_axis_and_angle (&quat, &rotang, &vec3);
          cogl_matrix_rotate (&anim_trans, rotang, vec3.x, vec3.y, vec3.z);
          printf ("Anim match | %s | %f %f %f %f\n", node->name, rotang, vec3.x, vec3.y, vec3.z);
          break;
        }
    }

  cogl_matrix_multiply (&cur_mtx, &cur_mtx, &anim_trans);

  cogl_set_modelview_matrix (&cur_mtx);

  if (node->mesh_verts->len > 0)
    {
      nx_cogl_primitive_draw (to_draw);

      cogl_object_unref (to_draw);
    }

  int tmp1;
  for (tmp1=0; tmp1<node->children->len; ++tmp1)
    {
      MaiNode *child;
      child = g_ptr_array_index(node->children, tmp1);
      _mai_anim_draw_recursive (self, child, &cur_mtx);
    }
}
