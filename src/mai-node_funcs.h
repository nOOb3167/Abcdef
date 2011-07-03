#ifdef XGOB_VFUN
#error What
#else

#ifndef MAI_NODE_FUNCS_H_
#define MAI_NODE_FUNCS_H_

#include <assimp.h>
#include <aiScene.h>
#include <error.h>
#include <src/mai-node.h>

struct _MaiNode *
mai_node_new_from (struct aiNode *from, struct _MaiNode *parent);

#endif /* MAI_NODE_FUNCS_H_ */
#endif /* not XGOB_VFUN */
