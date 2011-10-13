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

int
main (int argc, char **argv)
{
  g_thread_init (NULL);
  g_xassert (g_thread_supported ());

  g_type_init ();

  gtk_init( &argc, &argv );

  TfGfxThreads *tgt;
  tgt = tf_gfx_threads_get_instance ();
  tf_gfx_threads_start (tgt);

  void nothing (MTfThreadCogl *c, gpointer d) {printf ("HELLO AAA\n");}
  tf_gfx_threads_ext_cogl_heartbeat_connect (tgt, nothing, NULL);

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
