#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_opengl.h>
#include <glib.h>
#include <cogl_include.h>

#include <error.h>
#include <gfx_lib_setup.h>


static void
_allegro_setup (ALLEGRO_DISPLAY **disp);

static void
_cogl_setup (int width, int height, CoglHandle *ofs, CoglHandle *tx);

static struct context_holder *
_context_holder_new_WHILE_COGL (void);

static void
_context_holder_get_win (HDC *hdc, HGLRC *hglrc);

static void
_context_holder_display_set_WHILE_ALLEGRO (struct context_holder *what, ALLEGRO_DISPLAY *display);

static void
_context_cogl_allegro (struct context_holder *what);

void
_context_allegro_cogl (struct context_holder *what);

void
_context_switch_allegro (struct context_holder *what);

void
_context_switch_cogl (struct context_holder *what);

struct context_fbstate *
_context_fbstate_new (int width, int height, CoglHandle ofs, CoglHandle tx, ALLEGRO_DISPLAY *display);

void
_cogl_setup_basic (void);

void
_create_offscreen_framebuffer (int width, int height, CoglHandle *ofs, CoglHandle *tx);


static struct context_holder *_g_context_holder;
static struct context_fbstate *_g_fbstate;

void
context_cogl_allegro (void)
{
    _context_cogl_allegro (_g_context_holder);
}

void
context_allegro_cogl (void)
{
    _context_allegro_cogl (_g_context_holder);
}

void
context_switch_allegro (void)
{
  _context_switch_allegro (_g_context_holder);
}

void
context_switch_cogl (void)
{
  _context_switch_cogl (_g_context_holder);
}

/**
 * Warning Cogl MUST be current at this point
 */
struct context_holder *
_context_holder_new_WHILE_COGL (void)
{
    struct context_holder *ret;
    ret = g_new (struct context_holder, 1);
    ret->state = 0;
    ret->display = 0;
    _context_holder_get_win (&ret->hdc, &ret->hglrc);
    return ret;
}

void
_context_holder_display_set_WHILE_ALLEGRO (struct context_holder *what, ALLEGRO_DISPLAY *display)
{
    if (what->state != 0)
        xexit ("CONTEXT_HOLDER_DISPLAY_SET BUT STATE NOT 0");
    what->display = display;
    what->state = 1;
}

void
_context_cogl_allegro (struct context_holder *what)
{
    if (what->state != 2)
        xexit ("CONTEXT_COGL_ALLEGRO BUT STATE NOT 2");

    HDC current_dc;
    HGLRC current_hglrc;
    _context_holder_get_win (&current_dc, &current_hglrc);
    what->hdc = current_dc;
    what->hglrc = current_hglrc;

    al_set_current_opengl_context (what->display);

    what->state = 1;
}

void
_context_allegro_cogl (struct context_holder *what)
{
    if (what->state != 1)
        xexit ("CONTEXT_ALLEGRO_COGL BUT STATE NOT 1");

    BOOL ret;
    ret = wglMakeCurrent (what->hdc, what->hglrc);
    if (!ret)
        xexit ("MAKE_CURRENT");

    what->state = 2;
}

void
_context_switch_allegro (struct context_holder *what)
{
  g_xassert (what->state != 0);
  if (what->state == 1)
      return;

  HDC current_dc;
  HGLRC current_hglrc;
  _context_holder_get_win (&current_dc, &current_hglrc);
  what->hdc = current_dc;
  what->hglrc = current_hglrc;

  al_set_current_opengl_context (what->display);

  what->state = 1;
}

void
_context_switch_cogl (struct context_holder *what)
{
  g_xassert (what->state != 0);
  if (what->state == 2)
    return;

  BOOL ret;
  ret = wglMakeCurrent (what->hdc, what->hglrc);
  g_xassert (ret);

  what->state = 2;
}

void
_context_holder_get_win (HDC *hdc, HGLRC *hglrc)
{
    *hglrc = wglGetCurrentContext ();
    if (!(*hglrc))
        xexit ("CURRENT_HGLRC");
    *hdc = wglGetCurrentDC ();
    if (!(*hdc))
        xexit ("CURRENT_DC");
}

struct context_fbstate *
_context_fbstate_new (int width, int height, CoglHandle ofs, CoglHandle tx, ALLEGRO_DISPLAY *display)
{
    struct context_fbstate *fbs;
    fbs = g_new (struct context_fbstate, 1);
    fbs->display = display;
    fbs->offscreen = ofs;
    fbs->texture = tx;
    fbs->width = width;
    fbs->height = height;
    return fbs;
}

void
gfx_display_transfer (void)
{
  ALLEGRO_BITMAP *bmp;
  struct fbstate_data *fbd;

  context_switch_allegro ();
  bmp = al_create_bitmap (640, 480);

  context_switch_cogl ();
  fbd = fbstate_get_data ();

  context_switch_allegro ();
  ALLEGRO_LOCKED_REGION *rgn;
  rgn = al_lock_bitmap (bmp, ALLEGRO_PIXEL_FORMAT_BGR_888, ALLEGRO_LOCK_READWRITE);
  int cnt;
  char *data;
  for (cnt=0,data=rgn->data; cnt < 480; ++cnt,data+=rgn->pitch)
    {
      memcpy (data, &fbd->data[cnt*640*3], 640*3);
    }
  al_unlock_bitmap (bmp);

  al_set_target_backbuffer (fbd->display);
  al_draw_bitmap (bmp, 0, 0, 0);
  al_flip_display ();

  al_destroy_bitmap (bmp);
  fbstate_free (fbd);

  context_switch_cogl ();
}

void
gfx_display_clear (void)
{
  context_switch_cogl ();
  CoglColor clear_color;
  cogl_color_set_from_4ub (&clear_color, '\x0', '\x0', '\x0', 255);
  cogl_clear (&clear_color, COGL_BUFFER_BIT_COLOR);
}

/**
 * Magic setup routine.
 * Leaves context in Cogl.
 * Sets global context holder, fbstate.
 */
void
gfx_lib_setup ()
{
    struct context_holder *ch;
    struct context_fbstate *fbs;
    ALLEGRO_DISPLAY *disp;
    CoglHandle ofs, tx;

    int width = 640;
    int height = 480;

    /**
     * Yes this is screwed up.
     * Just treat as magic initialization sequence etc.
     */

    _cogl_setup (width, height, &ofs, &tx);

    ch = _context_holder_new_WHILE_COGL ();

    _allegro_setup (&disp);

    _context_holder_display_set_WHILE_ALLEGRO (ch, disp);

    fbs = _context_fbstate_new (width, height, ofs, tx, disp);

    _g_context_holder = ch;
    _g_fbstate = fbs;

    _context_allegro_cogl (ch);
}

/**
 * Just creates a new display for now.
 */
void
_allegro_setup (ALLEGRO_DISPLAY **disp)
{
  ALLEGRO_DISPLAY *d;

  g_assert (disp);

  al_init ();

  al_init_primitives_addon ();

  al_install_keyboard ();
  al_install_mouse ();

  al_set_new_bitmap_format (ALLEGRO_PIXEL_FORMAT_BGR_888);
  al_set_new_display_flags (ALLEGRO_OPENGL | ALLEGRO_NOFRAME);
  al_set_new_display_option (ALLEGRO_SINGLE_BUFFER, 0, ALLEGRO_REQUIRE);
  al_set_new_display_option (ALLEGRO_SWAP_METHOD, 2, ALLEGRO_REQUIRE);
  d = al_create_display (640, 480);
  g_xassert (d);

  *disp = d;
}

void
_cogl_setup (int width, int height, CoglHandle *ofs, CoglHandle *tx)
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

    On Allegro side:
      al_init should be OK
      al_create_display
        al_use_transform calls GL
        al_clear_to_color calls GL but harmless? glClearColor / glClear
        flip_display calls GL but harmless? SwapBuffers / glFlush

    al_set_current_display API will wglMakeCurrent
    Let allegro create window and GL context, pass to cogl
      Can not do that through cogl_onscreen_w32_set_foreign_window (too late) by then,
      cogls display creation with wglCreateContext, SetPixelFormat etc will retry creating context
    Watch out for onscreen_bind it wglMakeCurrents?

    Cogl: Constraints on the dummy pixel format: create_context -> choose_pixel_format and pixel_format_is_better
    Create_context ensures dummy but it DOES set the display->wgl_context
      It is the only context creation point? YES
    Dummy only called dummy, it _is_ the real thing? -> See below on dummy_dc relating to client_dc
    --Cogl_context_new also does cogl_display_setup so that's a third time?
      And second time with same display (The very first cogl_display_setup is on a temp display that gets killed off),
      however wgl_context is not recreated and dummy_hwnd/dummy_dc not recreated (guards)

    Rig the nontemp display's hwnd and dc (hwnd only is enough?)
      Problem might be that set_foreign_window is for onscreen not display :w, implement a display_set_dummy_hwnd_and_dc?
    Patch cogl_framebuffer_allocate not to screw with SetPixelFormat

    Check how dummy_dc relates to client_dc (CoglOnscreenWgl) and dummy_hwnd to hwnd (CoglOnscreenWin32)
    The two interact in onscreen_init vmethod
      onscreen_init will get 'hwnd' var either from foreign either will create, then ->hwnd and ->client_dc set from 'hwnd' var
      Dummy does affect real, client_dc will get SetPixelFormatted same format as dummy_dc

    Remember this already
      Cogl_framebuffer_allocate does not have anything to do with FBOs

    Cogl_framebuffer_allocate's onscreen_init call should not SetPixelFormat
    Onscreen_init ONLY called from framebuffer_allocate

    Plan is prevent 2nd (1st is temp, 3rd is guarded)
    GL Context creation at dummy time, then never again?
    There is only one wglCreateContext point and it's for wgl_display->dummy_dc
    Ok callgraph the bastard
      cogl_renderer_check_onscreen_template (the temp)
      cogl_display_setup
      cogl_context_new
        _cogl_winsys_display_setup
          create_context
            wglCreateContext

      cogl_framebuffer_allocate
        _cogl_winsys_onscreen_init
          SetPixelFormat

      cogl_renderer_check_onscreen_template (the temp)
      cogl_display_setup
      cogl_context_new
        _cogl_winsys_display_setup
          create_context
            SetPixelFormat

    I can't see why foreign_hwnd + Inhibit SetPixelFormat would not work
    Then just cogl_framebuffer_swap_buffers

    */

    g_xassert (ofs);
    g_xassert (tx);

    _cogl_setup_basic ();

    CoglHandle ofs_loc, tx_loc;
    _create_offscreen_framebuffer (width, height, &ofs_loc, &tx_loc);

    /**
     * Framebuffer clearing.
     */
    cogl_set_framebuffer (COGL_FRAMEBUFFER (ofs_loc));

    CoglColor clear_color;
    cogl_color_set_from_4ub (&clear_color, '0', '0', '0', 255);
    cogl_clear (&clear_color, COGL_BUFFER_BIT_COLOR);

    *ofs = ofs_loc;
    *tx = tx_loc;
}

void
_cogl_setup_basic (void)
{
    CoglRenderer *cogl_renderer;
    CoglDisplay *cogl_display;
    CoglContext *cogl_context;
    CoglSwapChain *swap_chain;
    CoglOnscreenTemplate *onscreen_template;

    cogl_renderer = cogl_renderer_new ();
    if (!cogl_renderer_connect (cogl_renderer, NULL))
        xexit ("COGL_RENDERER");
    swap_chain = cogl_swap_chain_new ();
    onscreen_template = cogl_onscreen_template_new (swap_chain);
    cogl_object_unref (swap_chain);
    if (!cogl_renderer_check_onscreen_template (cogl_renderer,
                                                onscreen_template,
                                                NULL))
        xexit ("COGL_CHECK_ONSCREEN_TEMPLATE");
    cogl_display = cogl_display_new (cogl_renderer,
                                     onscreen_template);

    cogl_object_unref (cogl_renderer);
    cogl_object_unref (onscreen_template);

    if (!cogl_display_setup (cogl_display, NULL))
        xexit ("COGL_DISPLAY_SETUP");

    cogl_context = cogl_context_new (cogl_display, NULL);
    if (!cogl_context)
        xexit ("COGL_CONTEXT");

    cogl_set_default_context (cogl_context);

    /*
     * Make current framebuffer be some kind of defult onscreen.
     * Should create an offscreen FBO then set _that_ current.
     */
    CoglOnscreen *onscreen;
    CoglFramebuffer *framebuffer;

    onscreen = cogl_onscreen_new (cogl_context, 640, 480);
    framebuffer = COGL_FRAMEBUFFER (onscreen);
    if (!cogl_framebuffer_allocate (framebuffer, NULL))
        xexit ("COGL_FRAMEBUFFER_ALLOCATE");

    cogl_set_framebuffer (framebuffer);
}

/**
 * Creates offscreen texture and buffer.
 * Setting globals or whatever left to caller.
 */
void
_create_offscreen_framebuffer (int width, int height, CoglHandle *ofs, CoglHandle *tx)
{
    CoglHandle tex64;
    CoglHandle offscreen_tex64;

    g_xassert (ofs);
    g_xassert (tx);

    tex64 = cogl_texture_new_with_size (width, height,
            (CoglTextureFlags)(COGL_TEXTURE_NO_AUTO_MIPMAP | COGL_TEXTURE_NO_SLICING | COGL_TEXTURE_NO_ATLAS),
            COGL_PIXEL_FORMAT_RGB_888);
    g_xassert (tex64);

    offscreen_tex64 = cogl_offscreen_new_to_texture (tex64);
    g_xassert (offscreen_tex64);

    *ofs = offscreen_tex64;
    *tx = tex64;
}

struct fbstate_data *
fbstate_get_data (void)
{
    struct fbstate_data *fbd;
    struct context_fbstate *fbs;

    fbd = g_malloc (sizeof (*fbd));

    g_xassert (_g_fbstate);
    fbs = _g_fbstate;

    int siz;
    char *data;
    siz = cogl_texture_get_data (fbs->texture, COGL_PIXEL_FORMAT_RGB_888, 0, NULL);
    //printf ("SIZ %d\n", siz);
    g_xassert (siz);

    data = g_malloc (sizeof (*data) * siz);
    siz = cogl_texture_get_data (fbs->texture, COGL_PIXEL_FORMAT_RGB_888, 0, (guint8 *)data);
    g_xassert (siz);
    //printf ("Data %p\nValid %d\n", data, siz);

    fbd->display = fbs->display;
    fbd->data = data;
    fbd->size = siz;
    fbd->width = fbs->width;
    fbd->height = fbs->height;

    return fbd;
}

void
fbstate_free (struct fbstate_data *fbd)
{
  g_free (fbd->data);
  g_free (fbd);
}
