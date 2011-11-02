#include <ar/ha-base.h>

#define SELF(x) HA_BASE(x)
#define TYPE_SELF HA_TYPE_BASE
#define SELF_CLASS(x) HA_BASE_CLASS(x)
#define SELF_GET_CLASS(x) HA_BASE_GET_CLASS(x)
#define GET_NEW ((HaBase *)g_object_new(ha_base_get_type(), NULL))

typedef HaBase Self;
typedef HaBaseClass SelfClass;

static void ha_base_class_init (HaBaseClass * c);
static void ha_base_init (HaBase * o);

static GObjectClass *parent_class = NULL;

GType
ha_base_get_type (void)
{
  static GType type = 0;

  if (0 == type)
    {
      static const GTypeInfo info = {
          sizeof (HaBaseClass),
          (GBaseInitFunc) NULL,
          (GBaseFinalizeFunc) NULL,
          (GClassInitFunc) ha_base_class_init,
          (GClassFinalizeFunc) NULL,
          NULL /* class_data */,
          sizeof (HaBase),
          0 /* n_preallocs */,
          (GInstanceInitFunc) ha_base_init,
          NULL};

      type = g_type_register_static (G_TYPE_OBJECT, "HaBase", &info, (GTypeFlags)0);
    }

  return type;
}

static void 
ha_base_class_init (HaBaseClass * c)
{
  parent_class = g_type_class_ref (G_TYPE_OBJECT);
}

static void 
ha_base_init (HaBase * o)
{
}

GObject * 
ha_base_new (void)
{
  HaBase *self;
  GObject *par;

  self = HA_BASE (GET_NEW);
  par = G_OBJECT (self);

  return G_OBJECT (self);
}
