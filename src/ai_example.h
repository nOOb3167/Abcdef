#ifndef AI_EXAMPLE_H_
#define AI_EXAMPLE_H_

#include <assimp.h>
#include <cogl_include.h>

#ifdef __cplusplus
extern "C" {
#endif

void
ai_import_file (const char *file_name);

void
ai_matrix_to_cogl_matrix (struct aiMatrix4x4 *ai_matrix, CoglMatrix *cogl_matrix);

#ifdef __cplusplus
}
#endif

#endif /* AI_EXAMPLE_H_ */
