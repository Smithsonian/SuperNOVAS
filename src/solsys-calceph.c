/**
 * @file
 *
 * @author A. Kovacs
 *
 *  SuperNOVAS major planet ephemeris lookup implementation via the CALCEPH C library
 *  See https://calceph.imcce.fr/docs/4.0.0/html/c/
 *
 *  CALCEPH source code is at https://gitlab.obspm.fr/imcce_calceph/calceph
 *
 */

#include <string.h>
#include <errno.h>
#include <semaphore.h>

/// \cond PRIVATE
#define USE_CALCEPH 1               ///< NOVAS CALCEPH integration prototypes
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"
#include "calceph.h"

#define CALCEPH_MOON                10  ///< Moon in CALCEPH
#define CALCEPH_SUN                 11  ///< Sun in CALCEPH
#define CALCEPH_SSB                 12  ///< Solar-system Barycenter in CALCEPH

/// Distance and time units to use for CALCEPH (AU would be conventient, but is not available
/// unless defined in the sphemeris file(s) themselves.
#define CALCEPH_UNITS               (CALCEPH_UNIT_KM | CALCEPH_UNIT_DAY)

/// Multiplicative normalization for the positions returned by CALCEPH to AU
#define NORM_POS                    (1e3 / NOVAS_AU)

/// Multiplicative normalization for the velocities returned by CALCEPH to AU/day
#define NORM_VEL                    (NORM_POS)

/// Whether to force serialized (non-parallel CALCEPH queries)
int serialized_calceph_queries;

/// \endcond

/// CALCEPH ephemeris specifically for planets (and Sun and Moon) only
static t_calcephbin *planets;

/// (boolean) whether the planets ephemeris data is thread safe to access
static int is_thread_safe_planets;

/// Semaphore for thread-safe access of planet ephemeris (if needed)
static sem_t sem_planets;

/// Generic CALCEPH ephemeris files for all types of Solar-system sources
static t_calcephbin *bodies;

/// (boolean) whether the generic solar-system bodies ephemeris data is thread safe to access
static int is_thread_safe_bodies;

/// Semaphore for thread-safe access of generic solar-system bodies ephemeris (if needed)
static sem_t sem_bodies;

/**
 * Provides an interface between the CALCEPG C library and NOVAS-C for regular (reduced) precision
 * applications. The user must set the CALCEPH ephemeris binary data to use using the
 * novas_use_calceph() or novas_use_calceph_planet() to activate the desired CALCEPH ephemeris
 * data prior to use.
 *
 * This call is always thread safe, even when CALCEPH and the ephemeris data may not be. When
 * necessary, the ephemeris access will be mutexed to ensure sequential access under the hood.
 *
 * REFERENCES:
 * <ol>
 *  <li>The CALCEPH C library; https://calceph.imcce.fr/docs/4.0.0/html/c/index.html</li>
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
 * @param origin         NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1)
 *                       -- relative to which to report positions and velocities.
 * @param[out] position  [AU] Position vector of 'body' at jd_tdb; equatorial rectangular
 *                       coordinates in AU referred to the ICRS.
 * @param[out] velocity  [AU/day] Velocity vector of 'body' at jd_tdb; equatorial rectangular
 *                       system referred to the ICRS, in AU/day.
 * @return               0 if successful, or else 1 if the 'body' is invalid, or 2 if the
 *                       'origin' is invalid, or 3 if there was an error providing ephemeris
 *                       data.
 *
 * @sa planet_calceph
 * @sa novas_use_calceph()
 * @sa novas_use_calceph_planet()
 *
 * @author Attila Kovacs
 * @since 1.2
 */
static short planet_calceph_hp(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin, double *position,
        double *velocity) {
  static const char *fn = "planet_calceph_hp";

  sem_t *sem = (planets == bodies) ? &sem_bodies : &sem_planets;
  const int lock = !is_thread_safe_planets || serialized_calceph_queries;
  double pv[6] = {};
  int i, target, center, success;

  if(!jd_tdb)
    return novas_error(-1, EINVAL, fn, "jd_tdb input time array is NULL.");

  switch(body) {
    case NOVAS_SSB:
      target = CALCEPH_SSB;
      break;
    case NOVAS_SUN:
      target = CALCEPH_SUN;
      break;
    case NOVAS_MOON:
      target = CALCEPH_MOON;
      break;
    default:
      if (body < NOVAS_MERCURY || body > NOVAS_PLUTO)
        return novas_error(1, EINVAL, fn, "Invalid major planet: %d", body);
      target = body;
  }

  switch(origin) {
    case NOVAS_BARYCENTER:
      center = CALCEPH_SSB;
      break;
    case NOVAS_HELIOCENTER:
      center = CALCEPH_SUN;
      break;
    default:
      return novas_error(2, EINVAL, fn, "Invalid origin type: %d", origin);
  }

  if(lock)
    if(sem_wait(sem) != 0)
      return novas_error(-1, errno, fn, "sem_wait()");

  success = calceph_compute_unit(planets, jd_tdb[0], jd_tdb[1], target, center, CALCEPH_UNITS, pv);

  if(lock)
    sem_post(sem);

  if(!success)
    return novas_error(3, EAGAIN, fn, "calceph_compute() failure (NOVAS ID=%d)", body);

  for(i = 3; --i >= 0;) {
    if(position) position[i] = pv[i] * NORM_POS;
    if(velocity) velocity[i] = pv[3 + i] * NORM_VEL;
  }

  return 0;
}

/**
 * Provides an interface between the CALCEPG C library and NOVAS-C for regular (reduced) precision
 * applications. The user must set the CALCEPH ephemeris binary data to use using the
 * novas_use_calceph() or novas_use_calceph_planet() to activate the desired CALCEPH ephemeris
 * data prior to use.
 *
 * This call is always thread safe, even when CALCEPH and the ephemeris data may not be. When
 * necessary, the ephemeris access will be mutexed to ensure sequential access under the hood.
 *
 * REFERENCES:
 * <ol>
 *  <li>The CALCEPH C library; https://calceph.imcce.fr/docs/4.0.0/html/c/index.html</li>
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
 * @author Attila Kovacs
 * @since 1.2
 */
static short planet_calceph(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position,
        double *velocity) {
  const double tjd[2] = { jd_tdb, 0.0 };

  prop_error("planet_calceph", planet_calceph_hp(tjd, body, origin, position, velocity), 0);
  return 0;
}

/**
 * Generic ephemeris handling via the CALCEPH C library. This call is always thread safe, even
 * when CALCEPH and the ephemeris data may not be. When necessary, the ephemeris access will be
 * mutexed to ensure sequential access under the hood.
 *
 * @param name          The name of the solar-system body. It is important only if the id is
 *                      -1.
 * @param id            The NAIF ID number of the solar-system body for which the position in
 *                      desired, or -1 if the name should be used instead to identify the
 *                      object.
 * @param jd_tdb_high   [day] The high-order part of Barycentric Dynamical Time (TDB) based
 *                      Julian date for which to find the position and velocity. Typically
 *                      this may be the integer part of the Julian date for high-precision
 *                      calculations, or else the entire Julian date for reduced precision.
 * @param jd_tdb_low    [day] The low-order part of Barycentric Dynamical Time (TDB) based
 *                      Julian date for which to find the position and velocity. Typically
 *                      this may be the fractional part of the Julian date for high-precision
 *                      calculations, or else 0.0 if the date is defined entirely by the
 *                      high-order component for reduced precision.
 * @param[out] origin   Set to NOVAS_BARYCENTER or NOVAS_HELIOCENTER to indicate relative to
 *                      which the ephemeris positions/velocities are reported.
 * @param[out] pos      [AU] position 3-vector to populate with rectangular equatorial
 *                      coordinates in AU. It may be NULL if position is not required.
 * @param[out] vel      [AU/day] velocity 3-vector to populate in rectangular equatorial
 *                      coordinates in AU/day. It may be NULL if velocities are not required.
 * @return              0 if successful, -1 if any of the pointer arguments are NULL, or some
 *                      non-zero value if the was an error s.t. the position and velocity
 *                      vector should not be used.
 *
 * @sa set_ephem_provider()
 * @sa ephemeris()
 * @sa NOVAS_EPHEM_OBJECT
 *
 *@author Attila Kovacs
 * @since 1.2
 */
static int novas_calceph(const char *name, long id, double jd_tdb_high, double jd_tdb_low, enum novas_origin *origin, double *pos, double *vel) {
  static const char *fn = "novas_calceph";

  double pv[6] = {};
  const int lock = !is_thread_safe_bodies || serialized_calceph_queries;
  int i, success;

  if(id == -1) {
    // Use name to get NAIF ID.
    int iid;
    if(!calceph_getidbyname(bodies, name, CALCEPH_USE_NAIFID, &iid))
      return novas_error(1, EINVAL, fn, "CALCEPH could not find a NAIF ID for '%s'", name);
    id = iid;
  }

  // Always return psoitions and velocities w.r.t. the SSB
  if(origin)
    *origin = NOVAS_SSB;

  if(lock)
    if(sem_wait(&sem_bodies) != 0)
      return novas_error(-1, errno, fn, "sem_wait()");

  success = calceph_compute_unit(bodies, jd_tdb_high, jd_tdb_low, id, NOVAS_SSB, (CALCEPH_USE_NAIFID | CALCEPH_UNITS), pv);

  if(lock)
    sem_post(&sem_bodies);

  if(!success)
    return novas_error(3, EAGAIN, fn, "calceph_compute() failure");

  for(i = 3; --i >= 0;) {
    if(pos) pos[i] = pv[i] * NORM_POS;
    if(vel) vel[i] = pv[3 + i] * NORM_VEL;
  }

  return 0;
}

/**
 * Sets a ephemeris provider for Solar-system objects using the CALCEPH C library and the specified set of
 * ephemeris files. If the supplied ephemeris files contain data for major planets also, they can be used
 * by planet_calceph() / planet_calceph_hp() also, unless a separate CALCEPH ephemeris data is set via
 * novas_use_calceph_planets().
 *
 * The call also make CALCEPH the default ephemeris provider for all types of Solar-system objects. If you
 * want to use another provider for major planets, you need to call set_planet_provider() /
 * set_planet_provider_hp() afterwards to specify a different provider for major planets (and Sun, Moon, SSB).
 *
 * @param eph   Pointer to the CALCEPH ephemeris data that have been opened.
 * @return  0 if successful, or else -1 (errno will indicate the type of error).
 *
 * @sa novas_use_calceph_planets()
 * @sa set_ephem_provider()
 *
 * @author Attila Kovacs
 * @since 1.2
 */
int novas_use_calceph(t_calcephbin *eph) {
  static const char *fn = "novas_use_calceph";

  if(!eph)
    return novas_error(-1, EINVAL, fn, "input ephemeris data is NULL");

  if(!calceph_prefetch(eph))
    return novas_error(-1, EAGAIN, fn, "calceph_prefetch() failed");

  // If first time, then initialize the bodies semaphore
  if(!bodies)
    sem_init(&sem_bodies, 0, 1);

  // Make sure we don't change the ephemeris provider while using it
  if(sem_wait(&sem_bodies) != 0)
    return novas_error(-1, errno, fn, "sem_wait()");

  is_thread_safe_bodies = calceph_isthreadsafe(eph);
  bodies = eph;
  sem_post(&sem_bodies);

  // Use CALCEPH as the default minor body ephemeris provider
  set_ephem_provider(novas_calceph);

  // If no planet provider is set (yet) use the same ephemeris for planets too
  // atleast until a dedicated planet provider is set.
  if (!planets) novas_use_calceph_planets(eph);

  return 0;
}

/**
 * Sets a ephemeris provider for the major planets (and Sun, Moon, and SSB) using the CALCEPH C library and
 * the specified ephemeris data.
 *
 * The call also make CALCEPH the default ephemeris providers for major planets.
 *
 * @param eph   Pointer to the CALCEPH ephemeris data for the major planets (including Sun, Moon, and SSB) that
 *              have been opened.
 * @return  0 if successful, or else -1 (errno will indicate the type of error).
 *
 * @sa novas_use_calceph()
 * @sa set_planet_provider()
 * @sa set_planet_provider_hp()
 *
 * @author Attila Kovacs
 * @since 1.2
 */
int novas_use_calceph_planets(t_calcephbin *eph) {
  static const char *fn = "novas_use_calceph_planets";

  if(!eph)
    return novas_error(-1, EINVAL, fn, "input ephemeris data is NULL");

  if(!calceph_prefetch(eph))
    return novas_error(-1, EAGAIN, fn, "calceph_prefetch() failed");

  // If first time, then initialize the planet semaphore
  if(!planets)
    sem_init(&sem_planets, 0, 1);

  // Make sure we don't change the ephemeris provider while using it
  if(sem_wait(&sem_planets) != 0)
    return novas_error(-1, errno, fn, "sem_wait()");

  is_thread_safe_planets = calceph_isthreadsafe(eph);
  planets = eph;
  sem_post(&sem_planets);

  // Use calceph as the default NOVAS planet provider
  set_planet_provider_hp(planet_calceph_hp);
  set_planet_provider(planet_calceph);

  return 0;
}
