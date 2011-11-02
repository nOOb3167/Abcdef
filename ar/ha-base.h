#ifndef __HA_BASE_H__
#define __HA_BASE_H__

#include <glib.h>
#include <glib-object.h>



#define HA_TYPE_BASE	(ha_base_get_type())
#define HA_BASE(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), ha_base_get_type(), HaBase)
#define HA_BASE_CONST(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), ha_base_get_type(), HaBase const)
#define HA_BASE_CLASS(klass)	G_TYPE_CHECK_CLASS_CAST((klass), ha_base_get_type(), HaBaseClass)
#define HA_IS_BASE(obj)	G_TYPE_CHECK_INSTANCE_TYPE((obj), ha_base_get_type ())

#define HA_BASE_GET_CLASS(obj)	G_TYPE_INSTANCE_GET_CLASS((obj), ha_base_get_type(), HaBaseClass)

typedef struct _HaBase HaBase;
struct _HaBase {
	GObject __parent__;
};

typedef struct _HaBaseClass HaBaseClass;
struct _HaBaseClass {
	GObjectClass __parent__;
};


GType	ha_base_get_type	(void);

GObject * 	ha_base_new	(void);

#endif /* __Ha:Base_H__ */
