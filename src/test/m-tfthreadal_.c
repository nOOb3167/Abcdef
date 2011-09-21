#include <src/test/m-tfthread.h>
#include <src/test/m-tfthreadal.h>

NX_GET_NEW (m_tfthreadal);

GObject *
m_tfthreadal_new (void)
{
  MTfThreadAl *self;
  MTfThread *par;

  self = M_TFTHREADAL (nx_get_new ());
  par = M_TFTHREAD (self);

  return G_OBJECT (self);
}
