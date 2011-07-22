#ifndef NXT_H_
#define NXT_H_

#include <src/cogl_include.h>

CoglHandle *g_testtex;

struct _MaiAnimInstance;
struct _MaiAnimInstance *g_mai;

struct xvtx
{
  float x,y,z;
};

struct xquat
{
  float w,x,y,z;
};

void
nxt_draw_array (struct xvtx *verts, int verts_count, unsigned int *indices, int indices_count);

#endif /* NXT_H_ */
