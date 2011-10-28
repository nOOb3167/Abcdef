#include <src/test/ar/ab-cd-ef.h>

#define SELF(x) AB_CD_EF(x)
#define TYPE_SELF AB_CD_TYPE_EF
#define SELF_CLASS(x) AB_CD_EF_CLASS(x)
#define SELF_GET_CLASS(x) AB_CD_EF_GET_CLASS(x)
#define GET_NEW ((AbCdEf *)g_object_new(ab_cd_ef_get_type(), NULL))

typedef AbCdEf Self;
typedef AbCdEfClass SelfClass;

static void ab_cd_ef_class_init (AbCdEfClass * c);
static void ab_cd_ef_init (AbCdEf * o);

static GObjectClass *parent_class = NULL;

GType
ab_cd_ef_get_type (void)
{
  static GType type = 0;

  if (0 == type)
    {
      static const GTypeInfo info = {
          sizeof (AbCdEfClass),
          (GBaseInitFunc) NULL,
          (GBaseFinalizeFunc) NULL,
          (GClassInitFunc) ab_cd_ef_class_init,
          (GClassFinalizeFunc) NULL,
          NULL /* class_data */,
          sizeof (AbCdEf),
          0 /* n_preallocs */,
          (GInstanceInitFunc) ab_cd_ef_init,
          NULL};

      type = g_type_register_static (G_TYPE_OBJECT, "AbCdEf", &info, (GTypeFlags)0);
    }

  return type;
}

static void 
ab_cd_ef_class_init (AbCdEfClass * c)
{
  parent_class = g_type_class_ref (G_TYPE_OBJECT);
}

static void 
ab_cd_ef_init (AbCdEf * o)
{
}

GObject * 
ab_cd_ef_new (void)
{
  AbCdEf *self;
  GObject *par;

  self = AB_CD_EF (GET_NEW);
  par = G_OBJECT (self);

  return G_OBJECT (self);
}
