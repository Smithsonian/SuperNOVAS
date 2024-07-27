/**
 * @author G. Kaplan and A. Kovacs
 *
 *  Dummy readeph() implementation for SuperNOVAS for use when minor planet ephermeris is
 *  unavailable.
 *
 *  Based on the NOVAS C Edition, Version 3.1:
 *
 *  U. S. Naval Observatory<br>
 *  Astronomical Applications Dept.<br>
 *  Washington, DC<br>
 *  <a href="http://www.usno.navy.mil/USNO/astronomical-applications">
 *  http://www.usno.navy.mil/USNO/astronomical-applications</a>
 *
 */
#include <stdlib.h>
#include <errno.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "novas.h"

double * readeph_dummy(int mp, const char *name, double jd_tdb, int *error) {
  static const char *fn = "readeph_dummy";
  double *pv;

  if(!name || !error) {
    set_error(-1, EINVAL, fn, "NULL parameter: name=%p, error=%p", name, error);
    return NULL;
  }

  if(isnanf(jd_tdb)) {
    set_error(-1, EINVAL, fn, "NaN jd_tdb");
    *error = -1;
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
double * default_readeph(int mp, const char *name, double jd_tdb, int *error) {
  return readeph_dummy(mp, name, jd_tdb, error);
}
#endif
