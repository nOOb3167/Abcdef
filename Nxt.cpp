/*
But why am I creating an onscreen?
Also see COGL_DEBUG_DISABLE_ATLAS

cogl-framebuffer.c:_cogl_framebuffer_init_bits
 The HAVE_COGL_GL path throws Invalid Enumeration Value
 The glGetIntegerv path kinda works?

cogl_display_setup -> display_setup -> winsys -> context_create

*/

#include <stdlib.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_windows.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_primitives.h>

#define COGL_ENABLE_EXPERIMENTAL_2_0_API
#include <cogl/cogl.h>

static void
_exit (const char *what)
{
    printf ("ERROR: %s\n", what);
    exit (1);
}

static void
_cogl_setup ()
{    
    /*
    cogl_renderer_new
      Does nothing basically, just allocates
    cogl_renderer_connect
      The vtable: _cogl_winsys_vtable_getters has _cogl_winsys_wgl_get_vtable
      That entry's renderer_connect is called
        Does nothing, allocates CoglRendererWgl (renderer->winsys)
      renderer->winsys_vtable is set
    cogl_swap_chain
      Allocates
    cogl_onscreen_template_new
      Allocates
    cogl_renderer_check_onscreen_template
      Renderer_connect vmethod (aka does nothing)
      Cogl_display_new
        CoglDisplay struct, links renderer, winsys?whichvtableorstruct?TheStruct!WillBeCoglDisplayWglSeeLater, onscreen template
        returns with display->setup=false
      Cogl_display_setup
        display_setup vmethod
          Sets display->winsys to new allocation of CoglDisplayWgl (Holds window_class, HGLRC wgl_context, dummy_hwnd, dummy_dc)
          Create_window_class
            --
          Create_context
            CreateWindowW
            Choose_pixel_format
            SetPixelFormat
            wglCreateContext
            wglMakeCurrent
        As far as I can tell then the display is unreffed   
    Cogl_display_new; Again
    Cogl_display_setup; Again
    Cogl_context_new
      context_init vmethod
        cogl_renderer_add_native_filter (win32_event_filter_cb)
        update_winsys_features (stuff with GL extensions)
      _cogl_pipeline_init_default_pipeline and related
        notice how storing depthtest etc
      Context->framebuffers nulled, so is context->current_pipeline
       But context->framebuffer_stack inited with an COGL_INVALID_HANDLE entry
      WARNING see "if (context->stub_winsys)" note about Clutter
       If winsys was stub_winsys it will create _cogl_onscreen_new and cogl_set_framebuffer
    Cogl_set_default_context
       Meh it's like already done by cogl_context_new
    Cogl_onscreen_new
       _cogl_framebuffer_init
    Cogl_framebuffer_allocate
       Nothing for offscreens, note says about cogl_offscreen_new_to_texture implicit allocation
       onscreen_init vmethod
         There's an if (onscreen->foreign_hwnd)
           WARNING SKIPS creating window but not setting pixel format surely this is retard mode/bug, patched cogl not to setpixelformat
         Else CreateWindowW, Get dummy_dc's pixel format, set the onscreen's pf to it
         WARNING Cogl patched not to do that for Main, use unpatched for Nxt    
    Cogl_set_framebuffer
       _cogl_framebuffer_flush_journal
       _cogl_set_framebuffers_real
          Dirty bound_framebuffer, gl_viewport
          _cogl_matrix_stack_dirty model and projection
          _cogl_clip_stack_dirty
    Notice cogl_flush -> _cogl_journal_flush "if (journal->entries->len == 0) return"
    
    --
    Doubt I really need cogl_onscreen_clutter_backend_set_size (640, 480);
    */

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
    
    onscreen = cogl_onscreen_new (cogl_context, 640, 480);
    framebuffer = COGL_FRAMEBUFFER (onscreen);
    if (!cogl_framebuffer_allocate (framebuffer, NULL))
        _exit ("COGL_FRAMEBUFFER_ALLOCATE");
        
    cogl_set_framebuffer (framebuffer);
    
    float vp[4];
    cogl_get_viewport (vp);
    
    printf ("Viewport %3.0f %3.0f %3.0f %3.0f\n", vp[0], vp[1], vp[2], vp[3]);
    
    cogl_rectangle (50, 50, 200, 200);
    cogl_flush ();
}

int
main (int argc, char **argv)
{
    int tmp;

    _cogl_setup ();
    
    printf ("COGL SETUP COMPLETE\n");

    return EXIT_SUCCESS;
}
