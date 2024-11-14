/**
 * @file
 *
 * @author G. Kaplan and A. Kovacs
 *
 *  SuperNOVAS major planet ephemeris lookup implementation using JPL 1997 ephemeris data, to be
 *  used together with eph_manager.c. This is a legacy API, maiunly for supporting older code
 *  written for NOVAS C.
 *
 *  A more generic solution is to implement a novas_ephem_provider (e.g. relying on the current
 *  version of the CSPICE library) and set it as the default ephemeris handler via
 *  set_ephem_provider(), and then use solsys-ephem.c instead to use the same implementation for
 *  major planets.
 *
 *  Based on the NOVAS C Edition, Version 3.1:
 *
 *  U. S. Naval Observatory<br>
 *  Astronomical Applications Dept.<br>
 *  Washington, DC<br>
 *  <a href="http://www.usno.navy.mil/USNO/astronomical-applications">
 *  http://www.usno.navy.mil/USNO/astronomical-applications</a>
 *
 * @sa solarsystem.h
 * @sa solsys-calceph.c
 * @sa solsys-cspice.c
 */

#include <errno.h>

#include "eph_manager.h"

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"

#define T0        NOVAS_JD_J2000
/// \endcond

/**
 * Provides an interface between the JPL direct-access solar system ephemerides and NOVAS-C
 * for highest precision applications.
 *
 * This function and planet_eph_manager() were designed to work with the 1997 version of the
 * JPL ephemerides, as noted in the references.
 *
 * The user must create the binary ephemeris files using software from JPL, and open the file
 * using function ephem_open(), prior to calling this function.
 *
 * REFERENCES:
 * <ol>
 *  <li>JPL. 2007, "JPL Planetary and Lunar Ephemerides: Export Information,"
 *  (Pasadena, CA: JPL) http://ssd.jpl.nasa.gov/?planet_eph_export.</li>
 *  <li>Kaplan, G. H. "NOVAS: Naval Observatory Vector Astrometry
 *  Subroutines"; USNO internal document dated 20 Oct 1988;
 *  revised 15 Mar 1990.</li>
 * </ol>
 *
 * @param jd_tdb         [day] Two-element array containing the Julian date, which may be
 *                       split any way (although the first element is usually the
 *                       "integer" part, and the second element is the "fractional"
 *                       part).  Julian date is on the TDB or "T_eph" time scale.
 * @param body           Major planet number (or that for Sun, Moon, SSB...)
 * @param origin         NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1), or 2 for Earth geocenter
 *                       -- relative to which to report positions and velocities.
 * @param[out] position  [AU] Position vector of 'body' at jd_tdb; equatorial rectangular
 *                       coordinates in AU referred to the ICRS.
 * @param[out] velocity  [AU/day] Velocity vector of 'body' at jd_tdb; equatorial rectangular
 *                       system referred to the ICRS, in AU/day.
 * @return               0 if successful, or else 1 if the 'body' is invalid, or 2 if the
 *                       'origin' is invalid, or 3 if there was an error providing ephemeris
 *                       data.
 *
 * @sa planet_eph_manager
 * @sa planet_ephem_provider_hp()
 * @sa ephem_open()
 * @sa set_planet_provider_hp()
 *
 * @since 1.0
 */
short planet_eph_manager_hp(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin, double *position,
        double *velocity) {
  static const char *fn = "planet_eph_manager_hp";
  short target, center = 0;

  /*
   Perform sanity checks on the input body and origin.
   */

  if(body < 1 || body >= NOVAS_PLANETS)
    return novas_error(1, EINVAL, fn, "input body number %d is out of range [0:%d]", body, NOVAS_PLANETS-1);

  if(origin < 0 || origin > 2)
    return novas_error(2, EINVAL, fn, "invalid origin type: %d", origin);

  /*
   Select 'target' according to value of 'body'.
   */

  switch(body) {
    case NOVAS_SUN:
      target = 10;
      break;
    case NOVAS_MOON:
      target = 9;
      break;
    default:
      target = body - 1;
  }

  /*
   Select 'center' according to the value of 'origin'.
   */

  if(origin == NOVAS_BARYCENTER)
    center = 11;
  else if(origin == NOVAS_HELIOCENTER)
    center = 10;
  else if(origin == 2)
    center = 2;

  /*
   Obtain position and velocity vectors.  The Julian date is split
   between two double-precision elements for highest precision.
   */

  prop_error(fn, planet_ephemeris(jd_tdb, target, center, position, velocity) == 0 ? 0 : 3, 0);

  return 0;
}

/**
 * Provides an interface between the JPL direct-access solar system ephemerides and NOVAS-C
 * for regular (reduced) precision applications.
 *
 * This function and planet_eph_manager_hp() were designed to work with the 1997 version of the JPL
 * ephemerides, as noted in the references.
 *
 * The user must create the binary ephemeris files using software from JPL, and open the file
 * using function ephem_open(), prior to calling this function.
 *
 * REFERENCES:
 * <ol>
 *  <li>JPL. 2007, "JPL Planetary and Lunar Ephemerides: Export Information,"
 *  (Pasadena, CA: JPL) http://ssd.jpl.nasa.gov/?planet_eph_export.</li>
 *  <li>Kaplan, G. H. "NOVAS: Naval Observatory Vector Astrometry
 *  Subroutines"; USNO internal document dated 20 Oct 1988;
 *  revised 15 Mar 1990.</li>
 * </ol>
 *
 * @param jd_tdb         [day] Two-element array containing the Julian date, which may be
 *                       split any way (although the first element is usually the
 *                       "integer" part, and the second element is the "fractional"
 *                       part).  Julian date is on the TDB or "T_eph" time scale.
 * @param body           Major planet number (or that for Sun, Moon, SSB...)
 * @param origin         NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1), or 2 for Earth geocenter
 *                       -- relative to which to report positions and velocities.
 * @param[out] position  [AU] Position vector of 'body' at jd_tdb; equatorial rectangular
 *                       coordinates in AU referred to the ICRS.
 * @param[out] velocity  [AU/day] Velocity vector of 'body' at jd_tdb; equatorial rectangular
 *                       system referred to the ICRS, in AU/day.
 * @return               0 if successful, or else an error code of solarsystem().
 *
 * @sa planet_eph_manager_hp()
 * @sa planet_ephem_provider()
 * @sa ephem_open()
 * @sa set_planet_provider()
 * @sa solarsystem()
 *
 * @since 1.0
 */
short planet_eph_manager(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position,
        double *velocity) {
  const double tjd[2] = { jd_tdb, 0.0 };
  prop_error("planet_eph_manager", planet_eph_manager_hp(tjd, body, origin, position, velocity), 0);
  return 0;
}

#if DEFAULT_SOLSYS == 1
/// \cond PRIVATE
novas_planet_provider planet_call = planet_eph_manager;
novas_planet_provider_hp planet_call_hp = planet_eph_manager_hp;
/// \endcond
#elif !BUILTIN_SOLSYS1
short solarsystem(double jd_tdb, short body, short origin, double *position, double *velocity) {
  prop_error("solarsystem", planet_eph_manager(jd_tdb, body, origin, position, velocity), 0);
  return 0;
}

short solarsystem_hp(const double jd_tdb[2], short body, short origin, double *position, double *velocity) {
  if(!jd_tdb)
    return novas_error(-1, EINVAL, "solarsystem_hp", "NULL jd_tdb 2-component input array");

  prop_error("solarsystem_hp", planet_eph_manager_hp(jd_tdb, body, origin, position, velocity), 0);
  return 0;
}
#endif
