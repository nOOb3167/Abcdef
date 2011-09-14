#include <stdlib.h>
#include <glib.h>
#include <glib-object.h>

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
