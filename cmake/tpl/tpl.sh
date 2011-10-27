#!/bin/sh

. tpl_aux.sh

sub_all_into "template_gob.txt" "$OUT_GOB"

sub_all_into "template_c.txt" "$OUT_C"
