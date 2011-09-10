#include <src/test/m-tfmsg.h>
#include <src/test/m-tfmsg-fbup.h>

NX_GET_NEW (m_tfmsg_fbup);

GObject *
m_tfmsg_fbup_new (gchar *data, gint size,
                  gint width, gint height)
{
  MTfMsgFbUp *self;
  MTfMsg *msg;

  g_xassert (data);

  self = M_TFMSG_FBUP (nx_get_new ());
  self->width = width;
  self->height = height;
  self->tex_data = data;
  self->tex_data_size = size;

  msg = M_TFMSG (self);
  msg->msg_type = TF_MSGTYPE_FBUP;


  return G_OBJECT (self);
}
