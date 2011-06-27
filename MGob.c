#include <stdlib.h>

#include <src/ab-cd.h>

int main (int argc, char **argv)
{
  g_type_init ();

  GObject *go;
  go = ab_cd_new ();

  return EXIT_SUCCESS;
}
