#include <nx_mat.h>

void
nx_mat_init_identity (NxMat *what)
{
  what->vals[0] = 1.0f;
  what->vals[1] = 0.0f;
  what->vals[2] = 0.0f;
  what->vals[3] = 0.0f;

  what->vals[4] = 0.0f;
  what->vals[5] = 1.0f;
  what->vals[6] = 0.0f;
  what->vals[7] = 0.0f;

  what->vals[8] = 0.0f;
  what->vals[9] = 0.0f;
  what->vals[10] = 1.0f;
  what->vals[11] = 0.0f;

  what->vals[12] = 0.0f;
  what->vals[13] = 0.0f;
  what->vals[14] = 0.0f;
  what->vals[15] = 1.0f;
}

void
nx_mat_multiply (NxMat *result, NxMat *a, NxMat *b)
{
  int i,j;
  for (i=0; i<4; ++i)
    for (j=0; j<4; ++j)
      {
        int t, sum;
        NX_MAT_ELT (result, i, j) = 0.0f;
        for (t=0; t<4; ++t)
          NX_MAT_ELT (result, i, j) += NX_MAT_ELT (a, i, t) * NX_MAT_ELT (b, t, j);
      }
}
