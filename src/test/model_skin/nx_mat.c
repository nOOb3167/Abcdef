#include <math.h>
#include <glib.h>
#include <src/error.h>
#include <nx_mat.h>

void
nx_vec_normalize4 (NxVec4 *out, NxVec4 *in)
{
  /**
   * Don't need a tmp vec to support out == in.. I think
   */
  float len;
  len = sqrtf (in->vals[0] * in->vals[0] +
               in->vals[1] * in->vals[1] +
               in->vals[2] * in->vals[2] +
               in->vals[3] * in->vals[3]);
  /**
   * Lol floating point comparison
   */
  g_xassert (len > 0.0f);
  out->vals[0] = in->vals[0] / len;
  out->vals[1] = in->vals[1] / len;
  out->vals[2] = in->vals[2] / len;
  out->vals[3] = in->vals[3] / len;
}

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
  NxMat tmp;
  tmp = *a;

  int i,j;
  for (i=0; i<4; ++i)
    for (j=0; j<4; ++j)
      {
        int t, sum;
        NX_MAT_ELT (result, i, j) = 0.0f;
        for (t=0; t<4; ++t)
          NX_MAT_ELT (result, i, j) += NX_MAT_ELT (&tmp, i, t) * NX_MAT_ELT (b, t, j);
      }
}

void
nx_mat_translation (NxMat *what, float x, float y, float z)
{
  NxMat tmp;
  nx_mat_init_identity (&tmp);

  NX_MAT_ELT (&tmp, 0, 3) = x;
  NX_MAT_ELT (&tmp, 1, 3) = y;
  NX_MAT_ELT (&tmp, 2, 3) = z;

  nx_mat_multiply (what, what, &tmp);
}

void
nx_mat_scale (NxMat *what, float x, float y, float z)
{
  NxMat tmp;
  nx_mat_init_identity (&tmp);

  NX_MAT_ELT (&tmp, 0, 0) = x;
  NX_MAT_ELT (&tmp, 1, 1) = y;
  NX_MAT_ELT (&tmp, 2, 2) = z;
  /*what->vals[0] = x;
  what->vals[5] = y;
  what->vals[10] = z;*/

  nx_mat_multiply (what, what, &tmp);
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
nx_mat_ortho (NxMat *what)
{
  nx_mat_init_identity (what);
  /**
   * O_O
   */
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

void
nx_mat_rotate (NxMat *matrix,
                    float angle,
                    float x,
                    float y,
                    float z)
{
  NxMat rotation;
  NxMat result;
  float c, s;

  nx_mat_init_identity (&rotation);

  angle *= G_PI / 180.0f;
  c = cosf (angle);
  s = sinf (angle);

  NX_MAT_ELT (&rotation, 0, 0) = x * x * (1.0f - c) + c;
  NX_MAT_ELT (&rotation, 1, 0) = y * x * (1.0f - c) + z * s;
  NX_MAT_ELT (&rotation, 2, 0) = x * z * (1.0f - c) - y * s;
  NX_MAT_ELT (&rotation, 3, 0) = 0.0f;

  NX_MAT_ELT (&rotation, 0, 1) = x * y * (1.0f - c) - z * s;
  NX_MAT_ELT (&rotation, 1, 1) = y * y * (1.0f - c) + c;
  NX_MAT_ELT (&rotation, 2, 1) = y * z * (1.0f - c) + x * s;
  NX_MAT_ELT (&rotation, 3, 1) = 0.0f;

  NX_MAT_ELT (&rotation, 0, 2) = x * z * (1.0f - c) + y * s;
  NX_MAT_ELT (&rotation, 1, 2) = y * z * (1.0f - c) - x * s;
  NX_MAT_ELT (&rotation, 2, 2) = z * z * (1.0f - c) + c;
  NX_MAT_ELT (&rotation, 3, 2) = 0.0f;

  NX_MAT_ELT (&rotation, 0, 3) = 0.0f;
  NX_MAT_ELT (&rotation, 1, 3) = 0.0f;
  NX_MAT_ELT (&rotation, 2, 3) = 0.0f;
  NX_MAT_ELT (&rotation, 3, 3) = 1.0f;

  nx_mat_multiply (&result, matrix, &rotation);
  *matrix = result;
}
