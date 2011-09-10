#include <src/test/m-tfmsg.h>

NX_GET_NEW (m_tfmsg);

GObject *
m_tfmsg_new (gchar *data, gint size,
             gint width, gint height)
{
  MTfMsg *self;

  g_xassert (data);

  self = M_TFMSG (nx_get_new ());
  self->tex_data = data;
  self->tex_data_size = size;
  self->width = width;
  self->height = height;

  return G_OBJECT (self);
}

