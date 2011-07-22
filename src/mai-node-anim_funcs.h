#ifndef MAI_NODE_ANIM_FUNCS_H_
#define MAI_NODE_ANIM_FUNCS_H_

#include <assimp.h>
#include <aiScene.h>
#include <src/cogl_include.h>
#include <error.h>
#include <src/Nxt.h>
#include <src/array_macro.h>
#include <src/mai-node-anim.h>

XPTR_ARRAY_MACRO_TYPE_DECLARE (mai_node_anim, MaiNodeAnim);

struct NxAnimKey
{
  double time;
  union
  {
    struct xvtx vec;
    struct xquat rot;
  } val;
};

MaiNodeAnim *
mai_node_anim_new_from (struct aiScene *scene, struct aiNodeAnim *from);

#endif /* MAI_NODE_ANIM_FUNCS_H_ */
