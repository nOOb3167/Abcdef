#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_windows.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_primitives.h>

#include <glib.h>
#include <src/error.h>

int
main (int argc, char **argv)
{
  ALLEGRO_DISPLAY *display;

  al_init ();

  display = al_create_display (100, 100);
  g_xassert (display);

  ALLEGRO_BITMAP *bmp;
  ALLEGRO_COLOR clr;
  bmp = al_create_bitmap (10, 10);
  g_xassert (bmp);
  clr = al_map_rgb (255, 0, 0);

  al_set_target_bitmap (bmp);
  al_clear_to_color (clr);

  al_set_target_backbuffer (display);

  al_draw_bitmap (bmp, 20, 20, 0);

  al_flip_display ();

  al_rest (1);

  return EXIT_SUCCESS;
}
