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

void
nx_mat_translation (NxMat *what, float x, float y, float z)
{
  nx_mat_init_identity (what);
  what->vals[12] = x;
  what->vals[13] = y;
  what->vals[14] = z;
}

void
nx_mat_scale (NxMat *what, float x, float y, float z)
{
  nx_mat_init_identity (what);
  what->vals[0] = x;
  what->vals[5] = y;
  what->vals[10] = z;
}

void
nx_mat_projection (NxMat *what, float near)
{
  nx_mat_init_identity (what);
  NX_MAT_ELT (what, 0, 0) = near;
  NX_MAT_ELT (what, 1, 1) = near;
  NX_MAT_ELT (what, 3, 2) = -1.0f;
  NX_MAT_ELT (what, 3, 3) = 0.0f;
}

void
nx_mat_transform (NxMat *what, NxVec4 *vec_inout)
{
  float vvmul (NxMat *what, int row, NxVec4 *vec)
  {
    int i;
    float res;
    res = 0.0f;
    for (i=0; i<4; ++i)
      {
        res += NX_MAT_ELT (what, row, i) * vec->vals[i];
      }
    return res;
  }

  NxVec4 temp;
  temp = *vec_inout;

  int i;
  for (i=0; i<4; ++i)
    {
      temp.vals[i] = vvmul (what, i, vec_inout);
    }

  *vec_inout = temp;
}
