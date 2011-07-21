#include <glib.h>
#include <stdlib.h>

#include <src/mgob/nx-base.h>
#include <src/mgob/nx-derived.h>
#include <src/mgob/nx-derived_funcs.h>

int main (int argc, char **argv)
{
  g_type_init ();

  NxBase *base;
  base = nx_base_new ();

  NxDerived *derived;
  derived = nx_derived_new ();

  nx_base_something (NX_BASE (derived));

  g_object_unref (derived);
  g_object_unref (base);

  return EXIT_SUCCESS;
}
