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
#include <src/misc.h>

void
_example_draw_at (int x, int y);

void
_example_draw_at (int x, int y)
{
    cogl_set_source_color4ub ('\xFF', '1', '1', 255);
    cogl_ortho (0, 64, 0, 64, -5, 5);

    CoglMatrix idmtx;
    cogl_matrix_init_identity (&idmtx);
    cogl_set_modelview_matrix (&idmtx);

    cogl_rectangle (x+1, y+1, x-1, y-1);

    cogl_flush ();
}

void
_display_loop (void)
{
  context_switch_allegro ();

  ALLEGRO_BITMAP *bmp;
  bmp = al_create_bitmap (640, 480);

  int blah;
  for (blah=0; blah < 64; ++blah)
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
      CoglColor clear_color;
      cogl_color_set_from_4ub (&clear_color, '0', '0', '0', 255);
      cogl_clear (&clear_color, COGL_BUFFER_BIT_COLOR);
      _example_draw_at (blah, blah);

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

    //g_testtex = cogl_texture_new_from_file ("testtex.bmp", COGL_TEXTURE_NONE, COGL_PIXEL_FORMAT_ANY, NULL);
    //g_xassert (g_testtex != COGL_INVALID_HANDLE);

    _example_draw_at (63, 63);

    _display_loop ();

    return EXIT_SUCCESS;
}
