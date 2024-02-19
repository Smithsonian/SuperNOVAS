/**
 * @file
 *
 * SuperNOVAS major planet ephemeris handler via the same generic ephemeris reader that is configured by set_ephem_provider() prior
 * to calling this routine.
 *
 * @date Created  on Jan 29, 2024
 * @author Attila Kovacs
 *
 * @sa solsys1.c
 * @sa solsys2.c
 */

#include <errno.h>

#include "novas.h"

/**
 * Major planet ephemeris data via the same generic ephemeris provider that is configured by
 * set_ephem_provider() prior to calling this routine. This is the highest precision version.
 *
 * @param jd_tdb         [day] Barycentric Dynamical Time (TDB) based Julian date, split into
 *                       high and low order components (e.g. integer and fractional parts) for
 *                       high-precision calculations.
 * @param body           Major planet number (or that for Sun, Moon, or Solar-system barycenter)
 * @param origin         NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1) relative to which to
 *                       report positions and velocities.
 * @param[out] position  [AU] Position vector of 'body' at jd_tdb; equatorial rectangular
 *                       coordinates in AU referred to the ICRS.
 * @param[out] velocity  [AU/day] Velocity vector of 'body' at jd_tdb; equatorial rectangular
 *                       system referred to the ICRS, in AU/day.
 * @return               0 if successful, or else an error code of solarsystem_hp().
 *
 * @sa planet_ephem_provider()
 * @sa set_ephem_provider()
 * @sa solarsystem_hp()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
short planet_ephem_provider_hp(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin, double *position, double *velocity) {
  static const char *names[] = NOVAS_PLANET_NAMES_INIT;

  novas_ephem_provider ephem_call;
  enum novas_origin o = NOVAS_BARYCENTER;
  int error;

  if(origin != NOVAS_BARYCENTER && origin != NOVAS_HELIOCENTER) {
    errno = EINVAL;
    return 1;
  }

  if(body < 0 || body >= NOVAS_PLANETS) {
    errno = EINVAL;
    return -1;
  }

  ephem_call = get_ephem_provider();
  if(!ephem_call) {
    errno = ENOSYS;
    return -1;
  }

  error = ephem_call(names[body], body, jd_tdb[0], jd_tdb[1], &o, position, velocity);
  if(error) return 2;

  if(o != origin) {
    double pos0[3], vel0[3];
    int i;
    int ref = (o == NOVAS_BARYCENTER) ? NOVAS_SUN : NOVAS_SSB;

    error = ephem_call(names[ref], ref, jd_tdb[0], jd_tdb[1], &o, pos0, vel0);
    if(error) return 2;

    for(i = 0; i < 3; i++) {
      if(position) position[i] -= pos0[i];
      if(velocity) velocity[i] -= vel0[i];
    }
  }

  return 0;
}

/**
 * Major planet ephemeris data via the same generic ephemeris provider that is configured by
 * set_ephem_provider() prior to calling this routine. This is the regular (reduced) precision
 * version.
 *
 * @param jd_tdb         [day] Barycentric Dynamical Time (TDB) based Julian date.
 * @param body           Major planet number (or that for Sun, Moon, or Solar-system barycenter)
 * @param origin         NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1) relative to which to
 *                       report positions and velocities.
 * @param[out] position  [AU] Position vector of 'body' at jd_tdb; equatorial rectangular
 *                       coordinates in AU referred to the ICRS.
 * @param[out] velocity  [AU/day] Velocity vector of 'body' at jd_tdb; equatorial rectangular
 *                       system referred to the ICRS, in AU/day.
 * @return               0 if successful, or else an error code of solarsystem().
 *
 * @sa planet_ephem_provider_hp()
 * @sa set_ephem_provider()
 * @sa solarsystem()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
short planet_ephem_provider(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position, double *velocity) {
  const double jd_tdb2[2] = { jd_tdb };
  return planet_ephem_provider_hp(jd_tdb2, body, origin, position, velocity);
}

#if !BUILTIN_SOLSYS_EPHEM_READER
short solarsystem(double jd_tdb, short body, short origin, double *position, double *velocity) {
  return planet_ephem_provider(jd_tdb, body, origin, position, velocity);
}

short solarsystem_hp(const double jd_tdb[2], short body, short origin, double *position, double *velocity) {
  return planet_ephem_provider_hp(jd_tdb, body, origin, position, velocity);
}
#endif

