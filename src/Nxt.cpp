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
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_windows.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_primitives.h>
#include <cogl_include.h>

#include <error.h>
#include <gfx_lib_setup.h>

void
_example_draw (void)
{
    cogl_set_source_color4ub ('\xFF', '1', '1', 255);
    cogl_ortho (0, 64, 0, 64, -1, 1);
    cogl_rectangle (64, 64, 62, 62);
    cogl_flush ();
}

void
_display_loop (void)
{
    struct fbstate_data fbd;
    fbd = fbstate_get_data ();

    context_cogl_allegro ();

    ALLEGRO_BITMAP *bmp;
    bmp = al_create_bitmap (64, 64);
    ALLEGRO_LOCKED_REGION *rgn;
    rgn = al_lock_bitmap (bmp, ALLEGRO_PIXEL_FORMAT_BGR_888, ALLEGRO_LOCK_READWRITE);
    int cnt;
    char *data;
    for (cnt=0,data=(char *)rgn->data; cnt < 64; ++cnt,data+=rgn->pitch)
      {
        memcpy ((void *)data, (void *)&fbd.data[cnt*64*3], 64*3);
      }
    al_unlock_bitmap (bmp);

    al_set_target_backbuffer (fbd.display);
    al_draw_bitmap (bmp, 10, 10, 0);
    al_flip_display ();

    al_rest (2);
}

int
main (int argc, char **argv)
{
    gfx_lib_setup ();
    
    printf ("COGL SETUP COMPLETE\n");

    _example_draw ();

    _display_loop ();

    return EXIT_SUCCESS;
}
