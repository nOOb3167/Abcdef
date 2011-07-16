#ifdef XGOB_VFUN
#error What
#else

#ifndef MAI_NODE_FUNCS_H_
#define MAI_NODE_FUNCS_H_

#include <assimp.h>
#include <aiScene.h>
#include <error.h>
#include <src/cogl_include.h>
#include <src/mai-node.h>

MaiNode *
mai_node_new_from (struct aiScene *scene, struct aiNode *from, MaiNode *parent);


void
mai_node_draw_recursive (MaiNode *self);

void
_mai_node_draw_recursive (MaiNode *self, CoglMatrix *acc_mtx);

#endif /* MAI_NODE_FUNCS_H_ */
#endif /* not XGOB_VFUN */
