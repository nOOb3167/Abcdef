#ifndef NXT_H_
#define NXT_H_

#include <src/cogl_include.h>
#include <src/array_macro.h>

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

struct NxVertexWeight
{
  unsigned int vertex_id;
  float weight;
};
typedef struct NxVertexWeight NxVertexWeight;

XARRAY_MACRO_TYPE_DECLARE (nx_vertex_weight, NxVertexWeight);

void
nxt_draw_array (struct xvtx *verts, int verts_count, unsigned int *indices, int indices_count);

#endif /* NXT_H_ */
