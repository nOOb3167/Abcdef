#include <stdlib.h>
#include <glib.h>
#include <sr.h>

struct NxState *g_state;

int
main (int argc, char **argv)
{
  ALLEGRO_DISPLAY *display;

  g_type_init ();

  /**
   * Initialize g_state
   */
  struct NxState state = {0};
  g_state = g_new0 (struct NxState, 1);
  *g_state = state;

  NxMat z_mat;

  nx_mat_projection (&z_mat, -1.0f);
  //nx_mat_translation (&z_mat, 0.0f, 0.0f, -3.0f);
  //NX_MAT_ELT (&z_mat, 2, 3) = -3.0f;

  /**
   * Experimental findings:
   *   For the identity ortho matrix to match blender's
   *   Z up, Y towards, X right, X has to be inverted, Y has to be inverted.
   *   But why is the translation on positive Z?
   */
  /*
  nx_mat_ortho (&z_mat);
  nx_mat_scale (&z_mat, -1.0f, -1.0f, 1.0f);
  nx_mat_translation (&z_mat, 0.0f, 0.0f, 3.0f);
  */

  //nx_mat_translation (&z_mat, 0.0f, 0.0f, -3.0f);
  g_state->p_mat = z_mat;
  g_state->w_mat = z_mat;

  al_init ();
  al_init_primitives_addon ();

  al_install_keyboard ();
  al_install_mouse ();

  display = al_create_display (100, 100);
  g_xassert (display);

  al_set_target_backbuffer (display);

  MaiModel *model;
  model = mai_model_new_from_file ("c_sr_weight.dae");
  MaiNode *mesh_node;
  mesh_node = g_hash_table_lookup (model->name_node_map, "Cube");
  g_xassert (mesh_node);

  MaiAnimInstance *mai;
  g_xassert (model->anims->len > 0);
  mai = mai_anim_instance_new_from_anim (
                                         g_mai_anim_ptr_array_index (model->anims, 0),
                                         model->name_node_map,
                                         model->nodes);

  sr_weight_dump (model);

  struct SrNodeGraph *sr_model;
  sr_node_graph_from_model (model, &sr_model);

//  struct SrNodeGraph *aux_sr_model;
//  sr_node_graph_copy (&aux_sr_model, sr_model);
//
//  GArray *trans_verts;
//  sr_skeletal_anim (model, mai, mesh_node, aux_sr_model, &trans_verts);

  ALLEGRO_KEYBOARD_STATE aks;

  int frame;
  for (frame=0; frame<60; ++frame)
    {
      al_get_keyboard_state (&aks);
      sr_update_global_ypr (&aks);
      al_clear_to_color (al_map_rgb (0, 0, 0));

      struct SrNodeGraph *aux_sr_model;
      sr_node_graph_copy (&aux_sr_model, sr_model);

      GArray *trans_verts;
      sr_skeletal_anim (model, mai, mesh_node, aux_sr_model, &trans_verts);

      mai->current_frame += mai->current_frame >= 30 ? -30 : 1;

      sr_skeletal_draw_node_trans (&g_state->w_mat, aux_sr_model, mesh_node, trans_verts);

      NxVec4 uvecs[2] = {{0.0f, 0.0f, 0.0f, 1.0f}, {3.0f, 3.0f, 0.0f, 1.0f}};
      NxMat uvmat;
      uvmat = g_state->w_mat;
      nx_mat_translation (&uvmat, 0.0f, 0.0f, -3.0f);
      sr_draw_unit_vec_at (&uvmat, &uvecs[0], &uvecs[1]);

      al_flip_display ();
      al_rest (0.05f);
    }

  return EXIT_SUCCESS;
}
