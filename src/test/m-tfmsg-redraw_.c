#include <src/test/m-tfmsg.h>
#include <src/test/m-tfmsg-redraw.h>

NX_GET_NEW (m_tfmsg_redraw);

GObject *
m_tfmsg_redraw_new (void)
{
  MTfMsgRedraw *self;
  MTfMsg *par;

  self = M_TFMSG_REDRAW (nx_get_new ());
  par = M_TFMSG (self);

  self->msg_type = TF_MSGTYPE_REDRAW;

  return G_OBJECT (self);
}
