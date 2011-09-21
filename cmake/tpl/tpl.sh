#!/bin/sh

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
OUT_GOB="${RET}.gob"

make_outbase "$CLASS_THIS"
OUT_C="${RET}_.c"


sed \
-e "/abc/ s/abc/def/g" \
-e "/ijk/ s/ijk/lmn/g" \
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
template_gob.txt \
> "$OUT_GOB"

sed \
-e "/abc/ s/abc/def/g" \
-e "/ijk/ s/ijk/lmn/g" \
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
template_c.txt \
> "$OUT_C"
