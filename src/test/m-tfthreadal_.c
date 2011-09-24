#include <stdio.h>
#include <src/test/m-tfthread.h>
#include <src/test/m-tfthreadal.h>

NX_GET_NEW (m_tfthreadal);

GObject *
m_tfthreadal_new_inside_thread (void)
{
  MTfThreadAl *self;
  MTfThread *par;

  self = M_TFTHREADAL (nx_get_new ());
  par = M_TFTHREAD (self);

  self->queue = al_create_event_queue ();
  g_xassert (self->queue);

  self->timer = al_create_timer (0.05f);
  g_xassert (self->timer);

  self->timer_source = al_get_timer_event_source (self->timer);
  g_xassert (self->timer_source);

  al_init_user_event_source (self->custom_source);

  al_register_event_source (self->queue, self->timer_source);
  al_register_event_source (self->queue, self->custom_source);

  self->kill_me = FALSE;

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

      if (self->kill_me)
          g_thread_exit (0);

      al_wait_for_event (self->queue, &ev);
      g_xassert (ALLEGRO_EVENT_TIMER == ev.type ||
                 TF_USER_EVENT_TYPE == ev.type);

      if (_tf_allegro_check_overflow (self))
          self->kill_me = TRUE;

      _tf_allegro_process_incoming (self);

      MTfMsgAllegroTimer *mm;
      mm = m_tfmsg_allegro_timer_new ();

      g_async_queue_push (self->qu_out, mm);
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
  this = M_TFTHREAD (self);

  /**
   * As far as I can tell, Allegro documentation and source indicate events are copied,
   * so allocating in automatic memory is fine.
   * Yes, see examples/ex_user_events.c
   */
  ALLEGRO_EVENT uev;

  g_object_ref (msg);
  uev.user.type = TF_USER_EVENT_TYPE;
  uev.user.data1 = msg;

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
