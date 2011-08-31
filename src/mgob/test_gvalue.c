#include <stdlib.h>
#include <glib.h>
#include <glib-object.h>

#define NX_VALUE_INIT  { 0, { { 0 } } }

int
main (int argc, char **argv)
{
  g_type_init ();

  GObject *obj;
  obj = g_object_new (G_TYPE_OBJECT, NULL);

  GValue val = NX_VALUE_INIT;
  g_value_init (&val, G_OBJECT_TYPE (obj));

  return EXIT_SUCCESS;
}
