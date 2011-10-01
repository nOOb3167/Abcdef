#include <stdio.h>
#include <src/test/tf-gfx-threads.h>
#include <src/test/m-tfthread.h>
#include <src/test/m-tfthreadal.h>

NX_GET_NEW (m_tfthreadal);

GObject *
m_tfthreadal_new (void)
{
  MTfThreadAl *self;
  MTfThread *par;

  self = M_TFTHREADAL (nx_get_new ());
  par = M_TFTHREAD (self);

  /**
   * Allegro MUST be initialized prior to calling any routines.
   */
  al_init ();

  self->queue = al_create_event_queue ();
  g_xassert (self->queue);

  self->timer = al_create_timer (0.05f);
  g_xassert (self->timer);

  self->timer_source = al_get_timer_event_source (self->timer);
  g_xassert (self->timer_source);

  self->custom_source = g_malloc0 (sizeof (*self->custom_source));
  al_init_user_event_source (self->custom_source);

  al_register_event_source (self->queue, self->timer_source);
  al_register_event_source (self->queue, self->custom_source);

  self->kill_me = FALSE;
  self->inited = TRUE;

  self->qu_out = g_async_queue_new ();
  g_xassert (self->qu_out);

  return G_OBJECT (self);
}

void
m_tfthreadal_event_loop_enter (MTfThreadAl *self)
{
  while (TRUE)
    {
      ALLEGRO_EVENT ev;

      al_wait_for_event (self->queue, &ev);

      if (_m_tfthreadal_check_overflow (self))
          self->kill_me = TRUE;

      _m_tfthreadal_process_incoming_one (self, &ev);
    }
}

/**
 * I believe locking is unnecessary.
 * Reading the Allegro source, queues and stuff have a mutex and condition variable member.
 */
void
m_tfthreadal_send_ (MTfThread *self, MTfMsg * msg)
{
  MTfThreadAl *this;
  this = M_TFTHREADAL (self);

  /**
   * As far as I can tell, Allegro documentation and source indicate events are copied,
   * so allocating in automatic memory is fine.
   * Yes, see examples/ex_user_events.c
   */
  ALLEGRO_EVENT uev;

  g_object_ref (msg);
  uev.user.type = TF_USER_EVENT_TYPE;
  uev.user.data1 = (intptr_t)msg;

  al_emit_user_event (this->custom_source, &uev, _m_tfthreadal_user_event_dtor);
}

void
m_tfthreadal_start_timer (MTfThreadAl *self)
{
  al_start_timer (self->timer);
}

void
_m_tfthreadal_user_event_dtor (ALLEGRO_USER_EVENT *event)
{
  MTfMsg *msg;
  msg = M_TFMSG (event->data1);

  printf ("UserEventDtor\n");

  g_object_unref (msg);
}

gboolean
_m_tfthreadal_check_overflow (MTfThreadAl *self)
{
  if (1000 > g_async_queue_length (self->qu_out))
      return TRUE;
  else
      return FALSE;
}

void
_m_tfthreadal_process_incoming_one (MTfThreadAl *self, ALLEGRO_EVENT *ev)
{
  g_xassert (ALLEGRO_EVENT_TIMER == ev->type ||
             TF_USER_EVENT_TYPE == ev->type);

  switch (ev->type)
  {
  case ALLEGRO_EVENT_TIMER:
    {
      MTfMsgHeartBeat *mm;
      mm = M_TFMSG_HEARTBEAT (m_tfmsg_heartbeat_new ());

      printf ("SZ %d\n", g_async_queue_length (self->qu_out));

      /**
       * Should be this instead: Select(Dispatch)->Send(mm)
       */
      g_object_ref (mm);
      g_async_queue_push (self->qu_out, mm);

      TfGfxThreads *tgt;
      tgt = tf_gfx_threads_get_instance ();

      MTfThread *tm;
      tm = tf_gfx_threads_get_data (tgt, TF_THREAD_COGL);

      /**
       * Send refs its argument so no g_object_ref necessary here.
       */
      m_tfthread_send (tm, M_TFMSG (mm));

      g_object_unref (mm);

      break;
    }

  case TF_USER_EVENT_TYPE:
    {
      MTfMsg *msg;
      msg = M_TFMSG (ev->user.data1);

      /**
       * Switch on msg->msg_type
       */
      printf ("UserEventType received\n");
      g_xassert (0);
      break;
    }

  default:
      g_xassert (("EEH MAJI, UNHANDLED MESSAGE TYPE", FALSE));
      break;
  }
}
