#ifndef MAI_NODE_FUNCS_H_
#define MAI_NODE_FUNCS_H_

#include <assimp.h>
#include <aiScene.h>
#include <error.h>
#include <src/mai-node.h>

#ifdef XGOB_VFUN
#else

struct _MaiNode *
mai_node_new_from (struct aiNode *from, struct _MaiNode *parent);

#endif /* XGOB_VFUN */
#endif /* MAI_NODE_FUNCS_H_ */
