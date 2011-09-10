#include <src/test/m-tfmsg.h>
#include <src/test/m-tfmsg-heartbeat.h>

NX_GET_NEW (m_tfmsg_heartbeat);

GObject *
m_tfmsg_heartbeat_new (void)
{
  MTfMsgHeartBeat *self;
  MTfMsg *msg;

  self = M_TFMSG_HEARTBEAT (nx_get_new ());
  self->frame_number = 0;

  msg = M_TFMSG (self);
  msg->msg_type = TF_MSGTYPE_HEARTBEAT;

  return G_OBJECT (self);
}
