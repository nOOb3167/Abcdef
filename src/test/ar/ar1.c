#include <stdlib.h>
#include <glib.h>
#include <glib-object.h>
#include <src/error.h>
#include <src/test/ar/ab-cd-ef.h>
#include <src/mgob/nx-base.h>
#include <ar/pr-ab-cd-ef.h>
#include <ar/hr4ab-cd-ef5nx-base6.h>

int
main (int argc, char **argv)
{
  g_type_init ();

  AbCdEf *re;
  re = AB_CD_EF (ab_cd_ef_new ());
  
  PrAbCdEf *rr;
  rr = PR_AB_CD_EF (pr_ab_cd_ef_new ());

  rr->add (rr, re);

  AbCdEf *elt;
  elt = rr->index (rr, 0);

  Hr4AbCdEf5NxBase6 *hr;
  hr = hr4ab_cd_ef5nx_base6_new ();

  NxBase *nx;
  nx = nx_base_new ();

  hr->insert (hr, re, nx);

  NxBase *nx2;
  nx2 = hr->lookup (hr, re);

  g_xassert (nx2 && nx == nx2);

  return EXIT_SUCCESS;
}
