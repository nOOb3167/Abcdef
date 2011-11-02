#!/bin/sh

. tpl_aux_arh.sh

sub_all_into "template4_arh_h.txt" "${OUT_BASE}.h"
sub_all_into "template4_arh_c.txt" "${OUT_BASE}.c"
