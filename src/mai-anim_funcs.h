#ifndef MAI_ANIM_FUNCS_H_
#define MAI_ANIM_FUNCS_H_

#include <assimp.h>
#include <aiScene.h>
#include <error.h>
#include <src/cogl_include.h>

#include <src/array_macro.h>
#include <src/mai-anim.h>

MaiAnim *
mai_anim_new_from (struct aiScene *scene, struct aiAnimation *from);

XARRAY_MACRO_TYPE_DECLARE (mai_anim, MaiAnim);
XPTR_ARRAY_MACRO_TYPE_DECLARE (mai_anim, MaiAnim);

#endif /* MAI_ANIM_FUNCS_H_ */
