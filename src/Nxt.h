#ifndef NXT_H_
#define NXT_H_

#include <src/cogl_include.h>
#include <src/array_macro.h>
#include <src/misc.h>

CoglHandle *g_testtex;

struct _MaiAnimInstance;
struct _MaiAnimInstance *g_mai;

void
nxt_draw_array (struct xvtx *verts, int verts_count, unsigned int *indices, int indices_count);

#endif /* NXT_H_ */
