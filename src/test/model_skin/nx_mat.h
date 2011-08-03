#ifndef NX_MAT_H_
#define NX_MAT_H_

struct NxMat
{
  float vals[16];
};
typedef struct NxMat NxMat;

#define NX_MAT_ELT(m,r,c) \
  ((m)->vals[(4*(r))+(c)])

void
nx_mat_init_identity (NxMat *what);

void
nx_mat_multiply (NxMat *result, NxMat *a, NxMat *b);

#endif /* NX_MAT_H_ */
