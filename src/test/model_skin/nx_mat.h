#ifndef NX_MAT_H_
#define NX_MAT_H_

#include <assimp.h>

struct NxVec4
{
  float vals[4];
};
typedef struct NxVec4 NxVec4;

void
nx_vec_add (NxVec4 *result, NxVec4 *a, NxVec4 *b);

void
nx_vec_sub (NxVec4 *result, NxVec4 *a, NxVec4 *b);

void
nx_vec_negate (NxVec4 *result, NxVec4 *vec);

void
nx_vec_scale (NxVec4 *result, NxVec4 *vec, float scale);

void
nx_vec_cross_product (NxVec4 *result, NxVec4 *a, NxVec4 *b);

void
nx_vec_normalize4 (NxVec4 *out, NxVec4 *in);

void
nx_vec_normalize3 (NxVec4 *out, NxVec4 *in);

struct NxMat
{
  float vals[16];
};
typedef struct NxMat NxMat;

#define NX_MAT_ELT(m,r,c) \
  ((m)->vals[(4*(c))+(r)])
#define NX_MAT_SWAP_ROWS(a,b) { float *_tmp = (a); (a)=(b); (b)=_tmp; }

void
nx_mat_init_identity (NxMat *what);

void
nx_mat_init_from_array (NxMat *what, float floats[]);

void
nx_mat_multiply (NxMat *result, NxMat *a, NxMat *b);

void
nx_mat_translation (NxMat *what, float x, float y, float z);

void
nx_mat_scale (NxMat *what, float x, float y, float z);

void
nx_mat_projection (NxMat *what, float near);

void
nx_mat_ortho (NxMat *what);

void
nx_mat_transform (NxMat *what, NxVec4 *vec_inout);

void
nx_mat_rotate (NxMat *matrix,
                    float angle,
                    float x,
                    float y,
                    float z);

void
nx_mat_from_ai_matrix (NxMat *mat, struct aiMatrix4x4 *ai_matrix);

int
nx_mat_get_inverse (NxMat *mat, NxMat *inv_out);

int
_nx_mat_invert_matrix_general (NxMat *matrix, NxMat *inv_out);

int
_nx_mat_invert_matrix_general (NxMat *matrix, NxMat *inv_out);

#endif /* NX_MAT_H_ */
