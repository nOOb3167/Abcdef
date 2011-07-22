#ifndef AI_EXAMPLE_H_
#define AI_EXAMPLE_H_

#include <assimp.h>
#include <cogl_include.h>
#include <glib.h>

#include <src/Nxt.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NX_DEGREES_TO_RADIANS (G_PI / 180.0f)
#define NX_RADIANS_TO_DEGREES (180.0f / G_PI)

void
ai_import_file (const char *file_name);

void
ai_matrix_to_cogl_matrix (struct aiMatrix4x4 *ai_matrix, CoglMatrix *cogl_matrix);

void
nx_cogl_quaternion_to_rotation_axis_and_angle (CoglQuaternion *quat, float *angle_out, struct xvtx *axis_out);

CoglPrimitive *
nx_cogl_primitive_new (GArray *verts, GArray *indices, GArray *uvs);

void
nx_cogl_primitive_draw (CoglPrimitive* prim);

#ifdef __cplusplus
}
#endif

#endif /* AI_EXAMPLE_H_ */
