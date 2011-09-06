#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <src/gfx_lib_setup.h>
#include <src/info_win.h>
#include <sr.h>

struct NxState *g_state;

CoglHandle *g_testtex;

int
main (int argc, char **argv)
{
  gtk_init( &argc, &argv );

  g_type_init ();

  iw_info_win ();

  gfx_lib_setup ();

  g_testtex = cogl_texture_new_from_file ("testtex_sr.bmp", COGL_TEXTURE_NONE, COGL_PIXEL_FORMAT_ANY, NULL);
  g_xassert (g_testtex != COGL_INVALID_HANDLE);

  //cogl_ortho (-1.0f, 1.0f, -1.0f, 1.0f, -5.0f, 100.0f);
  cogl_perspective (45.0f, 1.0f, 0.1f, 100.0f);
  cogl_set_source_texture (g_testtex);

  /**
   * WHY IS THIS NOT ENABLED BY DEFAULT
   */
  cogl_set_depth_test_enabled (TRUE);

  /**
   * Enabling this shows polygons actually missing not just culled out?
   * cogl_set_backface_culling_enabled (TRUE);
   */

  /**
   * Initialize g_state
   */
  struct NxState state = {0};
  g_state = g_new0 (struct NxState, 1);
  *g_state = state;

  g_state->dr_n = 1.0f;
  g_state->dr_f = 100.0f;
  g_state->vp_w = 100.0f;
  g_state->vp_h = 100.0f;
  g_state->vp_x = 0.0f;
  g_state->vp_y = 0.0f;

  NxMat z_mat;
  nx_mat_init_identity (&z_mat);
  nx_mat_translate (&z_mat, 0.0f, 0.0f, -4.0f);

  g_state->p_mat = z_mat;
  g_state->w_mat = z_mat;

  MaiModel *model;
  //model = mai_model_new_from_file ("c_sr_weight.dae");
  model = mai_model_new_from_file ("../misc/mtest1.dae");
  MaiNode *mesh_node;
  mesh_node = g_hash_table_lookup (model->name_node_map, "Cube");
  g_xassert (mesh_node);

  MaiAnimInstance *mai;
  g_xassert (model->anims->len > 0);
  mai = mai_anim_instance_new_from_anim (
                                         g_mai_anim_ptr_array_index (model->anims, 0),
                                         model->name_node_map,
                                         model->nodes);

  struct SrNodeGraph *sr_model;
  sr_node_graph_from_model (model, &sr_model);

  sr_weight_dump (model);

  int frame;
  for (frame=0; frame<600; ++frame)
    {
      ALLEGRO_KEYBOARD_STATE aks;

      gfx_display_clear ();

      context_switch_allegro ();
      al_get_keyboard_state (&aks);
      sr_update_global_ypr (&aks);
      al_clear_to_color (al_map_rgb (0, 0, 0));

      struct SrNodeGraph *aux_sr_model;
      sr_node_graph_copy (&aux_sr_model, sr_model);

      GArray *trans_verts;
      sr_skeletal_anim (model, mai, mesh_node, aux_sr_model, &trans_verts);

      mai->current_frame += mai->current_frame == 29 ? -29 : 1;

      /**
       * Allegro drawing origin top left.
       * Compensate (Want positive x/y go northwest).
       */
      NxMat comp;
      comp = g_state->w_mat;
      nx_mat_scale (&comp, -1.0f, 1.0f, -1.0f);

//      sr_allegro_skeletal_draw_node_trans (&comp, aux_sr_model, mesh_node, trans_verts);
      sr_skeletal_draw_node_trans (&comp, aux_sr_model,
                                   mesh_node, trans_verts);

//      sr_node_graph_draw (&comp, aux_sr_model);

      NxVec4 uvecs[2] = {{0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f}};
      NxMat uvmat;
      uvmat = g_state->w_mat;
//      sr_draw_unit_vec_at (&uvmat, &uvecs[0], &uvecs[1]);

      gfx_display_transfer ();

      al_rest (0.05f);
    }

  return EXIT_SUCCESS;
}
