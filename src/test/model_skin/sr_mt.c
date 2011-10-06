#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <src/gfx_lib_setup.h>
#include <src/mai-info-win.h>
#include <src/test/m-tfmsg.h>
#include <src/test/m-tfmsg-fbup.h>
#include <src/test/m-tfthread.h>
#include <src/test/m-tfthreadal.h>
#include <src/test/m-tfthreadallegro.h>
#include <src/test/m-tfthreadcogl.h>
#include <src/test/tf-gfx-threads.h>
#include <sr.h>

struct TfSharedData
{
  /**
   * Allegro pushes NULLs into qu_r, to trigger
   * cogl pop from qu.
   */
  GAsyncQueue *qu_r;
  GAsyncQueue *qu;
};

struct TfSharedData *
tf_shared_data_new ()
{
  struct TfSharedData *ret;
  ret = g_malloc0 (sizeof (*ret));
  ret->qu_r = g_async_queue_new ();
  ret->qu = g_async_queue_new ();
  return ret;
}

void
tf_allegro_display_transfer (ALLEGRO_DISPLAY *disp, MTfMsgFbUp *mm)
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
tf_init_allegro_x (gpointer data)
{
  struct TfSharedData *sha;
  ALLEGRO_DISPLAY *disp;
  ALLEGRO_EVENT_QUEUE *queue;
  ALLEGRO_EVENT_SOURCE *timer_source;
  ALLEGRO_TIMER *timer;

  sha = data;

  _allegro_setup (&disp);

  queue = al_create_event_queue ();
  g_xassert (queue);

  timer = al_create_timer (0.05f);
  g_xassert (timer);

  timer_source = al_get_timer_event_source (timer);
  g_xassert (timer_source);

  al_register_event_source (queue, timer_source);

  al_start_timer (timer);

  while (TRUE)
    {
      g_async_queue_push (sha->qu_r, "PRETEND_THIS_IS_NULL");

      MTfMsg *msg;
      while ((msg = g_async_queue_try_pop (sha->qu)))
        {
          g_xassert (M_IS_TFMSG (msg) && msg->msg_type != TF_MSGTYPE_INVALID);

          switch (msg->msg_type)
            {
          case TF_MSGTYPE_FBUP:
              tf_allegro_display_transfer (disp, M_TFMSG_FBUP (msg));
              break;

          default:
              g_xassert (("EEH MAJI, UNHANDLED MESSAGE TYPE", FALSE));
              break;
            }

          g_object_unref (msg);
        }

      al_rest (0.05f);
    }

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
tf_init_cogl_x (gpointer data)
{
  struct TfSharedData *sha;
  int width, height;
  CoglHandle ofs, tx;

  sha = data;

  width = 640;
  height = 480;

  _cogl_setup (width, height, &ofs, &tx);

  for (gint i = 0; i < 600; ++i)
    {
      g_async_queue_pop (sha->qu_r);

      CoglColor clear_color;
      cogl_color_set_from_4ub (&clear_color, '\x80', '\x0', '\x0', 255);
      cogl_clear (&clear_color, COGL_BUFFER_BIT_COLOR | COGL_BUFFER_BIT_DEPTH);

      gchar *tex_data;
      gint tex_siz;
      tf_cogl_texture_get_data (tx, &tex_data, &tex_siz);

      MTfMsgFbUp *mm;
      mm = M_TFMSG_FBUP (m_tfmsg_fbup_new (tex_data, tex_siz, width, height));

      g_async_queue_push (sha->qu, mm);
    }

  return NULL;
}

gpointer
tf_init_allegro_timer (gpointer data)
{
  MTfThreadAl *mtta;
  mtta = M_TFTHREADAL (data);

  m_tfthreadal_start_timer (mtta);
  m_tfthreadal_event_loop_enter (mtta);

  return NULL;
}

gpointer
tf_init_cogl (gpointer data)
{
  MTfThreadCogl *mtta;
  mtta = M_TFTHREADCOGL (data);

  m_tfthreadcogl_init_inthread (mtta);
  m_tfthreadcogl_event_loop_enter (mtta);

  return NULL;
}

gpointer
tf_init_allegro (gpointer data)
{
  MTfThreadAllegro *mtta;
  mtta = M_TFTHREADALLEGRO (data);

  m_tfthreadallegro_init_inthread (mtta);
  m_tfthreadallegro_event_loop_enter (mtta);

  return NULL;
}

int
main (int argc, char **argv)
{
  g_thread_init (NULL);
  g_xassert (g_thread_supported ());

  g_type_init ();

  gtk_init( &argc, &argv );

  al_init ();

  struct TfSharedData *sha;
  sha = tf_shared_data_new ();

  MTfThreadAl *mtta;
  mtta = M_TFTHREADAL (m_tfthreadal_new ());

  MTfThreadCogl *mttc;
  mttc = M_TFTHREADCOGL (m_tfthreadcogl_new ());

  MTfThreadAllegro *mttallegro;
  mttallegro = M_TFTHREADALLEGRO (m_tfthreadallegro_new ());

  GThread *gfx_threads[TF_NUM_THREADS];
//  gfx_threads[TF_THREAD_ALLEGRO] = g_thread_create (tf_init_allegro_x, sha, TRUE, NULL);
//  gfx_threads[TF_THREAD_ALLEGRO_TIMER] = g_thread_create (tf_init_allegro_timer,
//                                                             mtta,
//                                                             TRUE,
//                                                             NULL);
//  gfx_threads[TF_THREAD_COGL] = g_thread_create (tf_init_cogl_x, sha, TRUE, NULL);

  TfGfxThreads *tgt;
  tgt = tf_gfx_threads_get_instance ();
  tf_gfx_threads_add (tgt,
                      TF_THREAD_ALLEGRO_TIMER,
                      tf_init_allegro_timer,
                      M_TFTHREAD (mtta));
  tf_gfx_threads_add (tgt,
                      TF_THREAD_COGL,
                      tf_init_cogl,
                      M_TFTHREAD (mttc));
  tf_gfx_threads_add (tgt,
                      TF_THREAD_ALLEGRO,
                      tf_init_allegro,
                      M_TFTHREAD (mttallegro));

//  tf_gfx_threads_add (tgt,
//                      TF_THREAD_ALLEGRO_TIMER,
//                      g_thread_create (tf_init_allegro_timer,
//                                       mtta,
//                                       TRUE,
//                                       NULL),
//                      M_TFTHREAD (mtta));

  tf_gfx_threads_start (tgt);

  MTfThread *tm = tf_gfx_threads_get_data (tgt, TF_THREAD_ALLEGRO_TIMER);
  MTfThread *tm2 = tf_gfx_threads_get_data (tgt, TF_THREAD_COGL);
  MTfThread *tm3 = tf_gfx_threads_get_data (tgt, TF_THREAD_ALLEGRO);

//  for (int i = 0; i < TF_NUM_THREADS; ++ i)
//    {
//      g_thread_join (gfx_threads[i]);
//    }

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
