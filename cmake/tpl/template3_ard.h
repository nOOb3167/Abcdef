#ifndef __${TYPE_CAPS}_H__
#define __${TYPE_CAPS}_H__

#include <glib.h>
#include <glib-object.h>

${INCLUDE_PARENT_HEADER}

#define ${TAIL_THIS}_TYPE_${HEAD_THIS}	(${TYPE_LOWER}_get_type())
#define ${TYPE_CAPS}(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), ${TYPE_LOWER}_get_type(), ${TYPE_UPPER})
#define ${TYPE_CAPS}_CONST(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), ${TYPE_LOWER}_get_type(), ${TYPE_UPPER} const)
#define ${TYPE_CAPS}_CLASS(klass)	G_TYPE_CHECK_CLASS_CAST((klass), ${TYPE_LOWER}_get_type(), ${TYPE_UPPER}Class)
#define ${TAIL_THIS}_IS_${HEAD_THIS}(obj)	G_TYPE_CHECK_INSTANCE_TYPE((obj), ${TYPE_LOWER}_get_type ())

#define ${TYPE_CAPS}_GET_CLASS(obj)	G_TYPE_INSTANCE_GET_CLASS((obj), ${TYPE_LOWER}_get_type(), ${TYPE_UPPER}Class)

typedef struct _${TYPE_UPPER} ${TYPE_UPPER};
struct _${TYPE_UPPER} {
	${TYPE_PARENT_UPPER} __parent__;
        GPtrArray *array;
        guint len;
};

typedef struct _${TYPE_UPPER}Class ${TYPE_UPPER}Class;
struct _${TYPE_UPPER}Class {
	${TYPE_PARENT_UPPER}Class __parent__;
        void (*add) (${TYPE_UPPER} *array, ${VTYPE_UPPER} *data);
        ${VTYPE_UPPER} (*index) (${TYPE_UPPER} *array, int index);
};


GType	${TYPE_LOWER}_get_type	(void);

GObject * 	${TYPE_LOWER}_new	(void);

#endif /* __${CLASS_THIS}_H__ */
