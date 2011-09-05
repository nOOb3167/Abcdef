#include <src/array_macro.h>
#include <src/mgob/mgob-x1.h>

NX_GET_NEW (mgob_x1);

GObject *
mgob_x1_new (const char *name)
{
  MgobX1 *ret;
  ret = MGOB_X1 (nx_get_new ());
  ret->name = name;
  return G_OBJECT (ret);
}
