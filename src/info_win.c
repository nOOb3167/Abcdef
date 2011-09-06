#include <gtk/gtk.h>
#include <src/info_win.h>

static gboolean win_dead = FALSE;

static gboolean
signal_dead (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
  win_dead = TRUE;
  return FALSE;
}

int
iw_info_win (void)
{
  GtkWidget *win, *but, *hbox;
  win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (win, "delete-event",
                    G_CALLBACK (gtk_false), NULL);
  g_signal_connect (win, "destroy",
            G_CALLBACK (signal_dead), NULL);

  hbox = gtk_hbox_new (TRUE, 0);
  gtk_container_add (GTK_CONTAINER (win), hbox);

  but = gtk_button_new_with_label ("Abcdef");
  g_signal_connect_swapped (but, "clicked",
          G_CALLBACK (gtk_object_destroy), win);
  gtk_container_add (GTK_CONTAINER (hbox), but);

  gtk_widget_show_all (win);

  while (win_dead == FALSE)
    {
      gtk_main_iteration ();
    }

  return 0;
}
