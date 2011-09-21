#include <src/test/m-tfthread.h>

NX_GET_NEW (m_tfthread);

GObject *
m_tfthread_new (void)
{
  MTfThread *self;
  GObject *par;

  self = M_TFTHREAD (nx_get_new ());
  par = G_OBJECT (self);

  return G_OBJECT (self);
}

void
m_tfthread_send_ (MTfThread *self, MTfMsg * msg)
{
  g_xassert (("Pure virtual", 0));
}
