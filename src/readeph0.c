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

#include "novas.h"

double* readeph_dummy(int mp, const char *name, double jd_tdb, int *error) {
  double *pv;

  if(mp < 0 || !name || !error) {
    errno = EINVAL;
    return NULL;
  }

  if(isnanf(jd_tdb)) {
    errno = EINVAL;
    return NULL;
  }

  pv = (double*) calloc(6, sizeof(double));
  *error = 9;

  return pv;
}

#ifdef DEFAULT_READEPH
double *readeph(int mp, const char *name, double jd_tdb, int *error) {
  return readeph_dummy(mp, name, jd_tdb, error);
}
#else
double* default_readeph(int mp, const char *name, double jd_tdb, int *error) {
  return readeph_dummy(mp, name, jd_tdb, error);
}
#endif
