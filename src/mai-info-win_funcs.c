#include <gtk/gtk.h>
#include <src/mai-info-win.h>

NX_GET_NEW (mai_info_win);

gboolean
_iw_signal_dead (GtkWidget *widget, gpointer user_data)
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

void
_iw_fill_from_node_one (GtkTreeStore *store, GtkTreeIter *iter,
                        struct SrNodeGraph *gra, struct SrNode* node)
{
  GtkTreeIter it2;
  gtk_tree_store_append (store, &it2, iter);
  gtk_tree_store_set (store, &it2,
                      IW_COLUMN_NAME, node->name,
                      -1);

  for (int i = 0; i < node->child_names_len; ++i)
    {
      struct SrNode *ch;

      ch = g_hash_table_lookup (gra->name_node_map, node->child_names[i]);
      g_xassert (ch);

      _iw_fill_from_node_one (store, &it2, gra, ch);
    }
}

void
mai_info_win_clear_model (MaiInfoWin *iw)
{
  gtk_tree_store_clear (iw->store);
}

void
mai_info_win_fill_model_from_node_graph (MaiInfoWin *iw, struct SrNodeGraph *gra)
{
  g_xassert (gra->nodes_len > 0 && &gra->nodes[0] != NULL);
  _iw_fill_from_node_one (iw->store, NULL, gra, &gra->nodes[0]);
}

void
mai_info_win_fill_model_from_model (MaiInfoWin *iw, MaiModel *model)
{
  GtkTreeIter iter;
  GtkTreeIter it2;
  gtk_tree_store_append (iw->store, &iter, NULL);
  gtk_tree_store_set (iw->store, &iter,
                      IW_COLUMN_NAME, "Anims",
                      -1);
  gtk_tree_store_append (iw->store, &it2, &iter);

  for (gint i = 0; i < model->anims->len; ++i)
    {
      MaiAnim *an;
      an = g_object_ref (MAI_ANIM (
          g_mai_anim_ptr_array_index (model->anims, i)));
      g_xassert (an);

      gtk_tree_store_set (iw->store, &it2,
                          IW_COLUMN_NAME, an->name,
                          -1);
      g_object_unref (an);
    }
}

void
mai_info_win_display (MaiInfoWin *iw)
{
  gtk_tree_view_expand_all (GTK_TREE_VIEW (iw->tree_view));

  gtk_widget_show_all (iw->win);

  while (iw->win_dead == FALSE)
    {
      gtk_main_iteration ();
    }
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

  GtkWidget *tree_view;
  GtkTreeStore *store;
  _iw_setup_tree (&tree_view, &store);

  self->store = store;
  self->tree_view = tree_view;

  GtkWidget *hbox;
  hbox = gtk_bin_get_child (GTK_BIN (self->win));
  g_xassert (hbox);
  gtk_container_add (GTK_CONTAINER (hbox), self->tree_view);

  return G_OBJECT (self);
}
