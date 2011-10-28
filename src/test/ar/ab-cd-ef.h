#ifndef __AB_CD_EF_H__
#define __AB_CD_EF_H__

#include <glib.h>
#include <glib-object.h>



#define AB_CD_TYPE_EF	(ab_cd_ef_get_type())
#define AB_CD_EF(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), ab_cd_ef_get_type(), AbCdEf)
#define AB_CD_EF_CONST(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), ab_cd_ef_get_type(), AbCdEf const)
#define AB_CD_EF_CLASS(klass)	G_TYPE_CHECK_CLASS_CAST((klass), ab_cd_ef_get_type(), AbCdEfClass)
#define AB_CD_IS_EF(obj)	G_TYPE_CHECK_INSTANCE_TYPE((obj), ab_cd_ef_get_type ())

#define AB_CD_EF_GET_CLASS(obj)	G_TYPE_INSTANCE_GET_CLASS((obj), ab_cd_ef_get_type(), AbCdEfClass)

typedef struct _AbCdEf AbCdEf;
struct _AbCdEf {
	GObject __parent__;
};

typedef struct _AbCdEfClass AbCdEfClass;
struct _AbCdEfClass {
	GObjectClass __parent__;
};


GType	ab_cd_ef_get_type	(void);

GObject * 	ab_cd_ef_new	(void);

#endif /* __Ab:Cd:Ef_H__ */
