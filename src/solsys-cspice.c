/**
 * @file
 *
 * @author A. Kovacs
 * @since 1.2
 *
 *  SuperNOVAS Solar-system ephemeris lookup implementation via the NAIF CSPICE library.
 *
 *  This is an optional component of SuperNOVAS, which interfaces to the NAIF CSPICE Toolkit.
 *  As such, you may need the CSPICE runtime libraries in an accessible location (such
 *  as in `/usr/lib`), and you will need development files (C headers under a `cspice/`
 *  sub-folder, such as in `/usr/include/cspice/`; and unversioned libraries) to build. Thus,
 *  this module is compiled only if `CSPICE_SUPPORT` is set to 1 prior to the build.
 *
 *  Before building SuperNOVAS against CSPICE, you might want to check out the
 *  `Smithsonian/cspice-sharedlib` repository on Github to facilitate the building of CSPICE
 *  as a shared library instead of the static library built by the uptream distribution.
 *
 *  REFERENCES:
 *  <ol>
 *  <li>The NAIF CSPICE Toolkit: https://naif.jpl.nasa.gov/naif/toolkit.html</li>
 *  <li>the Smithsonian/cspice-sharedlib repository: https://github.com/Smithsonian/cspice-sharedlib</li>
 *  </ol>
 *
 * @sa solarsystem.h
 * @sa solsys-calceph.c
 */

#include <string.h>
#include <errno.h>
#include <semaphore.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"
#include "novas-cspice.h"

#include "cspice/SpiceUsr.h"
#include "cspice/SpiceZpr.h"        // for reset_c

/// Multiplicative normalization for the positions returned by km to AU
#define NORM_POS                    (1e3 / NOVAS_AU)

/// Multiplicative normalization for the velocities returned by km/s to AU/day
#define NORM_VEL                    (NORM_POS * 86400.0)
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
 * Supresses CSPICE error output and disables exit on error behavior, so we can check and process
 * CSPICE errors gracefully ourselves.
 *
 * REFERENCES:
 * <ol>
 * <li>https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/cspice/erract_c.html</li>
 * <li>https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/cspice/errprt_c.html</li>
 * </ol>
 *
 * @return 0
 */
static int suppress_cspice_errors() {
  erract_c("SET", 0, "RETURN");       // Do not exit in case of CSPICE errors.
  errprt_c("SET", 0, "NONE");         // Suppress CSPICE error messages
  return 0;
}

/**
 * Returns a short description of the CSPICE error in the supplied buffer, and resets the
 * CSPICE error state.
 *
 * @param[out] msg  the buffer in which to return the message.
 * @param len       (bytes) maximum length of the message to return including termination
 * @return          the CSPICE error code.
 */
static int get_cspice_error(char *msg, int len) {
  int err = return_c();
  getmsg_c("SHORT", len, msg);
  reset_c();
  return err;
}

/**
 * Adds a SPICE kernel to the currently managed open kernels. Subsequent ephemeris lookups through
 * CSPICE will use the added kernel. It's simply a wrapper around the CSPICE `furnsh_c()` routine,
 * with graceful error handling. You can of course add kernels using `furnsh_c()` directly to the
 * same effect.
 *
 * REFERENCES:
 * <ol>
 * <li>https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/cspice/furnsh_c.html</li>
 * </ol>
 *
 * @param filename      The fully qualified path to the ephemeris kernel data (e.g.
 *                      "/data/ephem/de440s.bsp")
 * @return              0 if successful, or else -1 if there was an error (errno will be set to
 *                      EINVAL).
 *
 * @sa cspice_remove_kernel()
 * @sa novas_use_cspice()
 *
 * @author Attila Kovacs
 * @since 1.2
 */
int cspice_add_kernel(const char *filename) {
  static const char *fn = "cspice_add_kernel";

  char msg[100];
  int err;

  if(!filename)
    return novas_error(-1, EINVAL, fn, "input filename is NULL");
  if(!filename[0])
    return novas_error(-1, EINVAL, fn, "input filename is empty");

  suppress_cspice_errors();

  prop_error(fn, mutex_lock(), 0);
  reset_c();
  furnsh_c(filename);
  err = get_cspice_error(msg, sizeof(msg));
  mutex_unlock();

  if(err)
    return novas_error(-1, EINVAL, fn, "furnsh_c(%s): %s", filename, msg);

  return 0;
}

/**
 * Removes a SPICE kernel from the currently managed open kernels. Subsequent ephemeris lookups
 * through CSPICE will not use the removed kernel data. It's simply a wrapper around the CSPICE
 * `unload_c()` routine, with graceful error handling. You can of course remove kernels using
 * `unload_c()` directly to the same effect.
 *
 * REFERENCES:
 * <ol>
 * <li>https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/cspice/unload_c.html</li>
 * </ol>
 *
 * @param filename      The fully qualified path to the ephemeris kernel data (e.g.
 *                      "/data/ephem/de440s.bsp")
 * @return              0 if successful, or else -1 if there was an error (errno will be set to
 *                      EINVAL).
 *
 * @sa cspice_add_kernel()
 *
 * @author Attila Kovacs
 * @since 1.2
 */
int cspice_remove_kernel(const char *filename) {
  static const char *fn = "cspice_remove_kernel";

  char msg[100];
  int err;

  if(!filename)
    return novas_error(-1, EINVAL, fn, "input filename is NULL");
  if(!filename[0])
    return novas_error(-1, EINVAL, fn, "input filename is empty");

  suppress_cspice_errors();

  prop_error(fn, mutex_lock(), 0);
  reset_c();
  unload_c(filename);
  err = get_cspice_error(msg, sizeof(msg));
  mutex_unlock();

  if(err)
    return novas_error(-1, EINVAL, fn, "unload_c(%s): %s", filename, msg);

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
 *  <li>https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/cspice/spkez_c.html</li>
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

  char msg[100];
  SpiceDouble pv[6];
  SpiceDouble lt;
  SpiceInt target, center;
  double tdb2000;   // seconds past J2000 TDB
  int i, err;

  if(!jd_tdb)
    return novas_error(-1, EINVAL, fn, "jd_tdb input time array is NULL.");

  target = novas_to_naif_planet(body);
  if(target < 0)
    return novas_trace(fn, 1, 0);

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
  err = get_cspice_error(msg, sizeof(msg));

  if(err) {
    SpiceInt alt = novas_to_dexxx_planet(body);
    if(alt != target) {
      // Try with DExxx ID (barycenter vs planet center)
      spkez_c(alt, tdb2000, "J2000", "NONE", center, pv, &lt);
      err = get_cspice_error(msg, sizeof(msg));
    }
  }

  mutex_unlock(sem);

  if(err)
    return novas_error(3, EAGAIN, fn, "spkez_c(NOVAS ID=%d, JD=%.1f): %s", body, (jd_tdb[0] + jd_tdb[1]), msg);

  for(i = 3; --i >= 0;) {
    if(position)
      position[i] = pv[i] * NORM_POS;
    if(velocity)
      velocity[i] = pv[3 + i] * NORM_VEL;
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
 * REFERENCES:
 * <ol>
 * <li>https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/cspice/spkez_c.html</li>
 * </ol>
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

  char msg[100];
  SpiceDouble pv[6];
  SpiceDouble lt;
  SpiceInt target, center;
  double tdb2000;
  int i, err;

  if(id == -1) {
    // Lookup by name...
    SpiceInt spiceCode = 0;
    SpiceBoolean spiceFound = 0;

    if(!name)
      return novas_error(-1, EINVAL, fn, "id=-1 and name is NULL");

    if(!name[0])
      return novas_error(-1, EINVAL, fn, "id=-1 and name is empty");

    reset_c();
    bodn2c_c(name, &spiceCode, &spiceFound);
    err = get_cspice_error(msg, sizeof(msg));

    if(!spiceFound)
      return novas_error(1, EINVAL, fn, "CSPICE could not find a NAIF ID for '%s'", name);

    if(err)
      return novas_error(1, EINVAL, fn, "CSPICE name lookup error for '%s': %s", name, msg);

    id = spiceCode;
  }

  target = id;

  // Always return positions and velocities w.r.t. the SSB
  if(origin)
    *origin = NOVAS_SSB;

  center = NAIF_SSB;

  tdb2000 = (jd_tdb_high + jd_tdb_low - NOVAS_JD_J2000) * 86400.0;

  prop_error(fn, mutex_lock(), 0);

  // See https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/frames.html#Reference%20Frames
  // "J2000" and "ICRF" are treated the same, with "J2000" being the compatibility label.
  reset_c();
  spkez_c(target, tdb2000, "J2000", "NONE", center, pv, &lt);
  err = get_cspice_error(msg, sizeof(msg));

  mutex_unlock();

  if(err)
    return novas_error(3, EAGAIN, fn, "spkez_c(name='%s', NAIF=%ld, JD=%.1f): %s",
            name ? name : "<null>", id, (jd_tdb_high + jd_tdb_low), msg);

  for(i = 3; --i >= 0;) {
    if(pos)
      pos[i] = pv[i] * NORM_POS;
    if(vel)
      vel[i] = pv[3 + i] * NORM_VEL;
  }

  return 0;
}

/**
 * Sets a ephemeris provider for NOVAS_EPHEM_OBJECT types using the NAIF CSPICE library.
 *
 * CSPICE is configured to suppress error messages and to not exit on errors, since we will check
 * errors and handle them ourselves. You can adjust the behavior after this call with the
 * CSPICE errprt_c() and erract_c() functions, respectively.
 *
 * @return  0
 *
 * @sa novas_use_cspice_planets()
 * @sa novas_use_cspice()
 * @sa cspice_add_kernel()
 * @sa set_ephem_provider()
 *
 * @author Attila Kovacs
 * @since 1.2
 */
int novas_use_cspice_ephem() {
  suppress_cspice_errors();
  set_ephem_provider(novas_cspice);
  return 0;
}

/**
 * Sets CSPICE as the ephemeris provider for the major planets (and Sun, Moon, SSB...) using the
 * NAIF CSPICE library.
 *
 * CSPICE is configured to suppress error messages and to not exit on errors, since we will check
 * errors and handle them ourselves. You can adjust the behavior after this call with the
 * CSPICE errprt_c() and erract_c() functions, respectively.
 *
 * @return  0
 *
 * @sa novas_use_cspice_ephem()
 * @sa novas_use_cspice()
 * @sa cspice_add_kernel()
 * @sa set_planet_provider()
 * @sa set_planet_provider_hp()
 *
 * @author Attila Kovacs
 * @since 1.2
 */
int novas_use_cspice_planets() {
  suppress_cspice_errors();
  set_planet_provider_hp(planet_cspice_hp);
  set_planet_provider(planet_cspice);
  return 0;
}

/**
 * Sets CSPICE as the default ephemeris provider for all types of Solar-system objects (both NOVAS_PLANET and
 * NOVAS_EPHEM_OBJECT types).
 *
 * CSPICE is configured to suppress error messages and to not exit on errors, since we will check
 * errors and handle them ourselves. You can adjust the behavior after this call with the
 * CSPICE errprt_c() and erract_c() functions, respectively.
 *
 * @return  0
 *
 * @sa novas_use_cspice_planets()
 * @sa novas_use_cspice_ephem()
 * @sa cspice_add_kernel()
 *
 * @author Attila Kovacs
 * @since 1.2
 */
int novas_use_cspice() {
  novas_use_cspice_planets();
  novas_use_cspice_ephem();
  return 0;
}



