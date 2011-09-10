#include <src/test/m-tfmsg.h>

NX_GET_NEW (m_tfmsg);

GObject *
m_tfmsg_new_with_data (gchar *data)
{
  MTfMsg *self;

  g_xassert (data);

  self = M_TFMSG (nx_get_new ());
  self->tex_data = data;

  return G_OBJECT (self);
}

