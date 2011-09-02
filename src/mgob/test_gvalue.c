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
  g_xassert (G_IS_VALUE (v1));
  g_xassert (G_IS_VALUE (v2));

  if (!g_value_type_compatible (G_VALUE_TYPE (v1),
                                G_VALUE_TYPE (v2)));
    {
      g_xassert (FALSE);

      return 0;
    }

  if (G_VALUE_HOLDS_INT (v1) && G_VALUE_HOLDS_INT (v2))
    {
      return g_value_get_int (v1) == g_value_get_int (v2);
    }

  if (G_VALUE_HOLDS_OBJECT (v1) && G_VALUE_HOLDS_OBJECT (v2))
    {
      return g_value_get_object (v1) == g_value_get_object (v2);
    }

  g_xassert (FALSE);

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

void
nx_g_value_new (GValue **v, GType t)
{
  *v = g_malloc (sizeof (**v));
  **v = nx_empty_value;
  g_value_init (*v, t);
}

GValue *
nx_box_object (GObject *obj)
{
  GValue *ret;

  nx_g_value_new (&ret, G_TYPE_OBJECT);
  g_value_set_object (ret, obj);

  return ret;
}

GObject *
nx_unbox_object (GValue *v)
{
  GObject *ret;

  if (v == NULL)
    return NULL;

  g_xassert (G_VALUE_HOLDS_OBJECT (v));
  ret = g_value_get_object (v);

  return ret;
}

GValue *
nx_hash_table_lookup (GHashTable *ht, GValue *v)
{
  GValue *ret;

  ret = g_hash_table_lookup (ht, v);
  nx_value_free (v);

  return ret;
}

int
main (int argc, char **argv)
{
  g_type_init ();

  GObject *obj;
  obj = g_object_new (G_TYPE_OBJECT, NULL);

  GObject *obj_nothere;
  obj_nothere = g_object_new (G_TYPE_OBJECT, NULL);


  MgobX1 *x1;
  x1 = mgob_x1_new ("x1");

  GHashTable *ht;
  ht = g_hash_table_new_full (nx_value_object_hash,
                              nx_value_object_equal,
                              nx_value_free,
                              nx_value_free);

  GValue *k1;
  nx_g_value_new (&k1, G_TYPE_OBJECT);
  g_value_set_object (k1, G_OBJECT (obj));

  GValue *k2;
  nx_g_value_new (&k2, G_TYPE_INT);
  g_value_set_int (k2, 1234);

  /**
   * It is okay to use G_TYPE_OBJECT for derived.
   * Will be using checked casts anyway, just have to
   * get the fundamental type covered.
   */
  GValue *v1;
  nx_g_value_new (&v1, G_TYPE_OBJECT);
  g_value_set_object (v1, G_OBJECT (x1));

  g_hash_table_insert (ht, k2, v1);

  MgobX1 *ok2o;
  ok2o = MGOB_X1 (nx_unbox_object (nx_hash_table_lookup (ht, nx_box_object (obj_nothere))));
  g_xassert (!ok2o);

  g_object_unref (obj);
  g_object_unref (x1);

  g_hash_table_unref (ht);

  return EXIT_SUCCESS;
}
