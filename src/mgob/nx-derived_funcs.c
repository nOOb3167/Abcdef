#include <stdio.h>
#include <src/mgob/nx-derived_funcs.h>

NxDerived *
nx_derived_new (void)
{
  return GET_NEW;
}

void
_nx_derived_something (NxDerived *self)
{
  printf ("_nx_derived_something\n");
}
