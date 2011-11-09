exec 0< tpl_unif_list.txt

fail ()
{
    echo "ERROR: $2"
    exit "$1"
}

one_ptr_line ()
{
    echo "PTR $1"

    line="$1"

    set x $1
    shift

    vflags="$1"
    type="$2"

    shift 2

    case "$vflags" in
    "G")
      vdestroy="g_object_unref"
      ;;
    "C")
      vdestroy="$1"
      shift
      ;;
    *)
      fail 1 "Unrecognized flag"
      ;;
    esac

    ./tpl_ar.sh ";$type" "ar;Pa:Base" "$vdestroy" || fail 1 "Substitution using tpl_ar.sh failed"
}

one_hash_line ()
{
    echo "HASH $1"

    line="$1"

    set x $1
    shift

    flags="$1"
    ktype="$2"
    vtype="$3"

    shift 3

    kflags="${flags%%;*}"
    vflags="${flags##*;}"

    if [ -z "$ktype" ] || [ -z "$vtype" ]
    then fail 1 "Hash with empty ktype or vtype"; fi

    khash=""
    kequal=""
    kdestroy=""
    vdestroy=""

    case "$kflags" in
    "G")
      khash="g_direct_hash"
      kequal="g_direct_equal"
      kdestroy="g_object_unref"
      ;;
    "C")
      khash="$1"
      kequal="$2"
      kdestroy="$3"
      shift 3
      ;;
    *)
      fail 1 "Unrecognized flag"
      ;;
    esac

    case "$vflags" in
    "G")
      vdestroy="g_object_unref"
      ;;
    "C")
      vdestroy="$1"
      shift
      ;;
    *)
      fail 1 "Unrecognized flag"
      ;;
    esac

    ./tpl_arh.sh ";$ktype" ";$vtype" "ar;Ha:Base" "$khash" "$kequal" "$kdestroy" "$vdestroy" || fail 1 "Substitution using tpl_arh.sh failed"
}

one_list_line ()
{
    head="${1%%\ *}"
    tail="${1#*\ }"

    case "$head" in
    "P")
      one_ptr_line "$tail"
      ;;
    "H")
      one_hash_line "$tail"
      ;;
    *)
      fail 1 "Unrecognized ctype"
      ;;
    esac

}

while read line;
do
  one_list_line "$line"
done
