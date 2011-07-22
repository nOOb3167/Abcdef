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
#include <Nxt.h>

void
_example_draw (void);

void
_example_draw (void)
{
    cogl_set_source_color4ub ('\xFF', '1', '1', 255);
    cogl_ortho (0, 64, 0, 64, -5, 5);

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
  bmp = al_create_bitmap (640, 480);

  int blah;
  for (blah=0; blah < 15; ++blah)
    {
      ALLEGRO_LOCKED_REGION *rgn;
      rgn = al_lock_bitmap (bmp, ALLEGRO_PIXEL_FORMAT_BGR_888, ALLEGRO_LOCK_READWRITE);
      int cnt;
      char *data;
      for (cnt=0,data=rgn->data; cnt < 480; ++cnt,data+=rgn->pitch)
        {
          memcpy (data, &fbd.data[cnt*640*3], 640*3);
        }
      al_unlock_bitmap (bmp);

      //context_switch_cogl ();
      //derp
      //context_switch_allegro ();

      al_set_target_backbuffer (fbd.display);
      al_draw_bitmap (bmp, 0, 0, 0);
      al_flip_display ();

      al_rest (0.1f);
    }

  al_rest (2);
}

int
main (int argc, char **argv)
{
    g_type_init ();

    gfx_lib_setup ();

    printf ("COGL SETUP COMPLETE\n");

    //g_testtex = cogl_texture_new_from_file ("testtex.bmp", COGL_TEXTURE_NONE, COGL_PIXEL_FORMAT_ANY, NULL);
    //g_xassert (g_testtex != COGL_INVALID_HANDLE);

    _example_draw ();

    _display_loop ();

    return EXIT_SUCCESS;
}
