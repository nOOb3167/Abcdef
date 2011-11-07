#!/bin/sh

. tpl_aux_ar.sh

fail ()
{
    echo "ERROR: $2" 1>&2
    exit "$1"
}

HNAME="pr-${OUT_BASE}.h"
CNAME="pr-${OUT_BASE}.c"

sub_all_into "template3_ard_h.txt" "$HNAME"
sub_all_into "template3_ard_c.txt" "$CNAME"

test -f "${HNAME}" || fail 1 "Possibly failed header template substitution"
test -f "${CNAME}" || fail 1 "Possibly failed code template substitution"

test -d "../../ar" || fail 1 "Expected path of template output dir not exists"

mv "${HNAME}" "${CNAME}" "../../ar" || fail 1 "Failed to move to template output directory"
