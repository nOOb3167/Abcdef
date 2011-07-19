#ifndef MAI_ANIM_FUNCS_H_
#define MAI_ANIM_FUNCS_H_

#include <assimp.h>
#include <aiScene.h>
#include <error.h>
#include <src/cogl_include.h>
#include <src/mai-anim.h>

MaiAnim *
mai_anim_new_from (struct aiScene *scene, struct aiAnimation *from);

#endif /* MAI_ANIM_FUNCS_H_ */
