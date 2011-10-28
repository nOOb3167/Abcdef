#include <stdlib.h>
#include <glib.h>
#include <glib-object.h>
#include <src/test/ar/ab-cd-ef.h>

int
main (int argc, char **argv)
{
  g_type_init ();

  AbCdEf *re;
  re = AB_CD_EF (ab_cd_ef_new ());

  return EXIT_SUCCESS;
}
