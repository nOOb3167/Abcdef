/*
But why am I creating an onscreen?
*/

#include <stdlib.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_windows.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_primitives.h>

#define COGL_ENABLE_EXPERIMENTAL_2_0_API
#include <cogl/cogl.h>

ALLEGRO_DISPLAY* display;

static void
_exit (const char *what)
{
    printf ("ERROR: %s\n", what);
    exit (1);
}

static void
_cogl_setup ()
{
    CoglRenderer *cogl_renderer;
    CoglDisplay *cogl_display;
    CoglContext *cogl_context;
    CoglSwapChain *swap_chain;
    CoglOnscreenTemplate *onscreen_template;
    
    cogl_renderer = cogl_renderer_new ();
    if (!cogl_renderer_connect (cogl_renderer, NULL))
        _exit ("COGL_RENDERER");
    swap_chain = cogl_swap_chain_new ();
    onscreen_template = cogl_onscreen_template_new (swap_chain);
    cogl_object_unref (swap_chain);
    if (!cogl_renderer_check_onscreen_template (cogl_renderer,
                                                onscreen_template,
                                                NULL))
        _exit ("COGL_CHECK_ONSCREEN_TEMPLATE");
    cogl_display = cogl_display_new (cogl_renderer,
                                     onscreen_template);

    cogl_object_unref (cogl_renderer);
    cogl_object_unref (onscreen_template);

    if (!cogl_display_setup (cogl_display, NULL))
        _exit ("COGL_DISPLAY_SETUP");
    
    cogl_context = cogl_context_new (cogl_display, NULL);
    if (!cogl_context)
        _exit ("COGL_CONTEXT");

    cogl_set_default_context (cogl_context);
    
    CoglOnscreen *onscreen;
    CoglFramebuffer *framebuffer;
    HWND hwnd;
    
    onscreen = cogl_onscreen_new (cogl_context, 640, 480);
    
    hwnd = al_get_win_window_handle (display);
    if (!hwnd)
        _exit ("HWND");
            
    cogl_onscreen_win32_set_foreign_window (onscreen,
                                            hwnd);
    cogl_onscreen_set_swap_throttled (onscreen, 0);
    
    framebuffer = COGL_FRAMEBUFFER (onscreen);
    if (!cogl_framebuffer_allocate (framebuffer, NULL))
        _exit ("COGL_FRAMEBUFFER_ALLOCATE");
    
    cogl_onscreen_clutter_backend_set_size (640, 480);
    
    cogl_set_framebuffer (framebuffer);
    
    float vp[4];
    cogl_get_viewport (vp);
    
    printf ("Viewport %3.0f %3.0f %3.0f %3.0f\n", vp[0], vp[1], vp[2], vp[3]);
}

int
main (int argc, char **argv)
{
    int tmp;

    al_init ();
    al_init_primitives_addon ();

    al_set_new_display_flags(ALLEGRO_OPENGL | ALLEGRO_NOFRAME);
    al_set_new_display_option(ALLEGRO_SINGLE_BUFFER, 0, ALLEGRO_REQUIRE);
    al_set_new_display_option(ALLEGRO_SWAP_METHOD, 2, ALLEGRO_REQUIRE);

    display = al_create_display (640, 480);
    if(!display) { return 1; }

    wglSwapIntervalEXT (0);

    al_set_target_backbuffer(display);

    _cogl_setup ();
    
    printf ("COGL SETUP COMPLETE\n");

    CoglFeatureFlags cff;
    cff = cogl_get_features ();

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
