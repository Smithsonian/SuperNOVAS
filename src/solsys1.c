/**
 * @file
 *
 * @author G. Kaplan and A. Kovacs
 * @version 0.9.0
 *
 *  SuperNOVAS major planet ephemeris lookup implementation using the 1997 ephemeris model, to be used together
 *  with eph_manager.c. A more generic solution is to implement a novas_ephem_reader_func (e.g. relying on the current
 *  version of the CSPICE library) and set it as the default ephemeris handler via set_ephem_reader(), and then
 *  use solsys-ephem.c instead to use the same implementation for major planets.
 *
 *  Based on the NOVAS C Edition, Version 3.1,  U. S. Naval Observatory
 *  Astronomical Applications Dept.
 *  Washington, DC
 *  <a href="http://www.usno.navy.mil/USNO/astronomical-applications">http://www.usno.navy.mil/USNO/astronomical-applications</a>
 *
 *  @sa solsys-ephem.c
 */

#include "novas.h"
#include "eph_manager.h"


/// \cond PRIVATE
#define T0        NOVAS_JD_J2000
/// \endcond



/**
 * Provides an interface between the JPL direct-access solar system
 * ephemerides and NOVAS-C for highest precision applications.
 *
 * This function and planet_ephemeris() were designed
 * to work with the 1997 version of the JPL ephemerides, as
 * noted in the references.
 *
 * The user must create the binary ephemeris files using
 * software from JPL, and open the file using function
 * ephem_open(), prior to calling this function.
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
 * @param body           Major planet number (or that for Sun, Moon, or Solar-system barycenter)
 * @param origin         NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1), or 2 for Earth geocenter
 *                       -- relative to which to report positions and velocities.
 * @param position       [AU] Position vector of 'body' at jd_tdb; equatorial rectangular
 *                       coordinates in AU referred to the ICRS.
 * @param velocity       [AU/day] Velocity vector of 'body' at jd_tdb; equatorial rectangular
 *                       system referred to the ICRS, in AU/day.
 * @return               0 if successful, or else an error code of solarsystem_hp().
 *
 * @sa planet_eph_manager()
 * @sa planet_ephemeris()
 * @sa ephem_open()
 * @sa set_planet_calc()
 *
 * @since 1.0
 */
short planet_eph_manager_hp (const double jd_tdb[2], enum novas_planet body,
        enum novas_origin origin,
        double *position, double *velocity) {
  short target, center = 0;

  /*
   Perform sanity checks on the input body and origin.
   */

  if (body < 1 || body >= NOVAS_PLANETS)
    return 1;
  else if (origin < 0 || origin > 2)
    return 2;

  /*
   Select 'target' according to value of 'body'.
   */

  switch (body)
  {
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

  if (origin == NOVAS_BARYCENTER)
    center = 11;
  else if (origin == NOVAS_HELIOCENTER)
    center = 10;
  else if (origin == 2)
    center = 2;

  /*
   Obtain position and velocity vectors.  The Julian date is split
   between two double-precision elements for highest precision.
   */

  planet_ephemeris (jd_tdb, target, center, position,velocity);

  return 0;
}

/**
 * Provides an interface between the JPL direct-access solar system
 * ephemerides and NOVAS-C for regular (reduced) precision applications.
 *
 * This function and planet_ephemeris() were designed
 * to work with the 1997 version of the JPL ephemerides, as
 * noted in the references.
 *
 * The user must create the binary ephemeris files using
 * software from JPL, and open the file using function
 * ephem_open(), prior to calling this
 * function.
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
 * @param body           Major planet number (or that for Sun, Moon, or Solar-system barycenter)
 * @param origin         NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1), or 2 for Earth geocenter
 *                       -- relative to which to report positions and velocities.
 * @param position       [AU] Position vector of 'body' at jd_tdb; equatorial rectangular
 *                       coordinates in AU referred to the ICRS.
 * @param velocity       [AU/day] Velocity vector of 'body' at jd_tdb; equatorial rectangular
 *                       system referred to the ICRS, in AU/day.
 * @return               0 if successful, or else an error code of solarsystem().
 *
 * @sa planet_eph_manager_hp()
 * @sa planet_ephemeris()
 * @sa ephem_open()
 * @sa set_planet_calc()
 * @sa solarsystem()
 *
 * @since 1.0
 */
short planet_eph_manager(double jd_tdb, enum novas_planet body, enum novas_origin origin,
        double *position, double *velocity) {
  const double tjd[2] = {jd_tdb, 0.0 };
  return planet_eph_manager_hp(tjd, body, origin, position, velocity);
}



#if DEFAULT_SOLSYS == 1
novas_planet_calculator default_planetcalc = planet_eph_manager;
novas_planet_calculator_hp default_planetcalc_hp = planet_eph_manager_hp;
#elif !BUILTIN_SOLSYS1
short solarsystem(double jd_tdb, short body, short origin, double *position, double *velocity) {
  return planet_eph_manager(jd_tdb, body, origin, position, velocity);
}

short solarsystem_hp(const double jd_tdb[2], short body, short origin, double *position, double *velocity) {
  return planet_eph_manager_hp(jd_tdb, body, origin, position, velocity);
}
#endif