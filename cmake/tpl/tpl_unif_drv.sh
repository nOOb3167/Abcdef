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

    flags="$1"
    type="$2"

    case "$flags" in
    "G")
      #ok
      ;;
    "C")
      #ok I guess, but have to recode the template to support custom destruct
      ;;
    *)
      fail 1 "Unrecognized flag"
      ;;
    esac

    if [ -z "$type" ]
    then fail 1 "No type specified"; fi

    ./tpl_ar.sh ";$type" "ar;Pa:Base" || fail 1 "Substitution using tpl_ar.sh failed"
}

one_hash_line ()
{
    echo "HASH $1"
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
