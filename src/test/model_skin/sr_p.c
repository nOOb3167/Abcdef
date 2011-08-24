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

  g_state->dr_n = 0.1f;
  g_state->dr_f = 100.0f;
  g_state->vp_w = 100.0f;
  g_state->vp_h = 100.0f;
  g_state->vp_x = 0.0f;
  g_state->vp_y = 0.0f;

  NxMat z_mat;

  nx_mat_projection_ndc (&z_mat, -1.0f, 1.0f, 1.0f, -1.0f, g_state->dr_n, g_state->dr_f);
  nx_mat_translate (&z_mat, 0.0f, 0.0f, -1.0f);

  g_state->p_mat = z_mat;
  g_state->w_mat = z_mat;

  al_init ();
  al_init_primitives_addon ();

  al_install_keyboard ();
  al_install_mouse ();

  display = al_create_display (100, 100);
  g_xassert (display);

  al_set_target_backbuffer (display);

  ALLEGRO_KEYBOARD_STATE aks;

  NxVec4 pvec = {1.0f, 0.0f, 0.0f, 1.0f};
  NxVec4 pvec2 = {-9.9f, 0.0f, 0.0f, 1.0f};
  sr_project_one (&z_mat, &pvec);
  sr_project_one (&z_mat, &pvec2);

  int frame;
  for (frame=0; frame<60; ++frame)
    {
      al_get_keyboard_state (&aks);
      sr_update_global_ypr (&aks);
      al_clear_to_color (al_map_rgb (0, 0, 0));

      NxVec4 uvecs[2] = {{0.0f, 0.0f, 0.0f, 1.0f}, {3.0f, 3.0f, 0.0f, 1.0f}};
      NxMat uvmat;
      uvmat = g_state->w_mat;
      //nx_mat_translation (&uvmat, 0.0f, 0.0f, -3.0f);
      sr_draw_unit_vec_at (&uvmat, &uvecs[0], &uvecs[1]);

      al_flip_display ();
      al_rest (0.05f);
    }

  return EXIT_SUCCESS;
}
