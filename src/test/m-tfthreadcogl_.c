#include <stdio.h>
#include <src/test/m-tfthread.h>
#include <src/test/m-tfmsg.h>
#include <src/test/m-tfmsg-fbup.h>
#include <src/test/tf-gfx-threads.h>
#include <src/test/m-tfthreadcogl.h>

NX_GET_NEW (m_tfthreadcogl);

GObject *
m_tfthreadcogl_new (void)
{
  MTfThreadCogl *self;
  MTfThread *par;

  self = M_TFTHREADCOGL (nx_get_new ());
  par = M_TFTHREAD (self);

  self->qu_in = g_async_queue_new ();
  g_xassert (self->qu_in);

  self->inited = FALSE;

  return G_OBJECT (self);
}

void
m_tfthreadcogl_init_inthread (MTfThreadCogl *self)
{
  _cogl_setup (self->width, self->height, &self->ofs, &self->tx);

  self->inited = TRUE;
}

void
m_tfthreadcogl_send_ (MTfThread *self, MTfMsg * msg)
{
  MTfThreadCogl *this;
  this = M_TFTHREADCOGL (self);

  g_object_ref (msg);
  g_async_queue_push (this->qu_in, msg);
}

void
m_tfthreadcogl_event_loop_enter (MTfThreadCogl *self)
{
  g_xassert (self->inited);

  while (TRUE)
    {
      MTfMsg *msg;

      msg = M_TFMSG (g_async_queue_pop (self->qu_in));

      _m_tfthreadcogl_process_incoming_one (self, msg);
    }
}

void
_m_tfthreadcogl_process_incoming_one (MTfThreadCogl *self, MTfMsg *msg)
{
  switch (msg->msg_type)
  {
  case TF_MSGTYPE_REDRAW:
    {
      g_xassert (0);
      break;
    }

  case TF_MSGTYPE_HEARTBEAT:
    {
      TfGfxThreads *tgt;
      MTfThread *tm;
      MTfMsgFbUp *mm;

      tgt = tf_gfx_threads_get_instance ();
      tm = tf_gfx_threads_get_data (tgt, TF_THREAD_ALLEGRO);

      /* This is where painting should be done */
      {
        CoglColor clear_color;
        cogl_color_set_from_4ub (&clear_color, '\x80', '\x0', '\x0', 255);
        cogl_clear (&clear_color, COGL_BUFFER_BIT_COLOR | COGL_BUFFER_BIT_DEPTH);

        gchar *tex_data;
        gint tex_siz;
        _m_tfthreadcogl_texture_get_data (self->tx, &tex_data, &tex_siz);

        mm = M_TFMSG_FBUP (m_tfmsg_fbup_new (tex_data, tex_siz,
                                             self->width, self->height));
      }

      m_tfthread_send (tm, M_TFMSG (mm));

      g_object_unref (mm);

      break;
    }

  default:
      g_xassert (("EEH MAJI, UNHANDLED MESSAGE TYPE", FALSE));
      break;
  }
}

void
_m_tfthreadcogl_texture_get_data (CoglHandle texture, gchar **data_out, gint *size_out)
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
