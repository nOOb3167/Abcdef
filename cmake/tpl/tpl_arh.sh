#!/bin/sh

. tpl_aux_arh.sh

HNAME="${OUT_BASE}.h"
CNAME="${OUT_BASE}.c"

sub_all_into "template4_arh_h.txt" "$HNAME"
sub_all_into "template4_arh_c.txt" "$CNAME"

test -f "${HNAME}" || fail 1 "Possibly failed header template substitution"
test -f "${CNAME}" || fail 1 "Possibly failed code template substitution"

test -d "../../ar" || fail 1 "Expected path of template output dir not exists"

mv "${HNAME}" "${CNAME}" "../../ar" || fail 1 "Failed to move to template output directory"
