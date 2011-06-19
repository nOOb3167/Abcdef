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
#include <ai_example.h>
#include <Nxt.h>

void
nxt_draw_array (struct xvtx *verts, int verts_count, unsigned int *indices, int indices_count)
{
  cogl_set_source_color4ub ('\xFF', '\xFF', '1', 255);
  cogl_ortho (0, 64, 0, 64, -1, 1);

  CoglMatrix idmtx;
  cogl_matrix_init_identity (&idmtx);
  cogl_set_modelview_matrix (&idmtx);

  CoglAttributeBuffer *bfr;
  bfr = cogl_attribute_buffer_new (sizeof (*verts) * verts_count, verts);
  g_xassert (bfr);
  CoglAttribute *attr;
  attr = cogl_attribute_new (bfr, "cogl_position_in",
                             sizeof (struct xvtx), offsetof (struct xvtx, x),
                             3, COGL_ATTRIBUTE_TYPE_FLOAT);
  g_xassert (attr);
  CoglIndices *idx;
  idx = cogl_indices_new (COGL_INDICES_TYPE_UNSIGNED_INT, indices, indices_count);
  g_xassert (idx);
  CoglPrimitive *prim;
  prim = cogl_primitive_new (COGL_VERTICES_MODE_TRIANGLES, verts_count, attr, NULL);
  g_xassert (prim);
  cogl_primitive_set_indices (prim, idx);

  cogl_translate (20.0f, 20.0f, 0.0f);
  cogl_scale (5.0f, 5.0f, 1.0f);

  cogl_primitive_draw (prim);
  cogl_flush ();
  cogl_object_unref (idx);
  cogl_object_unref (bfr);
  cogl_object_unref (attr);
  cogl_object_unref (prim);
}

void
_example_draw (void)
{
    cogl_set_source_color4ub ('\xFF', '1', '1', 255);
    cogl_ortho (0, 64, 0, 64, -1, 1);

    CoglMatrix idmtx;
    cogl_matrix_init_identity (&idmtx);
    cogl_set_modelview_matrix (&idmtx);

    const char *shd_source =
        "void main(void) \
        { \
          gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0); \
        }";
    CoglHandle shd;
    shd = cogl_create_shader (COGL_SHADER_TYPE_FRAGMENT);
    g_xassert (shd);
    cogl_shader_source (shd, shd_source);
    cogl_shader_compile (shd);
    g_xassert (cogl_shader_is_compiled (shd));
    CoglHandle shd_prg;
    shd_prg = cogl_create_program ();
    g_xassert (shd_prg);
    cogl_program_attach_shader (shd_prg, shd);
    cogl_program_link (shd_prg);
    cogl_program_use (shd_prg);

    cogl_rectangle (64, 64, 62, 62);

    cogl_program_use (COGL_INVALID_HANDLE);

    cogl_object_unref (shd);
    cogl_object_unref (shd_prg);

    struct xvtx datavec[3] =
        {
            {0.0f, 0.0f, 0.0f},
            {10.0f, 0.0f, 0.0f},
            {10.0f, 10.0f, 0.0f}
        };

    CoglAttributeBuffer *bfr;
    bfr = cogl_attribute_buffer_new (sizeof (datavec), datavec);
    g_xassert (bfr);
    CoglAttribute *attr;
    attr = cogl_attribute_new (bfr, "cogl_position_in",
                               sizeof (struct xvtx), offsetof (struct xvtx, x),
                               3, COGL_ATTRIBUTE_TYPE_FLOAT);
    g_xassert (attr);
    CoglPrimitive *prim;
    prim = cogl_primitive_new (COGL_VERTICES_MODE_TRIANGLES, 3, attr, NULL);
    g_xassert (prim);
    cogl_primitive_draw (prim);

    cogl_flush ();
    cogl_object_unref (bfr);
    cogl_object_unref (attr);
    cogl_object_unref (prim);
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
    for (cnt=0,data=rgn->data; cnt < 64; ++cnt,data+=rgn->pitch)
      {
        memcpy (data, &fbd.data[cnt*64*3], 64*3);
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

    ai_import_file ("cube.obj");

    _display_loop ();

    return EXIT_SUCCESS;
}
