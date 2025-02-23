/**
 * @file
 *
 * @author G. Kaplan and A. Kovacs
 *
 *  SuperNOVAS major planet ephemeris lookup wrapper for JPL ephemerides accessed over `pleph()`.
 *  It is a top-level wrapper, with an  intermediate Fortran routine, jplint_() providing the
 *  interface between the NOVAS planet calculator functions here and the pleph() call of the JPL
 *  library. (The sample source code of jplint.f is included in the distribution).
 *
 *  For supporting JPL ephemerides more generally, including for satellites, asteroids, and
 *  comets, you are probably better off using planet_ephem_provider(), and provide an interface,
 *  e.g. to the SPICE library, via novas_ephem_provider instead, which you can then activate
 *  at runtime with set_planet_provider().
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

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"

#define T0        NOVAS_JD_J2000
/// \endcond

/// Function prototype for the FORTRAN subroutine <code>jplint</code> in jplint.f
extern void jplint_(const double *jd_tdb, long *targ, long *cent, double *posvel, long *err_flg);

/// Function prototype for the FORTRAN subroutine <code>jplihp</code> in jplint.f
extern void jplihp_(const double *jd_tdb, long *targ, long *cent, double *posvel, long *err_flg);

/**
 * Obtains planet positions via the JPL direct-access solar system ephemerides, wtih normal
 * (reduced) precision -- typically good to the milliarcsecond level.
 *
 * It generalizes access to the JPL software by calling a Fortran interface subroutine,
 * 'jplint', instead of making a direct call to the JPL subroutine 'pleph', whose arguments
 * have changed several times throughout the years.  This way, any future change to the
 * arguments can be accommodated in 'jplint' rather than in this function.
 *
 * For supporting JPL ephemerides more generally, including for satellites, asteroids, and
 * comets, you are probably better off using planet_ephem_provider(), and provide an interface,
 * e.g. to the CSPICE library, via novas_ephem_provider instead, which you can then
 * activate dynamically with set_planet_provider().
 *
 * REFERENCES:
 * <ol>
 * <li>JPL. 2007, JPL Planetary and Lunar Ephemerides: Export Information,
 *     (Pasadena, CA: JPL) http://ssd.jpl.nasa.gov/?planet_eph_export.</li>
 * <li>Kaplan, G. H. "NOVAS: Naval Observatory Vector Astrometry
 *     Subroutines"; USNO internal document dated 20 Oct 1988; revised 15 Mar 1990.</li>
 * </ol>
 *
 * @param jd_tdb         [day] Two-element array containing the Julian date, which may be
 *                       split any way (although the first element is usually the
 *                       "integer" part, and the second element is the "fractional"
 *                       part).  Julian date is on the TDB or "T_eph" time scale.
 * @param body           Major planet number (or that for Sun, Moon, SSB..)
 * @param origin         NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1), or 2 for Earth geocenter
 *                       -- relative to which to report positions and velocities.
 * @param[out] position  [AU] Position vector of 'body' at jd_tdb; equatorial rectangular
 *                       coordinates in AU referred to the ICRS.
 * @param[out] velocity  [AU/day] Velocity vector of 'body' at jd_tdb; equatorial rectangular
 *                       system referred to the ICRS, in AU/day.
 * @return               0 if successful, or else 1 if the 'body' or 'origin' argument is
 *                       invalid, or else 2 if the 'jplint_()' call failed.
 *
 * @sa planet_jplint_hp()
 * @sa planet_ephem_provider()
 * @sa set_planet_provider()
 * @sa solarsystem()
 *
 * @since 1.0
 */
short planet_jplint(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *restrict position,
        double *restrict velocity) {
  static const char *fn = "planet_jplint";

  long targ, cent, err_flg = 0;
  double posvel[6] = {0};
  const double jd2[2] = { jd_tdb };
  int i;

  if(!jd_tdb)
    return novas_error(1, EINVAL, fn, "NULL jd_tdb array");

  // Perform sanity checks on the input body and origin.
  if((body < NOVAS_MERCURY) || (body > NOVAS_MOON))
    return novas_error(1, EINVAL, fn, "planet number %d is out of range [%d:%d]", body, NOVAS_MERCURY, NOVAS_MOON);

  // Select 'targ' according to the value of 'body'.
  if(body == NOVAS_SUN)
    targ = 11L;
  else if(body == NOVAS_MOON)
    targ = 10L;
  else
    targ = (long) body;

  // Select 'cent' according to the value of 'origin'.
  if(origin == NOVAS_BARYCENTER)
    cent = 12L;
  else if(origin == NOVAS_HELIOCENTER)
    cent = 11L;
  else
    return novas_error(1, EINVAL, fn, "invalid origin type %d", origin);

  // Call Fortran subroutine 'jplihp' to obtain position and velocity
  // array 'posvel'.  This is the only point in the NOVAS-C package
  // where the Fortran/C interface occurs.
  // Note that arguments must be sent to Fortran by reference, not by
  // value.
  jplint_(jd2, &targ, &cent, posvel, &err_flg);
  if(err_flg)
    return novas_error(2, EAGAIN, fn, "FORTRAN jplint_() error: %ld", err_flg);

  // Decompose 'posvel' into 'position' and 'velocity'.
  for(i = 3; --i >= 0;) {
    position[i] = posvel[i];
    velocity[i] = posvel[i + 3];
  }

  return 0;
}

/**
 * Obtains planet positions via the JPL direct-access solar system ephemerides, wtih high
 * precision -- typically good to below the microarcsecond level.
 *
 * It generalizes access to the JPL software by calling a Fortran interface subroutine,
 * 'jplint', instead of making a direct call to the JPL subroutine 'pleph', whose arguments
 * have changed several times throughout the years.  This way, any future change to the
 * arguments can be accommodated in 'jplint' rather than in this function.
 *
 * For supporting JPL ephemerides more generally, including for satellites, asteroids, and
 * comets, you are probably better off using planet_ephem_provider(), and provide an interface,
 * e.g. to the CSPICE library, via novas_ephem_provider instead, which you can then
 * activate dynamically with set_planet_provider().
 *
 * REFERENCES:
 * <ol>
 * <li>JPL. 2007, JPL Planetary and Lunar Ephemerides: Export Information,
 *     (Pasadena, CA: JPL) http://ssd.jpl.nasa.gov/?planet_eph_export.</li>
 * <li>Kaplan, G. H. "NOVAS: Naval Observatory Vector Astrometry
 *     Subroutines"; USNO internal document dated 20 Oct 1988; revised 15 Mar 1990.</li>
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
 * @sa planet_jplint()
 * @sa planet_ephem_provider_hp()
 * @sa set_planet_provider_hp()
 * @sa solarsystem_hp()
 *
 * @since 1.0
 */
short planet_jplint_hp(const double jd_tdb[restrict 2], enum novas_planet body, enum novas_origin origin,
        double *restrict position, double *restrict velocity) {
  static const char *fn = "planet_jplint_hp";

  long targ, cent, err_flg = 0;
  double posvel[6] = {0};
  int i;

  if(!jd_tdb)
    return novas_error(1, EINVAL, fn, "NULL jd_tdb array");

  // Perform sanity checks on the input body and origin.
  if((body < NOVAS_MERCURY) || (body > NOVAS_MOON))
    return novas_error(1, EINVAL, fn, "planet number %d is out of range [%d:%d]", body, NOVAS_MERCURY, NOVAS_MOON);

  // Select 'targ' according to the value of 'body'.
  if(body == NOVAS_SUN)
    targ = 11L;
  else if(body == NOVAS_MOON)
    targ = 10L;
  else
    targ = (long) body;

  // Select 'cent' according to the value of 'origin'.
  if(origin == NOVAS_BARYCENTER)
    cent = 12L;
  else if(origin == NOVAS_HELIOCENTER)
    cent = 11L;
  else
    return novas_error(1, EINVAL, fn, "invalid origin type %d", origin);

  // Call Fortran subroutine 'jplihp' to obtain position and velocity
  // array 'posvel'.  This is the only point in the NOVAS-C package
  // where the Fortran/C interface occurs.
  // Note that arguments must be sent to Fortran by reference, not by
  // value.
  jplihp_(jd_tdb, &targ, &cent, posvel, &err_flg);
  if(err_flg)
    return novas_error(2, EAGAIN, fn, "FORTRAN jplint_() error: %ld", err_flg);

  // Decompose 'posvel' into 'position' and 'velocity'.
  for(i = 3; --i >= 0;) {
    position[i] = posvel[i];
    velocity[i] = posvel[i + 3];
  }

  return 0;
}

#if DEFAULT_SOLSYS == 2
/// \cond PRIVATE
novas_planet_provider planet_call = planet_jplint;
novas_planet_provider_hp planet_call_hp = planet_jplint;
/// \endcond
#elif !BUILTIN_SOLSYS2
short solarsystem(double jd_tdb, short body, short origin, double *restrict position, double *restrict velocity) {
  prop_error("solarsystem", planet_jplint(jd_tdb, body, origin, position, velocity), 0);
  return 0;
}

short solarsystem_hp(const double jd_tdb[restrict 2], short body, short origin, double *restrict position, double *restrict velocity) {
  prop_error("solarsystem_hp", planet_jplint_hp(jd_tdb, body, origin, position, velocity), 0);
  return 0;
}
#endif
