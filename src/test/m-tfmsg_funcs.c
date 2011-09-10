#include <src/test/m-tfmsg.h>

NX_GET_NEW (m_tfmsg);

GObject *
m_tfmsg_new (void)
{
  MTfMsg *self;
  self = M_TFMSG (nx_get_new ());

  return G_OBJECT (self);
}
