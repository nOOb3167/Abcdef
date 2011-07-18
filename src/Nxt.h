#ifndef NXT_H_
#define NXT_H_

#include <src/cogl_include.h>

CoglHandle *g_testtex;

struct xvtx
{
  float x, y,z;
};

void
nxt_draw_array (struct xvtx *verts, int verts_count, unsigned int *indices, int indices_count);

#endif /* NXT_H_ */
