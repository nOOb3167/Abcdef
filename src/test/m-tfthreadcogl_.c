#include <src/test/m-tfthread.h>
#include <src/test/m-tfmsg.h>
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

  return G_OBJECT (self);
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
  g_xassert (TF_MSGTYPE_REDRAW == msg->msg_type);

  switch (msg->msg_type)
  {
  case TF_MSGTYPE_REDRAW:
    {
      g_xassert (0);
      break;
    }

  default:
      g_xassert (("EEH MAJI, UNHANDLED MESSAGE TYPE", FALSE));
      break;
  }
}
