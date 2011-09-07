#ifndef INFO_WIN_H_
#define INFO_WIN_H_

#include <gtk/gtk.h>
#include <sr.h>

void
iw_info_win (void);

void
_iw_fill_from_node_one (GtkTreeStore *store, GtkTreeIter *iter,
                        struct SrNodeGraph *gra, struct SrNode* node);

void
iw_fill_model_from_node_graph (GtkTreeStore *model, struct SrNodeGraph *gra);

#endif /* INFO_WIN_H_ */
