#include <gtk/gtk.h>
#include <src/info_win.h>

enum
{
   IW_COLUMN_NAME = 0,
   N_COLUMNS
};

static gboolean win_dead = FALSE;

void
_iw_setup_tree (GtkWidget **tree_out, GtkTreeStore **store_out);

gboolean
_iw_signal_dead (GtkWidget *widget, GdkEvent *event, gpointer user_data);

gboolean
_iw_signal_dead (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
  win_dead = TRUE;
  return FALSE;
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
   g_object_set (G_OBJECT (renderer),
                 "foreground", "red",
                 NULL);

   column = gtk_tree_view_column_new_with_attributes ("Author", renderer,
                                                      "text", IW_COLUMN_NAME,
                                                      NULL);

   gtk_tree_view_append_column (GTK_TREE_VIEW (tree), column);

   *tree_out = tree;
   *store_out = store;
}

int
iw_info_win (void)
{
  GtkWidget *win, *but, *hbox;
  win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (win, "delete-event",
                    G_CALLBACK (gtk_false), NULL);
  g_signal_connect (win, "destroy",
            G_CALLBACK (_iw_signal_dead), NULL);

  hbox = gtk_hbox_new (TRUE, 0);
  gtk_container_add (GTK_CONTAINER (win), hbox);

  but = gtk_button_new_with_label ("Abcdef");
  g_signal_connect_swapped (but, "clicked",
          G_CALLBACK (gtk_object_destroy), win);
  gtk_container_add (GTK_CONTAINER (hbox), but);

  GtkWidget *tree;
  GtkTreeStore *store;
  _iw_setup_tree (&tree, &store);

  GtkTreeIter iter;
  gtk_tree_store_append (store, &iter, NULL);

  gtk_tree_store_set (store, &iter, IW_COLUMN_NAME, "Greetings", -1);

  gtk_container_add (GTK_CONTAINER (hbox), tree);

  gtk_widget_show_all (win);

  while (win_dead == FALSE)
    {
      gtk_main_iteration ();
    }

  return 0;
}
