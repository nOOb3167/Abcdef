#ifndef __PA_BASE_H__
#define __PA_BASE_H__

#include <glib.h>
#include <glib-object.h>



#define PA_TYPE_BASE	(pa_base_get_type())
#define PA_BASE(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), pa_base_get_type(), PaBase)
#define PA_BASE_CONST(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), pa_base_get_type(), PaBase const)
#define PA_BASE_CLASS(klass)	G_TYPE_CHECK_CLASS_CAST((klass), pa_base_get_type(), PaBaseClass)
#define PA_IS_BASE(obj)	G_TYPE_CHECK_INSTANCE_TYPE((obj), pa_base_get_type ())

#define PA_BASE_GET_CLASS(obj)	G_TYPE_INSTANCE_GET_CLASS((obj), pa_base_get_type(), PaBaseClass)

typedef struct _PaBase PaBase;
struct _PaBase {
	GObject __parent__;
};

typedef struct _PaBaseClass PaBaseClass;
struct _PaBaseClass {
	GObjectClass __parent__;
};


GType	pa_base_get_type	(void);

GObject * 	pa_base_new	(void);

#endif /* __Pa:Base_H__ */
