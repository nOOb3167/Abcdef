#include <ar/pa-base.h>

#define SELF(x) PA_BASE(x)
#define TYPE_SELF PA_TYPE_BASE
#define SELF_CLASS(x) PA_BASE_CLASS(x)
#define SELF_GET_CLASS(x) PA_BASE_GET_CLASS(x)
#define GET_NEW ((PaBase *)g_object_new(pa_base_get_type(), NULL))

typedef PaBase Self;
typedef PaBaseClass SelfClass;

static void pa_base_class_init (PaBaseClass * c);
static void pa_base_init (PaBase * o);

static GObjectClass *parent_class = NULL;

GType
pa_base_get_type (void)
{
  static GType type = 0;

  if (0 == type)
    {
      static const GTypeInfo info = {
          sizeof (PaBaseClass),
          (GBaseInitFunc) NULL,
          (GBaseFinalizeFunc) NULL,
          (GClassInitFunc) pa_base_class_init,
          (GClassFinalizeFunc) NULL,
          NULL /* class_data */,
          sizeof (PaBase),
          0 /* n_preallocs */,
          (GInstanceInitFunc) pa_base_init,
          NULL};

      type = g_type_register_static (G_TYPE_OBJECT, "PaBase", &info, (GTypeFlags)0);
    }

  return type;
}

static void 
pa_base_class_init (PaBaseClass * c)
{
  parent_class = g_type_class_ref (G_TYPE_OBJECT);
}

static void 
pa_base_init (PaBase * o)
{
}

GObject * 
pa_base_new (void)
{
  PaBase *self;
  GObject *par;

  self = PA_BASE (GET_NEW);
  par = G_OBJECT (self);

  return G_OBJECT (self);
}
