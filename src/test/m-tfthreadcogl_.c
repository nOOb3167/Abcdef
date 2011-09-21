#include <src/test/m-tfthread.h>
#include <src/test/m-tfthreadcogl.h>

NX_GET_NEW (m_tfthreadcogl);

GObject *
m_tfthreadcogl_new (void)
{
  MTfThreadCogl *self;
  MTfThread *par;

  self = M_TFTHREADCOGL (nx_get_new ());
  par = M_TFTHREAD (self);

  return G_OBJECT (self);
}
