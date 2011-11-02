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

PATH_K="${1%%;*}"
PATH_V="${2%%;*}"
PATH_PARENT="${3%%;*}"

CLASS_K="${1#*;}"
CLASS_V="${2#*;}"
CLASS_PARENT="${3#*;}"

#------------------ MISC

HASH_KEY_FUNC="$4"
EQUAL_KEY_FUNC="$5"
KEY_DESTROY_FUNC="$6"
VALUE_DESTROY_FUNC="$7"

#------------------ KTYPE VTYPE L/H

tolower "$CLASS_K"
KTYPE_LOWER="$(echo $RET | tr : _)"

ctonc "$CLASS_K"
KTYPE_UPPER="struct _$RET"

tolower "$CLASS_V"
VTYPE_LOWER="$(echo $RET | tr : _)"

ctonc "$CLASS_V"
VTYPE_UPPER="struct _$RET"

#------------------ TYPE L/H and TYPE_CAPS

tolower "$CLASS_K"
_RET="hr4${RET}5"
tolower "$CLASS_V"
_RET="${_RET}${RET}6"
TYPE_LOWER="$(echo $_RET | tr : _)"

ctonc "$CLASS_K"
_RET="Hr4${RET}5"
ctonc "$CLASS_V"
_RET="${_RET}${RET}6"
TYPE_UPPER="${_RET}"

toupper "$TYPE_LOWER"
TYPE_CAPS="$RET"


tolower "$CLASS_PARENT"
TYPE_PARENT_LOWER="$(echo $RET | tr : _)"

ctonc "$CLASS_PARENT"
TYPE_PARENT_UPPER="$RET"

toupper "$TYPE_PARENT_LOWER"
TYPE_PARENT_CAPS="$RET"

#------------------ INCLUDE THIS/PARENT HEADER, OUT_BASE

tolower "$CLASS_K"
_RET="hr4${RET}5"
tolower "$CLASS_V"
_RET="${_RET}${RET}6"
INCLUDE_THIS_HEADER="#include <ar/$(echo ${_RET} | tr : -).h>"

if [ -z "$PATH_PARENT" ]
then
	INCLUDE_PARENT_HEADER=""
else
	make_include "$PATH_PARENT" "$CLASS_PARENT"
	INCLUDE_PARENT_HEADER="$RET"
fi

tolower "$CLASS_K"
_RET="hr4${RET}5"
tolower "$CLASS_V"
_RET="${_RET}${RET}6"
OUT_BASE="$(echo $_RET | tr : -)"

#------------------ HEADS TAILS

toupper "$TYPE_LOWER"
HEAD_THIS="${RET#hr}"

TAIL_THIS="HR"

get_head "$CLASS_PARENT"
toupper "${RET}"
HEAD_PARENT="$(echo $RET | tr : _)"

get_tail "$CLASS_PARENT"
toupper "${RET}"
TAIL_PARENT="$(echo $RET | tr : _)"

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
    -e "s/\${TYPE_PARENT_CAPS}/${TYPE_PARENT_CAPS}/g" \
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
    -e "s/\${KTYPE_LOWER}/${KTYPE_LOWER}/g" \
    -e "s/\${KTYPE_UPPER}/${KTYPE_UPPER}/g" \
    -e "s/\${HASH_KEY_FUNC}/${HASH_KEY_FUNC}/g" \
    -e "s/\${EQUAL_KEY_FUNC}/${EQUAL_KEY_FUNC}/g" \
    -e "s/\${KEY_DESTROY_FUNC}/${KEY_DESTROY_FUNC}/g" \
    -e "s/\${VALUE_DESTROY_FUNC}/${VALUE_DESTROY_FUNC}/g" \
    "${TEMPLATE_FILE}" \
    > "${OUTPUT_FILE}"
}
