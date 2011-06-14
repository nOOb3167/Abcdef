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
    cogl_set_source_color4ub ('1', '1', '1', 255);
    cogl_ortho (0, 64, 0, 64, -1, 1);
    cogl_rectangle (64, 64, 62, 62);
    cogl_flush ();
}

int
main (int argc, char **argv)
{
    gfx_lib_setup ();
    
    printf ("COGL SETUP COMPLETE\n");

    _example_draw ();

    return EXIT_SUCCESS;
}
