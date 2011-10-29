#include <src/error.h>
${INCLUDE_THIS_HEADER}

#define GET_NEW ((${TYPE_UPPER} *)g_object_new(${TYPE_LOWER}_get_type(), NULL))

static void ${TYPE_LOWER}_class_init (${TYPE_UPPER}Class * c);
static void ${TYPE_LOWER}_init (${TYPE_UPPER} * o);

static void ${TYPE_LOWER}_finalize (${TYPE_UPPER} * o);
static void ${TYPE_LOWER}_add (${TYPE_UPPER} *self, ${VTYPE_UPPER} *data);
${VTYPE_UPPER} * ${TYPE_LOWER}_index (${TYPE_UPPER} *self, int index);

static ${TYPE_PARENT_UPPER}Class *parent_class = NULL;

GType
${TYPE_LOWER}_get_type (void)
{
  static GType type = 0;

  if (0 == type)
    {
      static const GTypeInfo info = {
          sizeof (${TYPE_UPPER}Class),
          (GBaseInitFunc) NULL,
          (GBaseFinalizeFunc) NULL,
          (GClassInitFunc) ${TYPE_LOWER}_class_init,
          (GClassFinalizeFunc) NULL,
          NULL /* class_data */,
          sizeof (${TYPE_UPPER}),
          0 /* n_preallocs */,
          (GInstanceInitFunc) ${TYPE_LOWER}_init,
          NULL};

      type = g_type_register_static (${TAIL_PARENT}_TYPE_${HEAD_PARENT}, "${TYPE_UPPER}", &info, (GTypeFlags)0);
    }

  return type;
}

static void 
${TYPE_LOWER}_class_init (${TYPE_UPPER}Class * c)
{
  parent_class = g_type_class_ref (${TAIL_PARENT}_TYPE_${HEAD_PARENT});

  G_OBJECT_CLASS (c)->finalize = ${TYPE_LOWER}_finalize;
  ${TYPE_CAPS}_CLASS (c)->add = ${TYPE_LOWER}_add;
  ${TYPE_CAPS}_CLASS (c)->index = ${TYPE_LOWER}_index;
}

static void 
${TYPE_LOWER}_init (${TYPE_UPPER} * o)
{
}

static void
${TYPE_LOWER}_finalize (${TYPE_UPPER} * o)
{
  g_ptr_array_unref (o);
}

static void
${TYPE_LOWER}_add (${TYPE_UPPER} *self, ${VTYPE_UPPER} *data)
{
  g_ptr_array_add (self->array, data);
  self->len = self->array->len;
}

${VTYPE_UPPER} *
${TYPE_LOWER}_index (${TYPE_UPPER} *self, int index)
{
  g_xassert (index < self->array->len);
  return g_ptr_array_index (self->array, index);
}

GObject * 
${TYPE_LOWER}_new (void)
{
  ${TYPE_UPPER} *self;
  ${TYPE_PARENT_UPPER} *par;

  self = ${CHECKEDCAST_THIS} (GET_NEW);
  par = ${CHECKEDCAST_PARENT} (self);

  return G_OBJECT (self);
}
