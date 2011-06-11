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

 GObject
 twophase
   init, construct
   but construct vmethod invoked first,
    meant to chain to parent, basebasebase g_object_constructor then calls init,
    init returns then the constructs unchain back
   Aka if you want to use the builtin construct-param system:
    override construct vmethod
     construct
      1 chain parent
      2 essentially is a ctor with params. use the params to init self (at this point init will be invoked already)
     init
      1 parameterless ctor

Clutter
clutter_init
 _clutter_context_get_default: context creation
  ClutterCntx with backend g_object_new, backend be singleton much
   backend has the create_context and create_stage vmethods
 init_real
  _clutter_feature_init, Claims initializes cogl state!
   _clutter_backend_create_context, calls create_context vmethod!
    init_events vmethod, creates device_manager _clutter_backend_win32_events_init <- they do nothing relevant?
clutter_stage_new
 clutter_stage_init
  _clutter_backend_create_stage, create_stage backend vmethod
   creates CLUTTER_TYPE_STAGE_MANAGER
   create_stage backend vmethod
    creates CLUTTER_TYPE_STAGE_WIN32
     class_init/init do nothing but notice this thing has the HWND
  cogl_matrix calls and stuff
  clutter_stage_set_viewport: is called but didn't really investigate
   queue_full_redraw
    clutter_actor_queue_redraw for self (stage)
     _clutter_actor_queue_redraw_full actually seems important?
     hints at _clutter_stage_do_update

Also see crate.c queue redraw

--clutter_actor_show ()
--clutter_main ()

search for where clutter_stage_win32_realize / ->realize gets called from
it is where cogl_onscreen_new happens for example and CreateWindowW
see
 clutter_stage_realize
  _clutter_stage_window_realize <- stage's window iface, which has clutter_stage_win32_realize ?
   yes, from clutter-stage-win32.c: clutter_stage_window_iface_init
  _clutter_backend_ensure_context
see where _clutter_stage_window_realize got called from, that's calling the STAGE_WINDOW_GET_IFACE's vmethod
is called from clutter_stage_realize
 (this one is also what does _clutter_backend_ensure_context_internal)
  ..Ensure_context_internal is important, duplicate it,
  it calls the backend ensure_context
maybe clutter_actor_realize does the clutter_stage_realize thing (YES stage_class_init actor_class->realize = clutter_stage_realize)
Clutter_stage_get_default calls clutter_actor_realize for example
 clutter_stage_get_default seems important, dox claims gets called from clutter_init
Please observe get default stage / default stage creation!!
 Stage subclass of Actor so that clutter_actor_realize call will call clutter_stage_realize
 clutter_stage_realize -> _clutter_stage_window_realize -> vmethod clutter_stage_win32_realize

_cogl_winsys_onscreen_init from cogl_framebuffer_allocate trying to SetPixelFormat which fails
 let it create a window for us instead
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
    
    // The realize part now
    CoglOnscreen *onscreen;
    onscreen = cogl_onscreen_new (cogl_context, 640, 480);
    HWND hwnd;
    hwnd = al_get_win_window_handle (display);
    if (!hwnd)
        _exit ("HWND");
        
    HDC hax_hdc;
    hax_hdc = GetWindowDC (hwnd);
    if (!hax_hdc)
        _exit ("HAX_HDC");
    
    cogl_onscreen_win32_set_foreign_window (onscreen,
                                            hwnd);
    cogl_onscreen_set_swap_throttled (onscreen, 0);
    
    CoglFramebuffer *framebuffer;
    framebuffer = COGL_FRAMEBUFFER (onscreen);
    if (!cogl_framebuffer_allocate (framebuffer, NULL))
        _exit ("COGL_FRAMEBUFFER_ALLOCATE");
    
    // - Backend create context
     // But this does nothing, already here..
    // - End of Backend create context
    // End of realize part

    //Hmm if we didn't create window and setup GL context
    //_cogl_winsys_display_setup shoulda gotten called
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
