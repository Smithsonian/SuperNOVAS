/*
 Naval Observatory Vector Astrometry Software (NOVAS)
 C Edition, Version 3.1

 readeph0.c: Dummy readeph for use when minor planet ephermeris is unavailable

 U. S. Naval Observatory
 Astronomical Applications Dept.
 Washington, DC
 http://www.usno.navy.mil/USNO/astronomical-applications
 */

#include <stdlib.h>
#include <errno.h>

#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"

double * readeph_dummy(int mp, const char *name, double jd_tdb, int *error) {
  static const char *fn = "readeph_dummy";
  double *pv;

  if(!name || !error)
    error_return(-1, EINVAL, fn, "NULL parameter: name=%p, error=%p", name, error);

  if(isnanf(jd_tdb))
    error_return(-1, EINVAL, fn, "NaN jd_tdb");

  pv = (double*) calloc(6, sizeof(double));
  *error = 9;

  return pv;
}

#ifdef DEFAULT_READEPH
double *readeph(int mp, const char *name, double jd_tdb, int *error) {
  return readeph_dummy(mp, name, jd_tdb, error);
}
#else
double * default_readeph(int mp, const char *name, double jd_tdb, int *error) {
  return readeph_dummy(mp, name, jd_tdb, error);
}
#endif
