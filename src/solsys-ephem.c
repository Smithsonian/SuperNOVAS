/**
 * @file
 *
 * SuperNOVAS major planet ephemeris handler via the same generic ephemeris reader that is configured by set_ephem_reader() prior
 * to calling this routine.
 *
 * @date Created  on Jan 29, 2024
 * @author Attila Kovacs
 */

#include <errno.h>

#include "novas.h"

/**
 * Major planet ephemeris data via the same generic ephemeris reader that is configured by set_ephem_reader() prior
 * to calling this routine. This is the highest precision version.
 *
 * @param jd_tdb         [day] Two-element array containing the Julian date, which may be
 *                       split any way (although the first element is usually the
 *                       "integer" part, and the second element is the "fractional"
 *                       part).  Julian date is on the TDB or "T_eph" time scale.
 * @param body           Major planet number (or that for Sun, Moon, or Solar-system barycenter)
 * @param origin         NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1) relative to which to
 *                       report positions and velocities.
 * @param position       [AU] Position vector of 'body' at jd_tdb; equatorial rectangular
 *                       coordinates in AU referred to the ICRS.
 * @param velocity       [AU/day] Velocity vector of 'body' at jd_tdb; equatorial rectangular
 *                       system referred to the ICRS, in AU/day.
 * @return               0 if successful, or else an error code of solarsystem_hp().
 *
 * @sa solarsystem_ephem()
 * @sa solarsystem_hp()
 * @sa set_ephem_reader()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
short int solarsystem_ephem_hp(const double jd_tdb[2], enum novas_major_planet body, enum novas_origin origin, double *position, double *velocity) {
  static const char *names[] = NOVAS_MAJOR_PLANET_NAMES_INIT;

  novas_ephem_reader_func f;
  enum novas_origin o = NOVAS_BARYCENTER;
  int error;

  if(origin != NOVAS_BARYCENTER && origin != NOVAS_HELIOCENTER) {
    errno = EINVAL;
    return 1;
  }

  if(body < 0 || body >= NOVAS_MAJOR_PLANETS) {
    errno = EINVAL;
    return -1;
  }

  f = get_ephem_reader();
  if(!f) {
    errno = ENOSYS;
    return -1;
  }

  error = f(body, names[body], jd_tdb[0], jd_tdb[1], &o, position, velocity);
  if(error) return 2;

  if(o != origin) {
    double pos0[3], vel0[3];
    int i;
    int ref = origin == NOVAS_BARYCENTER : NOVAS_BARYCENTER_POS : NOVAS_SUN;

    error = f(ref, ref[origin], jd_tdb[0], jd_tdb[1], pos0, vel0);
    if(error) return 2;

    for(i = 0; i < 3; i++) {
      if(position) position[i] -= pos0[i];
      if(velocity) velocity[i] -= vel0[i];
    }
  }

  return 0;
}

/**
 * Major planet ephemeris data via the same generic ephemeris reader that is configured by set_ephem_reader() prior
 * to calling this routine. This is the regular (reduced) precision version.
 *
 * @param jd_tdb         [day] Two-element array containing the Julian date, which may be
 *                       split any way (although the first element is usually the
 *                       "integer" part, and the second element is the "fractional"
 *                       part).  Julian date is on the TDB or "T_eph" time scale.
 * @param body           Major planet number (or that for Sun, Moon, or Solar-system barycenter)
 * @param origin         NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1) relative to which to
 *                       report positions and velocities.
 * @param position       [AU] Position vector of 'body' at jd_tdb; equatorial rectangular
 *                       coordinates in AU referred to the ICRS.
 * @param velocity       [AU/day] Velocity vector of 'body' at jd_tdb; equatorial rectangular
 *                       system referred to the ICRS, in AU/day.
 * @return               0 if successful, or else an error code of solarsystem().
 *
 * @sa solarsystem_ephem_hp()
 * @sa solarsystem()
 * @sa set_ephem_reader()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
short int solarsystem_ephem(double jd_tdb, enum novas_major_planet body, enum novas_origin origin, double *position, double *velocity) {
  const double jd_tdb2[2] = { jd_tdb };
  return solarsystem_ephem_hp(jd_tdb2, body, origin, position, velocity);
}

#ifdef DEFAULT_SOLSYS
short int default_solarsystem(double jd_tdb, enum novas_major_planet body, enum novas_origin origin, double *position, double *velocity) {
  return solarsystem_ephem(jd_tdb, body, origin, position, velocity);
}

short int default_solarsystem_hp(const double jd_tdb[2], enum novas_major_planet body, enum novas_origin origin, double *position, double *velocity) {
  return solarsystem_ephem_hp(jd_tdb, body, origin, position, velocity);
}
#else
short int solarsystem(double jd_tdb, enum novas_major_planet body, enum novas_origin origin, double *position, double *velocity) {
  return solarsystem_ephem(jd_tdb, body, origin, position, velocity);
}

short int solarsystem_hp(const double jd_tdb[2], enum novas_major_planet body, enum novas_origin origin, double *position, double *velocity) {
  return solarsystem_ephem_hp(jd_tdb, body, origin, position, velocity);
}
#endif

