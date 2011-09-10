#include <src/test/m-tfmsg.h>

NX_GET_NEW (m_tfmsg);

GObject *
m_tfmsg_new (gchar *data, gint size)
{
  MTfMsg *self;

  g_xassert (data);

  self = M_TFMSG (nx_get_new ());
  self->tex_data = data;
  self->tex_data_size = size;

  return G_OBJECT (self);
}

