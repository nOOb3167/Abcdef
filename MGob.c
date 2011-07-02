#include <stdlib.h>
#include <stdio.h>

#include <src/ab-cd.h>
#include <src/ab-cd-alt.h>

int main (int argc, char **argv)
{
  g_type_init ();

  AbCd *go;
  go = ab_cd_new ();
  ab_cd_something (go);
  g_object_unref (go);

  AbCdAlt *aalt;
  aalt = ab_cd_alt_new ();
  ab_cd_alt_something (aalt);
  g_object_unref (aalt);

  return EXIT_SUCCESS;
}
