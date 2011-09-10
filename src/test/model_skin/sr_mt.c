#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <src/gfx_lib_setup.h>
#include <src/mai-info-win.h>
#include <src/test/m-tfmsg.h>
#include <sr.h>

struct TfSharedData
{
  GAsyncQueue *qu;
};

struct TfSharedData *tf_shared_data_new ()
{
  struct TfSharedData *ret;
  ret = g_malloc0 (sizeof (*ret));
  ret->qu = g_async_queue_new ();
  return ret;
}

void
tf_allegro_display_transfer (ALLEGRO_DISPLAY *disp, MTfMsg *mm)
{
  static ALLEGRO_BITMAP *bmp;

  if (bmp == NULL)
      bmp = al_create_bitmap (640, 480);

  {
      ALLEGRO_LOCKED_REGION *rgn;
      rgn = al_lock_bitmap (bmp,
                            ALLEGRO_PIXEL_FORMAT_BGR_888, ALLEGRO_LOCK_WRITEONLY);
      char *rdata;
      rdata = rgn->data;
      for (gint i = 0; i < 480; ++i, rdata += rgn->pitch)
        {
          memcpy (rdata, &mm->tex_data[i*640*3], 640*3);
        }
      al_unlock_bitmap (bmp);
  }

  al_set_target_backbuffer (disp);
  al_draw_bitmap (bmp, 0, 0, 0);
  al_flip_display ();
}

gpointer
tf_init_allegro (gpointer data)
{
  struct TfSharedData *sha;
  ALLEGRO_DISPLAY *disp;

  sha = data;

  g_async_queue_push (sha->qu, "HELLO");

  _allegro_setup (&disp);

  MTfMsg *mm;
  mm = M_TFMSG (g_async_queue_pop (sha->qu));

  tf_allegro_display_transfer (disp, mm);

  g_object_unref (mm);

  return NULL;
}

void
tf_cogl_texture_get_data (CoglHandle texture, gchar **data_out, gint *size_out)
{
  int siz;
  gchar *data;

  siz = cogl_texture_get_data (texture, COGL_PIXEL_FORMAT_RGB_888, 0, NULL);
  g_xassert (siz);

  data = g_malloc (sizeof (*data) * siz);

  siz = cogl_texture_get_data (texture, COGL_PIXEL_FORMAT_RGB_888, 0, (guint8 *)data);
  g_xassert (siz);

  *data_out = data;
  *size_out = siz;
}

gpointer
tf_init_cogl (gpointer data)
{
  struct TfSharedData *sha;
  int width, height;
  CoglHandle ofs, tx;

  sha = data;

  char * msg;
  msg = g_async_queue_pop (sha->qu);
  printf ("%s\n", msg);

  width = 640;
  height = 480;

  _cogl_setup (width, height, &ofs, &tx);

  CoglColor clear_color;
  cogl_color_set_from_4ub (&clear_color, '\x80', '\x0', '\x0', 255);
  cogl_clear (&clear_color, COGL_BUFFER_BIT_COLOR | COGL_BUFFER_BIT_DEPTH);

  gchar *tex_data;
  gint tex_siz;
  tf_cogl_texture_get_data (tx, &tex_data, &tex_siz);

  MTfMsg *mm;
  mm = M_TFMSG (m_tfmsg_new (tex_data, tex_siz, width, height));

  g_async_queue_push (sha->qu, mm);

  return NULL;
}

enum SrMtThreadEnum
{
  SR_MT_THREAD_ALLEGRO = 0,
  SR_MT_THREAD_COGL,
  SR_MT_NUM_THREADS
};

int
main (int argc, char **argv)
{
  g_thread_init (NULL);
  g_xassert (g_thread_supported ());

  g_type_init ();

  gtk_init( &argc, &argv );

  struct TfSharedData *sha;
  sha = tf_shared_data_new ();

  GThread *gfx_threads[SR_MT_NUM_THREADS];
  gfx_threads[SR_MT_THREAD_ALLEGRO] = g_thread_create (tf_init_allegro, sha, TRUE, NULL);
  gfx_threads[SR_MT_THREAD_COGL] = g_thread_create (tf_init_cogl, sha, TRUE, NULL);

  for (int i = 0; i < SR_MT_NUM_THREADS; ++ i)
    {
      g_thread_join (gfx_threads[i]);
    }

  MaiInfoWin *iw;
  iw = MAI_INFO_WIN (mai_info_win_new ());

  mai_info_win_clear_model (iw);
  mai_info_win_show (iw);

  int frame;
  for (frame=0; frame<600; ++frame)
    {
      mai_info_win_iteration (iw, FALSE);

      al_rest (0.05f);
    }

  return EXIT_SUCCESS;
}
