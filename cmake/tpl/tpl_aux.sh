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

#$0 this/path/;Na:Me par/path;Par:Name

PATH_THIS="${1%%;*}"
PATH_PARENT="${2%%;*}"

CLASS_THIS="${1#*;}"
CLASS_PARENT="${2#*;}"

#If no removal was made, was not given;
if [ "$PATH_THIS" = "$1" ]
then echo "INFO: NULL_PATH_THIS"; fi

if [ "$PATH_PARENT" = "$2" ]
then echo "INFO: NULL_PATH_PARENT"; fi

if [ "$CLASS_THIS" = "$1" ]
then echo "INFO: NULL_CLASS_THIS"; fi

if [ "$CLASS_PARENT" = "$1" ]
then echo "INFO: NULL_CLASS_PARENT"; fi


tolower "$CLASS_THIS"
TYPE_LOWER="$(echo $RET | tr : _)"

ctonc "$CLASS_THIS"
TYPE_UPPER="$RET"


tolower "$CLASS_PARENT"
TYPE_PARENT_LOWER="$(echo $RET | tr : _)"

ctonc "$CLASS_PARENT"
TYPE_PARENT_UPPER="$RET"


NXGETNEW="NX_GET_NEW ($TYPE_LOWER);"


if [ -z "$PATH_THIS" ]
then
	INCLUDE_THIS_HEADER=""
else
	make_include "$PATH_THIS" "$CLASS_THIS"
	INCLUDE_THIS_HEADER="$RET"
fi

if [ -z "$PATH_PARENT" ]
then
	INCLUDE_PARENT_HEADER=""
else
	make_include "$PATH_PARENT" "$CLASS_PARENT"
	INCLUDE_PARENT_HEADER="$RET"
fi


toupper "$CLASS_THIS"
CHECKEDCAST_THIS="$(echo $RET | tr : _)"

toupper "$CLASS_PARENT"
CHECKEDCAST_PARENT="$(echo $RET | tr : _)"


make_outbase "$CLASS_THIS"
OUT_BASE="${RET}"

make_outbase "$CLASS_THIS"
OUT_GOB="${RET}.gob"

make_outbase "$CLASS_THIS"
OUT_C="${RET}_.c"


get_head "$CLASS_THIS"
toupper "${RET}"
HEAD_THIS="$(echo $RET | tr : _)"

get_tail "$CLASS_THIS"
toupper "${RET}"
TAIL_THIS="$(echo $RET | tr : _)"

get_head "$CLASS_PARENT"
toupper "${RET}"
HEAD_PARENT="$(echo $RET | tr : _)"

get_tail "$CLASS_PARENT"
toupper "${RET}"
TAIL_PARENT="$(echo $RET | tr : _)"


toupper "${CLASS_THIS}"
TYPE_CAPS="$(echo $RET | tr : _)"

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
    "${TEMPLATE_FILE}" \
    > "${OUTPUT_FILE}"
}
