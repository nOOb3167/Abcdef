#ifndef NXT_H_
#define NXT_H_

struct xvtx
{
  float x, y,z;
};

void
nxt_draw_array (struct xvtx *verts, int size);

#endif /* NXT_H_ */
