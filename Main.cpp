/*
_cogl_swap_buffers_notify after SwapBuffers
 ... which does nothing at all

clutter-stage.c:358
about backend windows?
_cogl_check_driver_valid

--

  _cogl_framebuffer_init (COGL_FRAMEBUFFER (onscreen),
                          COGL_FRAMEBUFFER_TYPE_ONSCREEN,
                          COGL_PIXEL_FORMAT_RGBA_8888_PRE,
                          0xdeadbeef, 0xdeadbeef);

_cogl_onscreen_clutter_backend_set_size (width, height);

clutter_stage_win32_realize <- wait is this called _AFTER_ context creation? LOL
 Dont think so, _clutter_backend_create_context called from here

*/

#include <stdlib.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_primitives.h>
#include <cogl/cogl.h>

int main (int argc, char **argv)
{
  int tmp;
  ALLEGRO_DISPLAY* display;
  
  al_init ();
  al_init_primitives_addon ();
  
  al_set_new_display_flags(ALLEGRO_OPENGL | ALLEGRO_NOFRAME);
  al_set_new_display_option(ALLEGRO_SINGLE_BUFFER, 0, ALLEGRO_REQUIRE);
  al_set_new_display_option(ALLEGRO_SWAP_METHOD, 2, ALLEGRO_REQUIRE);

  display = al_create_display (640, 480);
  if(!display) { return 1; }
  
  wglSwapIntervalEXT (0);
  
  al_set_target_backbuffer(display);
  
  GError *error = NULL;
  if (!_cogl_check_driver_valid (&error))
    {
        return EXIT_FAILURE;
    }
  
  CoglFeatureFlags cff;
  cff = cogl_get_features ();
  
  float vp[4];
  cogl_get_viewport (vp);

  ALLEGRO_TIMER* timer;
  timer = al_create_timer (1.0f/1.0f);
  if(timer == NULL) { return 1; }

  ALLEGRO_EVENT_QUEUE* queue;
  ALLEGRO_EVENT ev;
  queue = al_create_event_queue ();
  if(queue == NULL) { return 1; }

  al_register_event_source (queue, al_get_timer_event_source (timer));
  al_start_timer (timer);

  printf("Starting wait.\n");
  while(1)
  {
    al_wait_for_event(queue, &ev);
    printf("Ev res %d\n", ev.type);
    al_set_target_backbuffer(display);
    al_draw_filled_rectangle (50, 50, 100, 100, al_map_rgb (255, 0, 0));
    al_flip_display();
  }

  
  return EXIT_SUCCESS;
}
