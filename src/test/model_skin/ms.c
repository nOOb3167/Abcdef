#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_windows.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_primitives.h>
#include <src/cogl_include.h>

#include <src/error.h>
#include <src/gfx_lib_setup.h>
#include <src/mai-model.h>
#include <src/mai-model_funcs.h>
#include <src/mai-node-anim.h>
#include <src/mai-node-anim_funcs.h>
#include <src/misc.h>
#include <src/ai_example.h>

CoglHandle *g_testtex;

void
_display_loop (void);

void
_ms_prim_draw (CoglPrimitive *prim, CoglMatrix *mtx);

void
_ms_stuff (MaiModel *mm);

void
nx_skin_transform (MaiModel *model, MaiAnimInstance *anim_instance, MaiNode *mesh_node, GArray **verts_out);

void
nx_skin_transform_vert (CoglMatrix *tmtx, struct xvtx *vert_inout);

void
_nx_combine_keys (struct NxAnimKey pos, struct NxAnimKey rot, CoglMatrix *combined_inout);

void
_display_loop (void)
{
  context_switch_allegro ();

  ALLEGRO_BITMAP *bmp;
  bmp = al_create_bitmap (640, 480);

  int blah;
  for (blah=0; blah < 10; ++blah)
    {
      struct fbstate_data fbd;

      context_switch_cogl ();
      fbd = fbstate_get_data ();

      context_switch_allegro ();
      ALLEGRO_LOCKED_REGION *rgn;
      rgn = al_lock_bitmap (bmp, ALLEGRO_PIXEL_FORMAT_BGR_888, ALLEGRO_LOCK_READWRITE);
      int cnt;
      char *data;
      for (cnt=0,data=rgn->data; cnt < 480; ++cnt,data+=rgn->pitch)
        {
          memcpy (data, &fbd.data[cnt*640*3], 640*3);
        }
      al_unlock_bitmap (bmp);

      context_switch_cogl ();

      context_switch_allegro ();
      al_set_target_backbuffer (fbd.display);
      al_draw_bitmap (bmp, 0, 0, 0);
      al_flip_display ();

      al_rest (0.1f);
    }

  al_rest (2.0f);
}

int
main (int argc, char **argv)
{
    g_type_init ();

    gfx_lib_setup ();

    printf ("COGL SETUP COMPLETE\n");

    g_testtex = cogl_texture_new_from_file ("testtex.bmp", COGL_TEXTURE_NONE, COGL_PIXEL_FORMAT_ANY, NULL);
    g_xassert (g_testtex != COGL_INVALID_HANDLE);

    MaiModel *mm;
    mm = mai_model_new_from_file ("c_multipart_collada_skin_ms.dae");

    _ms_stuff (mm);

    _display_loop ();

    return EXIT_SUCCESS;
}

void
_ms_prim_draw (CoglPrimitive *prim, CoglMatrix *mtx)
{
  CoglMatrix initial_mtx;
  cogl_matrix_init_identity (&initial_mtx);
  cogl_set_modelview_matrix (&initial_mtx);

  cogl_set_source_color4ub ('\x1', '\xFF', '\x1', 255);
  cogl_set_source_texture (g_testtex);
  cogl_ortho (0, 64, 0, 64, -200, 200);

  cogl_matrix_scale (&initial_mtx, 5.0f, 5.0f, 5.0f);
  cogl_matrix_translate (&initial_mtx, 4.0f, 4.0f, 0.0f);


  cogl_matrix_multiply (&initial_mtx, &initial_mtx, mtx);
  cogl_set_modelview_matrix (&initial_mtx);

  nx_cogl_primitive_draw (prim);

  cogl_flush ();
}

void
_ms_stuff (MaiModel *mm)
{
  MaiNode *scene_node;
  MaiNode *cube_node;
  MaiNode *bone_node;

  void nnmp (gpointer key, gpointer value, gpointer data)
  {
    printf ("nnmp %s\n", key);
  }
  g_hash_table_foreach (mm->name_node_map, nnmp, NULL);

  scene_node = g_hash_table_lookup (mm->name_node_map, "Scene");
  cube_node = g_hash_table_lookup (mm->name_node_map, "Cube");
  bone_node = g_hash_table_lookup (mm->name_node_map, "Bone");
  g_xassert (scene_node);
  g_xassert (cube_node);
  g_xassert (bone_node);

  MaiBone *bone;
  bone = g_mai_bone_ptr_array_index (cube_node->bones, 0);
  g_xassert (bone);

  /**
   * Confirmed bone->transform_matrix from mesh to bone space.
   *   When translating Cube by -2y, offset_matrix also -2y.
   * However bone space seems to be rot=identity; trans=What Blender 'n' properties key shows.
   *
   * XX:
   * Maybe the Scene node is superfluous.
   * Assimp http://assimp.sourceforge.net/lib_html/data.html section Bones suggested algo would suggest
   *   something that would not leave the Scene node in?
   * As far as I can tell the scene node transform is
   *   invert z
   *   Err isn't that newz=-oldy and newy=oldz?
   */

  /**
   * Results of transform mesh -> bone seem ok to me.
   */
  float tmp_vtx1[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  cogl_matrix_transform_point (bone->offset_matrix, &tmp_vtx1[0], &tmp_vtx1[1], &tmp_vtx1[2], &tmp_vtx1[3]);

  void acc_transform (MaiNode *node, CoglMatrix *acc_mtx)
  {
    if (node == NULL)
      {
        cogl_matrix_init_identity (acc_mtx);
        return;
      }
    acc_transform (node->parent, acc_mtx);
    cogl_matrix_multiply (acc_mtx, acc_mtx, node->transformation);
  }

  CoglMatrix bone_ws;
  acc_transform (bone_node, &bone_ws);

  /**
   * Seems ok (Z inverted due to Scene, that's the right thing?).
   * Err see above on swap.
   */
  cogl_matrix_transform_point (&bone_ws, &tmp_vtx1[0], &tmp_vtx1[1], &tmp_vtx1[2], &tmp_vtx1[3]);

  CoglMatrix cube_ws;
  acc_transform (cube_node, &cube_ws);
  CoglMatrix cube_ws_inv;
  nx_cogl_matrix_get_inverse (&cube_ws, &cube_ws_inv);

  cogl_matrix_transform_point (&cube_ws_inv, &tmp_vtx1[0], &tmp_vtx1[1], &tmp_vtx1[2], &tmp_vtx1[3]);
  cogl_matrix_transform_point (&cube_ws, &tmp_vtx1[0], &tmp_vtx1[1], &tmp_vtx1[2], &tmp_vtx1[3]);

  g_xassert (mm->anims->len > 0);

  GArray *new_verts;
  MaiAnimInstance *mai;
  mai = mai_model_get_anim_by_name (mm, g_mai_anim_ptr_array_index (mm->anims, 0)->name);
  mai->current_frame = 10;
  nx_skin_transform (mm,
                     mai,
                     cube_node, &new_verts);

  CoglPrimitive *prim;
  prim = nx_cogl_primitive_new (new_verts, cube_node->mesh_indices, cube_node->mesh_uvs);
  _ms_prim_draw (prim, &cube_ws);

  return;
}

/**
 * Should be taking an MaiAnimInstance anyway
 */
void
nx_skin_transform (MaiModel *model, MaiAnimInstance *anim_instance, MaiNode *mesh_node, GArray **verts_out)
{
  void acc_transform (MaiNode *node, CoglMatrix *acc_mtx)
  {
    if (node == NULL)
      {
        cogl_matrix_init_identity (acc_mtx);
        return;
      }
    acc_transform (node->parent, acc_mtx);
    cogl_matrix_multiply (acc_mtx, acc_mtx, node->transformation);
  }

  void acc_transform_cur (MaiNode *node, CoglMatrix *leaf_replacement_mtx, CoglMatrix *acc_mtx)
  {
    void acc_transform_aux (gboolean first, MaiNode *node, CoglMatrix *leaf_replacement_mtx, CoglMatrix *acc_mtx)
    {
      CoglMatrix *node_transformation;
      if (first)
        node_transformation = leaf_replacement_mtx;
      else
        NULL == node ? NULL : (node_transformation = node->transformation);
      if (node == NULL)
        {
          cogl_matrix_init_identity (acc_mtx);
          return;
        }
      acc_transform_aux (0, node->parent, leaf_replacement_mtx, acc_mtx);
      cogl_matrix_multiply (acc_mtx, acc_mtx, node_transformation);
    }
    acc_transform_aux (1, node, leaf_replacement_mtx, acc_mtx);
  }

  MaiAnim *anim;
  anim = anim_instance->anim;

  MaiNodeAnim *na;
  na = g_mai_node_anim_ptr_array_index (anim->channels, 0);
  g_xassert (0 == g_strcmp0 (na->node_name, "Bone"));
  struct NxAnimKey cur_pos_key;
  struct NxAnimKey cur_rot_key;
  cur_pos_key = g_array_index (na->position_keys, struct NxAnimKey, anim_instance->current_frame);
  cur_rot_key = g_array_index (na->rotation_keys, struct NxAnimKey, anim_instance->current_frame);

  MaiBone *bone;
  MaiNode *bone_node;

  g_xassert (mesh_node->bones->len > 0);
  bone = g_mai_bone_ptr_array_index (mesh_node->bones, 0);
  bone_node = g_hash_table_lookup (model->name_node_map, bone->name);
  g_xassert (bone_node);

  CoglMatrix bone_ws;
  CoglMatrix cur_bone;
  CoglMatrix cur_bone_ws;
  CoglMatrix mesh_node_ws;
  CoglMatrix mesh_node_ws_inv;

  _nx_combine_keys (cur_pos_key, cur_rot_key, &cur_bone);
  acc_transform_cur (bone_node, &cur_bone, &cur_bone_ws);
  acc_transform (bone_node, &bone_ws);
  acc_transform (mesh_node, &mesh_node_ws);
  nx_cogl_matrix_get_inverse (&mesh_node_ws, &mesh_node_ws_inv);

  CoglMatrix tmtx;
  cogl_matrix_init_identity (&tmtx);
  //cogl_matrix_multiply (&tmtx, &tmtx, &mesh_node_ws_inv);
  cogl_matrix_multiply (&tmtx, &tmtx, &cur_bone_ws);
  cogl_matrix_multiply (&tmtx, &tmtx, bone->offset_matrix);

  GArray *new_verts;
  new_verts = g_array_new (0, 1, sizeof (struct xvtx));

  int cnt;
  for (cnt=0; cnt<mesh_node->mesh_verts->len; ++cnt)
    {
      NxVertexWeight find_id (MaiBone *bone, int id)
      {
        g_xassert (id >= 0);
        NxVertexWeight vw;
        int cnt;
        for (cnt=0; cnt<bone->weights->len; ++cnt)
          {
            vw = g_nx_vertex_weight_array_index (bone->weights, cnt);
            if (vw.vertex_id == id)
              break;
          }
        g_xassert (cnt != bone->weights->len);
        return vw;
      }

      struct xvtx cov;
      cov = g_array_index (mesh_node->mesh_verts, struct xvtx, cnt);
      nx_skin_transform_vert (&tmtx, &cov);
      NxVertexWeight vw;
      vw = find_id (bone, cnt);
      /**
       * Warning in
       * http://sourceforge.net/projects/assimp/forums/forum/817654/topic/3880745
       * applying weights comes after trafo (offset_matrix&transform_aux)
       * But I'm doing after tmtx (which is offset_matrix&transform_aux&inverse_mesh_node)
       */
      cov.x *= vw.weight; cov.y *= vw.weight; cov.z *= vw.weight;
      float tmp_pt = 1.0f;
      cogl_matrix_transform_point (&mesh_node_ws_inv, &cov.x, &cov.y, &cov.z, &tmp_pt);
      g_array_append_vals (new_verts, &cov, 1);
    }

  *verts_out = new_verts;
}

void
nx_skin_transform_vert (CoglMatrix *tmtx, struct xvtx *vert_inout)
{
  float vert_f[4] = {vert_inout->x, vert_inout->y, vert_inout->z, 1.0f};
  cogl_matrix_transform_point (tmtx, &vert_f[0], &vert_f[1], &vert_f[2], &vert_f[3]);
  vert_inout->x = vert_f[0]; vert_inout->y = vert_f[1]; vert_inout->z = vert_f[2];
}

void
_nx_combine_keys (struct NxAnimKey pos, struct NxAnimKey rot, CoglMatrix *combined_inout)
{
  CoglMatrix cur_bone_mtx;
  cogl_matrix_init_identity (&cur_bone_mtx);
  CoglQuaternion quat;
  float rotang;
  cogl_quaternion_init_identity (&quat);
  quat.w = rot.val.rot.w; quat.x = rot.val.rot.x;
  quat.y = rot.val.rot.y; quat.z = rot.val.rot.z;
  struct xvtx vec3;
  nx_cogl_quaternion_to_rotation_axis_and_angle (&quat, &rotang, &vec3);
  cogl_matrix_rotate (&cur_bone_mtx, rotang, vec3.x, vec3.y, vec3.z);
  /**
   *cogl_matrix_translate (&cur_bone_mtx, pos.val.vec.x, pos.val.vec.y, pos.val.vec.z);
   * I think cogl has a different idea of what translation means
   * Indeed, I think the position coordinates are in parent space.
   * cogl_matrix_translate operates in current.
   * So the needed operation is add xyz to translation part.
   * But this won't do because directly setting CoglMatrix members = undefined:
   *  cur_bone_mtx.xw = pos.val.vec.x;
   *  cur_bone_mtx.yw = pos.val.vec.y;
   *  cur_bone_mtx.zw = pos.val.vec.z;
   * http://assimp.sourceforge.net/lib_html/structai_node_anim.html
   *  Says order is Scaling, Rotation, Translation
  */
  /**
   * http://sourceforge.net/projects/assimp/forums/forum/817654/topic/3880745
   * Says: now build a transformation matrix from it. First rotation, thenn push position in it as well.
   * ? But is his matrix multiplication order not opposite of cogl's ?
   */
  float tmp_floats[16];
  memcpy (tmp_floats, cogl_matrix_get_array (&cur_bone_mtx), 16 * sizeof (float));
  tmp_floats[12] = pos.val.vec.x;
  tmp_floats[13] = pos.val.vec.y;
  tmp_floats[14] = pos.val.vec.z;
  cogl_matrix_init_from_array (&cur_bone_mtx, tmp_floats);

  *combined_inout = cur_bone_mtx;
}
