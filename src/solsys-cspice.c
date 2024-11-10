/**
 * @file
 *
 * @author A. Kovacs
 *
 *  SuperNOVAS major planet ephemeris lookup implementation via the NAIF CSPICE library
 *  See https://naif.jpl.nasa.gov/naif/toolkit.html
 *
 */

#include <string.h>
#include <errno.h>
#include <semaphore.h>

/// \cond PRIVATE
#define USE_CSIPCE 1                ///< NOVAS cspice integration prototypes
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"
#include "naif.h"

#include "cspice/SpiceUsr.h"
#include "cspice/SpiceZpr.h"        // for reset_c

#define SPICE_FILE_SIZE 128         ///< (bytes) Maximum length of SPICE file names.
#define SPICE_WORD_SIZE 80          ///< (bytes) Maximum length for SPICE strings

/// Multiplicative normalization for the positions returned by km to AU
#define NORM_POS                    (1e3 / NOVAS_AU)

/// Multiplicative normalization for the velocities returned by km/s to AU/day
#define NORM_VEL                    (NORM_POS / 86400.0)
/// \endcond

/// Semaphore for thread-safe access of ephemerides
static sem_t *sem;

static int mutex_lock() {
  if(!sem) {
    sem = (sem_t *) calloc(1, sizeof(sem_t));
    if(!sem) {
      perror("ERROR! solsys-cspice: alloc sem_t");
      exit(errno);
    }
    sem_init(sem, 0, 1);
  }

  if(sem_wait(sem) != 0)
    return novas_error(-1, errno, "mutex_lock()", "sem_wait()");

  return 0;
}

static int mutex_unlock() {
  sem_post(sem);
  return 0;
}

/**
 * Provides an interface between the NAIF CSPICE C library and NOVAS-C for regular (reduced)
 * precision applications. The user must set the cspice ephemeris binary data to use using the
 * novas_use_cspice() or novas_use_cspice_planet() to activate CSPICE as the NOVAS ephemeris
 * provider.
 *
 * This call is generally thread safe (notwithstanding outside access to the ephemeris files),
 * even if CSPICE itself may not be. All ephemeris access will be mutexed to ensure sequential
 * access under the hood.
 *
 * The call will use whatever ephemeris (SPK) files were loaded by the CSPICE library prior
 * to the call (see furnsh_c() function)
 *
 * REFERENCES:
 * <ol>
 *  <li>NAIF CSPICE: https://naif.jpl.nasa.gov/naif/toolkit.html</li>
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
 * @sa planet_cspice
 * @sa novas_use_cspice()
 * @sa novas_use_cspice_planet()
 *
 * @author Attila Kovacs
 * @since 1.2
 */
static short planet_cspice_hp(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin, double *position,
        double *velocity) {
  static const char *fn = "planet_cspice_hp";

  SpiceDouble pv[6];
  SpiceDouble lt;
  SpiceInt target, center;
  double tdb2000;   // seconds past J2000 TDB
  int i, err;

  if(!jd_tdb)
    return novas_error(-1, EINVAL, fn, "jd_tdb input time array is NULL.");

  target = novas_to_naif_planet(body);
  if(target < 0) return novas_trace(fn, 1, 0);

  switch(origin) {
    case NOVAS_BARYCENTER:
      center = NAIF_SSB;
      break;
    case NOVAS_HELIOCENTER:
      center = NAIF_SUN;
      break;
    default:
      return novas_error(2, EINVAL, fn, "Invalid origin type: %d", origin);
  }

  tdb2000 = (jd_tdb[0] + jd_tdb[1] - NOVAS_JD_J2000) * 86400.0;

  prop_error(fn, mutex_lock(sem), 0);

  // Try with proper planet center NAIF ID first...
  //
  // See https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/frames.html#Reference%20Frames
  // "J2000" and "ICRF" are treated the same, with "J2000" being the compatibility label.
  reset_c();
  spkez_c(target, tdb2000, "J2000", "NONE", center, pv, &lt);
  err = return_c();
  reset_c();

  if(err) {
    SpiceInt alt = novas_to_dexxx_planet(body);
    if(alt != target) {
      // Try with DExxx ID (barycenter vs planet center)
      spkez_c(alt, tdb2000, "J2000", "NONE", center, pv, &lt);
      err = return_c();
      reset_c();
    }
  }

  mutex_unlock(sem);

  if(err)
    return novas_error(3, EAGAIN, fn, "spkez_c() error (NOVAS ID=%d)", body);

  for(i = 3; --i >= 0;) {
    if(position) position[i] = pv[i] * NORM_POS;
    if(velocity) velocity[i] = pv[3 + i] * NORM_VEL;
  }

  return 0;
}

/**
 * Provides an interface between the NAIF CSPICE library and NOVAS-C for regular (reduced)
 * precision applications.
 *
 * This call is generally thread safe (notwithstanding outside access to the ephemeris files),
 * even if CSPICE itself may not be. All ephemeris access will be mutexed to ensure sequential
 * access under the hood.
 *
 * The call will use whatever ephemeris (SPK) files were loaded by the CSPICE library prior
 * to the call (see furnsh_c() function)
 *
 * REFERENCES:
 * <ol>
 *  <li>NAIF CSPICE: https://naif.jpl.nasa.gov/naif/toolkit.html</li>
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
static short planet_cspice(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position,
        double *velocity) {
  const double tjd[2] = { jd_tdb, 0.0 };

  prop_error("planet_cspice", planet_cspice_hp(tjd, body, origin, position, velocity), 0);
  return 0;
}

/**
 * Generic ephemeris handling via the NAIF CSPICE library. This call is generally thread safe
 * (notwithstanding outside access to the ephemeris files), even if CSPICE itself may not be.
 * The ephemeris access will be mutexed to ensure sequential access under the hood.
 *
 * The call will use whatever ephemeris (SPK) files were loaded by the CSPICE library prior
 * to the call (see furnsh_c() function)
 *
 * @param name          The name of the solar-system body. It is important only if the 'id' is
 *                      -1.
 * @param id            The NAIF ID number of the solar-system body for which the position in
 *                      desired, or -1 if the 'name' should be used instead to identify the
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
 * @sa novas_cspice_use_ids()
 * @sa set_ephem_provider()
 * @sa ephemeris()
 * @sa NOVAS_EPHEM_OBJECT
 *
 *@author Attila Kovacs
 * @since 1.2
 */
static int novas_cspice(const char *name, long id, double jd_tdb_high, double jd_tdb_low, enum novas_origin *origin, double *pos, double *vel) {
  static const char *fn = "novas_cspice";

  SpiceDouble pv[6];
  SpiceDouble lt;
  SpiceInt target, center;
  double tdb2000;
  int i, err;

  if(id == -1) {
    // Lookup by name...
    SpiceChar spiceName[SPICE_WORD_SIZE] = {};
    SpiceInt spiceCode = 0;
    SpiceBoolean spiceFound = 0;

    if(!name)
      return novas_error(-1, EINVAL, fn, "id=-1 and name is NULL");

    if(!name[0])
      return novas_error(-1, EINVAL, fn, "id=-1 and name is empty");

    // Use name to get NAIF ID.
    strncpy(spiceName, name, sizeof(spiceName) - 1);

    reset_c();
    bodn2c_c(spiceName, &spiceCode, &spiceFound);
    err = return_c();
    reset_c();

    if(!spiceFound)
      return novas_error(1, EINVAL, fn, "CSPICE could not find a NAIF ID for '%s'", name);

    if(err)
      return novas_error(1, EINVAL, fn, "CSPICE name lookup error for '%s'", name);

    id = spiceCode;
  }

  target = id;

  // Always return psoitions and velocities w.r.t. the SSB
  if(origin)
    *origin = NOVAS_SSB;

  center = NAIF_SSB;

  tdb2000 = (jd_tdb_high + jd_tdb_low - NOVAS_JD_J2000) * 86400.0;

  prop_error(fn, mutex_lock(), 0);

  // See https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/frames.html#Reference%20Frames
  // "J2000" and "ICRF" are treated the same, with "J2000" being the compatibility label.
  reset_c();
  spkez_c(target, tdb2000, "J2000", "NONE", center, pv, &lt);
  err = return_c();
  reset_c();

  mutex_unlock();

  if(err)
    return novas_error(3, EAGAIN, fn, "spkez_c() failure (name='%s', NAIF=%ld)", name ? name : "<null>", id);

  for(i = 3; --i >= 0;) {
    if(pos) pos[i] = pv[i] * NORM_POS;
    if(vel) vel[i] = pv[3 + i] * NORM_VEL;
  }

  return 0;
}

/**
 * Sets a ephemeris provider for NOVAS_EPHEM_OBJECT types using the NAIF CSPICE library.
 *
 * @return  0
 *
 * @sa novas_use_cspice_planets()
 * @sa novas_use_cspice()
 * @sa set_ephem_provider()
 *
 * @author Attila Kovacs
 * @since 1.2
 */
int novas_use_cspice_ephem() {
  set_ephem_provider(novas_cspice);
  return 0;
}

/**
 * Sets CSPICE as the ephemeris provider for the major planets (and Sun, Moon, and SSB) using the
 * NAIF CSPICE library.
 *
 * @return  0
 *
 * @sa novas_use_cspice_ephem()
 * @sa novas_use_cspice()
 * @sa set_planet_provider()
 * @sa set_planet_provider_hp()
 *
 * @author Attila Kovacs
 * @since 1.2
 */
int novas_use_cspice_planets() {
  set_planet_provider_hp(planet_cspice_hp);
  set_planet_provider(planet_cspice);
  return 0;
}

/**
 * Sets CSPICE as the default ephemeris provider for all types of Solar-system objects (both NOVAS_PLANET and
 * NOVAS_EPHEM_OBJECT types).
 *
 * @return  0
 *
 * @sa novas_use_cspice_planets()
 * @sa novas_use_cspice_ephem()
 *
 * @author Attila Kovacs
 * @since 1.2
 */
int novas_use_cspice() {
  novas_use_cspice_planets();
  novas_use_cspice_ephem();
  return 0;
}



