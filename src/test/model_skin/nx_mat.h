#ifndef NX_MAT_H_
#define NX_MAT_H_

struct NxVec4
{
  float vals[4];
};
typedef struct NxVec4 NxVec4;

struct NxMat
{
  float vals[16];
};
typedef struct NxMat NxMat;

#define NX_MAT_ELT(m,r,c) \
  ((m)->vals[(4*(c))+(r)])

void
nx_mat_init_identity (NxMat *what);

void
nx_mat_multiply (NxMat *result, NxMat *a, NxMat *b);

void
nx_mat_translation (NxMat *what, float x, float y, float z);

void
nx_mat_scale (NxMat *what, float x, float y, float z);

void
nx_mat_projection (NxMat *what, float near);

void
nx_mat_transform (NxMat *what, NxVec4 *vec_inout);

#endif /* NX_MAT_H_ */
