#ifndef MISC_H_
#define MISC_H_

#include <src/array_macro.h>

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

#endif /* MISC_H_ */
