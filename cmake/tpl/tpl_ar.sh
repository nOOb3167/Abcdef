#!/bin/sh

. tpl_aux.sh

sub_all_into "template2_c.txt" "ab-cd-ef.c"
sub_all_into "template2_h.txt" "ab-cd-ef.h"
