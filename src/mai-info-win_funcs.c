#include <gtk/gtk.h>
#include <src/mai-info-win.h>

NX_GET_NEW (mai_info_win);

gboolean
_iw_signal_dead (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
  MaiInfoWin *iw;
  iw = MAI_INFO_WIN (user_data);

  iw->win_dead = TRUE;
  return FALSE;
}

void
_iw_create_window (MaiInfoWin *iw, GtkWidget **win_out)
{
  GtkWidget *win, *but, *hbox;
  win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (win, "delete-event",
                    G_CALLBACK (gtk_false), NULL);
  g_signal_connect (win, "destroy",
            G_CALLBACK (_iw_signal_dead), iw);

  hbox = gtk_hbox_new (TRUE, 0);
  gtk_container_add (GTK_CONTAINER (win), hbox);

  but = gtk_button_new_with_label ("Abcdef");
  g_signal_connect_swapped (but, "clicked",
          G_CALLBACK (gtk_object_destroy), win);
  gtk_container_add (GTK_CONTAINER (hbox), but);

  *win_out = win;
}

void
_iw_setup_tree (GtkWidget **tree_out, GtkTreeStore **store_out)
{
   GtkTreeStore *store;
   GtkWidget *tree;
   GtkTreeViewColumn *column;
   GtkCellRenderer *renderer;

   store = gtk_tree_store_new (N_COLUMNS,
                               G_TYPE_STRING);

   tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));

   renderer = gtk_cell_renderer_text_new ();
   /*
    * Lol markup > color property.
    * http://www.gtk.org/api/2.6/pango/PangoMarkupFormat.html
    *
    * g_object_set (G_OBJECT (renderer),
    *               "foreground", "green",
    *               NULL);
   */
   column = gtk_tree_view_column_new_with_attributes ("Author", renderer,
                                                      "markup", IW_COLUMN_NAME,
                                                      NULL);

   gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);

   *tree_out = tree;
   *store_out = store;
}

GObject *
mai_info_win_new (void)
{
  MaiInfoWin * self;

  self = MAI_INFO_WIN (nx_get_new ());

  GtkWidget *win;
  _iw_create_window (self, &win);

  self->win = win;
  self->win_dead = FALSE;

  return G_OBJECT (self);
}
