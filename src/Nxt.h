#ifndef NXT_H_
#define NXT_H_

struct xvtx
{
  float x, y,z;
};

void
nxt_draw_array (struct xvtx *verts, int verts_count, unsigned int *indices, int indices_count);

#endif /* NXT_H_ */
