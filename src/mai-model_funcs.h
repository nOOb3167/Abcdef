#ifndef MAI_MODEL_FUNCS_H_
#define MAI_MODEL_FUNCS_H_

#include <assimp.h>
#include <aiScene.h>

#include <src/mai-model.h>

MaiModel *
mai_model_new_from (struct aiScene *scene);

MaiModel *
mai_model_new_from_file (const char *file_name);

MaiAnimInstance *
mai_model_get_anim_by_name (MaiModel *self, char * name);

#endif /* MAI_MODEL_FUNCS_H_ */
