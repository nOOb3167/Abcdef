#!/bin/sh

. tpl_aux.sh

sub_all_into "template2_h.txt" "${OUT_BASE}.h"
sub_all_into "template2_c.txt" "${OUT_BASE}.c"
