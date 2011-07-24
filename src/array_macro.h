#ifndef ARRAY_MACRO_H_
#define ARRAY_MACRO_H_

#include <glib.h>

/*
 * xlower mai_node
 * xupper MaiNode
 */

#define XARRAY_MACRO_TYPE_DECLARE(xlower, xupper) \
struct G##xupper##Array_struct \
{ \
  GArray *array; \
  guint len; \
}; \
typedef struct G##xupper##Array_struct G##xupper##Array; \
G##xupper##Array *g_##xlower##_array_new (void); \
gchar *g_##xlower##_array_free (G##xupper##Array *array, gboolean free_segment); \
G##xupper##Array *g_##xlower##_array_append_val (G##xupper##Array *array, xupper *data); \
xupper g_##xlower##_array_index (G##xupper##Array *array, int index); \
void g_##xlower##_array_unref (G##xupper##Array *array);

#define XARRAY_MACRO_TYPE_DEFINE(xlower, xupper) \
G##xupper##Array *g_##xlower##_array_new (void) \
{ \
  G##xupper##Array *ret; \
  ret = g_new0 (G##xupper##Array, 1); \
  ret->array = g_array_new (0, 1, sizeof (xupper)); \
  ret->len = ret->array->len; \
  return ret; \
} \
gchar *g_##xlower##_array_free (G##xupper##Array *array, gboolean free_segment) \
{ \
  gchar *ret; \
  ret = g_array_free (array->array, free_segment); \
  g_free (array); \
  return ret; \
} \
G##xupper##Array *g_##xlower##_array_append_val (G##xupper##Array *array, xupper *data) \
{ \
  g_array_append_vals (array->array, data, 1); \
  array->len = array->array->len; \
  return array; \
} \
xupper g_##xlower##_array_index (G##xupper##Array *array, int index) \
{ \
  return g_array_index (array->array, xupper, index); \
} \
void g_##xlower##_array_unref (G##xupper##Array *array) \
{ \
  g_array_unref (array->array); \
}


/*
 * xlower mai_node
 * xupper MaiNode
 */

#define XPTR_ARRAY_MACRO_TYPE_DECLARE(xlower, xupper) \
struct G##xupper##PtrArray_struct \
{ \
  GPtrArray *array; \
  guint len; \
}; \
typedef struct G##xupper##PtrArray_struct G##xupper##PtrArray; \
G##xupper##PtrArray *g_##xlower##_ptr_array_new (void); \
gpointer *g_##xlower##_ptr_array_free (G##xupper##PtrArray *array, gboolean free_segment); \
void g_##xlower##_ptr_array_add (G##xupper##PtrArray *array, xupper *data); \
xupper *g_##xlower##_ptr_array_index (G##xupper##PtrArray *array, int index); \
void g_##xlower##_ptr_array_unref (G##xupper##PtrArray *array);

#define XPTR_ARRAY_MACRO_TYPE_DEFINE(xlower, xupper) \
G##xupper##PtrArray *g_##xlower##_ptr_array_new (void) \
{ \
  G##xupper##PtrArray *ret; \
  ret = g_new0 (G##xupper##PtrArray, 1); \
  ret->array = g_ptr_array_new (); \
  ret->len = ret->array->len; \
  return ret; \
} \
gpointer *g_##xlower##_ptr_array_free (G##xupper##PtrArray *array, gboolean free_segment) \
{ \
  gpointer *ret; \
  ret = g_ptr_array_free (array->array, free_segment); \
  g_free (array); \
  return ret; \
} \
void g_##xlower##_ptr_array_add (G##xupper##PtrArray *array, xupper *data) \
{ \
  g_ptr_array_add (array->array, data); \
  array->len = array->array->len; \
} \
xupper *g_##xlower##_ptr_array_index (G##xupper##PtrArray *array, int index) \
{ \
  return g_ptr_array_index (array->array, index); \
} \
void g_##xlower##_ptr_array_unref (G##xupper##PtrArray *array) \
{ \
  g_ptr_array_unref (array->array); \
}


struct HaHa
{
  int x,y;
};
typedef struct HaHa HaHa;

XARRAY_MACRO_TYPE_DECLARE (ha_ha, HaHa);
XPTR_ARRAY_MACRO_TYPE_DECLARE (ha_ha, HaHa);

#endif /* ARRAY_MACRO_H_ */
