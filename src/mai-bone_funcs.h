#ifndef MAI_BONE_FUNCS_H_
#define MAI_BONE_FUNCS_H_

#include <assimp.h>
#include <aiScene.h>
#include <src/array_macro.h>

XPTR_ARRAY_MACRO_TYPE_DECLARE (mai_bone, MaiBone);

MaiBone *
mai_bone_new_from (struct aiBone *bone);

#endif /* MAI_BONE_FUNCS_H_ */
