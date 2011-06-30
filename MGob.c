#include <stdlib.h>
#include <stdio.h>

#include <src/ab-cd.h>

int main (int argc, char **argv)
{
  g_type_init ();

  AbCd *go;
  go = ab_cd_new ();

  ab_cd_something (go);

  g_object_unref (go);

  return EXIT_SUCCESS;
}
