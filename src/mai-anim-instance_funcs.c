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
    {
      /**
       * Offset->weight->BoneNode->Inv(Node) I think.
       * Vertex starts in mesh, offset puts it into bone where it gets weight applied.
       * BoneNode puts it into World, Inv(Node) puts it into node.
       * Since I'm invoking primitive_draw with Node as transform, it will conveniently cancel the Inv(Node).
       */
      GArray *draw_this;
      draw_this = node->mesh_verts;
      if (node->bones->len > 0)
        {
          GArray *palletted;
          palletted = g_array_new (FALSE, TRUE, sizeof (struct xvtx));
          int cnt;
          for (cnt=0; cnt<node->mesh_verts->len; ++cnt)
            {
              struct xvtx vtx;
              vtx = g_array_index (node->mesh_verts, struct xvtx, cnt);
              int idx;
              for (idx=0; idx<node->bones->len; ++idx)
                {
                  /**
                   * I should be able to find the bone inside the name_node_map (Gives BoneNode).
                   */
                  MaiNode *bone_trans_node;
                  bone_trans_node = g_hash_table_lookup (self->name_node_map, g_mai_bone_ptr_array_index (node->bones, idx)->name);
                  g_xassert (bone_trans_node);

                  MaiNode *tmp_node;
                  CoglMatrix skin_trans_mtx;
                  cogl_matrix_init_identity (&skin_trans_mtx);
                  //http://sourceforge.net/projects/assimp/forums/forum/817654/topic/3880745
                  //Says multiply bone offset by local node to mesh matrix "node-to-mesh matrix"
                  /**
                   * Blah blah
                   *   So at each frame you need to calculate the bone matrix for each bone.
                   *   You do this by using the bone's so-called offset matrix or inverse bind matrix.
                   *   The offset matrix transforms from mesh coordinates to the bone's local coordinate system,
                   *   it's a mesh-to-bone matrix.
                   * And 10.1.1.97.1412.pdf says vi=Tiv^i=TiT^i-1v^
                   */
                  for (tmp_node=bone_trans_node; tmp_node!=NULL; tmp_node=tmp_node->parent)
                    {
                      CoglMatrix stcpy;
                      cogl_matrix_init_identity (&stcpy);
                      cogl_matrix_multiply (&stcpy, &stcpy, &skin_trans_mtx);
                      cogl_matrix_multiply (&skin_trans_mtx, tmp_node->transformation, &stcpy);
                    }
                  CoglMatrix in_world;
                  //cogl_matrix_multiply (&in_world, bone_trans_node->transformation, &skin_trans_mtx);
                  cogl_matrix_init_identity (&in_world);
                  cogl_matrix_multiply (&in_world, &in_world, &skin_trans_mtx);
                  CoglMatrix pre_inv_node;
                  CoglMatrix inv_node;
                  gboolean nondegenerate;
                  {
                    for (tmp_node=node; tmp_node!=NULL; tmp_node=tmp_node->parent)
                      {
                        CoglMatrix stcpy;
                        cogl_matrix_init_identity (&stcpy);
                        cogl_matrix_multiply (&stcpy, &stcpy, &pre_inv_node);
                        cogl_matrix_multiply (&pre_inv_node, tmp_node->transformation, &stcpy);
                      }
                  }
                  nondegenerate = cogl_matrix_get_inverse (&pre_inv_node, &inv_node);
                  g_xassert (nondegenerate);
                  CoglMatrix final;
                  cogl_matrix_multiply (&final, &inv_node, &in_world);
                  float pts[4] = {vtx.x, vtx.y, vtx.z, 1.0f};
                  cogl_matrix_transform_point (&final, &pts[0], &pts[1], &pts[2], &pts[3]);
                  //cogl_matrix_transform_point (&in_world, &pts[0], &pts[1], &pts[2], &pts[3]);
                  printf ("%f %f\n", vtx.y, pts[1]);
                }
              g_array_append_vals (palletted, &vtx, 1);
            }
          draw_this = palletted;
        }
      to_draw = nx_cogl_primitive_new (draw_this, node->mesh_indices, node->mesh_uvs);
    }

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
          cogl_quaternion_init_identity (&quat);
          quat.w = rot.val.rot.w; quat.x = rot.val.rot.x;
          quat.y = rot.val.rot.y; quat.z = rot.val.rot.z;
          struct xvtx vec3;
          nx_cogl_quaternion_to_rotation_axis_and_angle (&quat, &rotang, &vec3);
          cogl_matrix_rotate (&anim_trans, rotang, vec3.x, vec3.y, vec3.z);
          //printf ("Anim match | %s | %f %f %f %f\n", node->name, rotang, vec3.x, vec3.y, vec3.z);
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
