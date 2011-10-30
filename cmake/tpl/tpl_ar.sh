#!/bin/sh

. tpl_aux_ar.sh

sub_all_into "template3_ard_h.txt" "pr-${OUT_BASE}.h"
sub_all_into "template3_ard_c.txt" "pr-${OUT_BASE}.c"
