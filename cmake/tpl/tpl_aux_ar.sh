ctonc ()
{
	#Make sure ends with colon
	#(Ex so the remove shortest prefix succeeds until ARGS becomes zero
	# and doesn't leave the last part in place, looping infinitely)
	local ARGS CONCAT
	ARGS="$@:"
	CONCAT=""
	while true
	do
		CONCAT="$CONCAT${ARGS%%:*}"
		ARGS="${ARGS#*:}"
		if [ -z "$ARGS" ]
			then break; fi
	done
	RET="$CONCAT"
}

tolower ()
{
	RET="$(echo ""$*"" | tr [:upper:] [:lower:])"
}

toupper ()
{
	RET="$(echo ""$*"" | tr [:lower:] [:upper:])"
}

#make_include path class
make_include ()
{
	local PNAME
	tolower "$2"
	PNAME="$(echo $RET | tr : -)"
	PNAME="$1/$PNAME.h"
	RET="#include <$PNAME>"
}

#make_outbase class
make_outbase ()
{
	tolower "$1"
	RET="$(echo $RET | tr : -)"
}

#get_tail class
get_tail ()
{
    RET="${1%:*}"
}

#get_head class
get_head ()
{
    RET="${1##*:}"
}

#------------------ PATH CLASS

PATH_THIS="${1%%;*}"
PATH_PARENT="${2%%;*}"

CLASS_THIS="${1#*;}"
CLASS_PARENT="${2#*;}"

#------------------ MISC

VALUE_DESTROY_FUNC="$3"

#------------------ KTYPE VTYPE L/H

tolower "$CLASS_THIS"
VTYPE_LOWER="$(echo $RET | tr : _)"

ctonc "$CLASS_THIS"
VTYPE_UPPER="struct _$RET"

#------------------ TYPE L/H and TYPE_CAPS

tolower "$CLASS_THIS"
RET="pr_$RET"
TYPE_LOWER="$(echo $RET | tr : _)"

ctonc "$CLASS_THIS"
RET="Pr$RET"
TYPE_UPPER="$RET"


tolower "$CLASS_PARENT"
TYPE_PARENT_LOWER="$(echo $RET | tr : _)"

ctonc "$CLASS_PARENT"
TYPE_PARENT_UPPER="$RET"

toupper "${CLASS_THIS}"
RET="PR_$RET"
TYPE_CAPS="$(echo $RET | tr : _)"

#------------------ INCLUDE THIS/PARENT HEADER, OUT_BASE

tolower "$CLASS_THIS"
INCLUDE_THIS_HEADER="#include <ar/pr-$(echo $RET | tr : -).h>"

if [ -z "$PATH_PARENT" ]
then
	INCLUDE_PARENT_HEADER=""
else
	make_include "$PATH_PARENT" "$CLASS_PARENT"
	INCLUDE_PARENT_HEADER="$RET"
fi

make_outbase "$CLASS_THIS"
OUT_BASE="${RET}"

#------------------ HEADS TAILS

get_head "$CLASS_THIS"
toupper "${RET}"
HEAD_THIS="$(echo $RET | tr : _)"

get_tail "$CLASS_THIS"
toupper "${RET}"
RET="PR_$RET"
TAIL_THIS="$(echo $RET | tr : _)"

get_head "$CLASS_PARENT"
toupper "${RET}"
HEAD_PARENT="$(echo $RET | tr : _)"

get_tail "$CLASS_PARENT"
toupper "${RET}"
TAIL_PARENT="$(echo $RET | tr : _)"

#------------------ SPECIAL

SPECIAL_VTYPE_UPPER="${VTYPE_UPPER} *"
SPECIAL_TOPTR="data"
SPECIAL_INDEX="g_ptr_array_index (self->array, index)"

#------------------ INT SPECIAL

## Int Special
 if [ "$CLASS_THIS" = "Int" ]
then
    VTYPE_LOWER="int"
    VTYPE_UPPER="int"
    SPECIAL_VTYPE_UPPER="int"
    SPECIAL_TOPTR="GINT_TO_POINTER (data)"
    SPECIAL_INDEX="GPOINTER_TO_INT (g_ptr_array_index (self->array, index))"
fi

#------------------ MAIN SUBSTITUTION ROUTINE

#sub_all_into template_file out_file
sub_all_into ()
{
    TEMPLATE_FILE="$1"
    OUTPUT_FILE="$2"

    sed \
    -e "s/\${CLASS_THIS}/${CLASS_THIS}/g" \
    -e "s/\${CLASS_PARENT}/${CLASS_PARENT}/g" \
    -e "s/\${TYPE_LOWER}/${TYPE_LOWER}/g" \
    -e "s/\${TYPE_UPPER}/${TYPE_UPPER}/g" \
    -e "s/\${TYPE_PARENT_LOWER}/${TYPE_PARENT_LOWER}/g" \
    -e "s/\${TYPE_PARENT_UPPER}/${TYPE_PARENT_UPPER}/g" \
    -e "s/\${NXGETNEW}/${NXGETNEW}/g" \
    -e "s^\${INCLUDE_THIS_HEADER}^${INCLUDE_THIS_HEADER}^g" \
    -e "s^\${INCLUDE_PARENT_HEADER}^${INCLUDE_PARENT_HEADER}^g" \
    -e "s^\${CHECKEDCAST_THIS}^${CHECKEDCAST_THIS}^g" \
    -e "s^\${CHECKEDCAST_PARENT}^${CHECKEDCAST_PARENT}^g" \
    -e "s/\${HEAD_THIS}/${HEAD_THIS}/g" \
    -e "s/\${TAIL_THIS}/${TAIL_THIS}/g" \
    -e "s/\${HEAD_PARENT}/${HEAD_PARENT}/g" \
    -e "s/\${TAIL_PARENT}/${TAIL_PARENT}/g" \
    -e "s/\${TYPE_CAPS}/${TYPE_CAPS}/g" \
    -e "s/\${VTYPE_LOWER}/${VTYPE_LOWER}/g" \
    -e "s/\${VTYPE_UPPER}/${VTYPE_UPPER}/g" \
    -e "s/\${SPECIAL_VTYPE_UPPER}/${SPECIAL_VTYPE_UPPER}/g" \
    -e "s/\${SPECIAL_TOPTR}/${SPECIAL_TOPTR}/g" \
    -e "s/\${SPECIAL_INDEX}/${SPECIAL_INDEX}/g" \
    -e "s/\${VALUE_DESTROY_FUNC}/${VALUE_DESTROY_FUNC}/g" \
    "${TEMPLATE_FILE}" \
    > "${OUTPUT_FILE}"
}
