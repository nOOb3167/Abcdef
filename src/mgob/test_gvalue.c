#include <stdlib.h>
#include <glib.h>
#include <glib-object.h>
#include <src/mgob/mgob-x1.h>
#include <src/error.h>

#define NX_VALUE_INIT  { 0, { { 0 } } }

const GValue nx_empty_value = NX_VALUE_INIT;

guint
nx_value_object_hash (gconstpointer v)
{
  gpointer p;

  g_xassert (G_IS_VALUE (v));

  if (G_VALUE_HOLDS_INT (v))
    {
      return g_value_get_int (v);
    }

  if (G_VALUE_HOLDS_OBJECT (v))
    {
      return GPOINTER_TO_UINT (g_value_get_object (v));
    }

  g_xassert (FALSE);

  return 0;
}

gboolean
nx_value_object_equal (gconstpointer v1, gconstpointer v2)
{
  return v1 == v2;
}

void
nx_value_free (gpointer v)
{
  printf ("nx_value_free %p\n", v);

  g_xassert (G_IS_VALUE (v));
  g_value_reset (v);
  g_free (v);
}

int
main (int argc, char **argv)
{
  g_type_init ();

  GObject *obj;
  obj = g_object_new (G_TYPE_OBJECT, NULL);

  MgobX1 *x1;
  x1 = mgob_x1_new ("x1");

  GValue val = NX_VALUE_INIT;
  g_value_init (&val, G_TYPE_OBJECT);
  g_value_set_object (&val, obj);

  GHashTable *ht;
  ht = g_hash_table_new_full (nx_value_object_hash,
                              nx_value_object_equal,
                              nx_value_free,
                              nx_value_free);

  GValue *k1;
  k1 = g_malloc (sizeof (*k1));
  *k1 = nx_empty_value;
  g_value_init (k1, G_TYPE_OBJECT);
  g_value_set_object (k1, G_OBJECT (obj));

  GValue *k2;
  k2 = g_malloc (sizeof (*k2));
  *k2 = nx_empty_value;
  g_value_init (k2, G_TYPE_INT);
  g_value_set_int (k2, 1234);

  /**
   * It is okay to use G_TYPE_OBJECT for derived.
   * Will be using checked casts anyway, just have to
   * get the fundamental type covered.
   */
  GValue *v1;
  v1 = g_malloc (sizeof (*v1));
  *v1 = nx_empty_value;
  g_value_init (v1, G_TYPE_OBJECT);
  g_value_set_object (v1, G_OBJECT (x1));

  g_hash_table_insert (ht, k2, v1);

  g_object_unref (obj);
  g_object_unref (x1);

  g_hash_table_unref (ht);

  return EXIT_SUCCESS;
}
