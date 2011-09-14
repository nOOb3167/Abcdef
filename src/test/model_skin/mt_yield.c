#include <stdlib.h>
#include <glib.h>
#include <glib-object.h>

/**
 * Timer thread forced to use fail algo.
 * Its purpose is to send unsolicited messages:
 *   If the requesting thread knew when to query for heartbeat, the timer would be obsolete.
 *   (Decays to polling)
 *
 * Input thread will use allegro API to asynchronously read keyboard / mouse state.
 * This allows a wait on the input queue, pushing keyboard state into the output queue
 * in response to requests.
 *
 * Display thread similar to Input thread.
 *
 * Timer thread:
 *   Will want to emit into the core event handler queue.
 *   (Make caller allocate TfAlTimerState, set qu_out, pass to thread func)
 *   (Or pass a special struct type -> qu_out_in, tf_out)
 */

/**
 * Supposed only to use the two queues.
 * The other state is private.
 */
struct TfAlTimerState
{
  ALLEGRO_EVENT_QUEUE *queue;
  ALLEGRO_EVENT_SOURCE *timer_source;
  ALLEGRO_TIMER *timer;
  gboolean kill_me;

  GAsyncQueue *qu_in;
  GAsyncQueue *qu_out;
};

void
_tf_allegro_create_data (struct TfAlTimerState **tf_out)
{
  struct TfAlTimerState *tf;
  tf = g_malloc0 (sizeof (*tf));

  tf->queue = al_create_event_queue ();
  g_xassert (tf->queue);

  tf->timer = al_create_timer (0.05f);
  g_xassert (tf->timer);

  tf->timer_source = al_get_timer_event_source (tf->timer);
  g_xassert (tf->timer_source);

  tf->kill_me = FALSE;

  tf->qu_in = g_async_queue_new ();
  g_xassert (tf->qu_in);

  tf->qu_out = g_async_queue_new ();
  g_xassert (tf->qu_out);

  *tf_out = tf;
}

/**
 * I suppose I won't overflow the Input queue but check it anyway.
 * The Output queue might get overflowed in case of very long frame
 * processing delay or something, causing the messages not be consumed
 * on the other end.
 */
gboolean
_tf_allegro_check_overflow (struct TfAlTimerState *tf)
{
  if (1000 > g_async_queue_length (tf->qu_out) ||
      1000 > g_async_queue_length (tf->qu_in))
      return TRUE;
  return FALSE;
}

void
_tf_allegro_process_incoming (struct TfAlTimerState *tf)
{
  MTfMsg *msg;
  while ((!tf->kill_me) &&
         (msg = g_async_queue_try_pop (tf->qu_in)))
    {
      switch (msg->msg_type)
        {
      case TF_MSGTYPE_ALLEGRO_TIMER:
          MTfMsgAllegroTimer *mm;
          mm = M_TFMSG_ALLEGRO_TIMER (msg);

          if (mm->kill)
              tf->kill_me = TRUE;
          break;
      default:
          g_xassert (("EEH MAJI, UNHANDLED MESSAGE TYPE", FALSE));
          break;
        }
    }
}

gpointer
tf_allegro_timer_thread (gpointer data)
{
  struct TfAlTimerState *tf;

  tf = data;
  _tf_allegro_create_data (*tf);

  al_register_event_source (tf->queue, tf->timer_source);
  al_start_timer (tf->timer);

  /**
   * The problem with this approach is that the timer thread will only
   * process incoming requests upon receiving an ALLEGRO_EVENT_TIMER event.
   *
   * Creating an user event and emitting it to kill the timer
   * would allow to wait for the two sources (Timer events in the Allegro queue,
   * and incoming requests in the glib queue) simultaneously.
   *
   * That would allow responding to incoming requests immediately instead of only on
   * timer event boundaries.
   *
   * I've checked Allegros source code and I don't see any problematic code
   * that would prevent emitting of events from other threads.
   */
  while (TRUE)
    {
      ALLEGRO_EVENT ev;

      if (tf->kill_me)
          g_thread_exit (0);

      al_wait_for_event (tf->queue, &ev);
      g_xassert (ALLEGRO_EVENT_TIMER == ev.type);

      if (_tf_allegro_check_overflow (tf))
          tf->kill_me = TRUE;

      _tf_allegro_process_incoming (tf);

      MTfMsgAllegroTimer *mm;
      mm = m_tfmsg_allegro_timer_new ();

      g_async_queue_push (tf->qu_out, mm);
    }
}

int
main (int argc, char **argv)
{
  g_thread_init (NULL);
  g_type_init ();

  GAsyncQueue *qu;
  qu = g_async_queue_new ();

  gpointer
  th1_f (gpointer data)
  {
    gpointer wat;

    while (TRUE)
      {
        //wat = g_async_queue_pop (qu);
        wat = g_async_queue_try_pop (qu);
        g_thread_yield ();
      }

    return NULL;
  }

  GThread *th1;
  th1 = g_thread_create (th1_f, NULL, TRUE, NULL);

  g_thread_join (th1);

  return EXIT_SUCCESS;
}
