#ifndef ARRAY_MACRO_H_
#define ARRAY_MACRO_H_

#include <glib.h>

/*
 * xlower mai_node
 * xupper MaiNode
 */

#define XARRAY_MACRO_TYPE_DECLARE(xlower, xupper) \
typedef GArray G##xupper##Array; \
G##xupper##Array *g_##xlower##_array_new (void); \
gchar *g_##xlower##_array_free (G##xupper##Array *array, gboolean free_segment); \
G##xupper##Array *g_##xlower##_array_append_val (G##xupper##Array *array, xupper *data); \
xupper g_##xlower##_array_index (G##xupper##Array *array, int index); \
void g_##xlower##_array_unref (G##xupper##Array *array);

#define XARRAY_MACRO_TYPE_DEFINE(xlower, xupper) \
G##xupper##Array *g_##xlower##_array_new (void) \
{ \
  GArray *ret; \
  ret = g_array_new (0, 1, sizeof (xupper)); \
  return ret; \
} \
gchar *g_##xlower##_array_free (G##xupper##Array *array, gboolean free_segment) \
{ \
  return g_array_free (array, free_segment); \
} \
G##xupper##Array *g_##xlower##_array_append_val (G##xupper##Array *array, xupper *data) \
{ \
  return g_array_append_vals (array, data, 1); \
} \
xupper g_##xlower##_array_index (G##xupper##Array *array, int index) \
{ \
  return g_array_index (array, xupper, index); \
} \
void g_##xlower##_array_unref (G##xupper##Array *array) \
{ \
  g_array_unref (array); \
}

struct HaHa
{
  int x,y;
};
typedef struct HaHa HaHa;

XARRAY_MACRO_TYPE_DECLARE (ha_ha, HaHa);

#endif /* ARRAY_MACRO_H_ */
