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
nx_mat_init_from_array (NxMat *what, float floats[])
{
  memcpy (what->vals, floats, sizeof (*floats) * 16);
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

void
nx_mat_from_ai_matrix (NxMat *mat, struct aiMatrix4x4 *ai_matrix)
{
  g_xassert (mat);
  g_xassert (ai_matrix);

  float tmp_vals[16] = {
      ai_matrix->a1, ai_matrix->b1, ai_matrix->c1, ai_matrix->d1,
      ai_matrix->a2, ai_matrix->b2, ai_matrix->c2, ai_matrix->d2,
      ai_matrix->a3, ai_matrix->b3, ai_matrix->c3, ai_matrix->d3,
      ai_matrix->a4, ai_matrix->b4, ai_matrix->c4, ai_matrix->d4,
  };

  nx_mat_init_from_array (mat, tmp_vals);
}

int
nx_mat_get_inverse (NxMat *mat, NxMat *inv_out)
{
  g_xassert (mat);
  g_xassert (inv_out);

  int ret;
  NxMat inv;
  ret = _nx_invert_matrix_general (mat, inv);
  nx_mat_init_from_array (inv_out, inv.vals);
  return ret;
}

int
_nx_mat_invert_matrix_general (NxMat *matrix, NxMat *inv_out)
{
  NxMat *m = matrix;
  NxMat *out = inv_out;
  float wtmp[4][8];
  float m0, m1, m2, m3, s;
  float *r0, *r1, *r2, *r3;

  r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

  r0[0] = NX_MAT_ELT (m, 0, 0), r0[1] = NX_MAT_ELT (m, 0, 1),
    r0[2] = NX_MAT_ELT (m, 0, 2), r0[3] = NX_MAT_ELT (m, 0, 3),
    r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,

    r1[0] = NX_MAT_ELT (m, 1, 0), r1[1] = NX_MAT_ELT (m, 1, 1),
    r1[2] = NX_MAT_ELT (m, 1, 2), r1[3] = NX_MAT_ELT (m, 1, 3),
    r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,

    r2[0] = NX_MAT_ELT (m, 2, 0), r2[1] = NX_MAT_ELT (m, 2, 1),
    r2[2] = NX_MAT_ELT (m, 2, 2), r2[3] = NX_MAT_ELT (m, 2, 3),
    r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,

    r3[0] = NX_MAT_ELT (m, 3, 0), r3[1] = NX_MAT_ELT (m, 3, 1),
    r3[2] = NX_MAT_ELT (m, 3, 2), r3[3] = NX_MAT_ELT (m, 3, 3),
    r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;

  /* choose pivot - or die */
  if (fabsf (r3[0]) > fabsf (r2[0]))
    NX_MAT_SWAP_ROWS (r3, r2);
  if (fabsf (r2[0]) > fabsf (r1[0]))
    NX_MAT_SWAP_ROWS (r2, r1);
  if (fabsf (r1[0]) > fabsf (r0[0]))
    NX_MAT_SWAP_ROWS (r1, r0);
  if (0.0 == r0[0])
    return FALSE;

  /* eliminate first variable     */
  m1 = r1[0]/r0[0]; m2 = r2[0]/r0[0]; m3 = r3[0]/r0[0];
  s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
  s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
  s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
  s = r0[4];
  if (s != 0.0) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
  s = r0[5];
  if (s != 0.0) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
  s = r0[6];
  if (s != 0.0) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
  s = r0[7];
  if (s != 0.0) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

  /* choose pivot - or die */
  if (fabsf (r3[1]) > fabsf (r2[1]))
    NX_MAT_SWAP_ROWS (r3, r2);
  if (fabsf (r2[1]) > fabsf (r1[1]))
    NX_MAT_SWAP_ROWS (r2, r1);
  if (0.0 == r1[1])
    return FALSE;

  /* eliminate second variable */
  m2 = r2[1] / r1[1]; m3 = r3[1] / r1[1];
  r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
  r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
  s = r1[4]; if (0.0 != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
  s = r1[5]; if (0.0 != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
  s = r1[6]; if (0.0 != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
  s = r1[7]; if (0.0 != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

  /* choose pivot - or die */
  if (fabsf (r3[2]) > fabsf (r2[2]))
    NX_MAT_SWAP_ROWS (r3, r2);
  if (0.0 == r2[2])
    return FALSE;

  /* eliminate third variable */
  m3 = r3[2] / r2[2];
  r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
    r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6],
    r3[7] -= m3 * r2[7];

  /* last check */
  if (0.0 == r3[3])
    return FALSE;

  s = 1.0f / r3[3];             /* now back substitute row 3 */
  r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

  m2 = r2[3];                 /* now back substitute row 2 */
  s  = 1.0f / r2[2];
  r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
    r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
  m1 = r1[3];
  r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
    r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
  m0 = r0[3];
  r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
    r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

  m1 = r1[2];                 /* now back substitute row 1 */
  s  = 1.0f / r1[1];
  r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
    r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
  m0 = r0[2];
  r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
    r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

  m0 = r0[1];                 /* now back substitute row 0 */
  s  = 1.0f / r0[0];
  r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
    r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

  NX_MAT_ELT (out, 0, 0) = r0[4]; NX_MAT_ELT (out, 0, 1) = r0[5],
    NX_MAT_ELT (out, 0, 2) = r0[6]; NX_MAT_ELT (out, 0, 3) = r0[7],
    NX_MAT_ELT (out, 1, 0) = r1[4]; NX_MAT_ELT (out, 1, 1) = r1[5],
    NX_MAT_ELT (out, 1, 2) = r1[6]; NX_MAT_ELT (out, 1, 3) = r1[7],
    NX_MAT_ELT (out, 2, 0) = r2[4]; NX_MAT_ELT (out, 2, 1) = r2[5],
    NX_MAT_ELT (out, 2, 2) = r2[6]; NX_MAT_ELT (out, 2, 3) = r2[7],
    NX_MAT_ELT (out, 3, 0) = r3[4]; NX_MAT_ELT (out, 3, 1) = r3[5],
    NX_MAT_ELT (out, 3, 2) = r3[6]; NX_MAT_ELT (out, 3, 3) = r3[7];

  return TRUE;
}
