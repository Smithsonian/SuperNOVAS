/**
 * @file
 *
 * @author G. Kaplan and A. Kovacs
 * @version 1.2.0
 *
 *  SuperNOVAS astrometry software based on the Naval Observatory Vector Astrometry Software (NOVAS).
 *  It has been modified to fix outstanding issues and to make it easier to use.
 *
 *  Based on the NOVAS C Edition, Version 3.1:
 *
 *  U. S. Naval Observatory<br>
 *  Astronomical Applications Dept.<br>
 *  Washington, DC<br>
 *  <a href="http://www.usno.navy.mil/USNO/astronomical-applications">
 *  http://www.usno.navy.mil/USNO/astronomical-applications</a>
 */

#include <string.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>   // stdarg.h before stdio.h (for older gcc...)
#include <stddef.h>

#if !COMPAT
#  include <stdio.h>
#  include <stdlib.h>
#  include <ctype.h>
#endif

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"

#define CIO_INTERP_POINTS   6     ///< Number of points to load from CIO interpolation table at once.

#ifndef DEFAULT_SOLSYS
/// Will use solarsystem() and solarsystem_hp() that is linked with application
#  define DEFAULT_SOLSYS    0
#endif

#define C2                  (C * C)   ///< [m<sup>2</sup>/s<sup>2</sup>] Speed of light squared
#define EPREC               1e-12     ///< Required precision for eccentric anomaly in orbital calculation

// <---------- GLOBAL VARIABLES -------------->

#if !DEFAULT_SOLSYS
novas_planet_provider planet_call = (novas_planet_provider) solarsystem;
novas_planet_provider_hp planet_call_hp = (novas_planet_provider_hp) solarsystem_hp;
#endif

/// \endcond

/**
 * Celestial pole offset &psi; for high-precision applications. It was visible to users in NOVAS C 3.1,
 * hence we continue to expose it also for back compatibility.
 *
 * @sa EPS_COR
 * @sa cel_pole()
 */
double PSI_COR = 0.0;

/**
 * Celestial pole offset &epsilon; for high-precision applications. It was visible to users in NOVAS C 3.1,
 * hence we continue to expose it also for back compatibility.
 *
 * @sa PSI_COR
 * @sa cel_pole()
 */
double EPS_COR = 0.0;

/**
 * Maximum number of iterations for convergent inverse calculations. Most iterative inverse functions should
 * normally converge in a handful of iterations. In some pathological cases more iterations may be required.
 * This variable sets an absolute maximum for the number of iterations in order to avoid runaway (zombie)
 * behaviour. If inverse functions faile to converge, they will return a value indicating an error, and
 * errno should be set to ECANCELED.
 *
 * @since 1.1
 */
int novas_inv_max_iter = 100;

// Defined in novas.h
int grav_bodies_reduced_accuracy = DEFAULT_GRAV_BODIES_REDUCED_ACCURACY;

// Defined in novas.h
int grav_bodies_full_accuracy = DEFAULT_GRAV_BODIES_FULL_ACCURACY;


// <---------- LOCAL VARIABLES -------------->

/// Current debugging state for reporting errors and traces to stderr.
static enum novas_debug_mode novas_debug_state = NOVAS_DEBUG_OFF;

///< Opened CIO locator data file, or NULL.
static FILE *cio_file;

/// function to use for reading ephemeris data for all types of solar system sources
static novas_ephem_provider readeph2_call = NULL;

/// Function to use for reduced-precision calculations. (The full IAU 2000A model is used
/// always for high-precision calculations)
static novas_nutation_provider nutate_lp = nu2000k;

static int is_case_sensitive = 0; ///< (boolean) whether object names are case-sensitive.

/// \cond PRIVATE

/**
 * (<i>for internal use</i>) Propagates an error (if any) with an offset. If the error is
 * non-zero, it returns with the offset error value. Otherwise it keeps going as if it weren't
 * even there...
 *
 * @param loc     Function [:location] where error was produced.
 * @param n       error code that was received.
 * @param offset  Offset to add to error code (if &lt;0) to return.
 *
 * @sa novas_error()
 */
int novas_trace(const char *loc, int n, int offset) {
  if(n != 0) {
    n = n < 0 ? -1 : n + offset;
    if(novas_get_debug_mode() != NOVAS_DEBUG_OFF)
      fprintf(stderr, "       @ %s [=> %d]\n", loc, n);
  }
  return n;
}

/**
 * (<i>for internal use</i>) Traces an error before returning NAN.
 *
 * @param loc     Function [:location] where error was produced.
 * @return       NAN
 *
 * @since 1.1.1
 */
double novas_trace_nan(const char *loc) {
  if(novas_get_debug_mode() != NOVAS_DEBUG_OFF) {
    fprintf(stderr, "       @ %s [=> NAN]\n", loc);
  }
  return NAN;
}


/**
 * (<i>for internal use</i>) Sets an errno and report errors to the standard error, depending
 * on the current debug mode.
 *
 * @param en    {int} UNIX error number (see errno.h)
 * @param from  {string} Function (:location) where error originated
 * @param desc  {string} Description of error, with information to convey to user.
 *
 * @sa novas_error()
 * @sa novas_debug()
 *
 * @since 1.0
 * @author Attil Kovacs
 */
void novas_set_errno(int en, const char *from, const char *desc, ...) {
  va_list varg;

  va_start(varg, desc);
  if(novas_get_debug_mode() != NOVAS_DEBUG_OFF) {
    fprintf(stderr, "\n  ERROR! %s: ", from);
    vfprintf(stderr, desc, varg);
    fprintf(stderr, "\n");
  }
  va_end(varg);

  errno = en;
}

/**
 * (<i>for internal use</i>) Sets errno and reports errors to the standard error, depending
 * on the current debug mode, before returning the supplied return code.
 *
 * @param ret   return value
 * @param en    UNIX error code (see errno.h)
 * @param from  function (:location) where error originated
 * @param desc  description of error, with information to convey to user.
 *
 * @sa novas_set_errno()
 * @sa novas_trace()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int novas_error(int ret, int en, const char *from, const char *desc, ...) {
  va_list varg;

  va_start(varg, desc);
  if(novas_get_debug_mode() != NOVAS_DEBUG_OFF) {
    fprintf(stderr, "\n  ERROR! %s: ", from);
    vfprintf(stderr, desc, varg);
    fprintf(stderr, " [=> %d]\n", ret);
  }
  va_end(varg);

  errno = en;
  return ret;
}

/// \endcond

/**
 * Enables or disables reporting errors and traces to the standard error stream.
 *
 * @param mode    NOVAS_DEBUG_OFF (0; or &lt;0), NOVAS_DEBUG_ON (1), or NOVAS_DEBUG_EXTRA (2; or &gt;2).
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa novas_get_debug_mode()
 */
void novas_debug(enum novas_debug_mode mode) {
  if(mode < 0 || mode > NOVAS_DEBUG_EXTRA)
    mode = NOVAS_DEBUG_EXTRA;

  novas_debug_state = mode;
}

/**
 * Returns the current, thread-local, mode for reporting errors encountered (and traces).
 *
 * @return    The current debug mode in the calling thread.
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa novas_debug()
 */
enum novas_debug_mode novas_get_debug_mode() {
  return novas_debug_state;
}

/// \cond PRIVATE

/**
 * (<i>for internal use only</i>) Calculates the length of a 3-vector
 *
 * @param v     Pointer to a 3-component (x, y, z) vector. The argument cannot be NULL
 * @return      the length of the vector
 *
 * @sa vdot()
 * @sa vdist()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
double novas_vlen(const double *v) {
  return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

static double vdist2(const double *v1, const double *v2) {
  double d2 = 0.0;
  int i;
  for(i = 3; --i >= 0;) {
    const double d = v1[i] - v2[i];
    d2 += d * d;
  }
  return d2;
}

/**
 * (<i>for internal use only</i>) Calculates the distance between two 3-vectors.
 *
 * @param v1    Pointer to a 3-component (x, y, z) vector. The argument cannot be NULL
 * @param v2    Pointer to another 3-component (x, y, z) vector. The argument cannot
 *              be NULL
 * @return      The distance between the two vectors
 *
 * @sa vlen()
 * @sa vdot()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
double novas_vdist(const double *v1, const double *v2) {
  return sqrt(vdist2(v1, v2));
}

/**
 * (<i>for internal use only</i>) Calculates the dot product between two 3-vectors.
 *
 * @param v1    Pointer to a 3-component (x, y, z) vector. The argument cannot be NULL
 * @param v2    Pointer to another 3-component (x, y, z) vector. The argument cannot
 *              be NULL
 * @return      The dot product between the two vectors.
 *
 * @sa vlen()
 * @sa vdist()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
double novas_vdot(const double *v1, const double *v2) {
  return (v1[0] * v2[0]) + (v1[1] * v2[1]) + (v1[2] * v2[2]);
}

static double novas_add_beta(double beta1, double beta2) {
  return (beta1 + beta2) / (1 + beta1 * beta2);
}

/**
 * Adds velocities define in AU/day, using the relativistic formula.
 *
 * @param v1  [AU/day] First component
 * @param v2  [AU/day] Second component
 * @return    [AU/day] The relativistically coadded sum of the input velocities.
 *
 * @sa novas_z_add()
 *
 * @since 1.2
 * @author Attila Kovacs
 */
static double novas_add_vel(double v1, double v2) {
  return novas_add_beta(v1 / C_AUDAY, v2 / C_AUDAY) * C_AUDAY;
}
/// \endcond


/**
 * Converts a redshift value (z = &delta;f / f<sub>rest</sub>) to a radial velocity (i.e. rate) of recession.
 * It is based on the relativistic formula:
 * <pre>
 *  1 + z = sqrt((1 + &beta;) / (1 - &beta;))
 * </pre>
 * where &beta; = v / c.
 *
 * @param z   the redshift value (&delta;&lambda; / &lambda;<sub>rest</sub>).
 * @return    [km/s] Corresponding velocity of recession, or NAN if the input redshift is invalid, i.e. z &lt;= -1).
 *
 * @sa novas_v2z()
 * @sa redshift_vrad()
 *
 * @author Attila Kovacs
 * @since 1.2
 */
double novas_z2v(double z) {
  if(z <= -1.0) {
    novas_error(-1, EINVAL, "novas_z2v", "invalid redshift value z=%g", z);
    return NAN;
  }
  z += 1.0;
  z *= z;
  return (z - 1.0) / (z + 1.0) * C / NOVAS_KM;
}

/**
 * Computationally efficient implementation of 3D rotation with small angles.
 *
 * @param in            3D vector to rotate
 * @param ax            (rad) rotation angle around x
 * @param ax            (rad) rotation angle around x
 * @param ax            (rad) rotation angle around x
 * @param[out] out      Rotated vector. It can be the same as the input.
 *
 */
static void tiny_rotate(const double *in, double ax, double ay, double az, double *out) {
  const double x = in[0], y = in[1], z = in[2];
  const double A[3] = { ax * ax, ay * ay, az * az };

  out[0] = x - 0.5 * (A[1] + A[2]) * x - az * y + ay * z;
  out[1] = y - 0.5 * (A[0] + A[2]) * y + az * x - ax * z;
  out[2] = z - 0.5 * (A[0] + A[1]) * z - ay * x + ax * y;
}

/**
 * Checks if two Julian dates are equals under the precision that can be handled by this
 * library. In practive two dates are considered equal if they agree within 10<sup>-8</sup>
 * days (or about 1 ms) of each other.
 *
 *
 * @param jd1     [day] a Julian date (in any time measure)
 * @param jd2     [day] a Julian date in the same time measure as the first argument
 * @return        TRUE (1) if the two dates are effectively the same at the precision of
 *                comparison, or else FALSE (0) if they differ by more than the allowed
 *                tolerance.
 */
static int time_equals(double jd1, double jd2) {
  return fabs(jd1 - jd2) <= 1.0e-8;
}

/**
 * Transforms a rectangular equatorial (x, y, z) vector from J2000 coordinates to the
 * True of Date (TOD) reference frame at the given epoch
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TDB) based Julian date that defines
 *                  the output epoch. Typically it does not require much precision, and
 *                  Julian dates in other time measures will be unlikely to affect the
 *                  result
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in        Input (x, y, z) position or velocity vector in rectangular equatorial
 *                  coordinates at J2000
 * @param[out] out  Output position or velocity 3-vector in the True equinox of Date
 *                  coordinate frame. It can be the same vector as the input.
 * @return          0 if successful, or -1 if either of the vector arguments is NULL or the
 *                  accuracy is invalid.
 *
 * @sa j2000_to_gcrs()
 * @sa tod_to_j2000()
 * @sa gcrs_to_j2000()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int j2000_to_tod(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out) {
  static const char *fn = "j2000_to_tod";

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  prop_error(fn, precession(JD_J2000, in, jd_tdb, out), 0);
  prop_error(fn, nutation(jd_tdb, NUTATE_MEAN_TO_TRUE, accuracy, out, out), 0);

  return 0;
}

/**
 * Transforms a rectangular equatorial (x, y, z) vector from True of Date (TOD) reference frame
 * at the given epoch to the J2000 coordinates.
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TDB) based Julian date that defines the
 *                  input epoch. Typically it does not require much precision, and Julian dates
 *                  in other time measures will be unlikely to affect the result
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in        Input (x, y, z)  position or velocity 3-vector in the True equinox of Date
 *                  coordinate frame.
 * @param[out] out  Output position or velocity vector in rectangular equatorial coordinates at
 *                  J2000. It can be the same vector as the input.
 * @return          0 if successful, or -1 if either of the vector arguments is NULL or the
 *                  'accuracy' is invalid.
 *
 * @sa j2000_to_tod()
 * @sa j2000_to_gcrs()
 * @sa tod_to_gcrs()
 * @sa tod_to_cirs()
 * @sa tod_to_itrs()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int tod_to_j2000(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out) {
  static const char *fn = "tod_to_j2000";

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  prop_error(fn, nutation(jd_tdb, NUTATE_TRUE_TO_MEAN, accuracy, in, out), 0);
  prop_error(fn, precession(jd_tdb, out, JD_J2000, out), 0);

  return 0;
}

/**
 * Change GCRS coordinates to J2000 coordinates. Same as frame_tie() called with ICRS_TO_J2000
 *
 * @param in        GCRS input 3-vector
 * @param[out] out  J2000 output 3-vector
 * @return          0 if successful, or else an error from frame_tie()
 *
 * @sa j2000_to_gcrs()
 * @sa tod_to_j2000()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int gcrs_to_j2000(const double *in, double *out) {
  prop_error("gcrs_to_j2000", frame_tie(in, ICRS_TO_J2000, out), 0);
  return 0;
}

/**
 * Transforms a rectangular equatorial (x, y, z) vector from the Geocentric Celestial
 * Reference System (GCRS) to the Mean of Date (MOD) reference frame at the given epoch
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TT) based Julian date that defines the
 *                  output epoch. Typically it does not require much precision, and Julian
 *                  dates in other time measures will be unlikely to affect the result
 * @param in        GCRS Input (x, y, z) position or velocity vector
 * @param[out] out  Output position or velocity 3-vector in the Mean wquinox of Date coordinate
 *                  frame. It can be the same vector as the input.
 * @return          0 if successful, or -1 if either of the vector arguments is NULL.
 *
 * @sa mod_to_gcrs()
 * @sa gcrs_to_tod()
 *
 * @since 1.2
 * @author Attila Kovacs
 */
int gcrs_to_mod(double jd_tdb, const double *in, double *out) {
  static const char *fn = "gcrs_to_tod [internal]";
  prop_error(fn, frame_tie(in, ICRS_TO_J2000, out), 0);
  prop_error(fn, precession(NOVAS_JD_J2000, out, jd_tdb,  out), 0);
  return 0;
}

/**
 * Transforms a rectangular equatorial (x, y, z) vector from Mean of Date (MOD) reference
 * frame at the given epoch to the Geocentric Celestial Reference System(GCRS)
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TDB) based Julian date that defines the
 *                  input epoch. Typically it does not require much precision, and Julian dates
 *                  in other time measures will be unlikely to affect the result
 * @param in        Input (x, y, z)  position or velocity 3-vector in the Mean equinox of Date
 *                  coordinate frame.
 * @param[out] out  Output GCRS position or velocity vector. It can be the same vector as the
 *                  input.
 * @return          0 if successful, or -1 if either of the vector arguments is NULL.
 *
 * @sa gcrs_to_mod()
 * @sa tod_to_gcrs()
 *
 * @since 1.2
 * @author Attila Kovacs
 */
int mod_to_gcrs(double jd_tdb, const double *in, double *out) {
  static const char *fn = "tod_to_gcrs [internal]";
  prop_error(fn, precession(jd_tdb, in, NOVAS_JD_J2000, out), 0);
  prop_error(fn, frame_tie(out, J2000_TO_ICRS, out), 0);
  return 0;
}


/**
 * Transforms a rectangular equatorial (x, y, z) vector from the Geocentric Celestial
 * Reference System (GCRS) to the True of Date (TOD) reference frame at the given epoch
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TT) based Julian date that defines the
 *                  output epoch. Typically it does not require much precision, and Julian
 *                  dates in other time measures will be unlikely to affect the result
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in        GCRS Input (x, y, z) position or velocity vector
 * @param[out] out  Output position or velocity 3-vector in the True equinox of Date coordinate
 *                  frame. It can be the same vector as the input.
 * @return          0 if successful, or -1 if either of the vector arguments is NULL.
 *
 * @sa gcrs_to_cirs()
 * @sa tod_to_gcrs()
 * @sa j2000_to_tod()
 *
 * @since 1.2
 * @author Attila Kovacs
 */
int gcrs_to_tod(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out) {
  static const char *fn = "gcrs_to_tod [internal]";
  prop_error(fn, frame_tie(in, ICRS_TO_J2000, out), 0);
  prop_error(fn, j2000_to_tod(jd_tdb, accuracy, out, out), 0);
  return 0;
}

/**
 * Transforms a rectangular equatorial (x, y, z) vector from True of Date (TOD) reference
 * frame at the given epoch to the Geocentric Celestial Reference System(GCRS)
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TDB) based Julian date that defines the
 *                  input epoch. Typically it does not require much precision, and Julian dates
 *                  in other time measures will be unlikely to affect the result
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in        Input (x, y, z)  position or velocity 3-vector in the True equinox of Date
 *                  coordinate frame.
 * @param[out] out  Output GCRS position or velocity vector. It can be the same vector as the
 *                  input.
 * @return          0 if successful, or -1 if either of the vector arguments is NULL.
 *
 * @sa j2000_to_tod()
 * @sa tod_to_cirs()
 * @sa tod_to_j2000()
 * @sa tod_to_itrs()
 *
 * @since 1.2
 * @author Attila Kovacs
 */
int tod_to_gcrs(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out) {
  static const char *fn = "tod_to_gcrs [internal]";
  prop_error(fn, tod_to_j2000(jd_tdb, accuracy, in, out), 0);
  prop_error(fn, frame_tie(out, J2000_TO_ICRS, out), 0);
  return 0;
}

/**
 * Transforms a rectangular equatorial (x, y, z) vector from the Geocentric Celestial Reference
 * System (GCRS) to the Celestial Intermediate Reference System (CIRS) frame at the given epoch
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TDB) based Julian date that defines the
 *                  output epoch. Typically it does not require much precision, and Julian dates
 *                  in other time measures will be unlikely to affect the result
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in        GCRS Input (x, y, z) position or velocity vector
 * @param[out] out  Output position or velocity 3-vector in the True equinox of Date coordinate
 *                  frame. It can be the same vector as the input.
 * @return          0 if successful, or -1 if either of the vector arguments is NULL or the
 *                  accuracy is invalid, or an error from cio_location(), or
 *                  else 10 + the error from cio_basis().
 *
 * @sa gcrs_to_j2000()
 * @sa cirs_to_gcrs()
 *
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int gcrs_to_cirs(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out) {
  static const char *fn = "gcrs_to_cirs";
  double r_cio, v[3], x[3], y[3], z[3];
  short sys;

  if(!in || !out)
    return novas_error(-1, EINVAL, fn, "NULL input or output 3-vector: in=%p, out=%p", in, out);

  memcpy(v, in, sizeof(v));

  // Obtain the basis vectors, in the GCRS, of the celestial intermediate
  // system.
  prop_error(fn, cio_location(jd_tdb, accuracy, &r_cio, &sys), 0);
  prop_error(fn, cio_basis(jd_tdb, r_cio, sys, accuracy, x, y, z), 10);

  // Transform position vector to celestial intermediate system.
  out[0] = novas_vdot(x, v);
  out[1] = novas_vdot(y, v);
  out[2] = novas_vdot(z, v);

  return 0;
}

/**
 * Transforms a rectangular equatorial (x, y, z) vector from the Celestial Intermediate
 * Reference System (CIRS) frame at the given epoch to the Geocentric Celestial Reference
 * System (GCRS).
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TDB) based Julian date that defines
 *                  the output epoch. Typically it does not require much precision, and
 *                  Julian dates in other time measures will be unlikely to affect the
 *                  result
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in        CIRS Input (x, y, z) position or velocity vector
 * @param[out] out  Output position or velocity 3-vector in the GCRS coordinate frame.
 *                  It can be the same vector as the input.
 * @return          0 if successful, or -1 if either of the vector arguments is NULL
 *                  or the accuracy is invalid, or an error from cio_location(), or else
 *                  10 + the error from cio_basis().
 *
 * @sa tod_to_gcrs()
 * @sa gcrs_to_cirs()
 * @sa cirs_to_itrs()
 * @sa cirs_to_tod()
 *
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int cirs_to_gcrs(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out) {
  static const char *fn = "cirs_to_gcrs";
  double r_cio, vx[3], vy[3], vz[4], x, y, z;
  short sys;
  int i;

  if(!in || !out)
    return novas_error(-1, EINVAL, fn, "NULL input or output 3-vector: in=%p, out=%p", in, out);

  // Obtain the basis vectors, in the GCRS, of the celestial intermediate
  // system.
  prop_error(fn, cio_location(jd_tdb, accuracy, &r_cio, &sys), 0);
  prop_error(fn, cio_basis(jd_tdb, r_cio, sys, accuracy, vx, vy, vz), 10);

  x = in[0];
  y = in[1];
  z = in[2];

  // Transform position vector to GCRS system.
  for(i = 3; --i >= 0;) {
    out[i] = x * vx[i] + y * vy[i] + z * vz[i];
  }

  return 0;
}

/**
 * Set a custom function to use for regular precision (see NOVAS_REDUCED_ACCURACY)
 * ephemeris calculations instead of the default solarsystem() routine.
 *
 * @param func    The function to use for solar system position/velocity calculations.
 *                See solarsystem() for further details on what is required of this
 *                function.
 *
 * @author Attila Kovacs
 * @since 1.0
 *
 * @sa get_planet_provider()
 * @sa set_planet_provider_hp()
 * @sa solarsystem()
 * @sa NOVAS_REDUCED_ACCURACY
 */
int set_planet_provider(novas_planet_provider func) {
  if(!func)
    return novas_error(-1, EINVAL, "set_planet_provider", "NULL 'func' parameter");

  planet_call = func;
  return 0;
}

/**
 * Returns the custom (low-precision) ephemeris provider function for major planets
 * (and Sun, Moon, SSB...), if any.
 *
 * @return    the custom (low-precision) planet ephemeris provider function.
 *
 * @author Attila Kovacs
 * @since 1.2
 *
 * @sa set_planet_provider()
 * @sa get_planet_provider_hp()
 * @sa get_ephem_provider()
 */
novas_planet_provider get_planet_provider() {
  return planet_call;
}

/**
 * Set a custom function to use for high precision (see NOVAS_FULL_ACCURACY) ephemeris
 * calculations instead of the default solarsystem_hp() routine.
 *
 * @param func    The function to use for solar system position/velocity calculations.
 *                See solarsystem_hp() for further details on what is required of this
 *                function.
 *
 * @author Attila Kovacs
 * @since 1.0
 *
 * @sa get_planet_provider_hp()
 * @sa set_planet_provider()
 * @sa solarsystem_hp()
 * @sa NOVAS_FULL_ACCURACY
 */
int set_planet_provider_hp(novas_planet_provider_hp func) {
  if(!func)
    return novas_error(-1, EINVAL, "set_planet_provider_hp", "NULL 'func' parameter");

  planet_call_hp = func;
  return 0;
}

/**
 * Returns the custom high-precision ephemeris provider function for major planets
 * (and Sun, Moon, SSB...), if any.
 *
 * @return    the custom high-precision planet ephemeris provider function.
 *
 * @author Attila Kovacs
 * @since 1.2
 *
 * @sa set_planet_provider_hp()
 * @sa get_planet_provider()
 * @sa get_ephem_provider()
 */
novas_planet_provider_hp get_planet_provider_hp() {
  return planet_call_hp;
}

/**
 * Computes the apparent place of a star, referenced to dynamical equator at date 'jd_tt',
 * given its catalog mean place, proper motion, parallax, and radial velocity. See `place()`
 * for more information.
 *
 * REFERENCES:
 * <ol>
 *     <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *     <li>Explanatory Supplement to the Astronomical Almanac (1992),Chapter 3.</li>
 * </ol>
 *
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian date.
 * @param star      Pointer to catalog entry structure containing catalog data for
 *                  the object in the ICRS.
 * @param obs       Observer location (can be NULL if not relevant)
 * @param ut1_to_tt   [s] Difference TT-UT1 at 'jd_tt', in seconds of time.
 * @param system    The type of coordinate reference system in which coordinates are to
 *                  be returned.
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] pos  The position and radial velocity of of the catalog source in the
 *                  specified coordinate system and relative to the specified observer
 *                  location (if applicable)
 * @return          0 if successful, or -1 if one of the required arguments is NULL, or
 *                  else 1 if the observer location is invalid, or an error code from
 *                  place().
 *
 * @sa get_ut1_to_tt()
 *
 * @author Attila Kovacs
 * @since 1.0
 */
int place_star(double jd_tt, const cat_entry *star, const observer *obs, double ut1_to_tt, enum novas_reference_system system,
        enum novas_accuracy accuracy, sky_pos *pos) {
  static const char *fn = "place_star";
  object source = {0};

  if(!star || !pos)
    return novas_error(-1, EINVAL, fn, "NULL input star=%p or output pos=%p pointer", star, pos);

  source.type = NOVAS_CATALOG_OBJECT;
  source.star = *star;

  prop_error(fn, place(jd_tt, &source, obs, ut1_to_tt, system, accuracy, pos), 0);
  return 0;
}

/**
 * Computes the place of a star at date 'jd_tt', for an observer in the specified coordinate
 * system, given the star's ICRS catalog place, proper motion, parallax, and radial velocity.
 *
 * Notwithstanding the different set of return values, this is the same as calling place_star()
 * with the same arguments.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 * <li>Explanatory Supplement to the Astronomical Almanac (1992), Chapter 3.</li>
 * </ol>
 *
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian date.
 * @param star      Pointer to catalog entry structure containing catalog data for the object
 *                  in the ICRS.
 * @param obs       Observer location. It may be NULL if not relevant.
 * @param ut1_to_tt [s] Difference TT-UT1 at 'jd_tt', in seconds of time.
 * @param sys       Coordinate reference system in which to produce output values
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] ra   [h] Topocentric right ascension in hours, referred to true equator and
 *                  equinox of date 'jd_tt' or NAN when returning with an error code.
 *                  (It may be NULL if not required)
 * @param[out] dec  [deg] Topocentric declination in degrees, referred to true equator and
 *                  equinox of date 'jd_tt' or NAN when returning with an error code.
 *                  (It may be NULL if not required)
 * @param[out] rv   [AU/day] radial velocity relative ot observer, or NAN when returning with
 *                  an error code. (It may be NULL if not required)
 * @return          0 if successful, -1 if a required pointer argument is NULL, or else
 *                  20 + the error code from place_star().
 *
 * @sa radec_planet()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int radec_star(double jd_tt, const cat_entry *star, const observer *obs, double ut1_to_tt, enum novas_reference_system sys,
        enum novas_accuracy accuracy, double *ra, double *dec, double *rv) {
  sky_pos output = SKY_POS_INIT;

  // Default return values in case of error.
  if(ra)
    *ra = NAN;
  if(dec)
    *dec = NAN;
  if(rv)
    *rv = NAN;

  prop_error("radec_star", place_star(jd_tt, star, obs, ut1_to_tt, sys, accuracy, &output), 20);

  if(ra)
    *ra = output.ra;
  if(dec)
    *dec = output.dec;
  if(rv)
    *rv = output.rv;

  return 0;
}

/**
 * Computes the place of a solar system body at the specified time for an observer in the
 * specified coordinate system. This is the same as calling place() with the same arguments,
 * except the different set of return values used.
 *
 * REFERENCES:
 * <ol>
 *     <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *     <li>Explanatory Supplement to the Astronomical Almanac (1992),Chapter 3.</li>
 * </ol>
 *
 * @param jd_tt     [day] Terretrial Time (TT) based Julian date.
 * @param ss_body   Pointer to structure containing the body designation for the solar
 *                  system body.
 * @param obs       Observer location. It may be NULL if not relevant.
 * @param ut1_to_tt [s] Difference TT-UT1 at 'jd_tt', in seconds of time.
 * @param sys       Coordinate reference system in which to produce output values

 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] ra   [h] Topocentric apparent right ascension in hours, referred to the
 *                  true equator and equinox of date, or NAN when returning with an error
 *                  code. (It may be NULL if not required)
 * @param[out] dec  [deg] Topocentric apparent declination in degrees referred to the
 *                  true equator and equinox of date, or NAN when returning with an error
 *                  code. (It may be NULL if not required)
 * @param[out] dis  [AU] True distance from Earth to the body at 'jd_tt' in AU, or NAN when
 *                  returning with an error code. (It may be NULL if not needed).
 * @param[out] rv   [AU/day] radial velocity relative ot observer, or NAN when returning with
 *                  an error code. (It may be NULL if not required)
 * @return          0 if successful, or -1 if the object argument is NULL or if
 *                  the value of 'where' in structure 'location' is invalid, or 10 + the
 *                  error code from place().
 *
 * @sa radec_star()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int radec_planet(double jd_tt, const object *ss_body, const observer *obs, double ut1_to_tt, enum novas_reference_system sys,
        enum novas_accuracy accuracy, double *ra, double *dec, double *dis, double *rv) {
  static const char *fn = "radec_planet";
  sky_pos output = SKY_POS_INIT;

  // Default return values in case of error.
  if(ra)
    *ra = NAN;
  if(dec)
    *dec = NAN;
  if(dis)
    *dis = NAN;
  if(rv)
    *rv = NAN;

  switch(ss_body->type) {
    case NOVAS_PLANET:
    case NOVAS_EPHEM_OBJECT:
    case NOVAS_ORBITAL_OBJECT:
      break;
    default:
      return novas_error(-1, EINVAL, fn, "object is not solar-system type: type=%d", ss_body->type);
  }

  prop_error(fn, place(jd_tt, ss_body, obs, ut1_to_tt, sys, accuracy, &output), 10);

  if(ra)
    *ra = output.ra;
  if(dec)
    *dec = output.dec;
  if(dis)
    *dis = output.dis;
  if(rv)
    *rv = output.rv;

  return 0;
}

/**
 * Computes the apparent place of a star, referenced to dynamical equator at date 'jd_tt',
 * given its catalog mean place, proper motion, parallax, and radial velocity.
 *
 * Notwithstanding the different set of return values, this is the same as calling
 * place_star() with a NULL observer location and NOVAS_TOD as the system for an object that
 * specifies the star.
 *
 * REFERENCES:
 * <ol>
 *     <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *     <li>Explanatory Supplement to the Astronomical Almanac (1992),Chapter 3.</li>
 * </ol>
 *
 * @deprecated Use place_cirs() is now preferred, especially for high accuracy calculations.
 *
 * @param jd_tt     [day] Terretrial Time (TT) based Julian date.
 * @param star      Pointer to catalog entry structure containing catalog data for
 *                  the object in the ICRS.
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] ra   [h] Apparent right ascension in hours, referred to true equator and
 *                  equinox of date 'jd_tt' (it may be NULL if not required).
 * @param[out] dec  [deg] Apparent declination in degrees, referred to true equator and
 *                  equinox of date 'jd_tt' (it may be NULL if not required).
 * @return          0 if successful, -1 if a required pointer argument is NULL, or else an
 *                  the error from make_object(), or 20 + the error from place().
 *
 * @sa place_tod()
 * @sa place_star()
 * @sa astro_star()
 * @sa local_star()
 * @sa topo_star()
 * @sa virtual_star()
 * @sa app_planet()
 */
short app_star(double jd_tt, const cat_entry *star, enum novas_accuracy accuracy, double *ra, double *dec) {
  prop_error("app_star", radec_star(jd_tt, star, NULL, 0.0, NOVAS_TOD, accuracy, ra, dec, NULL), 0);
  return 0;
}

/**
 * Computes the virtual place of a star, referenced to GCRS, at date 'jd_tt', given its
 * catalog mean place, proper motion, parallax, and radial velocity.
 *
 * Notwithstanding the different set of return values, this is the same as calling
 * place_star() with a NULL observer location and NOVAS_GCRS as the system, or place_gcrs()
 * for an object that specifies the star.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 * <li>Explanatory Supplement to the Astronomical Almanac (1992), Chapter 3.</li>
 * </ol>
 *
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian date.
 * @param star      Pointer to catalog entry structure containing catalog data for
 *                  the object in the ICRS.
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] ra   [h] Virtual right ascension in hours, referred to the GCRS
 *                  (it may be NULL if not required).
 * @param[out] dec  [deg] Virtual declination in degrees, referred to the GCRS
 *                  (it may be NULL if not required).
 * @return          0 if successful, or -1 if a required pointer argument is NULL, or
 *                  20 + the error from place().
 *
 * @sa place_star()
 * @sa place_gcrs()
 * @sa app_star()
 * @sa astro_star()
 * @sa local_star()
 * @sa topo_star()
 * @sa virtual_planet()
 */
short virtual_star(double jd_tt, const cat_entry *star, enum novas_accuracy accuracy, double *ra, double *dec) {
  prop_error("virtual_star", radec_star(jd_tt, star, NULL, 0.0, NOVAS_GCRS, accuracy, ra, dec, NULL), 0);
  return 0;
}

/**
 * Computes the astrometric place of a star, referred to the ICRS without light
 * deflection or aberration, at date 'jd_tt', given its catalog mean place, proper
 * motion, parallax, and radial velocity.
 *
 * Notwithstanding the different set of return values, this is the same as calling
 * place_star() with a NULL observer location and NOVAS_ICRS as the system, or
 * place_icrs() for an object that specifies the star.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 * <li>Explanatory Supplement to the Astronomical Almanac (1992), Chapter 3.</li>
 * </ol>
 *
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian date.
 * @param star      Pointer to catalog entry structure containing catalog data for
 *                  the object in the ICRS.
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] ra   [h] Astrometric right ascension in hours, referred to the ICRS,
 *                  without light deflection or aberration. (It may be NULL if not
 *                  required)
 * @param[out] dec  [deg] Astrometric declination in degrees, referred to the ICRS,
 *                  without light deflection or aberration. (It may be NULL if not
 *                  required)
 * @return          0 if successful, or -1 if a required pointer argument is NULL, or
 *                  20 + the error from place().
 *
 * @sa place_star()
 * @sa place_icrs()
 * @sa app_star()
 * @sa local_star()
 * @sa topo_star()
 * @sa virtual_star()
 * @sa astro_planet()
 */
short astro_star(double jd_tt, const cat_entry *star, enum novas_accuracy accuracy, double *ra, double *dec) {
  prop_error("astro_star", radec_star(jd_tt, star, NULL, 0.0, NOVAS_ICRS, accuracy, ra, dec, NULL), 0);
  return 0;
}

/**
 * Computes the apparent place of a solar system body. This is the same as calling
 * place() for the body with NOVAS_TOD as the system, except the different set of
 * return values used.
 *
 * REFERENCES:
 * <ol>
 *     <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *     <li>Explanatory Supplement to the Astronomical Almanac (1992),Chapter 3.</li>
 * </ol>
 *
 * @deprecated Use place_cirs() is now preferred, especially for high accuracy calculations.
 *
 * @param jd_tt     [day] Terretrial Time (TT) based Julian date.
 * @param ss_body   Pointer to structure containing the body designation for the solar
 *                  system body.
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] ra   [h] Apparent right ascension in hours, referred to true equator and
 *                  equinox of date 'jd_tt'. (It may be NULL if not required)
 * @param[out] dec  [deg] Apparent declination in degrees, referred to true equator and
 *                  equinox of date 'jd_tt'. (It may be NULL if not required)
 * @param[out] dis  [AU] True distance from Earth to the body at 'jd_tt' in AU (can be
 *                  NULL if not needed).
 * @return          0 if successful, or -1 if the object argument is NULL, or else 1 if
 *                  the value of 'type' in structure 'ss_body' is invalid, or 10 + the
 *                  error code from place().
 *
 * @sa place_tod()
 * @sa astro_planet()
 * @sa local_planet()
 * @sa topo_planet()
 * @sa virtual_planet()
 * @sa app_star()
 */
short app_planet(double jd_tt, const object *ss_body, enum novas_accuracy accuracy, double *ra, double *dec, double *dis) {
  prop_error("app_planet", radec_planet(jd_tt, ss_body, NULL, 0.0, NOVAS_TOD, accuracy, ra, dec, dis, NULL), 0);
  return 0;
}

/**
 * Computes the virtual place of a solar system body, referenced to the GCRS. This is the
 * same as calling place_gcrs() for the body, except the different set of return values used.
 *
 * REFERENCES:
 * <ol>
 *     <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *     <li>Explanatory Supplement to the Astronomical Almanac (1992),Chapter 3.</li>
 * </ol>
 *
 * @param jd_tt     [day] Terretrial Time (TT) based Julian date.
 * @param ss_body   Pointer to structure containing the body designation for the solar system
 *                  body.
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] ra   [h] Virtual right ascension in hours, referred to the GCRS (it may be NULL
 *                  if not required).
 * @param[out] dec  [deg] Virtual declination in degrees, referred to the GCRS (it may be NULL
 *                  if not required).
 * @param[out] dis  [AU] True distance from Earth to the body at 'jd_tt' in AU (can be NULL if
 *                  not needed).
 * @return          0 if successful, or -1 if the object argument is NULL, or else 1 if the
 *                  value of 'type' in structure 'ss_body' is invalid, or 10 + the error code
 *                  from place().
 *
 * @sa place_gcrs()
 * @sa app_planet()
 * @sa astro_planet()
 * @sa local_planet()
 * @sa topo_planet()
 * @sa app_star()
 */
short virtual_planet(double jd_tt, const object *ss_body, enum novas_accuracy accuracy, double *ra, double *dec, double *dis) {
  prop_error("virtual_planet", radec_planet(jd_tt, ss_body, NULL, 0.0, NOVAS_GCRS, accuracy, ra, dec, dis, NULL), 0);
  return 0;
}

/**
 * Computes the astrometric place of a solar system body, referenced to the ICRS without light
 * deflection or aberration. This is the same as calling place_icrs() for the body, except the
 * different set of return values used.
 *
 *
 * REFERENCES:
 * <ol>
 *     <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *     <li>Explanatory Supplement to the Astronomical Almanac (1992),Chapter 3.</li>
 * </ol>
 *
 * @param jd_tt     [day] Terretrial Time (TT) based Julian date.
 * @param ss_body   Pointer to structure containing the body designation for the solar system
 *                  body.
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] ra   [h] Astrometric right ascension in hours, referred to the ICRS, without light
 *                  deflection or aberration. (It may be NULL if not required)
 * @param[out] dec  [deg] Astrometric declination in degrees, referred to the ICRS, without light
 *                  deflection or aberration. (It may be NULL if not required)
 * @param[out] dis  [AU] True distance from Earth to the body at 'jd_tt' in AU (can be NULL if
 *                  not needed).
 * @return          0 if successful, or -1 if the object is NULL, or else 1 if the value of 'type'
 *                  in structure 'ss_body' is invalid, or 10 + the error code from place().
 *
 * @sa place_icrs()
 * @sa app_planet()
 * @sa local_planet()
 * @sa topo_planet()
 * @sa virtual_planet()
 * @sa astro_star()
 */
short astro_planet(double jd_tt, const object *ss_body, enum novas_accuracy accuracy, double *ra, double *dec, double *dis) {
  prop_error("astro_planet", radec_planet(jd_tt, ss_body, NULL, 0.0, NOVAS_ICRS, accuracy, ra, dec, dis, NULL), 0);
  return 0;
}

/**
 * Computes the topocentric (True of Date; TOD) apparent place of a star at date 'jd_tt', given
 * its ICRS catalog place, proper motion, parallax, and radial velocity.
 *
 * Notwithstanding the different set of return values, this is the same as calling place_star()
 * with the same observer location and NOVAS_TOD for an object that specifies the star.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 * <li>Explanatory Supplement to the Astronomical Almanac (1992), Chapter 3.</li>
 * </ol>
 *
 * @deprecated Using place() with system NOVAS_CIRS is now preferred for topocentric calculations,
 *             especially when high precision is required. However, you will have to follow the IAU
 *             2000 method consistently to produce equivalent calculations throughout.
 *
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian date.
 * @param ut1_to_tt   [s] Difference TT-UT1 at 'jd_tt', in seconds of time.
 * @param star      Pointer to catalog entry structure containing catalog data for
 *                  the object in the ICRS.
 * @param position  Position of the observer
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] ra   [h] Topocentric right ascension in hours, referred to true equator and
 *                  equinox of date 'jd_tt'. (It may be NULL if not required)
 * @param[out] dec  [deg] Topocentric declination in degrees, referred to true equator and
 *                  equinox of date 'jd_tt'. (It may be NULL if not required)
 * @return          0 if successful, -1 if a required pointer argument is NULL, or else
 *                  20 + the error code from place_star().
 *
 * @sa place_star()
 * @sa app_star()
 * @sa local_star()
 * @sa topo_star()
 * @sa virtual_star()
 * @sa astro_planet()
 * @sa get_ut1_to_tt()
 */
short topo_star(double jd_tt, double ut1_to_tt, const cat_entry *star, const on_surface *position, enum novas_accuracy accuracy, double *ra,
        double *dec) {
  static const char *fn = "topo_star";
  observer obs = {0};
  prop_error(fn, make_observer(NOVAS_OBSERVER_ON_EARTH, position, NULL, &obs), 0);
  prop_error(fn, radec_star(jd_tt, star, &obs, ut1_to_tt, NOVAS_TOD, accuracy, ra, dec, NULL), 0);
  return 0;
}

/**
 * Computes the local apparent place of a star at date 'jd_tt', in the GCRS, given its catalog
 * mean place, proper motion, parallax, and radial velocity.
 *
 * Notwithstanding the different set of return values, this is the same as calling place_star()
 * with the same observer location NOVAS_GCRS for an object that specifies the star.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 * <li>Explanatory Supplement to the Astronomical Almanac (1992), Chapter 3.</li>
 * </ol>
 *
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian date.
 * @param ut1_to_tt   [s] Difference TT-UT1 at 'jd_tt', in seconds of time.
 * @param star      Pointer to catalog entry structure containing catalog data for
 *                  the object in the ICRS.
 * @param position  Position of the observer
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] ra   [h] Local right ascension in hours, referred to the GCRS (it may be NULL
 *                  if not required).
 * @param[out] dec  [deg] Local right ascension in hours, referred to the GCRS (it may be
 *                  NULL if not required).
 * @return          0 if successful, or -1 if any of the required pointer arguments is NULL,
 *                  or else 20 + the error from place().
 *
 * @sa place_star()
 * @sa app_star()
 * @sa astro_star()
 * @sa topo_star()
 * @sa virtual_star()
 * @sa astro_planet()
 * @sa get_ut1_to_tt()
 */
short local_star(double jd_tt, double ut1_to_tt, const cat_entry *star, const on_surface *position, enum novas_accuracy accuracy,
        double *ra, double *dec) {
  static const char *fn = "local_star";
  observer obs = {0};
  prop_error(fn, make_observer(NOVAS_OBSERVER_ON_EARTH, position, NULL, &obs), 0);
  prop_error(fn, radec_star(jd_tt, star, &obs, ut1_to_tt, NOVAS_GCRS, accuracy, ra, dec, NULL), 0);
  return 0;
}

/**
 * Computes the topocentric apparent place of a solar system body at the specified time.
 * This is the same as calling place() for the body for the same observer location and
 * NOVAS_TOD as the reference system, except the different set of return values used.
 *
 * @deprecated Using place() with system NOVAS_CIRS is now preferred for topocentric
 *             calculations, especially when high precision is required. However, you will
 *             have to follow the IAU 2000 method consistently to produce equivalent
 *             calculations throughout.
 *
 * REFERENCES:
 * <ol>
 *     <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *     <li>Explanatory Supplement to the Astronomical Almanac (1992),Chapter 3.</li>
 * </ol>
 *
 * @param jd_tt     [day] Terretrial Time (TT) based Julian date.
 * @param ss_body   Pointer to structure containing the body designation for the solar
 *                  system body.
 * @param ut1_to_tt [s] Difference TT-UT1 at 'jd_tt', in seconds of time.
 * @param position  Position of the observer
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] ra   [h] Topocentric apparent right ascension in hours, referred to the
 *                  true equator and equinox of date. (It may be NULL if not required)
 * @param[out] dec  [deg] Topocentric apparent declination in degrees referred to the
 *                  true equator and equinox of date. (It may be NULL if not required)
 * @param[out] dis  [AU] True distance from Earth to the body at 'jd_tt' in AU (may be
 *                  NULL if not needed).
 * @return          0 if successful, or -1 if the object argument is NULL, or else 1 if
 *                  the value of 'where' in structure 'location' is invalid, or 10 + the
 *                  error code from place().
 *
 * @sa app_planet()
 * @sa local_planet()
 * @sa topo_planet()
 * @sa virtual_planet()
 * @sa astro_star()
 * @sa get_ut1_to_tt()
 */
short topo_planet(double jd_tt, const object *ss_body, double ut1_to_tt, const on_surface *position, enum novas_accuracy accuracy,
        double *ra, double *dec, double *dis) {
  static const char *fn = "topo_planet";
  observer obs = {0};
  prop_error(fn, make_observer(NOVAS_OBSERVER_ON_EARTH, position, NULL, &obs), 0);
  prop_error(fn, radec_planet(jd_tt, ss_body, &obs, ut1_to_tt, NOVAS_TOD, accuracy, ra, dec, dis, NULL), 0);
  return 0;
}

/**
 * Computes the local apparent place of a solar system body, in the GCRS. This is the
 * same as calling place() for the body for the same observer location and NOVAS_GCRS
 * as the reference system, except the different set of return values used.
 *
 * REFERENCES:
 * <ol>
 *     <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *     <li>Explanatory Supplement to the Astronomical Almanac (1992),Chapter 3.</li>
 * </ol>
 *
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian date.
 * @param ss_body   Pointer to structure containing the body designation for the solar
 *                  system body.
 * @param ut1_to_tt [s] Difference TT-UT1 at 'jd_tt', in seconds of time.
 * @param position  Position of the observer
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] ra   [h] Local right ascension in hours, referred to the GCRS (it may be
 *                  NULL if not required).
 * @param[out] dec  [deg] Local right ascension in hours, referred to the GCRS (it may
 *                  be NULL if not required).
 * @param[out] dis  [AU] True distance from Earth to the body at 'jd_tt' in AU (it may
 *                  be NULL if not required).
 * @return          0 if successful, or -1 if the object argument is NULL, or else 1 if
 *                  the value of 'where' in structure 'location' is invalid, or 10 + the
 *                  error code from place().
 *
 * @sa astro_planet()
 * @sa topo_planet()
 * @sa virtual_planet()
 * @sa app_star()
 * @sa get_ut1_to_tt()
 */
short local_planet(double jd_tt, const object *ss_body, double ut1_to_tt, const on_surface *position, enum novas_accuracy accuracy,
        double *ra, double *dec, double *dis) {
  static const char *fn = "local_planet";
  observer obs = {0};
  prop_error(fn, make_observer(NOVAS_OBSERVER_ON_EARTH, position, NULL, &obs), 0);
  prop_error(fn, radec_planet(jd_tt, ss_body, &obs, ut1_to_tt, NOVAS_GCRS, accuracy, ra, dec, dis, NULL), 0);
  return 0;
}

/**
 * Computes the ICRS position of a star, given its True of Date (TOD) apparent place at date
 * 'jd_tt'. Proper motion, parallax and radial velocity are assumed to be zero.
 *
 * REFERENCES:
 * <ol>
 *     <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *     <li>Explanatory Supplement to the Astronomical Almanac (1992),Chapter 3.</li>
 * </ol>
 *
 * @param jd_tt         [day] Terrestrial Time (TT) based Julian date.
 * @param tra           [h] Apparent (TOD) right ascension in hours, referred to true equator
 *                      and equinox of date.
 * @param tdec          [deg] Apparent (TOD) declination in degrees, referred to true equator
 *                      and equinox of date.
 * @param accuracy      NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] ira      [h] ICRS right ascension in hours, or NAN when returning with an error code.
 * @param[out] idec     [deg] ICRS declination in degrees, or NAN when returning with an error code.
 * @return              0 if successful; -1 if the supplied output pointers are NULL,
 *                      1 if the iterative process did not converge after 30 iterations, or an
 *                      error from vector2radec(), or else &gt; 10 + an error from app_star().
 *
 * @sa make_cat_entry()
 * @sa proper_motion()
 * @sa precession()
 */
short mean_star(double jd_tt, double tra, double tdec, enum novas_accuracy accuracy, double *ira, double *idec) {
  static const char *fn = "mean_star";
  cat_entry star = CAT_ENTRY_INIT;
  double pos[3];
  int iter;

  if(!ira || !idec)
    return novas_error(-1, EINVAL, fn, "NULL output pointer: ira=%p, idec=%p", ira, idec);

  // Default return values...
  *ira = NAN;
  *idec = NAN;

  // create a position vector based on the apparent RA and declination of the star.
  star.ra = tra;
  star.dec = tdec;
  starvectors(&star, pos, NULL);

  // Get initial approximation by precessing star position at 'jd_tt'
  // to its position at J2000.0.
  precession(jd_tt, pos, JD_J2000, pos);

  prop_error(fn, vector2radec(pos, &star.ra, &star.dec), 10);

  // Iteratively find ICRS coordinates that produce input apparent place
  // of star at date 'jd_tt'.
  for(iter = novas_inv_max_iter; --iter >= 0;) {
    double ra1, dec1;

    prop_error(fn, app_star(jd_tt, &star, accuracy, &ra1, &dec1), 20);

    // If within tolerance, we are done
    if(fabs(tra - ra1) < 1.0e-12 && fabs(tdec - dec1) < 1.0e-11) {
      *ira = star.ra < 0.0 ? star.ra + DAY_HOURS : star.ra;
      *idec = star.dec;
      return 0;
    }

    // Correct for overshoot
    star.ra = remainder(star.ra + (tra - ra1), DAY_HOURS);
    star.dec = remainder(star.dec + (tdec - dec1), DEG360);
  }

  return novas_error(1, ECANCELED, fn, "failed to converge");
}

/**
 * Calculates the ICRS position and velocity of the observer relative to the Solar System Barycenter (SSB).
 *
 * @param jd_tdb        [day] Barycentric Dynamical Time (TDB) based Julian date.
 * @param ut1_to_tt     [s] TT - UT1 time difference. Used only when 'location->where' is
 *                      NOVAS_OBSERVER_ON_EARTH (1) or NOVAS_OBSERVER_IN_EARTH_ORBIT (2).
 * @param accuracy      NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param obs           The observer location, relative to which the output positions and velocities
 *                      are to be calculated
 * @param geo_pos       [AU] ICRS position vector of the geocenter w.r.t. the Solar System
 *                      Barycenter (SSB). If either geo_pos or geo_vel is NULL, it will be calculated
 *                      when needed.
 * @param geo_vel       [AU/day] ICRS velocity vector of the geocenter w.r.t. the Solar System
 *                      Barycenter (SSB). If either geo_pos or geo_vel is NULL, it will be
 *                      calculated when needed.
 * @param[out] pos      [AU] Position 3-vector of the observer w.r.t. the Solar System Barycenter
 *                      (SSB). It may be NULL if not required.
 * @param[out] vel      [AU/day] Velocity 3-vector of the observer w.r.t. the Solar System
 *                      Barycenter (SSB). It must be distinct from the pos output vector, and may be
 *                      NULL if not required.
 * @return              0 if successful, or the error from geo_posvel(), or else -1 (with errno
 *                      indicating the type of error).
 *
 * @author Attila Kovacs
 * @since 1.1
 *
 * @see place()
 */
int obs_posvel(double jd_tdb, double ut1_to_tt, enum novas_accuracy accuracy, const observer *obs, const double *geo_pos,
        const double *geo_vel, double *pos, double *vel) {
  static const char *fn = "get_obs_posvel";

  if(!obs)
    return novas_error(-1, EINVAL, fn, "NULL observer parameter");

  if(obs->where < 0 || obs->where >= NOVAS_OBSERVER_PLACES)
    return novas_error(-1, EINVAL, fn, "Invalid observer location: %d", obs->where);

  if(pos == vel)
    return novas_error(-1, EINVAL, fn, "identical output pos and vel pointers @ %p.", pos);

  if(obs->where == NOVAS_SOLAR_SYSTEM_OBSERVER) {
    if(pos)
      memcpy(pos, obs->near_earth.sc_pos, XYZ_VECTOR_SIZE);
    if(vel)
      memcpy(vel, obs->near_earth.sc_vel, XYZ_VECTOR_SIZE);
    return 0;
  }

  if(!geo_pos || !geo_vel) {
    const double tdb2[2] = { jd_tdb };
    object earth = NOVAS_EARTH_INIT;
    double gpos[3], gvel[3];
    prop_error(fn, ephemeris(tdb2, &earth, NOVAS_BARYCENTER, accuracy, gpos, gvel), 0);
    if(pos)
      memcpy(pos, gpos, XYZ_VECTOR_SIZE);
    if(vel)
      memcpy(vel, gvel, XYZ_VECTOR_SIZE);
  }
  else {
    if(pos)
      memcpy(pos, geo_pos, XYZ_VECTOR_SIZE);
    if(vel)
      memcpy(vel, geo_vel, XYZ_VECTOR_SIZE);
  }

  // ---------------------------------------------------------------------
  // Get position and velocity of observer.
  // ---------------------------------------------------------------------
  switch(obs->where) {
    case NOVAS_OBSERVER_ON_EARTH:
    case NOVAS_AIRBORNE_OBSERVER:
    case NOVAS_OBSERVER_IN_EARTH_ORBIT: {
      double pog[3] = {0}, vog[3] = {0};
      int i;

      // For topocentric place, get geocentric position and velocity vectors
      // of observer
      prop_error(fn, geo_posvel(jd_tdb, ut1_to_tt, accuracy, obs, pog, vog), 0);
      for(i = 3; --i >= 0;) {
        if(pos)
          pos[i] += pog[i];
        if(vel)
          vel[i] = novas_add_vel(vel[i], vog[i]);
      }

      break;
    }

    default:
      // Nothing to do
      ;
  }

  return 0;
}

/**
 * Computes the apparent direction of a celestial object at a specified time and in a specified
 * coordinate system and a specific near-Earth origin.
 *
 * While <code>coord_sys</code> defines the celestial pole (i.e. equator) orientation of the
 * coordinate system, <code>location->where</code> sets the origin of the reference place relative
 * to which positions and velocities are reported.
 *
 * For all but ICRS coordinate outputs, the calculated positions and velocities include aberration
 * corrections for the moving frame of the observer as well as gravitational deflection due to the
 * Sun and Earth and other major gravitating bodies in the Solar system, provided planet positions
 * are available via a novas_planet_provider function.
 *
 * In case of a dynamical equatorial system (such as CIRS or TOD) and an Earth-based observer, the
 * polar wobble parameters set via a prior call to cel_pole() together with he ut1_to_tt argument
 * decide whether the resulting 'topocentric' output frame is Pseudo Earth Fixed (PEF; if
 * cel_pole() was not set and DUT1 is 0) or ITRS (actual rotating Earth; if cel_pole() was set
 * and ut1_to_tt includes the DUT1 component).
 *
 * NOTES:
 * <ol>
 * <li>This version fixes a NOVAS C 3.1 issue that velocities and solar-system distances were not
 * antedated for light-travel time.</li>
 * <li>In a departure from the original NOVAS C, the radial velocity for major planets (and Sun and
 * Moon) includes gravitational redshift corrections for light originating at the surface, assuming
 * it's observed from near Earth or else from a large distance away.</li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 *     <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *     <li>Klioner, S. (2003), Astronomical Journal 125, 1580-1597.</li>
 * </ol>
 *
 * @param jd_tt         [day] Terrestrial Time (TT) based Julian date.
 * @param source        Pointer to a celestrial object data structure
 * @param location      The observer location, relative to which the output positions and velocities
 *                      are to be calculated
 * @param ut1_to_tt     [s] TT - UT1 time difference. Used only when 'location->where' is
 *                      NOVAS_OBSERVER_ON_EARTH (1) or NOVAS_OBSERVER_IN_EARTH_ORBIT (2).
 * @param coord_sys     The coordinate system that defines the orientation of the celestial pole.
 *                      If it is NOVAS_ICRS (3), a geometric position and radial velocity is returned. For
 *                      all other systems, the returned position is the apparent position including
 *                      aberration and gravitational deflection corrections, and the radial velocity
 *                      is in the direction the eflected light was emitted from the source.
 * @param accuracy      NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] output   Data structure to populate with the result.
 * @return              0 if successful,<br>
 *                      1 if 'coord_sys' is invalid,<br>
 *                      2 if 'accuracy' is invalid,<br>
 *                      3 if the observer is at or very near (within ~1.5m of) the observed location,<br>
 *                      10--40: error is 10 + the error ephemeris(),<br>
 *                      40--50: error is 40 + the error from geo_posvel(),<br>
 *                      50--70: error is 50 + error from light_time2(),<br>
 *                      70--80: error is 70 + error from grav_def(),<br>
 *                      80--90: error is 80 + error from cio_location(),<br>
 *                      90--100: error is 90 + error from cio_basis().
 *
 * @sa novas_geom_posvel()
 * @sa novas_sky_pos()
 * @sa place_star()
 * @sa place_icrs()
 * @sa place_gcrs()
 * @sa place_cirs()
 * @sa radec_star()
 * @sa radec_planet()
 * @sa cel_pole()
 * @sa get_ut1_to_tt()
 */
short place(double jd_tt, const object *source, const observer *location, double ut1_to_tt, enum novas_reference_system coord_sys,
        enum novas_accuracy accuracy, sky_pos *output) {
  static const char *fn = "place";
  static object earth, sun;

  static int first_time = 1;
  static THREAD_LOCAL enum novas_accuracy acc_last = -1;
  static THREAD_LOCAL double tlast1 = 0.0;
  static THREAD_LOCAL double peb[3], veb[3], psb[3];

  observer obs;
  novas_planet_bundle planets = {0};
  int pl_mask = (accuracy == NOVAS_FULL_ACCURACY) ? grav_bodies_full_accuracy : grav_bodies_reduced_accuracy;
  double x, jd_tdb, pob[3], vob[3], pos[3] = { 0 }, vel[3], vpos[3], t_light, d_sb;
  int i;

  if(!source)
    return novas_error(-1, EINVAL, fn, "NULL input 'source' pointer");

  // Check for invalid value of 'coord_sys' or 'accuracy'.
  if(coord_sys < 0 || coord_sys >= NOVAS_REFERENCE_SYSTEMS)
    return novas_error(1, EINVAL, fn, "invalid reference system: %d", coord_sys);

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(2, EINVAL, fn, "invalid accuracy: %d", accuracy);

  if(!location)
    make_observer_at_geocenter(&obs);
  else
    obs = *location;

  // Create a null star 'cat_entry' and  Earth and Sun 'object's.
  if(first_time) {
    make_planet(NOVAS_EARTH, &earth);
    make_planet(NOVAS_SUN, &sun);
    first_time = 0;
  }

  // Compute 'jd_tdb', the TDB Julian date corresponding to 'jd_tt'.
  jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY;

  // ---------------------------------------------------------------------
  // Get position and velocity of Earth (geocenter) and Sun.
  // ---------------------------------------------------------------------
  if(!time_equals(jd_tt, tlast1) || accuracy != acc_last) {
    double vsb[3];
    const double jd[2] = { jd_tdb };

    // Get position and velocity of Earth wrt barycenter of solar system, in ICRS.
    prop_error("place:ephemeris:earth", ephemeris(jd, &earth, NOVAS_BARYCENTER, accuracy, peb, veb), 10);

    // Get position and velocity of Sun wrt barycenter of solar system, in ICRS.
    prop_error("place:ephemeris:sun", ephemeris(jd, &sun, NOVAS_BARYCENTER, accuracy, psb, vsb), 10);

    tlast1 = jd_tt;
    acc_last = accuracy;
  }

  // ---------------------------------------------------------------------
  // Get position and velocity of observer.
  // ---------------------------------------------------------------------
  prop_error(fn, obs_posvel(jd_tdb, ut1_to_tt, accuracy, &obs, peb, veb, pob, vob), 40);

  // ---------------------------------------------------------------------
  // Find geometric position of observed object (ICRS)
  // ---------------------------------------------------------------------
  if(source->type == NOVAS_CATALOG_OBJECT) {
    // Observed object is star.
    double dt = 0.0;

    // Get position of star updated for its space motion.
    starvectors(&source->star, pos, vel);

    dt = d_light(pos, pob);
    proper_motion(JD_J2000, pos, vel, (jd_tdb + dt), pos);

    // Get position of star wrt observer (corrected for parallax).
    bary2obs(pos, pob, pos, &t_light);

    output->dis = 0.0;
    d_sb = novas_vlen(pos);
  }
  else {
    // Get position of body wrt observer, antedated for light-time.
    prop_error(fn, light_time2(jd_tdb, source, pob, 0.0, accuracy, pos, vel, &t_light), 50);

    if(novas_vlen(pos) < 1e-11)
      return novas_error(3, EINVAL, fn, "observer is at or very near the observed location");

    // Calculate distance to Sun.
    d_sb = 0.0;
    for(i = 3; --i >= 0;) {
      double d = psb[i] - (pob[i] + pos[i]);
      d_sb += d * d;
    }
    d_sb = sqrt(d_sb);

    // AK: Fix for antedating distance and velocities...
    output->dis = t_light * C_AUDAY;
  }

  if(coord_sys != NOVAS_ICRS) {
    prop_error(fn, obs_planets(jd_tdb, accuracy, pob, pl_mask, &planets), 70);
  }

  // ---------------------------------------------------------------------
  // Compute direction in which light was emitted from the source
  // ---------------------------------------------------------------------
  if(coord_sys == NOVAS_ICRS || source->type == NOVAS_CATALOG_OBJECT) {
    // For sidereal sources and ICRS the 'velocity' position is the same as the geometric position.
    memcpy(vpos, pos, sizeof(pos));
  }
  else {
    double psrc[3];  // Barycentric position of Solar-systemn source (antedated)

    // A.K.: For this we calculate gravitational deflection of the observer seen from the source
    // i.e., reverse tracing the light to find the direction in which it was emitted.
    for(i = 3; --i >= 0;) {
      vpos[i] = -pos[i];
      psrc[i] = pos[i] + pob[i];
    }

    // vpos -> deflected direction in which observer is seen from source.
    prop_error(fn, grav_planets(vpos, psrc, &planets, vpos), 70);

    // vpos -> direction in which light was emitted from observer's perspective...
    for(i = 3; --i >= 0;)
      vpos[i] = -vpos[i];
  }

  // ---------------------------------------------------------------------
  // Compute radial velocity (all vectors in ICRS).
  // ---------------------------------------------------------------------
  output->rv = rad_vel2(source, vpos, vel, pos, vob, novas_vdist(pob, peb), novas_vdist(pob, psb), d_sb);

  if(coord_sys != NOVAS_ICRS) {
    // ---------------------------------------------------------------------
    // Apply gravitational deflection
    // ---------------------------------------------------------------------
    prop_error(fn, grav_planets(pos, pob, &planets, pos), 70);

    // ---------------------------------------------------------------------
    // Apply aberration correction.
    // ---------------------------------------------------------------------
    aberration(pos, vob, t_light, pos);
  }

  // ---------------------------------------------------------------------
  // Transform, if necessary, to output coordinate system.
  // ---------------------------------------------------------------------
  switch(coord_sys) {
    case NOVAS_J2000: {
      // Transform to equator and equinox of date.
      gcrs_to_j2000(pos, pos);
      break;
    }

    case NOVAS_MOD: {
      // Transform to equator and equinox of date.
      gcrs_to_mod(jd_tdb, pos, pos);
      break;
    }

    case NOVAS_TOD: {
      // Transform to equator and equinox of date.
      gcrs_to_tod(jd_tdb, accuracy, pos, pos);
      break;
    }

    case NOVAS_CIRS: {
      // Transform to equator and CIO of date.
      prop_error(fn, gcrs_to_cirs(jd_tdb, accuracy, pos, pos), 80);
      break;
    }

    default:
      // Nothing else to do.
      ;
  }

  // ---------------------------------------------------------------------
  // Finish up.
  // ---------------------------------------------------------------------
  vector2radec(pos, &output->ra, &output->dec);

  x = 1.0 / novas_vlen(pos);
  for(i = 3; --i >= 0;)
    output->r_hat[i] = pos[i] * x;

  return 0;
}

/**
 * Converts ICRS right ascension and declination to galactic longitude and latitude.
 *
 * REFERENCES:
 * <ol>
 * <li>Hipparcos and Tycho Catalogues, Vol. 1, Section 1.5.3.</li>
 * </ol>
 *
 * @param ra          [h] ICRS right ascension in hours.
 * @param dec         [deg] ICRS declination in degrees.
 * @param[out] glon   [deg] Galactic longitude in degrees.
 * @param[out] glat   [deg] Galactic latitude in degrees.
 *
 * @return            0 if successful, or -1 if either of the output pointer arguments
 *                    are NULL.
 *
 * @sa gal2equ()
 */
int equ2gal(double ra, double dec, double *glon, double *glat) {
  double pos1[3], pos2[3], xyproj, cosd;

  // Rotation matrix A_g from Hipparcos documentation eq. 1.5.11.
  // AK: Transposed compared to NOVAS C 3.1 for dot product handling.
  static const double ag[3][3] = { //
          { -0.0548755604, -0.8734370902, -0.4838350155 }, //
          { +0.4941094279, -0.4448296300, +0.7469822445 }, //
          { -0.8676661490, -0.1980763734, +0.4559837762 } };

  if(!glon || !glat)
    return novas_error(-1, EINVAL, "equ2gal", "NULL output pointer: glon=%p, glat=%p", glon, glat);

  // Form position vector in equatorial system from input coordinates
  ra *= HOURANGLE;
  dec *= DEGREE;
  cosd = cos(dec);

  pos1[0] = cosd * cos(ra);
  pos1[1] = cosd * sin(ra);
  pos1[2] = sin(dec);

  // Rotate position vector to galactic system, using Hipparcos documentation eq. 1.5.13.
  pos2[0] = novas_vdot(ag[0], pos1);
  pos2[1] = novas_vdot(ag[1], pos1);
  pos2[2] = novas_vdot(ag[2], pos1);

  // Decompose galactic vector into longitude and latitude.
  xyproj = sqrt(pos2[0] * pos2[0] + pos2[1] * pos2[1]);
  *glon = xyproj ? atan2(pos2[1], pos2[0]) / DEGREE : 0.0;
  if(*glon < 0.0)
    *glon += DEG360;

  *glat = atan2(pos2[2], xyproj) / DEGREE;

  return 0;
}

/**
 * Convert right ascension and declination to ecliptic longitude and latitude.  To convert
 * GCRS RA and dec to ecliptic coordinates (mean ecliptic and equinox of J2000.0), set
 * 'coord_sys' to NOVAS_GCRS_EQUATOR(2); in this case the value of 'jd_tt' can be set to
 * anything, since J2000.0 is assumed. Otherwise, all input coordinates are dynamical at
 * 'jd_tt'.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date. (Unused if 'coord_sys'
 *                    is NOVAS_GCRS_EQUATOR[2])
 * @param coord_sys   The astrometric reference system of the coordinates. If 'coord_sys' is
 *                    NOVAS_GCRS_EQUATOR(2), the input GCRS coordinates are converted to
 *                    J2000 ecliptic coordinates.
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param ra          [h] Right ascension in hours, referred to specified equator and equinox
 *                    of date.
 * @param dec         [deg] Declination in degrees, referred to specified equator and equinox
 *                    of date.
 * @param[out] elon   [deg] Ecliptic longitude in degrees, referred to specified ecliptic and
 *                    equinox of date.
 * @param[out] elat   [deg] Ecliptic latitude in degrees, referred to specified ecliptic and
 *                    equinox of date.
 * @return            0 if successful, or else 1 if the value of 'coord_sys' is invalid.
 *
 * @sa equ2ecl_vec()
 * @sa ecl2equ()
 *
 */
short equ2ecl(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double ra, double dec, double *elon,
        double *elat) {
  static const char *fn = "equ2ecl";
  double cosd, pos[3], xyproj;

  if(!elon || !elat)
    return novas_error(-1, EINVAL, fn, "NULL output pointer: elon=%p, elat=%p", elon, elat);

  // Form position vector in equatorial system from input coordinates.
  ra *= HOURANGLE;
  dec *= DEGREE;
  cosd = cos(dec);

  pos[0] = cosd * cos(ra);
  pos[1] = cosd * sin(ra);
  pos[2] = sin(dec);

  // Convert the vector from equatorial to ecliptic system.
  prop_error(fn, equ2ecl_vec(jd_tt, coord_sys, accuracy, pos, pos), 0);

  // Decompose ecliptic vector into ecliptic longitude and latitude.
  xyproj = sqrt(pos[0] * pos[0] + pos[1] * pos[1]);

  *elon = (xyproj > 0.0) ? atan2(pos[1], pos[0]) / DEGREE : 0.0;
  if(*elon < 0.0)
    *elon += DEG360;

  *elat = atan2(pos[2], xyproj) / DEGREE;

  return 0;
}

/**
 * Converts an equatorial position vector to an ecliptic position vector. To convert
 * ICRS RA and dec to ecliptic coordinates (mean ecliptic and equinox of J2000.0), set
 * 'coord_sys' to NOVAS_GCRS_EQUATOR(2); in this case the value of 'jd_tt' can be set
 * to anything, since J2000.0 is assumed. Otherwise, all input coordinates are dynamical
 * at 'jd_tt'.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date. (Unused if 'coord_sys'
 *                    is NOVAS_GCRS_EQUATOR[2])
 * @param coord_sys   The astrometric reference system type of the coordinates.
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in          Position vector, referred to specified equator and equinox of date.
 * @param[out] out    Position vector, referred to specified ecliptic and equinox of date.
 *                    It can be the same vector as the input. If 'coord_sys' is
 *                    NOVAS_GCRS_EQUATOR(2), the input GCRS coordinates are converted to
 *                    J2000 ecliptic coordinates.
 * @return            0 if successful, -1 if either vector argument is NULL or the accuracy
 *                    is invalid, or else 1 if the value of 'coord_sys' is invalid.
 *
 * @sa equ2ecl()
 * @sa ecl2equ_vec()
 */
short equ2ecl_vec(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, const double *in, double *out) {
  static const char *fn = "equ2ecl_vec";
  double pos0[3], obl, c, s;

  if(!in || !out)
    return novas_error(-1, EINVAL, fn, "NULL input or output 3-vector: in=%p, out=%p", in, out);

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  // Get obliquity, depending upon the "system" of the input coordinates.
  switch(coord_sys) {
    case NOVAS_MEAN_EQUATOR:      // Input: mean equator and equinox of date
    case NOVAS_TRUE_EQUATOR: {    // Input: true equator and equinox of date
      static THREAD_LOCAL enum novas_accuracy acc_last = -1;
      static THREAD_LOCAL double t_last = 0.0, oblm, oblt;

      memcpy(pos0, in, sizeof(pos0));

      if(!oblm || !time_equals(jd_tt, t_last) || accuracy != acc_last) {
        const double jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY; // TDB date
        e_tilt(jd_tdb, accuracy, &oblm, &oblt, NULL, NULL, NULL);
        t_last = jd_tt;
        acc_last = accuracy;
      }

      obl = (coord_sys == NOVAS_MEAN_EQUATOR ? oblm : oblt) * DEGREE;
      break;
    }

    case NOVAS_GCRS_EQUATOR: /* Input: GCRS */{
      static THREAD_LOCAL enum novas_accuracy acc_2000 = -1;
      static THREAD_LOCAL double ob2000;

      frame_tie(in, ICRS_TO_J2000, pos0);

      if(accuracy != acc_2000) {
        ob2000 = mean_obliq(JD_J2000);
        acc_2000 = accuracy;
      }

      obl = ob2000 * ARCSEC;
      break;
    }

    default:
      return novas_error(1, EINVAL, fn, "invalid equator type: %d", coord_sys);
  }

  c = cos(obl);
  s = sin(obl);

  // Rotate position vector to ecliptic system.
  out[0] = pos0[0];
  out[1] = pos0[1] * c + pos0[2] * s;
  out[2] = -pos0[1] * s + pos0[2] * c;

  return 0;
}

/**
 * Converts an ecliptic position vector to an equatorial position vector. To convert
 * ecliptic coordinates (mean ecliptic and equinox of J2000.0) to GCRS RA and dec to, set
 * 'coord_sys' to NOVAS_GCRS_EQUATOR(2); in this case the value of 'jd_tt' can be set to
 * anything, since J2000.0 is assumed. Otherwise, all input coordinates are dynamical at
 * 'jd_tt'.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date. (Unused if 'coord_sys'
 *                    is NOVAS_GCRS_EQUATOR[2])
 * @param coord_sys   The astrometric reference system type of the coordinates
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in          Position vector, referred to specified ecliptic and equinox of date.
 * @param[out] out    Position vector, referred to specified equator and equinox of date. It
 *                    can be the same vector as the input.
 * @return            0 if successful, -1 if either vector argument is NULL or the accuracy
 *                    is invalid, or else 1 if the value of 'coord_sys' is invalid.
 *
 * @sa ecl2equ()
 * @sa equ2ecl_vec()
 */
short ecl2equ_vec(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, const double *in, double *out) {
  static const char *fn = "ecl2equ_vec";
  double obl = 0.0, c, s, x, y, z;

  if(!in || !out)
    return novas_error(-1, EINVAL, fn, "NULL input or output 3-vector: in=%p, out=%p", in, out);

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  // Get obliquity, depending upon the "system" of the input coordinates.
  switch(coord_sys) {

    case NOVAS_MEAN_EQUATOR:        // Output: mean equator and equinox of date
    case NOVAS_TRUE_EQUATOR: {      // Output: true equator and equinox of date
      static THREAD_LOCAL enum novas_accuracy acc_last = -1;
      static THREAD_LOCAL double t_last = 0.0, oblm, oblt;

      if(!oblm || !time_equals(jd_tt, t_last) || accuracy != acc_last) {
        const double jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY;    // TDB date
        e_tilt(jd_tdb, accuracy, &oblm, &oblt, NULL, NULL, NULL);
        t_last = jd_tt;
        acc_last = accuracy;
      }

      obl = (coord_sys == NOVAS_MEAN_EQUATOR ? oblm : oblt) * DEGREE;
      break;
    }

    case NOVAS_GCRS_EQUATOR: {      // Output: GCRS
      static THREAD_LOCAL double ob2000;

      if(ob2000 == 0.0) {
        ob2000 = mean_obliq(JD_J2000);
      }

      obl = ob2000 * ARCSEC;
      break;
    }

    default:
      return novas_error(1, EINVAL, fn, "invalid equator type: %d", coord_sys);
  }

  x = in[0];
  y = in[1];
  z = in[2];

  c = cos(obl);
  s = sin(obl);

  // Rotate position vector to ecliptic system.
  out[0] = x;
  out[1] = y * c - z * s;
  out[2] = y * s + z * c;

  // Case where output vector is to be in ICRS, rotate from dynamical
  // system to ICRS.
  if(coord_sys == NOVAS_GCRS_EQUATOR) {
    frame_tie(out, J2000_TO_ICRS, out);
  }

  return 0;
}

/**
 * Transforms topocentric (TOD) right ascension and declination to zenith distance and
 * azimuth. This method should not be used to convert CIRS apparent coordinates (IAU
 * 2000 standard) -- for those you should use cirs_to_itrs() followed by itrs_to_hor()
 * instead.
 *
 * It uses a method that properly accounts for polar motion, which is significant at the
 * sub-arcsecond level. This function can also adjust coordinates for atmospheric
 * refraction.
 *
 * @deprecated  The name of this function does not reveal what type of equatorial
 *              coordinates it requires. To make it less ambiguous, you should use
 *              tod_to_itrs() followed by itrs_to_hor() instead, possibly following it
 *              with refract_astro() if you also want to apply optical refraction.
 *
 * NOTES:
 * <ul>
 *  <li>'xp' and 'yp' can be set to zero if sub-arcsecond accuracy is not needed.</li>
 *  <li> The directions 'zd'= 0 (zenith) and 'az'= 0 (north) are here considered fixed in
 *  the terrestrial system. Specifically, the zenith is along the geodetic normal, and north
 *  is toward the ITRS pole.</li>
 *  <li>If 'ref_option' is NOVAS_STANDARD_ATMOSPHERE (1), then 'rar'='ra' and 'decr'='dec'.
 * </ul>
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. (2008). USNO/AA Technical Note of 28 Apr 2008, "Refraction as a Vector."</li>
 * </ol>
 *
 * @param jd_ut1      [day] UT1 based Julian date
 * @param ut1_to_tt   [s] TT - UT1 Time difference in seconds
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param xp          [arcsec] Conventionally-defined x coordinate of celestial intermediate
 *                    pole with respect to ITRS reference pole, in arcseconds.
 * @param yp          [arcsec] Conventionally-defined y coordinate of celestial intermediate
 *                    pole with respect to ITRS reference pole, in arcseconds.
 * @param location    The observer location
 * @param ra          [h] Topocentric right ascension of object of interest, in hours, referred
 *                    to true equator and equinox of date.
 * @param dec         [deg] Topocentric declination of object of interest, in degrees, referred
 *                    to true equator and equinox of date.
 * @param ref_option  NOVAS_STANDARD_ATMOSPHERE (1), or NOVAS_WEATHER_AT_LOCATION (2) if to use
 *                    the weather
 * @param[out] zd     [deg] Topocentric zenith distance in degrees (unrefracted).
 * @param[out] az     [deg] Topocentric azimuth (measured east from north) in degrees.
 * @param[out] rar    [h] Topocentric right ascension of object of interest, in hours, referred
 *                    to true equator and equinox of date, affected by refraction if 'ref_option'
 *                    is non-zero. (It may be NULL if not required)
 * @param[out] decr   [deg] Topocentric declination of object of interest, in degrees, referred
 *                    to true equator and equinox of date. (It may be NULL if not required)
 * @return            0 if successful, or -1 if one of the 'zd' or 'az' output pointers are NULL.
 *
 * @sa itrs_to_hor()
 * @sa tod_to_itrs()
 * @sa NOVAS_TOD
 *
 */
int equ2hor(double jd_ut1, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const on_surface *location, double ra,
        double dec, enum novas_refraction_model ref_option, double *zd, double *az, double *rar, double *decr) {

  double lon, lat, sinlat, coslat, sinlon, coslon, cosdec;
  double uze[3], une[3], uwe[3], uz[3], un[3], uw[3], p[3];
  double pz, pn, pw, proj, pr[3];

  // Default output values in case of error return;
  if(az)
    *az = NAN;
  if(zd)
    *zd = NAN;
  if(rar)
    *rar = ra;
  if(decr)
    *decr = dec;

  if(!location || !zd || !az)
    return novas_error(-1, EINVAL, "equ2hor", "NULL input observer location=%p, output az=%p, or zd=%p pointer", location, az, zd);

  lon = location->longitude * DEGREE;
  lat = location->latitude * DEGREE;

  sinlat = sin(lat);
  coslat = cos(lat);
  sinlon = sin(lon);
  coslon = cos(lon);

  ra *= HOURANGLE;
  dec *= DEGREE;

  cosdec = cos(dec);

  // Define unit vector 'p' toward object in celestial system
  // (wrt equator and equinox of date).
  p[0] = cosdec * cos(ra);
  p[1] = cosdec * sin(ra);
  p[2] = sin(dec);

  // Set up orthonormal basis vectors in local Earth-fixed system.

  // Define vector toward local zenith in Earth-fixed system (z axis).
  uze[0] = coslat * coslon;
  uze[1] = coslat * sinlon;
  uze[2] = sinlat;

  // Define vector toward local north in Earth-fixed system (x axis).
  une[0] = -sinlat * coslon;
  une[1] = -sinlat * sinlon;
  une[2] = coslat;

  // Define vector toward local west in Earth-fixed system (y axis).
  uwe[0] = sinlon;
  uwe[1] = -coslon;
  uwe[2] = 0.0;

  // Obtain vectors in celestial system.

  // Rotate Earth-fixed orthonormal basis vectors to celestial system
  // (wrt equator and equinox of date).
  ter2cel(jd_ut1, 0.0, ut1_to_tt, EROT_GST, accuracy, NOVAS_DYNAMICAL_CLASS, xp, yp, uze, uz);
  ter2cel(jd_ut1, 0.0, ut1_to_tt, EROT_GST, accuracy, NOVAS_DYNAMICAL_CLASS, xp, yp, une, un);
  ter2cel(jd_ut1, 0.0, ut1_to_tt, EROT_GST, accuracy, NOVAS_DYNAMICAL_CLASS, xp, yp, uwe, uw);

  // Compute coordinates of object w.r.t orthonormal basis.

  // Compute components of 'p' - projections of 'p' onto rotated
  // Earth-fixed basis vectors.
  pz = novas_vdot(p, uz);
  pn = novas_vdot(p, un);
  pw = novas_vdot(p, uw);

  // Compute azimuth and zenith distance.
  proj = sqrt(pn * pn + pw * pw);
  *az = proj ? -atan2(pw, pn) / DEGREE : 0.0;
  if(*az < 0.0)
    *az += DEG360;

  *zd = atan2(proj, pz) / DEGREE;

  // Apply atmospheric refraction if requested.
  if(ref_option) {
    // Get refraction in zenith distance.
    const double zd0 = *zd;
    const double refr = refract_astro(location, ref_option, zd0);

    // Apply refraction to celestial coordinates of object.
    if(refr > 0.0) {
      // Shift position vector of object in celestial system to account
      // for refraction (see USNO/AA Technical Note 1998-09).
      const double sinzd = sin(*zd * DEGREE);
      const double coszd = cos(*zd * DEGREE);
      const double sinzd0 = sin(zd0 * DEGREE);
      const double coszd0 = cos(zd0 * DEGREE);

      int j;

      *zd -= refr;

      // Compute refracted position vector.
      for(j = 3; --j >= 0;)
        pr[j] = ((p[j] - coszd0 * uz[j]) / sinzd0) * sinzd + uz[j] * coszd;

      // Compute refracted right ascension and declination.
      proj = sqrt(pr[0] * pr[0] + pr[1] * pr[1]);

      if(rar) {
        *rar = proj ? atan2(pr[1], pr[0]) / HOURANGLE : 0.0;
        if(*rar < 0.0)
          *rar += DAY_HOURS;
      }

      if(decr)
        *decr = atan2(pr[2], proj) / DEGREE;
    }
  }
  return 0;
}

/**
 * Converts GCRS right ascension and declination to coordinates with respect to the equator
 * of date (mean or true). For coordinates with respect to the true equator of date, the
 * origin of right ascension can be either the true equinox or the celestial intermediate
 * origin (CIO). This function only supports the CIO-based method.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date. (Unused if 'coord_sys'
 *                    is NOVAS_ICRS_EQUATOR)
 * @param sys         Dynamical equatorial system type
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1) (unused if
 *                    'coord_sys' is not NOVAS_ICRS [3])
 * @param rag         [h] GCRS right ascension in hours.
 * @param decg        [deg] GCRS declination in degrees.
 * @param[out] ra     [h] Right ascension in hours, referred to specified equator and right
 *                    ascension origin of date.
 * @param[out] dec    [deg] Declination in degrees, referred to specified equator of date.
 * @return            0 if successful, or -1 with errno set to EINVAL if the output pointers
 *                    are NULL or the coord_sys is invalid, otherwise &lt;0 if an error from
 *                    vector2radec(), 10--20 error is  10 + error cio_location(); or else
 *                    20 + error from cio_basis()
 */
short gcrs2equ(double jd_tt, enum novas_dynamical_type sys, enum novas_accuracy accuracy, double rag, double decg, double *ra, double *dec) {
  static const char *fn = "gcrs2equ";
  double jd_tdb, r, d, pos1[3], pos2[3];

  if(!ra || !dec)
    return novas_error(-1, EINVAL, fn, "NULL output pointer: ra=%p, dec=%p", ra, dec);

  // 'jd_tdb' is the TDB Julian date.
  jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY;

  // Form position vector in equatorial system from input coordinates.
  r = rag * 15.0 * DEG2RAD;
  d = decg * DEG2RAD;

  pos1[0] = cos(d) * cos(r);
  pos1[1] = cos(d) * sin(r);
  pos1[2] = sin(d);

  // Transform the position vector based on the value of 'coord_sys'.
  switch(sys) {
    // Transform the position vector from GCRS to mean equator and equinox
    // of date.

    // If requested, transform further to true equator and equinox of date.
    case NOVAS_DYNAMICAL_TOD:
      gcrs_to_tod(jd_tdb, accuracy, pos1, pos2);
      break;

    case NOVAS_DYNAMICAL_MOD: {
      gcrs_to_mod(jd_tdb, pos1, pos2);
      break;
    }

    case NOVAS_DYNAMICAL_CIRS:
      prop_error(fn, gcrs_to_cirs(jd_tdb, accuracy, pos1, pos2), 10);
      break;

    default:
      return novas_error(-1, EINVAL, fn, "invalid dynamical system type: %d", sys);
  }

  // Convert the position vector to equatorial spherical coordinates.
  prop_error(fn, -vector2radec(pos2, ra, dec), 0);

  return 0;
}

/**
 * Computes the Greenwich sidereal time, either mean or apparent, at the specified Julian date.
 * The Julian date can be broken into two parts if convenient, but for the highest precision,
 * set 'jd_high' to be the integral part of the Julian date, and set 'jd_low' to be the fractional
 * part.
 *
 * NOTES:
 * <ol>
 * <li>Contains fix for known <a href="https://aa.usno.navy.mil/software/novas_faq">sidereal time
 * units bug.</a></li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 * </ol>
 *
 * @param jd_ut1_high [day] High-order part of UT1 Julian date.
 * @param jd_ut1_low  [day] Low-order part of UT1 Julian date. (You can leave it at zero if
 *                    'jd_high' specified the date with sufficient precision)
 * @param ut1_to_tt   [s] TT - UT1 Time difference in seconds
 * @param gst_type    NOVAS_MEAN_EQUINOX (0) or NOVAS_TRUE_EQUINOX (1), depending on whether
 *                    wanting mean or apparent GST, respectively.
 * @param erot        EROT_ERA (0) or EROT_GST (1), depending on whether to use GST relative
 *                    to equinox of date (pre IAU 2006) or ERA relative to the CIO (IAU 2006
 *                    standard).
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] gst    [h] Greenwich (mean or apparent) sidereal time, in hours [0:24]. (In case
 *                    the returned error code is &gt;1 the gst value will be set to NAN.)
 * @return            0 if successful, or -1 if the 'gst' argument is NULL, 1 if 'accuracy' is
 *                    invalid 2 if 'method' is invalid, or else 10--30 with 10 + the error from
 *                    cio_location().
 *
 * @sa era()
 * @sa tod_to_itrs()
 * @sa itrs_to_tod()
 * @sa cel_pole()
 * @sa get_ut1_to_tt()
 */
short sidereal_time(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_equinox_type gst_type,
        enum novas_earth_rotation_measure erot, enum novas_accuracy accuracy, double *gst) {
  static const char *fn = "sidereal_time";
  double jd_ut, jd_tt, jd_tdb, t, theta, st, eqeq;

  if(!gst)
    return novas_error(-1, EINVAL, fn, "NULL 'gst' output pointer");

  // Default return value
  *gst = NAN;

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  // Time argument for precession and nutation components of sidereal
  // time is TDB.  First approximation is TDB = TT, then refine.
  jd_ut = jd_ut1_high + jd_ut1_low;
  jd_tt = jd_ut + (ut1_to_tt / DAY);
  jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY;

  t = (jd_tdb - JD_J2000) / JULIAN_CENTURY_DAYS;

  // Compute the Earth Rotation Angle.  Time argument is UT1.
  theta = era(jd_ut1_high, jd_ut1_low);

  // Compute the equation of the equinoxes if needed, depending upon the
  // input values of 'gst_type' and 'method'.  If not needed, set to zero.
  if(((gst_type == NOVAS_MEAN_EQUINOX) && (erot == EROT_ERA))       // GMST; CIO-TIO
          || ((gst_type == NOVAS_TRUE_EQUINOX) && (erot == EROT_GST))) {    // GAST; equinox
    static THREAD_LOCAL enum novas_accuracy acc_last = -1;
    static THREAD_LOCAL double jd_last;
    static THREAD_LOCAL double ee;

    if(!time_equals(jd_tdb, jd_last) || accuracy != acc_last) {
      e_tilt(jd_tdb, accuracy, NULL, NULL, &ee, NULL, NULL);
      jd_last = jd_tdb;
      acc_last = accuracy;
    }
    eqeq = ee * 15.0;
  }
  else {
    eqeq = 0.0;
  }

  // AK: default return value.
  *gst = NAN;

  // Compute Greenwich sidereal time depending upon input values of
  // method' and 'gst_type'.
  switch(erot) {
    case EROT_ERA: {
      // Use 'CIO-TIO-theta' method.  See Circular 179, Section 6.5.4.
      const double ux[3] = { 1.0, 0.0, 0.0 };
      double ra_cio, ha_eq, x[3], y[3], z[3], eq[3];
      short ref_sys;

      // Obtain the basis vectors, in the GCRS, of the celestial intermediate system.
      prop_error(fn, cio_location(jd_tdb, accuracy, &ra_cio, &ref_sys), 10);

      cio_basis(jd_tdb, ra_cio, ref_sys, accuracy, x, y, z);

      // Compute the direction of the true equinox in the GCRS.
      tod_to_gcrs(jd_tdb, accuracy, ux, eq);

      // Compute the hour angle of the equinox wrt the TIO meridian
      // (near Greenwich, but passes through the CIP and TIO).
      ha_eq = theta - atan2(novas_vdot(eq, y), novas_vdot(eq, x)) / DEGREE;

      // For mean sidereal time, subtract the equation of the equinoxes.

      // AK: Fix for documented bug in NOVAS 3.1 --> 3.1.1
      ha_eq -= (eqeq / 3600.0);

      ha_eq = remainder(ha_eq / 15.0, DAY_HOURS);
      if(ha_eq < 0.0)
        ha_eq += DAY_HOURS;

      *gst = ha_eq;
      return 0;
    }

    case EROT_GST:
      // Use equinox method.  See Circular 179, Section 2.6.2.

      // Precession-in-RA terms in mean sidereal time taken from third
      // reference, eq. (42), with coefficients in arcseconds.
      st = eqeq + 0.014506 + ((((-0.0000000368 * t - 0.000029956) * t - 0.00000044) * t + 1.3915817) * t + 4612.156534) * t;

      // Form the Greenwich sidereal time.
      *gst = remainder((st / 3600.0 + theta) / 15.0, DAY_HOURS);
      if(*gst < 0.0)
        *gst += DAY_HOURS;
      return 0;

    default:        // Invalid value of 'method'.
      return novas_error(2, EINVAL, fn, "invalid Earth rotation measure type: %d", erot);
  }
}

/**
 * Returns the value of the Earth Rotation Angle (theta) for a given UT1 Julian date.  The
 * expression used is taken from the note to IAU Resolution B1.8 of 2000. The input Julian date
 * cane be split into an into high and low order parts (e.g. integer and fractional parts) for
 * improved accuracy, or else one of the components (e.g. the low part) can be set to zero if
 * no split is desired.
 *
 * The algorithm used here is equivalent to the canonical theta = 0.7790572732640 +
 * 1.00273781191135448 * t, where t is the time in days from J2000 (t = jd_high + jd_low -
 * JD_J2000), but it avoids many two-PI 'wraps' that decrease precision (adopted from SOFA Fortran
 * routine iau_era00; see also expression at top of page 35 of IERS Conventions (1996)).
 *
 *  REFERENCES:
 *  <ol>
 *   <li>IAU Resolution B1.8, adopted at the 2000 IAU General Assembly, Manchester, UK.</li>
 *   <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 *  </ol>
 *
 * @param jd_ut1_high   [day] High-order part of UT1 Julian date.
 * @param jd_ut1_low    [day] Low-order part of UT1 Julian date.
 * @return              [deg] The Earth Rotation Angle (theta) in degrees [0:360].
 *
 * @sa sidereal_time()
 * @sa cirs_to_itrs()
 * @sa itrs_to_cirs()
 */
double era(double jd_ut1_high, double jd_ut1_low) {
  double theta, thet1, thet2, thet3;

  thet1 = remainder(0.7790572732640 + 0.00273781191135448 * (jd_ut1_high - JD_J2000), 1.0);
  thet2 = remainder(0.00273781191135448 * jd_ut1_low, 1.0);
  thet3 = remainder(jd_ut1_high, 1.0) + remainder(jd_ut1_low, 1.0);

  theta = remainder(thet1 + thet2 + thet3, 1.0) * DEG360;
  if(theta < 0.0)
    theta += DEG360;

  return theta;
}

/**
 * Rotates a vector from the terrestrial to the celestial system. Specifically, it transforms
 * a vector in the ITRS (rotating earth-fixed system) to the True of Date (TOD), CIRS, or GCRS
 * (a local space-fixed system) by applying rotations for polar motion, Earth rotation (for
 * TOD); and nutation, precession, and the dynamical-to-GCRS frame tie (for GCRS).
 *
 * If 'system' is NOVAS_CIRS then method EROT_ERA must be used. Similarly, if 'system' is
 * NOVAS_TOD then method must be EROT_ERA. Otherwise an error 3 is returned.
 *
 * If both 'xp' and 'yp' are set to 0 no polar motion is included in the transformation.
 *
 * @deprecated This function can be confusing to use due to the output coordinate system
 *             being specified by a combination of two options. Use itrs_to_cirs() or
 *             itrs_to_tod() instead. You can then follow these with other conversions to
 *             GCRS (or whatever else) as appropriate.
 *
 * REFERENCES:
 *  <ol>
 *   <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *   <li>Kaplan, G. H. (2003), 'Another Look at Non-Rotating Origins', Proceedings of IAU XXV
 *   Joint Discussion 16.</li>
 *  </ol>
 *
 * @param jd_ut1_high   [day] High-order part of UT1 Julian date.
 * @param jd_ut1_low    [day] Low-order part of UT1 Julian date.
 * @param ut1_to_tt     [s] TT - UT1 Time difference in seconds
 * @param erot          EROT_ERA (0) or EROT_GST (1), depending on whether to use GST relative
 *                      to equinox of date (pre IAU 2006) or ERA relative to the CIO (IAU 2006
 *                      standard) as the Earth rotation measure. The main effect of this option
 *                      is that it selects the output coordinate system as CIRS or TOD if
 *                      the output coordinate class is NOVAS_DYNAMICAL_CLASS.
 * @param accuracy      NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param class         Output coordinate class NOVAS_REFERENCE_CLASS (0, or any value other than 1)
 *                      or NOVAS_DYNAMICAL_CLASS (1). Use the former if the output coordinates are
 *                      to be in the GCRS, and the latter if they are to be in CIRS or TOD (the 'erot'
 *                      parameter selects which dynamical system to use for the output.)
 * @param xp            [arcsec] Conventionally-defined X coordinate of celestial intermediate
 *                      pole with respect to ITRS pole, in arcseconds.
 * @param yp            [arcsec] Conventionally-defined Y coordinate of celestial intermediate
 *                      pole with respect to ITRS pole, in arcseconds.
 * @param in            Position vector, geocentric equatorial rectangular coordinates,
 *                      referred to ITRS axes (terrestrial system) in the normal case
 *                      where 'option' is NOVAS_GCRS (0).
 * @param[out] out      Position vector, equatorial rectangular coordinates in the specified
 *                      output system (GCRS if 'class' is NOVAS_REFERENCE_CLASS;
 *                      or else either CIRS if 'erot' is EROT_ERA, or TOD if 'erot' is EROT_GST).
 *                      It may be the same vector as the input.
 * @return              0 if successful, -1 if either of the vector arguments is NULL, 1 if
 *                      'accuracy' is invalid, 2 if 'method' is invalid 10--20, or else 10 + the
 *                      error from cio_location(), or 20 + error from cio_basis().
 *
 * @sa itrs_to_cirs()
 * @sa cirs_to_gcrs()
 * @sa itrs_to_tod()
 * @sa tod_to_j2000()
 * @sa frame_tie()
 * @sa cel2ter()
 */
short ter2cel(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_earth_rotation_measure erot, enum novas_accuracy accuracy,
        enum novas_equatorial_class class, double xp, double yp, const double *in, double *out) {
  static const char *fn = "ter2cel";
  double jd_ut1, jd_tt, jd_tdb;

  if(!in || !out)
    return novas_error(-1, EINVAL, fn, "NULL input or output 3-vector: in=%p, out=%p", in, out);

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  // Compute the TT Julian date corresponding to the input UT1 Julian
  // date.
  jd_ut1 = jd_ut1_high + jd_ut1_low;
  jd_tt = jd_ut1 + (ut1_to_tt / DAY);

  // Compute the TDB Julian date corresponding to the input UT1 Julian
  // date.
  jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY;

  // Apply polar motion
  if(xp || yp)
    wobble(jd_tt, WOBBLE_ITRS_TO_PEF, xp, yp, in, out);
  else if(out != in)
    memcpy(out, in, XYZ_VECTOR_SIZE);

  switch(erot) {
    case (EROT_ERA): {
      // 'CIO-TIO-THETA' method. See second reference, eq. (3) and (4).

      // Compute and apply the Earth rotation angle, 'theta', transforming the
      // vector to the celestial intermediate system.
      spin(-era(jd_ut1_high, jd_ut1_low), out, out);

      if(class != NOVAS_DYNAMICAL_CLASS)
        prop_error(fn, cirs_to_gcrs(jd_tdb, accuracy, out, out), 10);

      break;
    }
    case (EROT_GST): {
      double gast;

      sidereal_time(jd_ut1_high, jd_ut1_low, ut1_to_tt, NOVAS_TRUE_EQUINOX, EROT_GST, accuracy, &gast);
      spin(-15.0 * gast, out, out);

      if(class != NOVAS_DYNAMICAL_CLASS) {
        tod_to_gcrs(jd_tdb, accuracy, out, out);
      }
      break;
    }

    default:
      return novas_error(2, EINVAL, fn, "invalid Earth rotation measure type: %d", erot);
  }

  return 0;
}

/**
 * Rotates a vector from the celestial to the terrestrial system.  Specifically, it transforms
 * a vector in the GCRS, or the dynamcal CIRS or TOD frames to the ITRS (a rotating earth-fixed
 * system) by applying rotations for the GCRS-to-dynamical frame tie, precession, nutation, Earth
 * rotation, and polar motion.
 *
 * If 'system' is NOVAS_CIRS then method EROT_ERA must be used. Similarly, if 'system'
 * is NOVAS_TOD then method must be EROT_ERA. Otherwise an error 3 is returned.
 *
 * If both 'xp' and 'yp' are set to 0 no polar motion is included in the transformation.
 *
 * @deprecated This function can be confusing to use due to the input coordinate system
 *             being specified by a combination of two options. Use itrs_to_cirs() or
 *             itrs_to_tod() instead. You can then follow these with other conversions to
 *             GCRS (or whatever else) as appropriate.
 *
 * REFERENCES:
 *  <ol>
 *   <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *   <li>Kaplan, G. H. (2003), 'Another Look at Non-Rotating Origins', Proceedings of IAU XXV J
 *   oint Discussion 16.</li>
 *  </ol>
 *
 * @param jd_ut1_high   [day] High-order part of UT1 Julian date.
 * @param jd_ut1_low    [day] Low-order part of UT1 Julian date.
 * @param ut1_to_tt     [s] TT - UT1 Time difference in seconds
 * @param erot          EROT_ERA (0) or EROT_GST (1), depending on whether to use GST relative to
 *                      equinox of date (pre IAU 2006) or ERA relative to the CIO (IAU 2006 standard)
 *                      as the Earth rotation measure. The main effect of this option
 *                      is that it specifies the input coordinate system as CIRS or TOD when
 *                      the input coordinate class is NOVAS_DYNAMICAL_CLASS.
 * @param accuracy      NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param class         Input coordinate class, NOVAS_REFERENCE_CLASS (0) or NOVAS_DYNAMICAL_CLASS (1).
 *                      Use the former if the input coordinates are in the GCRS, and the latter if they
 *                      are CIRS or TOD (the 'erot' parameter selects which dynamical system the input is
 *                      specified in.)
 * @param xp            [arcsec] Conventionally-defined X coordinate of celestial intermediate
 *                      pole with respect to ITRS pole, in arcseconds.
 * @param yp            [arcsec] Conventionally-defined Y coordinate of celestial intermediate
 *                      pole with respect to ITRS pole, in arcseconds.
 * @param in            Input position vector, geocentric equatorial rectangular coordinates in the
 *                      specified input coordinate system (GCRS if 'class' is NOVAS_REFERENCE_CLASS;
 *                      or else either CIRS if 'erot' is EROT_ERA, or TOD if 'erot' is EROT_GST).
 * @param[out] out      ITRS position vector, geocentric equatorial rectangular coordinates
 *                      (terrestrial system). It can be the same vector as the input.
 * @return              0 if successful, -1 if either of the vector arguments is NULL, 1 if 'accuracy'
 *                      is invalid, 2 if 'method' is invalid, or else 10 + the error from
 *                      cio_location(), or 20 + error from cio_basis().
 *
 * @sa gcrs_to_cirs()
 * @sa cirs_to_itrs()
 * @sa frame_tie()
 * @sa j2000_to_tod()
 * @sa tod_to_itrs()
 * @sa ter2cel()
 */
short cel2ter(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_earth_rotation_measure erot, enum novas_accuracy accuracy,
        enum novas_equatorial_class class, double xp, double yp, const double *in, double *out) {
  static const char *fn = "cel2ter";
  double jd_ut1, jd_tt, jd_tdb;

  if(!in || !out)
    return novas_error(-1, EINVAL, fn, "NULL input or output 3-vector: in=%p, out=%p", in, out);

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  // Compute the TT Julian date corresponding to the input UT1 Julian date.
  jd_ut1 = jd_ut1_high + jd_ut1_low;
  jd_tt = jd_ut1 + (ut1_to_tt / DAY);

  // Compute the TDB Julian date corresponding to the input UT1 Julian date
  jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY;

  switch(erot) {
    case EROT_ERA:
      // IAU 2006 standard method
      if(class != NOVAS_DYNAMICAL_CLASS) {
        // See second reference, eq. (3) and (4).
        prop_error(fn, gcrs_to_cirs(jd_tt, accuracy, in, out), 10);
      }
      else if (out != in) {
        memcpy(out, in, XYZ_VECTOR_SIZE);
      }

      // Compute and apply the Earth rotation angle, 'theta', transforming the
      // vector to the terrestrial intermediate system.
      spin(era(jd_ut1_high, jd_ut1_low), out, out);
      break;

    case EROT_GST: {
      double gast;

      // Pre IAU 2006 method
      if(class != NOVAS_DYNAMICAL_CLASS) {
        gcrs_to_tod(jd_tdb, accuracy, in, out);
      }
      else if (out != in) {
        memcpy(out, in, XYZ_VECTOR_SIZE);
      }

      // Apply Earth rotation.
      sidereal_time(jd_ut1_high, jd_ut1_low, ut1_to_tt, NOVAS_TRUE_EQUINOX, EROT_GST, accuracy, &gast);
      spin(15.0 * gast, out, out);
      break;
    }

    default:
      return novas_error(2, EINVAL, fn, "invalid Earth rotation measure type: %d", erot);
  }

  // Apply polar motion, transforming the vector to the ITRS.
  if(xp || yp)
    wobble(jd_tdb, WOBBLE_PEF_TO_ITRS, xp, yp, out, out);

  return 0;
}

/**
 * Transforms a vector from one coordinate system to another with same origin and axes rotated
 * about the z-axis.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 * </ol>
 *
 * @param angle   [deg] Angle of coordinate system rotation, positive counterclockwise when
 *                viewed from +z, in degrees.
 * @param in      Input position vector.
 * @param[out] out    Position vector expressed in new coordinate system rotated about z by
 *                    'angle'. It can be the same vector as the input.
 *
 * @return        0 if successful, or -1 if the output vector is NULL.
 *
 */
int spin(double angle, const double *in, double *out) {
  static THREAD_LOCAL double ang_last = -999.0;
  static THREAD_LOCAL double xx, yx, xy, yy, zz = 1.0;

  double x, y, z;

  if(!in || !out)
    return novas_error(-1, EINVAL, "spin", "NULL input or output 3-vector: in=%p, out=%p", in, out);

  angle = remainder(angle * DEGREE, TWOPI);

  if(fabs(angle - ang_last) >= 1.0e-12) {
    const double cosang = cos(angle);
    const double sinang = sin(angle);

    // Rotation matrix (non-zero elements only).
    xx = cosang;
    yx = sinang;
    xy = -sinang;
    yy = cosang;

    ang_last = angle;
  }

  x = in[0];
  y = in[1];
  z = in[2];

  // Perform rotation.
  out[0] = xx * x + yx * y;
  out[1] = xy * x + yy * y;
  out[2] = zz * z;

  return 0;
}

/**
 * Corrects a vector in the ITRS (rotating Earth-fixed system) for polar motion, and also
 * corrects the longitude origin (by a tiny amount) to the Terrestrial Intermediate Origin
 * (TIO).  The ITRS vector is thereby transformed to the terrestrial intermediate system,
 * based on the true (rotational) equator and TIO.  Because the true equator is the plane
 * orthogonal to the direction of the Celestial Intermediate Pole (CIP), the components of
 * the output vector are referred to z and x axes toward the CIP and TIO, respectively.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 * <li>Lambert & Bizouard (2002), Astronomy and Astrophysics 394, 317-321.</li>
 * </ol>
 *
 * @param jd_tt         [day] Terrestrial Time (TT) based Julian date.
 * @param direction     WOBBLE_ITRS_TO_PEF (0) or WOBBLE_PEF_TO_ITRS (1; or nonzero)
 * @param xp            [arcsec] Conventionally-defined X coordinate of Celestial Intermediate
 *                      Pole with respect to ITRS pole, in arcseconds.
 * @param yp            [arcsec] Conventionally-defined Y coordinate of Celestial Intermediate
 *                      Pole with respect to ITRS pole, in arcseconds.
 * @param in            Input position vector, geocentric equatorial rectangular coordinates,
 *                      in the original system defined by 'direction'
 * @param[out] out      Output Position vector, geocentric equatorial rectangular coordinates,
 *                      in the final system defined by 'direction'. It can be the same vector
 *                      as the input.
 *
 * @return              0 if successful, or -1 if the output vector argument is NULL.
 *
 * @sa cel_pole()
 * @sa cirs_to_itrs()
 * @sa tod_to_itrs()
 * @sa place()
 * @sa sidereal_time()
 * @sa e_tilt()
 * @sa NOVAS_FULL_ACCURACY
 */
int wobble(double jd_tt, enum novas_wobble_direction direction, double xp, double yp, const double *in, double *out) {
  double xpole, ypole, t, s1, y1;

  if(!in || !out)
    return novas_error(-1, EINVAL, "wobble", "NULL input or output 3-vector: in=%p, out=%p", in, out);

  xpole = xp * ARCSEC;
  ypole = yp * ARCSEC;

  // Compute approximate longitude of TIO (s'), using eq. (10) of the second reference
  t = (jd_tt - JD_J2000) / JULIAN_CENTURY_DAYS;
  s1 = -47.0e-6 * ARCSEC * t;

  y1 = in[1];

  // Compute elements of rotation matrix.
  // Equivalent to R3(-s')R2(x)R1(y) as per IERS Conventions (2003).
  if(direction == WOBBLE_ITRS_TO_PEF)
    tiny_rotate(in, -ypole, -xpole, s1, out);
  else
    tiny_rotate(in, ypole, xpole, -s1, out);

  // Second-order correction for the non-negligible xp, yp product...
  out[0] += xpole * ypole * y1;

  return 0;
}

/**
 * Computes the position and velocity vectors of a terrestrial observer with respect to the
 * center of the Earth.
 *
 * This function ignores polar motion, unless the observer's longitude and latitude have been
 * corrected for it, and variation in the length of day (angular velocity of earth).
 *
 * The true equator and equinox of date do not form an inertial system.  Therefore, with
 * respect to an inertial system, the very small velocity component (several meters/day) due
 * to the precession and nutation of the Earth's axis is not accounted for here.
 *
 *  REFERENCES:
 *  <ol>
 *  <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *  </ol>
 *
 * @param location    Location of observer in Earth's rotating frame
 * @param lst         [h] Local apparent sidereal time at reference meridian in hours.
 * @param[out] pos    [AU]  Position vector of observer with respect to center of Earth,
 *                    equatorial rectangular coordinates, referred to true equator
 *                    and equinox of date, components in AU. If reference meridian is
 *                    Greenwich and 'lst' = 0, 'pos' is
 *                    effectively referred to equator and Greenwich. (It may be NULL if no
 *                    position data is required).
 * @param[out] vel    [AU/day] Velocity vector of observer with respect to center of Earth,
 *                    equatorial rectangular coordinates, referred to true equator
 *                    and equinox of date, components in AU/day. (It must be distinct from
 *                    the pos output vector, and may be NULL if no velocity data is required).
 *
 * @return            0 if successful, or -1 if location is NULL or if the pos and vel output
 *                    arguments are identical pointers.
 *
 * @sa make_on_surface()
 * @sa geo_posvel()
 * @sa sidereal_time()
 */
int terra(const on_surface *location, double lst, double *pos, double *vel) {
  static const char *fn = "terra";
  double df, df2, phi, sinphi, cosphi, c, s, ach, ash, stlocl, sinst, cosst;
  double ht_km;
  int j;

  if(!location)
    return novas_error(-1, EINVAL, fn, "NULL observer location pointer");

  if(pos == vel)
    return novas_error(-1, EINVAL, fn, "identical output pos and vel 3-vectors @ %p", pos);

  // Compute parameters relating to geodetic to geocentric conversion.
  df = 1.0 - EF;
  df2 = df * df;

  phi = location->latitude * DEGREE;
  sinphi = sin(phi);
  cosphi = cos(phi);
  c = 1.0 / sqrt(cosphi * cosphi + df2 * sinphi * sinphi);
  s = df2 * c;
  ht_km = location->height / NOVAS_KM;
  ach = ERAD * c / NOVAS_KM + ht_km;
  ash = ERAD / NOVAS_KM * s + ht_km;

  // Compute local sidereal time factors at the observer's longitude.
  stlocl = lst * HOURANGLE + location->longitude * DEGREE;
  sinst = sin(stlocl);
  cosst = cos(stlocl);

  // Compute position vector components in kilometers.
  if(pos) {
    pos[0] = ach * cosphi * cosst;
    pos[1] = ach * cosphi * sinst;
    pos[2] = ash * sinphi;
  }

  // Compute velocity vector components in kilometers/sec.
  if(vel) {
    vel[0] = -ANGVEL * ach * cosphi * sinst;
    vel[1] = ANGVEL * ach * cosphi * cosst;
    vel[2] = 0.0;
  }

  // Convert position and velocity components to AU and AU/DAY.
  for(j = 0; j < 3; j++) {
    if(pos)
      pos[j] /= AU_KM;
    if(vel)
      vel[j] /= AU_KM / DAY;
  }

  return 0;
}

/**
 * Computes quantities related to the orientation of the Earth's rotation axis at Julian date 'jd_tdb'.
 *
 *  Values of the celestial pole offsets 'PSI_COR' and 'EPS_COR' are set using function 'cel_pole', if
 *  desired.  See the prolog of cel_pole() for details.
 *
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TDB) based Julian date.
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] mobl      [deg] Mean obliquity of the ecliptic in degrees. It may be NULL if not required.
 * @param[out] tobl      [deg] True obliquity of the ecliptic in degrees. It may be NULL if not required.
 * @param[out] ee        [deg] Equation of the equinoxes in seconds of time. It may be NULL if not required.
 * @param[out] dpsi      [arcsec] Nutation in longitude in arcseconds. It may be NULL if not required.
 * @param[out] deps      [arcsec] Nutation in obliquity in arcseconds. It may be NULL if not required.
 *
 * @return          0 if successful, or -1 if the accuracy argument is invalid
 *
 * @sa cel_pole()
 * @sa place()
 * @sa equ2ecl()
 * @sa ecl2equ()
 */
int e_tilt(double jd_tdb, enum novas_accuracy accuracy, double *mobl, double *tobl, double *ee, double *dpsi, double *deps) {
  static THREAD_LOCAL enum novas_accuracy acc_last = -1;
  static THREAD_LOCAL double jd_last = 0;
  static THREAD_LOCAL double d_psi, d_eps, mean_ob, true_ob, c_terms;

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, "e_tilt", "invalid accuracy: %d", accuracy);

  // Compute the nutation angles (arcseconds) if the input Julian date
  // is significantly different from the last Julian date, or the
  // accuracy mode has changed from the last call.
  if(!time_equals(jd_tdb, jd_last) || (accuracy != acc_last)) {
    // Compute time in Julian centuries from epoch J2000.0.
    const double t = (jd_tdb - JD_J2000) / JULIAN_CENTURY_DAYS;

    nutation_angles(t, accuracy, &d_psi, &d_eps);

    // Obtain complementary terms for equation of the equinoxes in arcseconds.
    c_terms = ee_ct(jd_tdb, 0.0, accuracy) / ARCSEC;

    // Compute mean obliquity of the ecliptic in degrees.
    mean_ob = mean_obliq(jd_tdb) / 3600.0;

    // Compute true obliquity of the ecliptic in degrees.
    true_ob = mean_ob + d_eps / 3600.0;

    // Reset the values of the last Julian date and last mode.
    jd_last = jd_tdb;
    acc_last = accuracy;
  }

  // Set output values.
  if(dpsi)
    *dpsi = d_psi + PSI_COR;
  if(deps)
    *deps = d_eps + EPS_COR;
  if(ee)
    *ee = (d_psi * cos(mean_ob * DEGREE) + c_terms) / 15.0;
  if(mobl)
    *mobl = mean_ob;
  if(tobl)
    *tobl = true_ob;

  return 0;
}

/// \cond PRIVATE

/**
 * Converts <i>dx,dy</i> pole offsets to d&psi; d&epsilon;. The former is in GCRS, vs the latter in
 * True of Date (TOD) -- and note the different units!
 *
 * NOTES:
 * <ol>
 * <li>The current UT1 - UTC time difference, and polar offsets, historical data and near-term
 * projections are published in the
 * <a href="https://www.iers.org/IERS/EN/Publications/Bulletins/bulletins.html>IERS Bulletins</a>
 * </li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 *  <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 *  <li>Kaplan, G. (2003), USNO/AA Technical Note 2003-03.</li>
 * </ol>
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian Date.
 * @param dx          [mas] Earth orientation: GCRS pole offset dx, e.g. as published by IERS Bulletin A.
 * @param dy          [mas] Earth orientation: GCRS pole offset dy, e.g. as published by IERS Bulletin A.
 * @param[out] dpsi   [arcsec] Calculated TOD orientation d&psi;.
 * @param[out] deps   [arcsec] Calculated TOD orientation d&epsilon;.
 * @return            0
 *
 * @sa cel_pole()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int polar_dxdy_to_dpsideps(double jd_tt, double dx, double dy, double *dpsi, double *deps) {
  // Components of modeled pole unit vector referred to GCRS axes, that is, dx and dy.
  const double t = (jd_tt - JD_J2000) / JULIAN_CENTURY_DAYS;

  // The following algorithm, to transform dx and dy to
  // delta-delta-psi and delta-delta-epsilon, is from eqs. (7)-(9) of the
  // second reference.
  //
  // Trivial model of pole trajectory in GCRS allows computation of dz.
  const double x = (2004.190 * t) * ARCSEC;
  const double dz = -(x + 0.5 * x * x * x) * dx;

  // Form pole offset vector (observed - modeled) in GCRS.
  double dp[3] = { dx * MAS, dy * MAS, dz * MAS };

  // Precess pole offset vector to mean equator and equinox of date.
  gcrs_to_mod(jd_tt, dp, dp);

  // Compute delta-delta-psi and delta-delta-epsilon in arcseconds.
  if(dpsi) {
    // Compute sin_e of mean obliquity of date.
    const double sin_e = sin(mean_obliq(jd_tt) * ARCSEC);
    *dpsi = (dp[0] / sin_e) / ARCSEC;
  }
  if(deps)
    *deps = dp[1] / ARCSEC;

  return 0;
}

/// \endcond

/**
 * specifies the celestial pole offsets for high-precision applications.  Each set of offsets is
 * a correction to the modeled position of the pole for a specific date, derived from observations
 * and published by the IERS.
 *
 * The variables 'PSI_COR' and 'EPS_COR' are used only in NOVAS function e_tilt().
 *
 * This function, if used, should be called before any other NOVAS functions for a given date.
 * Values of the pole offsets specified via a call to this function will be used until explicitly
 * changed.
 *
 * 'tjd' is used only if 'type' is POLE_OFFSETS_X_Y (2), to transform dx and dy to the equivalent
 * &Delta;&delta;&psi; and &Delta;&delta;&epsilon; values.
 *
 * If 'type' is POLE_OFFSETS_X_Y (2), dx and dy are unit vector component corrections, but are
 * expressed in milliarcseconds simply by multiplying by 206264806, the number of milliarcseconds
 * in one radian.
 *
 * NOTES:
 * <ol>
 * <li>The current UT1 - UTC time difference, and polar offsets, historical data and near-term
 * projections are published in the
 * <a href="https://www.iers.org/IERS/EN/Publications/Bulletins/bulletins.html>IERS Bulletins</a>
 * </li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 *  <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 *  <li>Kaplan, G. (2003), USNO/AA Technical Note 2003-03.</li>
 * </ol>
 *
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian date.
 * @param type      POLE_OFFSETS_DPSI_DEPS (1) or POLE_OFFSETS_X_Y (2)
 * @param dpole1    [mas] Value of celestial pole offset in first coordinate, (&Delta;&delta;&psi;
 *                  or dx) in milliarcseconds.
 * @param dpole2    [mas] Value of celestial pole offset in second coordinate, (&Delta;&delta;&epsilon;
 *                  or dy) in milliarcseconds.
 * @return          0 if successful, or else 1 if 'type' is invalid.
 *
 * @sa cirs_to_itrs()
 * @sa tod_to_itrs()
 * @sa e_tilt()
 * @sa place()
 * @sa get_ut1_to_tt()
 * @sa sidereal_time()
 * @sa NOVAS_FULL_ACCURACY
 */
short cel_pole(double jd_tt, enum novas_pole_offset_type type, double dpole1, double dpole2) {
  switch(type) {
    case POLE_OFFSETS_DPSI_DEPS:

      // Angular coordinates of modeled pole referred to mean ecliptic of
      // date, that is,delta-delta-psi and delta-delta-epsilon.
      PSI_COR = dpole1 * 1.0e-3;
      EPS_COR = dpole2 * 1.0e-3;
      break;

    case POLE_OFFSETS_X_Y: {
      polar_dxdy_to_dpsideps(jd_tt, dpole1, dpole2, &PSI_COR, &EPS_COR);
      break;
    }

    default:
      return novas_error(1, EINVAL, "cel_pole", "invalid polar offset type: %d", type);
  }

  return 0;
}

/**
 * Computes the "complementary terms" of the equation of the equinoxes. The input Julian date
 * can be split into high and low order parts for improved accuracy. Typically, the split is
 * into integer and fractiona parts. If the precision of a single part is sufficient, you may
 * set the low order part to 0.
 *
 * The series used in this function was derived from the first reference.  This same series was
 * also adopted for use in the IAU's Standards of Fundamental Astronomy (SOFA) software (i.e.,
 * subroutine eect00.for and function <code>eect00.c</code>).
 *
 * The low-accuracy series used in this function is a simple implementation derived from the first
 * reference, in which terms smaller than 2 microarcseconds have been omitted.
 *
 * REFERENCES:
 * <ol>
 * <li>Capitaine, N., Wallace, P.T., and McCarthy, D.D. (2003). Astron. & Astrophys. 406, p.
 * 1135-1149. Table 3.</li>
 * <li>IERS Conventions (2010), Chapter 5, p. 60, Table 5.2e.<br>
 * (Table 5.2e presented in the printed publication is a truncated
 * series. The full series, which is used in NOVAS, is available
 * on the IERS Conventions Center website:
 * <a href="ftp://tai.bipm.org/iers/conv2010/chapter5/tab5.2e.txt">
 * ftp://tai.bipm.org/iers/conv2010/chapter5/tab5.2e.txt</a>)
 * </li>
 * </ol>
 *
 * @param jd_tt_high  [day] High-order part of TT based Julian date.
 * @param jd_tt_low   [day] Low-order part of TT based Julian date.
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @return            [rad] Complementary terms, in radians.
 *
 * @sa e_tilt()
 * @sa cel_pole()
 * @sa nutation()
 * @sa sidereal_time()
 */
double ee_ct(double jd_tt_high, double jd_tt_low, enum novas_accuracy accuracy) {

  // Argument coefficients for t^0.
  const int8_t ke0_t[33][14] = { //
          { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 0, 2, -2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 0, 2, -2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 0, 2, -2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 0, 2, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 0, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 1, 2, -2, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 1, 2, -2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 0, 4, -4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 0, 1, -1, 1, 0, -8, 12, 0, 0, 0, 0, 0, 0 }, //
          { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 0, 2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 1, 0, 2, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 1, 0, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 0, 2, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 1, -2, 2, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 1, -2, 2, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 0, 0, 0, 0, 0, 8, -13, 0, 0, 0, 0, 0, -1 }, //
          { 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 2, 0, -2, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 1, 0, 0, -2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 1, 2, -2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 1, 0, 0, -2, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 0, 4, -2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 0, 0, 2, -2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 1, 0, -2, 0, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
          { 1, 0, -2, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };

  // Argument coefficients for t^1.
  //const char ke1[14] = {0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0};

  // Sine and cosine coefficients for t^0.
  const float se0_t[33][2] = { //
          { +2640.96e-6, -0.39e-6 }, //
          { +63.52e-6, -0.02e-6 }, //
          { +11.75e-6, +0.01e-6 }, //
          { +11.21e-6, +0.01e-6 }, //
          { -4.55e-6, +0.00e-6 }, //
          { +2.02e-6, +0.00e-6 }, //
          { +1.98e-6, +0.00e-6 }, //
          { -1.72e-6, +0.00e-6 }, //
          { -1.41e-6, -0.01e-6 }, //
          { -1.26e-6, -0.01e-6 }, //
          { -0.63e-6, +0.00e-6 }, //
          { -0.63e-6, +0.00e-6 }, //
          { +0.46e-6, +0.00e-6 }, //
          { +0.45e-6, +0.00e-6 }, //
          { +0.36e-6, +0.00e-6 }, //
          { -0.24e-6, -0.12e-6 }, //
          { +0.32e-6, +0.00e-6 }, //
          { +0.28e-6, +0.00e-6 }, //
          { +0.27e-6, +0.00e-6 }, //
          { +0.26e-6, +0.00e-6 }, //
          { -0.21e-6, +0.00e-6 }, //
          { +0.19e-6, +0.00e-6 }, //
          { +0.18e-6, +0.00e-6 }, //
          { -0.10e-6, +0.05e-6 }, //
          { +0.15e-6, +0.00e-6 }, //
          { -0.14e-6, +0.00e-6 }, //
          { +0.14e-6, +0.00e-6 }, //
          { -0.14e-6, +0.00e-6 }, //
          { +0.14e-6, +0.00e-6 }, //
          { +0.13e-6, +0.00e-6 }, //
          { -0.11e-6, +0.00e-6 }, //
          { +0.11e-6, +0.00e-6 }, //
          { +0.11e-6, +0.00e-6 } };

  // Sine and cosine coefficients for t^1.
  const double se1[2] = { -0.87e-6, +0.00e-6 };

  novas_delaunay_args fa2;
  double fa[14];

  // Interval between fundamental epoch J2000.0 and current date.
  const double t = ((jd_tt_high - JD_J2000) + jd_tt_low) / JULIAN_CENTURY_DAYS;

  // High accuracy mode.
  if(accuracy == NOVAS_FULL_ACCURACY) {
    double s0 = 0.0, s1 = 0.0;
    int i;

    // Fill the 5 Earth-Sun-Moon fundamental args
    fund_args(t, (novas_delaunay_args*) fa);

    // Add planet longitudes
    for(i = NOVAS_MERCURY; i <= NOVAS_NEPTUNE; i++) {
      int j = i - NOVAS_MERCURY;
      fa[5 + j] = planet_lon(t, i);
    }

    // General accumulated precession longitude
    fa[13] = accum_prec(t);

    // Evaluate the complementary terms.
    for(i = 33; --i >= 0;) {
      const int8_t *ke = &ke0_t[i][0];
      const float *se = &se0_t[i][0];

      double a = 0.0;
      int j;

      for(j = 14; --j >= 0;)
        if(ke[j])
          a += ke[j] * fa[j];

      s0 += se[0] * sin(a);
      if(se[1])
        s0 += se[1] * cos(a);
    }

    // AK: Skip 0 terms from ke1[]
    //
    // a = 0.0;
    // for(j = 0; j < 14; j++) a += (double) (ke1[j]) * fa[j];
    s1 += se1[0] * sin(fa[4]);

    return (s0 + s1 * t) * ARCSEC;
  }

  // Low accuracy mode: Terms smaller than 2 microarcseconds omitted
  fund_args(t, &fa2);

  return (2640.96e-6 * sin(fa2.Omega) //
  + 63.52e-6 * sin(2.0 * fa2.Omega) //
  + 11.75e-6 * sin(2.0 * fa2.F - 2.0 * fa2.D + 3.0 * fa2.Omega) //
  + 11.21e-6 * sin(2.0 * fa2.F - 2.0 * fa2.D + fa2.Omega) //
  - 4.55e-6 * sin(2.0 * fa2.F - 2.0 * fa2.D + 2.0 * fa2.Omega) //
  + 2.02e-6 * sin(2.0 * fa2.F + 3.0 * fa2.Omega) //
  + 1.98e-6 * sin(2.0 * fa2.F + fa2.Omega) //
  - 1.72e-6 * sin(3.0 * fa2.Omega) //
  - 0.87e-6 * t * sin(fa2.Omega) //
  ) * ARCSEC;
}

/**
 * Transforms a vector from the dynamical reference system to the International Celestial
 * Reference System (ICRS), or vice versa. The dynamical reference system is based on the
 * dynamical mean equator and equinox of J2000.0.  The ICRS is based on the space-fixed ICRS
 * axes defined by the radio catalog positions of several hundred extragalactic objects.
 *
 * For geocentric coordinates, the same transformation is used between the dynamical reference
 * system and the GCRS.
 *
 * NOTES:
 * <ol>
 * <li>More efficient 3D rotation implementation for small angles by A. Kovacs</li>
 * </ol>
 *
 * REFERENCES:
 *  <ol>
 *   <li>Hilton, J. and Hohenkerk, C. (2004), Astronomy and Astrophysics 413, 765-770, eq. (6)
 *   and (8).</li>
 *   <li>IERS (2003) Conventions, Chapter 5.</li>
 *  </ol>
 *
 * @param in          Position vector, equatorial rectangular coordinates.
 * @param direction   <0 for for dynamical to ICRS transformation, or else &gt;=0 for ICRS to
 *                    dynamical transformation. Alternatively you may use the constants
 *                    J2000_TO_ICRS (-1; or negative) or ICRS_TO_J2000 (0; or positive).
 * @param[out] out    Position vector, equatorial rectangular coordinates. It can be the same
 *                    vector as the input.
 * @return            0 if successfor or -1 if either of the vector arguments is NULL.
 *
 * @sa j2000_to_gcrs()
 * @sa gcrs_to_j2000()
 * @sa tod_to_j2000()
 * @sa j2000_to_tod()
 * @sa j2000_to_gcrs()
 */
int frame_tie(const double *in, enum novas_frametie_direction direction, double *out) {

  // 'xi0', 'eta0', and 'da0' are ICRS frame biases in arcseconds taken
  // from IERS (2003) Conventions, Chapter 5.
  static const double xi0 = -0.0166170 * ARCSEC;
  static const double eta0 = -0.0068192 * ARCSEC;
  static const double da0 = -0.01460 * ARCSEC;

  if(!in || !out)
    return novas_error(-1, EINVAL, "frame_tie", "NULL input or output 3-vector: in=%p, out=%p", in, out);

  if(direction < 0)
    tiny_rotate(in, -eta0, xi0, da0, out);
  else
    tiny_rotate(in, eta0, -xi0, -da0, out);

  return 0;
}

/**
 * Applies proper motion, including foreshortening effects, to a star's position.
 *
 * REFERENCES:
 * <ol>
 *  <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 * </ol>
 *
 * @param jd_tdb_in  [day] Barycentric Dynamical Time (TDB) based Julian date of the first epoch.
 * @param pos        [AU] Position vector at first epoch.
 * @param vel        [AU/day] Velocity vector at first epoch.
 * @param jd_tdb_out [day] Barycentric Dynamical Time (TDB) based Julian date of the second epoch.
 * @param[out] out   Position vector at second epoch. It can be the same vector as the input.
 * @return           0 if successful, or -1 if any of the vector areguments is NULL.
 *
 * @sa transform_cat()
 */
int proper_motion(double jd_tdb_in, const double *pos, const double *vel, double jd_tdb_out, double *out) {
  const double dt = jd_tdb_out - jd_tdb_in;
  int j;

  if(!pos || !vel || !out)
    return novas_error(-1, EINVAL, "proper_motion", "NULL input or output 3-vector: pos=%p, vel=%p, out=%p", pos, vel, out);

  for(j = 3; --j >= 0;)
    out[j] = pos[j] + vel[j] * dt;

  return 0;
}

/**
 * Moves the origin of coordinates from the barycenter of the solar system to the observer
 * (or the geocenter); i.e., this function accounts for parallax (annual+geocentric or just
 * annual).
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 * </ol>
 *
 * @param pos             [AU] Position vector, referred to origin at solar system barycenter,
 *                        components in AU.
 * @param pos_obs         [AU] Position vector of observer (or the geocenter), with respect to
 *                        origin at solar system barycenter, components in AU.
 * @param[out] out        [AU] Position vector, referred to origin at center of mass of the Earth,
 *                        components in AU. It may be NULL if not required, or be the same vector
 *                        as either of the inputs.
 * @param[out] lighttime  [day] Light time from object to Earth in days. It may be NULL if not
 *                        required.
 * @return                0 if successful, or -1 if any of the essential pointer arguments is NULL.
 *
 * @sa place()
 * @sa light_time2()
 */
int bary2obs(const double *pos, const double *pos_obs, double *out, double *lighttime) {
  int j;

  // Default output value in case of error return
  if(lighttime)
    *lighttime = NAN;

  if(!pos || !pos_obs || !out)
    return novas_error(-1, EINVAL, "bary2obs", "NULL input or output 3-vector: pos=%p, pos_obs=%p, out=%p", pos, pos_obs, out);

  // Translate vector to geocentric coordinates.
  for(j = 3; --j >= 0;)
    out[j] = pos[j] - pos_obs[j];

  // Calculate length of vector in terms of light time.
  if(lighttime)
    *lighttime = novas_vlen(out) / C_AUDAY;

  return 0;
}

/**
 * Computes the geocentric position and velocity of an observer. The final vectors are expressed in the GCRS.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date.
 * @param ut1_to_tt   [s] TT - UT1 time difference in seconds
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param obs         Observer location
 * @param[out] pos    [AU] Position 3-vector of observer, with respect to origin at geocenter,
 *                    referred to GCRS axes, components in AU. (It may be NULL if not required.)
 * @param[out] vel    [AU/day] Velocity 3-vector of observer, with respect to origin at geocenter,
 *                    referred to GCRS axes, components in AU/day. (It must be distinct from the
 *                    pos output vector, and may be NULL if not required)
 * @return            0 if successful, -1 if the 'obs' is NULL or the two output vectors are
 *                    the same, or else 1 if 'accuracy' is invalid, or 2 if 'obserrver->where' is
 *                    invalid.
 *
 * @sa place()
 * @sa make_observer()
 * @sa get_ut1_to_tt()
 * @sa cel_pole()
 */
short geo_posvel(double jd_tt, double ut1_to_tt, enum novas_accuracy accuracy, const observer *obs, double *pos, double *vel) {
  static const char *fn = "geo_posvel";
  static THREAD_LOCAL double t_last = 0;
  static THREAD_LOCAL enum novas_accuracy acc_last = -1;
  static THREAD_LOCAL double gast;

  double gmst, eqeq, pos1[3], vel1[3], jd_tdb, jd_ut1;

  if(!obs)
    return novas_error(-1, EINVAL, fn, "NULL observer location pointer");

  if(pos == vel)
    return novas_error(-1, EINVAL, fn, "identical output pos and vel 3-vectors @ %p", pos);

  // Invalid value of 'accuracy'.
  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  // Compute 'jd_tdb', the TDB Julian date corresponding to 'jd_tt'.
  jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY;

  switch(obs->where) {

    case NOVAS_OBSERVER_AT_GEOCENTER:                   // Observer at geocenter.  Trivial case.
      if(pos)
        memset(pos, 0, XYZ_VECTOR_SIZE);
      if(vel)
        memset(vel, 0, XYZ_VECTOR_SIZE);
      return 0;

      // Other two cases: Get geocentric position and velocity vectors of
      // observer wrt equator and equinox of date.

    case NOVAS_OBSERVER_ON_EARTH:                       // Observer on surface of Earth.
      // Compute UT1 and sidereal time.
      jd_ut1 = jd_tt - (ut1_to_tt / DAY);
      if(!time_equals(jd_ut1, t_last) || accuracy != acc_last) {
        sidereal_time(jd_ut1, 0.0, ut1_to_tt, NOVAS_MEAN_EQUINOX, EROT_ERA, accuracy, &gmst);
        e_tilt(jd_tdb, accuracy, NULL, NULL, &eqeq, NULL, NULL);
        gast = gmst + eqeq / 3600.0;
        t_last = jd_ut1;
        acc_last = accuracy;
      }

      // Function 'terra' does the hard work, given sidereal time.
      terra(&obs->on_surf, gast, pos1, vel1);
      break;

    case NOVAS_OBSERVER_IN_EARTH_ORBIT: {               // Observer on near-earth spacecraft.
      const double kms = DAY / AU_KM;
      int i;

      // Convert units to AU and AU/day.
      for(i = 3; --i >= 0;) {
        pos1[i] = obs->near_earth.sc_pos[i] / AU_KM;
        vel1[i] = obs->near_earth.sc_vel[i] * kms;
      }

      break;
    }

    case NOVAS_AIRBORNE_OBSERVER: {                     // Airborne observer
      const double kms = DAY / AU_KM;
      observer surf = *obs;
      int i;

      surf.where = NOVAS_OBSERVER_ON_EARTH;

      // Get the stationary observer velocity at the location
      prop_error(fn, geo_posvel(jd_tt, ut1_to_tt, accuracy, &surf, pos1, vel1), 0);

      // Add in the aircraft motion
      for(i = 3; --i >= 0;)
        vel1[i] = novas_add_vel(vel1[i], obs->near_earth.sc_vel[i] * kms);

      break;
    }

    case NOVAS_SOLAR_SYSTEM_OBSERVER: {               // Observer in Solar orbit
      const object earth = NOVAS_EARTH_INIT;
      const double tdb[2] = { jd_tdb, 0.0 };
      int i;

      // Get the position and velocity of the geocenter rel. to SSB
      prop_error(fn, ephemeris(tdb, &earth, NOVAS_BARYCENTER, accuracy, pos1, vel1), 0);

      // Return velocities w.r.t. the geocenter.
      for(i = 3; --i >= 0;) {
        if(pos)
          pos[i] = obs->near_earth.sc_pos[i] - pos1[i];
        if(vel)
          vel[i] = novas_add_vel(obs->near_earth.sc_vel[i], -vel1[i]);
      }

      // Already in GCRS...
      return 0;
    }

    default:
      return novas_error(2, EINVAL, fn, "invalid observer type (where): %d", obs->where);
  }

  // Transform geocentric position vector of observer to GCRS.
  if(pos)
    tod_to_gcrs(jd_tdb, accuracy, pos1, pos);

  // Transform geocentric velocity vector of observer to GCRS.
  if(vel)
    tod_to_gcrs(jd_tdb, accuracy, vel1, vel);


  return 0;
}

/**
 * Computes the geocentric position and velocity of a solar system body, as antedated for
 * light-time. It is effectively the same as the original NOVAS C light_time(), except that
 * this returns the antedated source velocity vector also.
 *
 * NOTES:
 * <ol>
 * <li>This function is called by place() to calculate observed positions, radial velocity,
 * and distance for the time when the observed light originated from the source.</li>
 * </ol>
 *
 * @param jd_tdb      [day] Barycentric Dynamical Time (TDB) based Julian date
 * @param body        Pointer to structure containing the designation for the solar system
 *                    body
 * @param pos_obs         [AU] Position 3-vector of observer (or the geocenter), with respect
 *                        to origin at solar system barycenter, referred to ICRS axes,
 *                        components in AU.
 * @param tlight0         [day] First approximation to light-time, in days (can be set to 0.0
 *                        if unknown).
 * @param accuracy        NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] p_src_obs  [AU] Position 3-vector of body, relative to observer, referred to ICRS
 *                        axes, components in AU.
 * @param[out] v_ssb      [AU/day] Velocity 3-vector of body, with respect to the Solar-system
 *                        barycenter, referred to ICRS axes, components in AU/day.
 * @param[out] tlight     [day] Calculated light time, or NAN when returning with an error code.
 *
 * @return            0 if successful, -1 if any of the pointer arguments is NULL or if the
 *                    output vectors are the same or if they are the same as pos_obs, 1 if
 *                    the algorithm failed to converge after 10 iterations, or 10 + the error
 *                    from ephemeris().
 *
 * @sa light_time()
 * @sa place()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int light_time2(double jd_tdb, const object *body, const double *pos_obs, double tlight0, enum novas_accuracy accuracy, double *p_src_obs,
        double *v_ssb, double *tlight) {
  static const char *fn = "light_time2";
  int iter = 0;

  double tol, jd[2] = {0};

  if(!tlight)
    return novas_error(-1, EINVAL, fn, "NULL 'tlight' output pointer");

  // Default return value.
  *tlight = NAN;

  if(!body || !pos_obs)
    return novas_error(-1, EINVAL, fn, "NULL input pointer: body=%p, pos_obs=%p", body, pos_obs);

  if(p_src_obs == pos_obs || v_ssb == pos_obs || p_src_obs == v_ssb)
    return novas_error(-1, EINVAL, fn, "identical 3-vectors: pos_obs=%p, p_src_obs=%p, v_ssb=%p", pos_obs, p_src_obs, v_ssb);

  // Set light-time convergence tolerance.  If full-accuracy option has
  // been selected, split the Julian date into whole days + fraction of
  // day.
  if(accuracy == NOVAS_FULL_ACCURACY) {
    tol = 1.0e-12;

    jd[0] = floor(jd_tdb);
    jd[1] = jd_tdb - jd[0];
  }
  else {
    tol = 1.0e-9;

    jd[0] = jd_tdb;
  }

  // Iterate to obtain correct light-time (usually converges rapidly).
  for(iter = 0; iter < novas_inv_max_iter; iter++) {
    int error;
    double dt = 0.0;

    error = ephemeris(jd, body, NOVAS_BARYCENTER, accuracy, p_src_obs, v_ssb);
    bary2obs(p_src_obs, pos_obs, p_src_obs, tlight);
    prop_error(fn, error, 10);

    dt = *tlight - tlight0;
    if(fabs(dt) <= tol)
      return 0;

    jd[1] -= dt;
    tlight0 = *tlight;
  }

  return novas_error(1, ECANCELED, fn, "failed to converge");
}

/**
 * Computes the geocentric position of a solar system body, as antedated for light-time.
 *
 *
 * @param jd_tdb      [day] Barycentric Dynamical Time (TDB) based Julian date
 * @param body        Pointer to structure containing the designation for the solar system
 *                    body
 * @param pos_obs     [AU] Position 3-vector of observer (or the geocenter), with respect
 *                    to origin at solar system barycenter, referred to ICRS axes,
 *                    components in AU.
 * @param tlight0     [day] First approximation to light-time, in days (can be set to 0.0
 *                    if unknown).
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] pos_src_obs    [AU] Position 3-vector of body, with respect to origin at observer (or
 *                            the geocenter), referred to ICRS axes, components in AU. It can be the
 *                            same vector as either of the inputs.
 * @param[out] tlight [day] Calculated light time
 *
 * @return            0 if successful, -1 if any of the poiinter arguments is NULL, 1 if the
 *                    algorithm failed to converge after 10 iterations, or 10 + the error
 *                    from ephemeris().
 *
 * @sa light_time2()
 * @sa place()
 */
short light_time(double jd_tdb, const object *body, const double *pos_obs, double tlight0, enum novas_accuracy accuracy,
        double *pos_src_obs, double *tlight) {
  double vel[3];
  prop_error("light_time", light_time2(jd_tdb, body, pos_obs, tlight0, accuracy, pos_src_obs, vel, tlight), 0);
  return 0;
}

/**
 * Returns the difference in light-time, for a star, between the barycenter of the solar system
 * and the observer (or the geocenter) (<i>Usage A</i>).
 *
 * Alternatively (<i>Usage B</i>), this function returns the light-time from the observer (or the
 * geocenter) to a point on a light ray that is closest to a specific solar system body.  For this
 * purpose, 'pos_src' is the position vector toward observed object, with respect to origin at
 * observer (or the geocenter); 'pos_body' is the position vector of solar system body, with
 * respect to origin at observer (or the geocenter), components in AU; and the returned value is
 * the light time to point on line defined by 'pos' that is closest to solar system body (positive
 * if light passes body before hitting observer, i.e., if 'pos_body' is within 90 degrees of
 * 'pos_src').
 *
 * NOTES:
 * <ol>
 * <li>This function is called by place()</li>
 * </ol>
 *
 * @param pos_src   Position vector towards observed object, with respect to the SSB
 *                  (<i>Usage A</i>), or relative to the observer / geocenter (<i>Usage B</i>).
 * @param pos_body  [AU] Position of observer relative to SSB (<i>Usage A</i>), or position of
 *                  intermediate solar-system body with respect to the observer / geocenter
 *                  (<i>Usage B</i>).
 * @return          [day] Difference in light time to observer, either relative to SSB
 *                  (<i>Usage A</i>) or relative intermediate solar-system body
 *                  (<i>Usage B</i>); or else NAN if either of the input arguments is NULL.
 *
 * @sa place()
 */
double d_light(const double *pos_src, const double *pos_body) {
  double d_src;

  if(!pos_src || !pos_body) {
    novas_set_errno(EINVAL, "d_light", "NULL input 3-vector: pos_src=%p, pos_body=%p [=> NAN]", pos_src, pos_body);
    return NAN;
  }

  d_src = novas_vlen(pos_src);

  // Light-time returned is the projection of vector 'pos_obs' onto the
  // unit vector 'u1' (formed from 'pos_body'), divided by the speed of light.
  return d_src > 1e-30 ? novas_vdot(pos_body, pos_src) / d_src / C_AUDAY : 0.0;
}

/**
 * Computes the total gravitational deflection of light for the observed object due to the
 * specified gravitating bodies in the solar system.  This function is valid for an observed body
 * within the solar system as well as for a star.
 *
 * NOTES:
 * <ol>
 * <li>The gravitational deflection due to planets requires a planet calculator function to be
 * configured, e.g. via set_planet_provider().</li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Klioner, S. (2003), Astronomical Journal 125, 1580-1597, Section 6.</li>
 * </ol>
 *
 * @param pos_src     [AU] Position 3-vector of observed object, with respect to origin at
 *                    observer (or the geocenter), referred to ICRS axes, components
 *                    in AU.
 * @param pos_obs     [AU] Position 3-vector of observer (or the geocenter), with respect to
 *                    origin at solar system barycenter, referred to ICRS axes,
 *                    components in AU.
 * @param planets     Apparent planet data containing positions and velocities for the major
 *                    gravitating bodies in the solar-system.
 * @param[out] out    [AU] Position vector of observed object, with respect to origin at
 *                    observer (or the geocenter), referred to ICRS axes, corrected
 *                    for gravitational deflection, components in AU. It can be the same
 *                    vector as the input, but not the same as pos_obs.
 * @return            0 if successful, -1 if any of the pointer arguments is NULL
 *                    or if the output vector is the same as pos_obs.
 *
 * @sa obs_planets()
 * @sa grav_undo_planets()
 * @sa grav_def()
 * @sa novas_geom_to_app()
 * @sa grav_bodies_full_accuracy
 * @sa grav_bodies_reduced_accuracy
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int grav_planets(const double *pos_src, const double *pos_obs, const novas_planet_bundle *planets, double *out) {
  static const char *fn = "grav_planets";
  static const double rmass[] = NOVAS_RMASS_INIT;

  double tsrc;
  int i;

  if(!pos_src || !pos_obs)
    return novas_error(-1, EINVAL, fn, "NULL input 3-vector: pos_src=%p, pos_obs=%p", pos_src, pos_obs);

  if(!out)
    return novas_error(-1, EINVAL, fn, "NULL output 3-vector");

  if(!planets)
    return novas_error(-1, EINVAL, fn, "NULL input planet data");

  // Initialize output vector of observed object to equal input vector.
  if(out != pos_src)
    memcpy(out, pos_src, XYZ_VECTOR_SIZE);

  tsrc = novas_vlen(pos_src) / C_AUDAY;

  for(i = 1; i < NOVAS_PLANETS; i++) {
    double lt, dpl, p1[3];
    int k;

    if((planets->mask & (1 << i)) == 0)
      continue;

    // Distance from observer to gravitating body
    dpl = novas_vlen(&planets->pos[i][0]);

    // If observing from within ~1500 km of the gravitating body, then skip deflections by it...
    if(dpl < 1e-5)
      continue;

    // Calculate light time to the point where incoming geometric light ray is closest to gravitating body.
    lt = d_light(pos_src, &planets->pos[i][0]);

    // If gravitating body is in opposite direction from the source then use the gravitating
    // body position at the time the light is observed.
    if(lt < 0.0)
      lt = 0.0;

    // If source is between gravitating body and observer, then use gravitating body position
    // at the time light originated from source.
    else if(tsrc < lt)
      lt = tsrc;

    // Differential light time w.r.t. the apparent planet center
    lt -= dpl / C_AUDAY;

    // Calculate planet position at the time it is gravitationally acting on light.
    for(k = 3; --k >= 0;)
      p1[k] = pos_obs[k] + planets->pos[i][k] - lt * planets->vel[i][k];

    // Compute deflection due to gravitating body.
    grav_vec(out, pos_obs, p1, rmass[i], out);
  }

  return 0;
}

/**
 * Calculates the positions and velocities for the Solar-system bodies, e.g. for use for graviational
 * deflection calculations. The planet positions are calculated relative to the observer location, while
 * velocities are w.r.t. the SSB. Both positions and velocities are antedated for light travel time, so
 * they accurately reflect the apparent position (and barycentric motion) of the bodies from the
 * observer's perspective.
 *
 *
 * @param jd_tdb        [day] Barycentric Dynamical Time (TDB) based Julian date
 * @param accuracy      NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1). In full accuracy
 *                      mode, it will calculate the deflection due to the Sun, Jupiter, Saturn
 *                      and Earth. In reduced accuracy mode, only the deflection due to the
 *                      Sun is calculated.
 * @param pos_obs       [AU] Position 3-vector of observer (or the geocenter), with respect to
 *                      origin at solar system barycenter, referred to ICRS axes,
 *                      components in AU.
 * @param pl_mask       Bitwise `(1 << planet-number)` mask indicating which planets to request
 *                      data for. See enum novas_planet for the enumeration of planet numbers.
 * @param[out] planets  Pointer to apparent planet data to populate.
 *                      have positions and velocities calculated successfully. See enum
 *                      novas_planet for the enumeration of planet numbers.
 * @return              0 if successful, -1 if any of the pointer arguments is NULL
 *                      or if the output vector is the same as pos_obs, or the error from
 *                      ephemeris().
 *
 * @sa enum novas_planet
 * @sa grav_planets()
 * @sa grav_undo_planets()
 * @sa set_planet_provider()
 * @sa set_planet_provider_hp()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int obs_planets(double jd_tdb, enum novas_accuracy accuracy, const double *pos_obs, int pl_mask, novas_planet_bundle *planets) {
  static const char *fn = "obs_planets";

  static object body[NOVAS_PLANETS];
  static int initialized;

  enum novas_debug_mode dbmode = novas_get_debug_mode();
  int i, error = 0;

  if(!planets)
    return novas_error(-1, EINVAL, fn, "NULL planet data");

  planets->mask = 0;

  if(!pos_obs)
    return novas_error(-1, EINVAL, fn, "NULL observer position parameter");

  // Set up the structures of type 'object' containing the body information.
  if(!initialized) {
    for(i = 0; i < NOVAS_PLANETS; i++)
      make_planet(i, &body[i]);
    initialized = 1;
  }

  // Temporarily disable debug traces, unless in extra debug mode
  if(dbmode != NOVAS_DEBUG_EXTRA)
    novas_debug(NOVAS_DEBUG_OFF);

  // Cycle through gravitating bodies.
  for(i = 0; i < NOVAS_PLANETS; i++) {
    const int bit = (1 << i);
    double tl;
    int stat;

    if((pl_mask & bit) == 0)
      continue;

    // Calculate positions and velocities antedated for light time.
    stat = light_time2(jd_tdb, &body[i], pos_obs, 0.0, accuracy, &planets->pos[i][0], &planets->vel[i][0], &tl);
    if(stat) {
      if(!error)
        error = stat > 10 ? stat - 10 : -1;
      continue;
    }

    planets->mask |= bit;
  }

  // Re-enable debug traces
  novas_debug(dbmode);

  // If could not calculate deflection due to Sun, return with error
  if((planets->mask & (1 << NOVAS_SUN)) == 0)
    prop_error("grav_init_planet:sun", error, 0);

  // If could not get positions for another gravitating body then
  // return error only if in extra debug mode...
  if(planets->mask != pl_mask && novas_get_debug_mode() == NOVAS_DEBUG_EXTRA)
    prop_error(fn, error, 0);

  return 0;
}

/**
 * Computes the total gravitational deflection of light for the observed object due to the
 * major gravitating bodies in the solar system.  This function valid for an observed body
 * within the solar system as well as for a star.
 *
 * If 'accuracy' is NOVAS_FULL_ACCURACY (0), the deflections due to the Sun, Jupiter, Saturn,
 * and Earth are calculated.  Otherwise, only the deflection due to the Sun is calculated.
 * In either case, deflection for a given body is ignored if the observer is within ~1500 km
 * of the center of the gravitating body.
 *
 * NOTES:
 * <ol>
 * <li>This function is called by place() to calculate gravitational deflections as
 * appropriate for positioning sources precisely. The gravitational deflection due to
 * planets requires a planet calculator function to be configured, e.g. via set_planet_provider().
 * </li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Klioner, S. (2003), Astronomical Journal 125, 1580-1597, Section 6.</li>
 * </ol>
 *
 * @param jd_tdb      [day] Barycentric Dynamical Time (TDB) based Julian date
 * @param unused      The type of observer frame (no longer used)
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1). In full accuracy
 *                    mode, it will calculate the deflection due to the Sun, Jupiter, Saturn
 *                    and Earth. In reduced accuracy mode, only the deflection due to the
 *                    Sun is calculated.
 * @param pos_src     [AU] Position 3-vector of observed object, with respect to origin at
 *                    observer (or the geocenter), referred to ICRS axes, components
 *                    in AU.
 * @param pos_obs     [AU] Position 3-vector of observer (or the geocenter), with respect to
 *                    origin at solar system barycenter, referred to ICRS axes,
 *                    components in AU.
 * @param[out] out    [AU] Position vector of observed object, with respect to origin at
 *                    observer (or the geocenter), referred to ICRS axes, corrected
 *                    for gravitational deflection, components in AU. It can be the same
 *                    vector as the input, but not the same as pos_obs.
 * @return            0 if successful, -1 if any of the pointer arguments is NULL
 *                    or if the output vector is the same as pos_obs, or the error from
 *                    obs_planets().
 *
 * @sa grav_undef()
 * @sa place()
 * @sa novas_geom_to_app()
 * @sa set_planet_provider()
 * @sa set_planet_provider_hp()
 * @sa grav_bodies_full_accuracy
 * @sa grav_bodies_reduced_accuracy
 */
short grav_def(double jd_tdb, enum novas_observer_place unused, enum novas_accuracy accuracy, const double *pos_src, const double *pos_obs,
        double *out) {
  static const char *fn = "grav_def";

  novas_planet_bundle planets = {0};
  int pl_mask = (accuracy == NOVAS_FULL_ACCURACY) ? grav_bodies_full_accuracy : grav_bodies_reduced_accuracy;

  (void) unused;

  if(!pos_src || !out)
    return novas_error(-1, EINVAL, fn, "NULL source position 3-vector: pos_src=%p, out=%p", pos_src, out);

  prop_error(fn, obs_planets(jd_tdb, accuracy, pos_obs, pl_mask, &planets), 0);
  prop_error(fn, grav_planets(pos_src, pos_obs, &planets, out), 0);
  return 0;
}

/**
 * Corrects position vector for the deflection of light in the gravitational field of an
 * arbitrary body.  This function valid for an observed body within the solar system as
 * well as for a star.
 *
 * NOTES:
 * <ol>
 * <li>This function is called by grav_def() to calculate appropriate gravitational
 * deflections for sources.
 * </li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Murray, C.A. (1981) Mon. Notices Royal Ast. Society 195, 639-648.</li>
 * <li>See also formulae in Section B of the Astronomical Almanac, or</li>
 * <li>Kaplan, G. et al. (1989) Astronomical Journal 97, 1197-1210, section iii f.</li>
 * </ol>
 *
 * @param pos_src   [AU] Position 3-vector of observed object, with respect to origin at
 *                  observer (or the geocenter), components in AU.
 * @param pos_obs   [AU] Position vector of gravitating body, with respect to origin
 *                  at solar system barycenter, components in AU.
 * @param pos_body  [AU] Position 3-vector of gravitating body, with respect to origin
 *                  at solar system barycenter, components in AU.
 * @param rmass     [1/Msun] Reciprocal mass of gravitating body in solar mass units, that
 *                  is, Sun mass / body mass.
 * @param[out] out  [AU]  Position 3-vector of observed object, with respect to origin at
 *                  observer (or the geocenter), corrected for gravitational
 *                  deflection, components in AU. It can the same vector as the input.
 * @return          0 if successful, or -1 if any of the input vectors is NULL.
 *
 * @sa place()
 * @sa grav_def()
 */
int grav_vec(const double *pos_src, const double *pos_obs, const double *pos_body, double rmass, double *out) {
  static const char *fn = "grav_vec";
  double pq[3], pe[3], pmag, emag, qmag, phat[3] = {0}, ehat[3], qhat[3];
  int i;

  if(!pos_src || !out)
    return novas_error(-1, EINVAL, fn, "NULL input or output 3-vector: pos_src=%p, out=%p", pos_src, out);

  // Default output in case of error
  if(out != pos_src)
    memcpy(out, pos_src, XYZ_VECTOR_SIZE);

  if(!pos_obs || !pos_body)
    return novas_error(-1, EINVAL, fn, "NULL input 3-vector: pos_obs=%p, pos_body=%p", pos_obs, pos_body);

  // Construct vector 'pq' from gravitating body to observed object and
  // construct vector 'pe' from gravitating body to observer.
  for(i = 3; --i >= 0;) {
    pe[i] = pos_obs[i] - pos_body[i];
    pq[i] = pe[i] + pos_src[i];
  }

  // Compute vector magnitudes and unit vectors.
  pmag = novas_vlen(pos_src);
  emag = novas_vlen(pe);
  qmag = novas_vlen(pq);

  // Gravitating body is the observer or the observed object. No deflection.
  if(!emag || !qmag)
    return 0;

  for(i = 3; --i >= 0;) {
    if(pmag)
      phat[i] = pos_src[i] / pmag;
    ehat[i] = pe[i] / emag;
    qhat[i] = pq[i] / qmag;
  }

  // Deflection calculation...
  {
    // Compute dot products of vectors
    const double edotp = novas_vdot(ehat, phat);
    const double pdotq = novas_vdot(phat, qhat);
    const double qdote = novas_vdot(qhat, ehat);

    // Compute scalar factors.
    const double fac1 = 2.0 * GS / (C * C * emag * AU * rmass);
    const double fac2 = 1.0 + qdote;

    // Construct corrected position vector 'pos2'.
    for(i = 3; --i >= 0;)
      out[i] += pmag * fac1 * (pdotq * ehat[i] - edotp * qhat[i]) / fac2;
  }

  return 0;
}

/**
 * Corrects position vector for aberration of light.  Algorithm includes relativistic terms.
 *
 * NOTES:
 * <ol>
 * <li>This function is called by place() to account for aberration when calculating the position
 * of the source.</li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Murray, C. A. (1981) Mon. Notices Royal Ast. Society 195, 639-648.</li>
 * <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 * </ol>
 *
 * @param pos         [AU]  Position vector of source relative to observer
 * @param vobs        [AU/day]  Velocity vector of observer, relative to the solar system barycenter,
 *                    components in AU/day.
 * @param lighttime   [day] Light time from object to Earth in days (if known). Or set to 0, and this
 *                    function will compute it.
 * @param[out] out    [AU] Position vector, referred to origin at center of mass of the
 *                    Earth, corrected for aberration, components in AU. It can be the same
 *                    vector as one of the inputs.
 *
 * @return            0 if successful, or -1 if any of the vector arguments are NULL.
 *
 * @sa place()
 */
int aberration(const double *pos, const double *vobs, double lighttime, double *out) {
  double p1mag, vemag, beta, cosd, gammai, p, q, r;

  if(!pos || !vobs || !out)
    return novas_error(-1, EINVAL, "aberration", "NULL input or output 3-vector: pos=%p, vobs=%p, out=%p", pos, vobs, out);

  vemag = novas_vlen(vobs);
  if(!vemag) {
    if(out != pos)
      memcpy(out, pos, XYZ_VECTOR_SIZE);
    return 0;
  }

  beta = vemag / C_AUDAY;

  if(lighttime <= 0.0) {
    p1mag = novas_vlen(pos);
    lighttime = p1mag / C_AUDAY;
  }
  else
    p1mag = lighttime * C_AUDAY;

  cosd = novas_vdot(pos, vobs) / (p1mag * vemag);
  gammai = sqrt(1.0 - beta * beta);
  p = beta * cosd;
  q = (1.0 + p / (1.0 + gammai)) * lighttime;
  r = 1.0 + p;

  out[0] = (gammai * pos[0] + q * vobs[0]) / r;
  out[1] = (gammai * pos[1] + q * vobs[1]) / r;
  out[2] = (gammai * pos[2] + q * vobs[2]) / r;

  return 0;
}


/**
 * Predicts the radial velocity of the observed object as it would be measured by spectroscopic
 * means.  Radial velocity is here defined as the radial velocity measure (z) times the speed of
 * light. For major planets (and Sun and Moon), it includes gravitational corrections for light
 * originating at the surface and observed from near Earth or else from a large distance away. For
 * other solar system bodies, it applies to a fictitious emitter at the center of the observed
 * object, assumed massless (no gravitational red shift). The corrections do not in general apply
 * to reflected light. For stars, it includes all effects, such as gravitational redshift,
 * contained in the catalog barycentric radial velocity measure, a scalar derived from spectroscopy.
 * Nearby stars with a known kinematic velocity vector (obtained independently of spectroscopy) can
 * be treated like solar system objects.
 *
 * Gravitational blueshift corrections for the Solar and Earth potential for observers are included.
 * However, the result does not include a blueshift correction for observers (e.g. spacecraft)
 * orbiting other major Solar-system bodies. You may adjust the amount of gravitational redshift
 * correction applied to the radial velocity via `redshift_vrad()`, `unredshift_vrad()` and
 * `grav_redshift()` if necessary.
 *
 * All the input arguments are BCRS quantities, expressed with respect to the ICRS axes. 'vel_src'
 * and 'vel_obs' are kinematic velocities - derived from geometry or dynamics, not spectroscopy.
 *
 * If the object is outside the solar system, the algorithm used will be consistent with the
 * IAU definition of stellar radial velocity, specifically, the barycentric radial velocity
 * measure, which is derived from spectroscopy.  In that case, the vector 'vel_src' can be very
 * approximate -- or, for distant stars or galaxies, zero -- as it will be used only for a small
 * geometric correction that is proportional to proper motion.
 *
 * Any of the distances (last three input arguments) can be set to zero (0.0) or negative if the
 * corresponding general relativistic gravitational potential term is not to be evaluated.
 * These terms generally are important at the meter/second level only. If 'd_obs_geo' and
 * 'd_obs_sun' are both zero, an average value will be used for the relativistic term for the
 * observer, appropriate for an observer on the surface of the Earth. 'd_src_sun', if given, is
 * used only for solar system objects.
 *
 * NOTES:
 * <ol>
 * <li>This function does not accont for the gravitational deflection of Solar-system sources.
 * For that purpose, the rad_vel2() function, introduced in v1.1, is more appropriate.</li>
 * <li>The NOVAS C implementation did not include relatistic corrections for a moving observer
 * if both `d_obs_geo` and `d_obs_sun` were zero. As of SuperNOVAS v1.1, the relatistic corrections
 * for a moving observer will be included in the radial velocity measure always.</li>
 * <li>In a departure from the original NOVAS C, the radial velocity for major planets (and Sun and
 * Moon) includes gravitational redshift corrections for light originating at the surface, assuming
 * it's observed from near Earth or else from a large distance away.</li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Lindegren & Dravins (2003), Astronomy & Astrophysics 401, 1185-1201.</li>
 * <li>Unlike NOVAS C, this function will return a radial velocity for the Sun that is gravitationally
 * referenced to the Sun's photosphere. (NOVAS C returns the radial velocity for a massless Sun)</li>
 * </ol>
 *
 * @param source        Celestial object observed
 * @param pos_src       [AU|*] Geometric position vector of object with respect to observer.
 *                      For solar system sources it should be corrected for light-time. For
 *                      non-solar-system objects, the position vector defines a direction only,
 *                      with arbitrary magnitude.
 * @param vel_src       [AU/day] Velocity vector of object with respect to solar system
 *                      barycenter.
 * @param vel_obs       [AU/day] Velocity vector of observer with respect to solar system
 *                      barycenter.
 * @param d_obs_geo     [AU] Distance from observer to geocenter, or &lt;=0.0 if
 *                      gravitational blueshifting due to Earth potential around observer can be
 *                      ignored.
 * @param d_obs_sun     [AU] Distance from observer to Sun, or &lt;=0.0 if gravitational
 *                      bluehifting due to Solar potential around observer can be ignored.
 * @param d_src_sun     [AU] Distance from object to Sun, or &lt;=0.0 if gravitational
 *                      redshifting due to Solar potential around source can be ignored.
 * @param[out] rv       [km/s] The observed radial velocity measure times the speed of light,
 *                      or NAN if there was an error.
 * @return              0 if successfule, or else -1 if there was an error (errno will be set
 *                      to EINVAL if any of the arguments are NULL, or to some other value to
 *                      indicate the type of error).
 *
 * @sa rad_vel2()
 *
 */
int rad_vel(const object *source, const double *pos_src, const double *vel_src, const double *vel_obs, double d_obs_geo, double d_obs_sun,
        double d_src_sun, double *rv) {
  static const char *fn = "rad_vel";
  int stat;

  if(!rv)
    return novas_error(-1, EINVAL, fn, "NULL input source");

  *rv = rad_vel2(source, pos_src, vel_src, pos_src, vel_obs, d_obs_geo, d_obs_sun, d_src_sun);
  stat = isnan(*rv) ? -1 : 0;
  prop_error(fn, stat, 0);

  return 0;
}

/**
 * Predicts the radial velocity of the observed object as it would be measured by spectroscopic
 * means. This is a modified version of the original NOVAS C 3.1 rad_vel(), to account for
 * the different directions in which light is emitted vs in which it detected, e.g. when it is
 * gravitationally deflected.
 *
 * Radial velocity is here defined as the radial velocity measure (z) times the speed of light.
 * For major planets (and Sun and Moon), it includes gravitational corrections for light
 * originating at the surface and observed from near Earth or else from a large distance away. For
 * other solar system bodies, it applies to a fictitious emitter at the center of the observed
 * object, assumed massless (no gravitational red shift). The corrections do not in general apply
 * to reflected light. For stars, it includes all effects, such as gravitational redshift,
 * contained in the catalog barycentric radial velocity measure, a scalar derived from spectroscopy.
 * Nearby stars with a known kinematic velocity vector (obtained independently of spectroscopy) can
 * be treated like solar system objects.
 *
 * Gravitational blueshift corrections for the Solar and Earth potential for observers are included.
 * However, the result does not include a blueshift correction for observers (e.g. spacecraft)
 * orbiting other major Solar-system bodies. You may adjust the amount of gravitational redshift
 * correction applied to the radial velocity via `redshift_vrad()`, `unredshift_vrad()` and
 * `grav_redshift()` if necessary.
 *
 * All the input arguments are BCRS quantities, expressed with respect to the ICRS axes. 'vel_src'
 * and 'vel_obs' are kinematic velocities - derived from geometry or dynamics, not spectroscopy.
 *
 * If the object is outside the solar system, the algorithm used will be consistent with the
 * IAU definition of stellar radial velocity, specifically, the barycentric radial velocity
 * measure, which is derived from spectroscopy.  In that case, the vector 'vel_src' can be very
 * approximate -- or, for distant stars or galaxies, zero -- as it will be used only for a small
 * geometric and relativistic (time dilation) correction, including the proper motion.
 *
 * Any of the distances (last three input arguments) can be set to a negative value if the
 * corresponding general relativistic gravitational potential term is not to be evaluated.
 * These terms generally are important only at the meter/second level. If 'd_obs_geo' and
 * 'd_obs_sun' are both zero, an average value will be used for the relativistic term for the
 * observer, appropriate for an observer on the surface of the Earth. 'd_src_sun', if given, is
 * used only for solar system objects.
 *
 * NOTES:
 * <ol>
 * <li>This function is called by place() and novas_sky_pos() to calculate radial velocities along
 * with the apparent position of the source.</li>
 * <li>For major planets (and Sun and Moon), the radial velocity includes gravitational redshift
 * corrections for light originating at the surface, assuming it's observed from near Earth or
 * else from a large distance away.</li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Lindegren & Dravins (2003), Astronomy & Astrophysics 401, 1185-1201.</li>
 * </ol>
 *
 * @param source        Celestial object observed
 * @param pos_emit      [AU|*] position vector of object with respect to observer in the
 *                      direction that light was emitted from the source.
 *                      For solar system sources it should be corrected for light-time. For
 *                      non-solar-system objects, the position vector defines a direction only,
 *                      with arbitrary magnitude.
 * @param vel_src       [AU/day] Velocity vector of object with respect to solar system
 *                      barycenter.
 * @param pos_det       [AU|*] apparent position vector of source, as seen by the observer.
 *                      It may be the same vector as `pos_emit`, in which case the routine
 *                      behaves like the original NOVAS_C rad_vel().
 * @param vel_obs       [AU/day] Velocity vector of observer with respect to solar system
 *                      barycenter.
 * @param d_obs_geo     [AU] Distance from observer to geocenter, or &lt;=0.0 if
 *                      gravitational blueshifting due to Earth potential around observer can be
 *                      ignored.
 * @param d_obs_sun     [AU] Distance from observer to Sun, or &lt;=0.0 if gravitational
 *                      bluehifting due to Solar potential around observer can be ignored.
 * @param d_src_sun     [AU] Distance from object to Sun, or &lt;=0.0 if gravitational
 *                      redshifting due to Solar potential around source can be ignored.
 *                      Additionally, a value &lt;0 will also skip corrections for light
 *                      originating at the surface of the observed major solar-system body.
 * @return              [km/s] The observed radial velocity measure times the speed of light,
 *                      or NAN if there was an error (errno will be set to EINVAL if any of the
 *                      arguments are NULL, or to some other value to indicate the type of error).
 *
 * @sa rad_vel()
 * @sa place()
 * @sa novas_sky_pos()
 * @sa novas_v2z()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
double rad_vel2(const object *source, const double *pos_emit, const double *vel_src, const double *pos_det, const double *vel_obs,
        double d_obs_geo, double d_obs_sun, double d_src_sun) {
  static const char *fn = "rad_vel2";

  double rel; // redshift factor i.e., f_src / fobs = (1 + z)
  double uk[3], r, phi, beta_src, beta_obs, beta;
  int i;

  if(!source) {
    novas_set_errno(EINVAL, fn, "NULL input source");
    return NAN;
  }

  if(!pos_emit || !vel_src || !pos_det) {
    novas_set_errno(EINVAL, fn, "NULL input source pos/vel: pos_emit=%p, vel_src=%p, pos_det=%p", pos_emit, vel_src, pos_det);
    return NAN;
  }

  if(!vel_obs) {
    novas_set_errno(EINVAL, fn, "NULL input observer velocity");
    return NAN;
  }

  // Compute geopotential at observer, unless observer is within Earth.
  r = d_obs_geo * AU;
  phi = (r > 0.95 * NOVAS_EARTH_RADIUS) ? GE / r : 0.0;

  // Compute solar potential at observer unless well within the Sun
  r = d_obs_sun * AU;
  phi += (r > 0.95 * NOVAS_SOLAR_RADIUS) ? GS / r : 0.0;

  // Compute relativistic potential at observer.
  if(d_obs_geo == 0.0 && d_obs_sun == 0.0) {
    // Use average value for an observer on the surface of Earth
    // Lindegren & Dravins eq. (42), inverse.
    rel = 1.0 - 1.550e-8;
  }
  else {
    // Lindegren & Dravins eq. (41), second factor in parentheses.
    rel = 1.0 - phi / C2;
  }

  // Compute unit vector toward object (direction of emission).
  r = novas_vlen(pos_emit);
  for(i = 0; i < 3; i++)
    uk[i] = pos_emit[i] / r;

  // Complete radial velocity calculation.
  switch(source->type) {
    case NOVAS_CATALOG_OBJECT: {
      // Objects outside the solar system.
      // For stars, update barycentric radial velocity measure for change
      // in view angle.
      const cat_entry *star= &source->star;
      const double ra = star->ra * HOURANGLE;
      const double dec = star->dec * DEGREE;
      const double cosdec = cos(dec);

      // Compute radial velocity measure of sidereal source rel. barycenter
      // Including proper motion
      beta_src = NOVAS_KM * star->radialvelocity / C;

      if(star->parallax > 0.0) {
        double du[3];

        du[0] = uk[0] - (cosdec * cos(ra));
        du[1] = uk[1] - (cosdec * sin(ra));
        du[2] = uk[2] - sin(dec);

        beta_src += novas_vdot(vel_src, du) / C_AUDAY;
      }

      break;
    }

    case NOVAS_PLANET:
      if(d_src_sun >= 0.0) {
        // Gravitational potential for light originating at surface of major solar system body.
        const double zpl[NOVAS_PLANETS] = NOVAS_PLANET_GRAV_Z_INIT;
        if(source->number > 0 && source->number < NOVAS_PLANETS)
          rel *= (1.0 + zpl[source->number]);
      } // @suppress("No break at end of case")
      /* fallthrough */

    case NOVAS_EPHEM_OBJECT:
    case NOVAS_ORBITAL_OBJECT:
      // Solar potential at source (bodies strictly outside the Sun's volume)
      if(d_src_sun * AU > NOVAS_SOLAR_RADIUS)
        rel /= 1.0 - GS / (d_src_sun * AU) / C2;

      // Compute observed radial velocity measure of a planet rel. barycenter
      beta_src = novas_vdot(uk, vel_src) / C_AUDAY;

      break;

    default:
      novas_set_errno(EINVAL, fn, "invalid source type: %d", source->type);
      return NAN;
  }

  // Compute unit vector toward object (direction of detection).
  r = novas_vlen(pos_det);
  for(i = 0; i < 3; i++)
    uk[i] = pos_det[i] / r;

  // Radial velocity measure of observer rel. barycenter
  beta_obs = novas_vdot(uk, vel_obs) / C_AUDAY;

  // Differential barycentric radial velocity measure (relativistic formula)
  beta = novas_add_beta(beta_src, -beta_obs);

  // Include relativistic redhsift factor due to relative motion
  rel *= (1.0 + beta) / sqrt(1.0 - vdist2(vel_obs, vel_src) / C2);

  // Convert observed radial velocity measure to kilometers/second.
  return novas_z2v(rel - 1.0);
}


/**
 * Precesses equatorial rectangular coordinates from one epoch to another. Unlike the original
 * NOVAS routine, this routine works for any pairing of the time arguments.
 *
 * This function calculates precession for the old (pre IAU 2000) methodology. Its main use
 * for NOVAS users is to allow converting older catalog coordinates e.g. to J2000 coordinates,
 * which then can be converted to the now standard ICRS system via frame_tie().
 *
 * NOTE:
 * <ol>
 * <li>Unlike the original NOVAS C 3.1 version, this one does not require that one
 *     of the time arguments must be J2000. You can precess from any date to
 *     any other date, and the intermediate epoch of J2000 will be handled internally
 *     as needed.</li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Explanatory Supplement To The Astronomical Almanac, pp. 103-104.</li>
 * <li>Capitaine, N. et al. (2003), Astronomy And Astrophysics 412, pp. 567-586.</li>
 * <li>Hilton, J. L. et al. (2006), IAU WG report, Celest. Mech., 94, pp. 351-367.</li>
 * </ol>
 *
 * @param jd_tdb_in   [day] Barycentric Dynamic Time (TDB) based Julian date of the input
 *                    epoch
 * @param in          Position 3-vector, geocentric equatorial rectangular coordinates,
 *                    referred to mean dynamical equator and equinox of the initial epoch.
 * @param jd_tdb_out  [day] Barycentric Dynamic Time (TDB) based Julian date of the output
 *                    epoch
 * @param[out] out    Position 3-vector, geocentric equatorial rectangular coordinates,
 *                    referred to mean dynamical equator and equinox of the final epoch.
 *                    It can be the same vector as the input.
 * @return            0 if successful, or -1 if either of the position vectors is NULL.
 *
 * @sa nutation()
 * @sa frame_tie()
 * @sa tt2tdb()
 * @sa cio_basis()
 * @sa NOVAS_TOD
 */
short precession(double jd_tdb_in, const double *in, double jd_tdb_out, double *out) {
  static THREAD_LOCAL double t_last;
  static THREAD_LOCAL double xx, yx, zx, xy, yy, zy, xz, yz, zz;
  double t;

  if(!in || !out)
    return novas_error(-1, EINVAL, "precession", "NULL input or output 3-vector: in=%p, out=%p", in, out);

  if(jd_tdb_in == jd_tdb_out) {
    if(out != in)
      memcpy(out, in, XYZ_VECTOR_SIZE);
    return 0;
  }

  // Check to be sure that either 'jd_tdb1' or 'jd_tdb2' is equal to JD_J2000.
  if(!time_equals(jd_tdb_in, JD_J2000) && !time_equals(jd_tdb_out, JD_J2000)) {
    // Do the precession in two steps...
    precession(jd_tdb_in, in, JD_J2000, out);
    precession(JD_J2000, out, jd_tdb_out, out);
    return 0;
  }

  // 't' is time in TDB centuries between the two epochs.
  t = (jd_tdb_out - jd_tdb_in) / JULIAN_CENTURY_DAYS;
  if(jd_tdb_out == JD_J2000)
    t = -t;

  if(!time_equals(t, t_last)) {
    double psia, omegaa, chia, sa, ca, sb, cb, sc, cc, sd, cd;
    double eps0 = 84381.406;

    // Numerical coefficients of psi_a, omega_a, and chi_a, along with
    // epsilon_0, the obliquity at J2000.0, are 4-angle formulation from
    // Capitaine et al. (2003), eqs. (4), (37), & (39).
    psia = ((((-0.0000000951 * t + 0.000132851) * t - 0.00114045) * t - 1.0790069) * t + 5038.481507) * t;
    omegaa = ((((+0.0000003337 * t - 0.000000467) * t - 0.00772503) * t + 0.0512623) * t - 0.025754) * t + eps0;
    chia = ((((-0.0000000560 * t + 0.000170663) * t - 0.00121197) * t - 2.3814292) * t + 10.556403) * t;

    eps0 *= ARCSEC;
    psia *= ARCSEC;
    omegaa *= ARCSEC;
    chia *= ARCSEC;

    sa = sin(eps0);
    ca = cos(eps0);
    sb = sin(-psia);
    cb = cos(-psia);
    sc = sin(-omegaa);
    cc = cos(-omegaa);
    sd = sin(chia);
    cd = cos(chia);

    // Compute elements of precession rotation matrix equivalent to
    // R3(chi_a) R1(-omega_a) R3(-psi_a) R1(epsilon_0).
    xx = cd * cb - sb * sd * cc;
    yx = cd * sb * ca + sd * cc * cb * ca - sa * sd * sc;
    zx = cd * sb * sa + sd * cc * cb * sa + ca * sd * sc;
    xy = -sd * cb - sb * cd * cc;
    yy = -sd * sb * ca + cd * cc * cb * ca - sa * cd * sc;
    zy = -sd * sb * sa + cd * cc * cb * sa + ca * cd * sc;
    xz = sb * sc;
    yz = -sc * cb * ca - sa * cc;
    zz = -sc * cb * sa + cc * ca;

    t_last = t;
  }

  if(jd_tdb_out == JD_J2000) {
    const double x = in[0], y = in[1], z = in[2];
    // Perform rotation from epoch to J2000.0.
    out[0] = xx * x + xy * y + xz * z;
    out[1] = yx * x + yy * y + yz * z;
    out[2] = zx * x + zy * y + zz * z;
  }
  else {
    const double x = in[0], y = in[1], z = in[2];
    // Perform rotation from J2000.0 to epoch.
    out[0] = xx * x + yx * y + zx * z;
    out[1] = xy * x + yy * y + zy * z;
    out[2] = xz * x + yz * y + zz * z;
  }

  return 0;
}

/**
 * Nutates equatorial rectangular coordinates from mean equator and equinox of epoch to true
 * equator and equinox of epoch. Inverse transformation may be applied by setting flag
 * 'direction'.
 *
 * This is the old (pre IAU 2006) method of nutation calculation. If you follow the now
 * standard IAU 2000/2006 methodology you will want to use nutation_angles() instead.
 *
 * REFERENCES:
 * <ol>
 * <li>Explanatory Supplement To The Astronomical Almanac, pp. 114-115.</li>
 * </ol>
 *
 *
 * @param jd_tdb      [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param direction   NUTATE_MEAN_TO_TRUE (0) or NUTATE_TRUE_TO_MEAN (-1; or non-zero)
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in          Position 3-vector, geocentric equatorial rectangular coordinates,
 *                    referred to mean equator and equinox of epoch.
 * @param[out] out    Position vector, geocentric equatorial rectangular coordinates,
 *                    referred to true equator and equinox of epoch. It can be the same
 *                    as the input position.
 *
 * @return            0 if successful, or -1 if one of the vector arguments is NULL.
 *
 * @sa nutation_angles()
 * @sa tt2tdb()
 * @sa NOVAS_TOD
 */
int nutation(double jd_tdb, enum novas_nutation_direction direction, enum novas_accuracy accuracy, const double *in, double *out) {
  double oblm, oblt, psi;
  double cm, sm, ct, st, cp, sp;
  double xx, yx, zx, xy, yy, zy, xz, yz, zz;

  if(!in || !out)
    return novas_error(-1, EINVAL, "nutation", "NULL input or output 3-vector: in=%p, out=%p", in, out);

  // Call 'e_tilt' to get the obliquity and nutation angles.
  e_tilt(jd_tdb, accuracy, &oblm, &oblt, NULL, &psi, NULL);

  oblm *= DEGREE;
  oblt *= DEGREE;
  psi *= ARCSEC;

  cm = cos(oblm);
  sm = sin(oblm);
  ct = cos(oblt);
  st = sin(oblt);
  cp = cos(psi);
  sp = sin(psi);

  // Nutation rotation matrix follows.
  xx = cp;
  yx = -sp * cm;
  zx = -sp * sm;
  xy = sp * ct;
  yy = cp * cm * ct + sm * st;
  zy = cp * sm * ct - cm * st;
  xz = sp * st;
  yz = cp * cm * st - sm * ct;
  zz = cp * sm * st + cm * ct;

  if(direction == NUTATE_MEAN_TO_TRUE) {
    const double x = in[0], y = in[1], z = in[2];
    // Perform rotation.
    out[0] = xx * x + yx * y + zx * z;
    out[1] = xy * x + yy * y + zy * z;
    out[2] = xz * x + yz * y + zz * z;
  }
  else {
    const double x = in[0], y = in[1], z = in[2];
    // Perform inverse rotation.
    out[0] = xx * x + xy * y + xz * z;
    out[1] = yx * x + yy * y + yz * z;
    out[2] = zx * x + zy * y + zz * z;
  }

  return 0;
}

/**
 * Set the function to use for low-precision IAU 2000 nutation calculations instead of the
 * default nu2000k().
 *
 * @param func  the new function to use for low-precision IAU 2000 nutation calculations
 * @return      0 if successful, or -1 if the input argument is NULL
 *
 * @sa nutation_angles()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int set_nutation_lp_provider(novas_nutation_provider func) {
  if(!func)
    return novas_error(-1, EINVAL, "set_nutation_lp_provider", "NULL 'func' parameter");

  nutate_lp = func;
  return 0;
}

/**
 * Returns the values for nutation in longitude and nutation in obliquity for a given TDB
 * Julian date.  The nutation model selected depends upon the input value of 'accuracy'. See
 * notes below for important details.
 *
 * This function selects the nutation model depending first upon the input value of 'accuracy'.
 * If 'accuracy' is NOVAS_FULL_ACCURACY (0), the IAU 2000A nutation model is used. Otherwise
 * the model set by set_nutation_lp_provider() is used, or else the default nu2000k().
 *
 * See the prologs of the nutation functions in file 'nutation.c' for details concerning the
 * models.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 * </ol>
 *
 * @param t           [cy] TDB time in Julian centuries since J2000.0
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] dpsi   [arcsec] Nutation in longitude in arcseconds.
 * @param[out] deps   [arcsec] Nutation in obliquity in arcseconds.
 *
 * @return            0 if successful, or -1 if the output pointer arguments are NULL
 *
 * @sa set_nutation_lp_provider()
 * @sa nutation()
 * @sa iau2000b()
 * @sa nu2000k()
 * @sa cio_basis()
 * @sa NOVAS_CIRS
 * @sa NOVAS_JD_J2000
 */
int nutation_angles(double t, enum novas_accuracy accuracy, double *dpsi, double *deps) {
  if(!dpsi || !deps) {
    if(dpsi)
      *dpsi = NAN;
    if(deps)
      *deps = NAN;

    return novas_error(-1, EINVAL, "nutation_angles", "NULL output pointer: dspi=%p, deps=%p", dpsi, deps);
  }

  if(accuracy == NOVAS_FULL_ACCURACY) {
    // High accuracy mode -- use IAU 2000A.
    iau2000a(JD_J2000, t * JULIAN_CENTURY_DAYS, dpsi, deps);
  }
  else {
    // Low accuracy mode -- model depends upon value of 'low_acc_choice'.
    nutate_lp(JD_J2000, t * JULIAN_CENTURY_DAYS, dpsi, deps);
  }

  // Convert output to arcseconds.
  *dpsi /= ARCSEC;
  *deps /= ARCSEC;

  return 0;
}

/**
 * Compute the fundamental arguments (mean elements) of the Sun and Moon.
 *
 * REFERENCES:
 * <ol>
 * <li>Simon et al. (1994) Astronomy and Astrophysics 282, 663-683, esp. Sections 3.4-3.5.</li>
 * </ol>
 *
 * @param t       [cy] TDB time in Julian centuries since J2000.0
 * @param[out] a  [rad] Fundamental arguments data to populate (5 doubles) [0:2&pi;]
 *
 * @return        0 if successful, or -1 if the output pointer argument is NULL.
 *
 * @sa nutation_angles()
 * @sa ee_ct()
 * @sa NOVAS_JD_J2000
 */
int fund_args(double t, novas_delaunay_args *a) {
  if(!a)
    return novas_error(-1, EINVAL, "fund_args", "NULL output pointer");

  // higher order terms (for 0.1 uas precision) only if |t| > 0.0001
  if(fabs(t) > 1e-4) {
    const double t2 = t * t;
    a->l = t2 * (31.8792 + t * (0.051635 + t * (-0.00024470)));
    a->l1 = t2 * (-0.5532 + t * (0.000136 + t * (-0.00001149)));
    a->F = t2 * (-12.7512 + t * (-0.001037 + t * (0.00000417)));
    a->D = t2 * (-6.3706 + t * (0.006593 + t * (-0.00003169)));
    a->Omega = t2 * (7.4722 + t * (0.007702 + t * (-0.00005939)));
  }
  else
    memset(a, 0, sizeof(*a));

  a->l += 485868.249036 + t * 1717915923.2178;
  a->l1 += 1287104.793048 + t * 129596581.0481;
  a->F += 335779.526232 + t * 1739527262.8478;
  a->D += 1072260.703692 + t * 1602961601.2090;
  a->Omega += 450160.398036 - t * 6962890.5431;

  a->l = norm_ang(a->l * ARCSEC);
  a->l1 = norm_ang(a->l1 * ARCSEC);
  a->F = norm_ang(a->F * ARCSEC);
  a->D = norm_ang(a->D * ARCSEC);
  a->Omega = norm_ang(a->Omega * ARCSEC);

  return 0;
}

/**
 * Returns the planetary longitude, for Mercury through Neptune, w.r.t. mean dynamical
 * ecliptic and equinox of J2000, with high order terms omitted (Simon et al. 1994,
 * 5.8.1-5.8.8).
 *
 * @param t       [cy] Julian centuries since J2000
 * @param planet  Novas planet id, e.g. NOVAS_MARS.
 * @return        [rad] The approximate longitude of the planet in radians [-&pi;:&pi;],
 *                or NAN if the `planet` id is out of range.
 *
 * @sa accum_prec()
 * @sa nutation_angles()
 * @sa ee_ct()
 * @sa NOVAS_JD_J2000
 *
 * @since 1.0
 * @author Attila Kovacs
 */
double planet_lon(double t, enum novas_planet planet) {
  double lon;

  switch(planet) {
    case NOVAS_MERCURY:
      lon = 4.402608842461 + 2608.790314157421 * t;
      break;
    case NOVAS_VENUS:
      lon = 3.176146696956 + 1021.328554621099 * t;
      break;
    case NOVAS_EARTH:
      lon = 1.753470459496 + 628.307584999142 * t;
      break;
    case NOVAS_MARS:
      lon = 6.203476112911 + 334.061242669982 * t;
      break;
    case NOVAS_JUPITER:
      lon = 0.599547105074 + 52.969096264064 * t;
      break;
    case NOVAS_SATURN:
      lon = 0.874016284019 + 21.329910496032 * t;
      break;
    case NOVAS_URANUS:
      lon = 5.481293871537 + 7.478159856729 * t;
      break;
    case NOVAS_NEPTUNE:
      lon = 5.311886286677 + 3.813303563778 * t;
      break;
    default:
      novas_set_errno(EINVAL, "planet_lon", "invalid planet number: %d", planet);
      return NAN;
  }

  return remainder(lon, TWOPI);
}

/**
 * Returns the general precession in longitude (Simon et al. 1994), equivalent to 5028.8200
 * arcsec/cy at J2000.
 *
 * @param t   [cy] Julian centuries since J2000
 * @return    [rad] the approximate precession angle [-&pi;:&pi;].
 *
 * @sa planet_lon()
 * @sa nutation_angles()
 * @sa ee_ct()
 * @sa NOVAS_JD_J2000
 *
 * @since 1.0
 * @author Attila Kovacs
 */
double accum_prec(double t) {
  // General precession in longitude (Simon et al. 1994), equivalent
  // to 5028.8200 arcsec/cy at J2000.
  return remainder(remainder(0.000005391235 * t, TWOPI) + remainder(0.024380407358 * t * t, TWOPI), TWOPI);
}

/**
 * Computes the mean obliquity of the ecliptic.
 *
 * REFERENCES:
 * <ol>
 * <li>Capitaine et al. (2003), Astronomy and Astrophysics 412, 567-586.</li>
 * </ol>
 *
 * @param jd_tdb      [day] Barycentric Dynamic Time (TDB) based Julian date
 * @return            [arcsec] Mean obliquity of the ecliptic in arcseconds.
 *
 * @sa e_tilt()
 * @sa equ2ecl()
 * @sa ecl2equ()
 * @sa tt2tdb()
 *
 */
double mean_obliq(double jd_tdb) {
  // Compute time in Julian centuries from epoch J2000.0.
  const double t = (jd_tdb - JD_J2000) / JULIAN_CENTURY_DAYS;

  // Compute the mean obliquity in arcseconds.  Use expression from the
  // reference's eq. (39) with obliquity at J2000.0 taken from eq. (37)
  // or Table 8.
  return ((((-0.0000000434 * t - 0.000000576) * t + 0.00200340) * t - 0.0001831) * t - 46.836769) * t + 84381.406;
}

/**
 * Converts an vector in equatorial rectangular coordinates to equatorial spherical
 * coordinates.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 * </ol>
 *
 * @param pos       Position 3-vector, equatorial rectangular coordinates.
 * @param[out] ra   [h] Right ascension in hours [0:24] or NAN if the position vector is NULL or a
 *                  null-vector. It may be NULL if notrequired.
 * @param[out] dec  [deg] Declination in degrees [-90:90] or NAN if the position vector is NULL or
 *                  a null-vector. It may be NULL if not required.
 * @return          0 if successful, -1 of any of the arguments are NULL, or
 *                  1 if all input components are 0 so 'ra' and 'dec' are indeterminate,
 *                  or else 2 if both x and y are zero, but z is nonzero, and so 'ra' is
 *                  indeterminate.
 *
 * @sa radec2vector()
 */
short vector2radec(const double *pos, double *ra, double *dec) {
  static const char *fn = "vector2radec";
  double xyproj;

  // Default return values.
  if(ra)
    *ra = NAN;
  if(dec)
    *dec = NAN;

  if(!pos)
    return novas_error(-1, EINVAL, fn, "NULL input or output pointer: pos=%p, ra=%p, dec=%p", pos, ra, dec);

  xyproj = sqrt(pos[0] * pos[0] + pos[1] * pos[1]);
  if(xyproj == 0.0) {
    if(pos[2] == 0)
      return novas_error(1, EINVAL, fn, "all input components are zero");

    if(ra)
      *ra = 0.0;
    if(dec)
      *dec = (pos[2] < 0.0) ? -90.0 : 90.0;

    return novas_error(2, EDOM, fn, "indeterminate RA for equatorial pole input");
  }

  if(ra) {
    *ra = atan2(pos[1], pos[0]) / HOURANGLE;
    if(*ra < 0.0)
      *ra += DAY_HOURS;
  }
  if(dec)
    *dec = atan2(pos[2], xyproj) / DEGREE;

  return 0;
}

/**
 * Converts equatorial spherical coordinates to a vector (equatorial rectangular coordinates).
 *
 * @param ra          [h] Right ascension (hours).
 * @param dec         [deg] Declination (degrees).
 * @param dist        [AU] Distance (AU)
 * @param[out] pos    [AU] Position 3-vector, equatorial rectangular coordinates (AU).
 *
 * @return            0 if successful, or -1 if the vector argument is NULL.
 *
 * @sa vector2radec()
 * @sa starvectors()
 */
int radec2vector(double ra, double dec, double dist, double *pos) {
  double cosdec;

  if(!pos)
    return novas_error(-1, EINVAL, "radec2vector", "NULL output 3-vector");

  dec *= DEGREE;
  ra *= HOURANGLE;

  cosdec = cos(dec);

  pos[0] = dist * cosdec * cos(ra);
  pos[1] = dist * cosdec * sin(ra);
  pos[2] = dist * sin(dec);

  return 0;
}

/**
 * Converts angular quantities for stars to vectors.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 * </ol>
 *
 * @param star         Pointer to catalog entry structure containing ICRS catalog
 * @param[out] pos     [AU] Position vector, equatorial rectangular coordinates,
 *                     components in AU. It may be NULL if not required.
 * @param[out] vel     [AU/day] Velocity vector, equatorial rectangular coordinates,
 *                     components in AU/Day. It must be distinct from the pos output
 *                     vector, and may be NULL if not required.
 *
 * @return             0 if successful, or -1 if the star argument is NULL or the
 *                     output vectors are the same pointer.
 *
 * @sa make_cat_entry()
 */
int starvectors(const cat_entry *star, double *pos, double *vel) {
  double paralx, r, d, cra, sra, cdc, sdc;

  if(!star)
    return novas_error(-1, EINVAL, "starvectors", "NULL input cat_entry");

  if(pos == vel)
    return novas_error(-1, EINVAL, "starvectors", "identical output pos and vel 3-vectors @ %p", pos, vel);

  // If parallax is unknown, undetermined, or zero, set it to 1e-6
  // milliarcsecond, corresponding to a distance of 1 gigaparsec.
  paralx = star->parallax;
  if(star->parallax <= 0.0)
    paralx = 1.0e-6;

  r = star->ra * HOURANGLE;
  d = star->dec * DEGREE;

  cra = cos(r);
  sra = sin(r);
  cdc = cos(d);
  sdc = sin(d);

  // Convert right ascension, declination, and parallax to position vector
  // in equatorial system with units of AU.
  if(pos) {
    const double dist = 1.0 / sin(paralx * MAS);
    pos[0] = dist * cdc * cra;
    pos[1] = dist * cdc * sra;
    pos[2] = dist * sdc;
  }

  if(vel) {
    // Compute Doppler factor, which accounts for change in
    // light travel time to star.
    const double k = 1.0 / (1.0 - NOVAS_KM * star->radialvelocity / C);

    // Convert proper motion and radial velocity to orthogonal components of
    // motion with units of AU/day.
    const double pmr = k * star->promora / (paralx * JULIAN_YEAR_DAYS);
    const double pmd = k * star->promodec / (paralx * JULIAN_YEAR_DAYS);
    const double rvl = k * NOVAS_KM * star->radialvelocity / (AU / DAY);

    // Transform motion vector to equatorial system.
    vel[0] = -pmr * sra - pmd * sdc * cra + rvl * cdc * cra;
    vel[1] = pmr * cra - pmd * sdc * sra + rvl * cdc * sra;
    vel[2] = pmd * cdc + rvl * sdc;
  }

  return 0;
}

/**
 * Computes the Terrestrial Time (TT) or Terrestrial Dynamical Time (TDT) Julian date
 * corresponding to a Barycentric Dynamical Time (TDB) Julian date.
 *
 * Expression used in this function is a truncated form of a longer and more precise
 * series given in the first reference.  The result is good to about 10 microseconds.
 *
 * @deprecated Use the less computationally intensive an more accurate tt2tdb()
 *            routine instead.
 *
 * REFERENCES:
 * <ol>
 * <li>Fairhead, L. & Bretagnon, P. (1990) Astron. & Astrophys. 229, 240.</li>
 * <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 * <li><a href="https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/FORTRAN/req/time.html#The%20Relationship%20between%20TT%20and%20TDB">
 * https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/FORTRAN/req/time.html</a></li>
 * <li><a href="https://gssc.esa.int/navipedia/index.php/Transformations_between_Time_Systems">
 * https://gssc.esa.int/navipedia/index.php/Transformations_between_Time_Systems</a></li>
 * </ol>
 *
 * @param jd_tdb         [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param[out] jd_tt     [day] Terrestrial Time (TT) based Julian date. (It may be NULL
 *                       if not required)
 * @param[out] secdiff   [s] Difference 'tdb_jd'-'tt_jd', in seconds. (It may be NULL if
 *                       not required)
 * @return               0 if successful, or -1 if the tt_jd pointer argument is NULL.
 *
 * @sa tt2tdb()
 */
int tdb2tt(double jd_tdb, double *jd_tt, double *secdiff) {
  const double t = (jd_tdb - JD_J2000) / JULIAN_CENTURY_DAYS;

  // Expression given in USNO Circular 179, eq. 2.6.
  const double d = 0.001657 * sin(628.3076 * t + 6.2401) + 0.000022 * sin(575.3385 * t + 4.2970) + 0.000014 * sin(1256.6152 * t + 6.1969)
  + 0.000005 * sin(606.9777 * t + 4.0212) + 0.000005 * sin(52.9691 * t + 0.4444) + 0.000002 * sin(21.3299 * t + 5.5431)
  + 0.000010 * t * sin(628.3076 * t + 4.2490);

  // The simpler formula with a precision of ~30 us.
  //  const double t = (jd_tt - JD_J2000) / JULIAN_CENTURY_DAYS;
  //  const double g = 6.239996 + 630.0221385924 * t;
  //  const double d = 0.001657 * sin(g + 0.01671 * sin(g));

  if(jd_tt)
    *jd_tt = jd_tdb - d / DAY;
  if(secdiff)
    *secdiff = d;

  return 0;
}

/**
 * Returns the TDB - TT time difference in seconds for a given TT date.
 *
 * Note, as of version 1.1, it uses the same calculation as the more precise original tdb2tt(). It thus has an acuracy of
 * about 10 &mu;s vs around 30 &mu;s with the simpler formula from the references below.
 *
 *
 * REFERENCES
 * <ol>
 * <li>Fairhead, L. & Bretagnon, P. (1990) Astron. & Astrophys. 229, 240.</li>
 * <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 * <li><a href="https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/FORTRAN/req/time.html#The%20Relationship%20between%20TT%20and%20TDB">
 * https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/FORTRAN/req/time.html</a></li>
 * <li><a href="https://gssc.esa.int/navipedia/index.php/Transformations_between_Time_Systems">
 * https://gssc.esa.int/navipedia/index.php/Transformations_between_Time_Systems</a></li>
 * </ol>
 *
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian date
 * @return          [s] TDB - TT time difference.
 *
 * @sa tdb2tt()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
double tt2tdb(double jd_tt) {
  double dt;

  tdb2tt(jd_tt, NULL, &dt);
  return dt;
}

/**
 * Computes the true right ascension of the celestial intermediate origin (CIO) at a given TT
 * Julian date.  This is the negative value for the equation of the origins.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 * </ol>
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] ra_cio [h] Right ascension of the CIO, with respect to the true equinox of
 *                    date, in hours (+ or -), or NAN when returning with an error code.
 * @return            0 if successful, -1 if the output pointer argument is NULL,
 *                    1 if 'accuracy' is invalid, 10--20: 10 + error code from cio_location(),
 *                    or else 20 + error from cio_basis()
 */
short cio_ra(double jd_tt, enum novas_accuracy accuracy, double *ra_cio) {
  static const char *fn = "cio_ra";
  const double unitx[3] = { 1.0, 0.0, 0.0 };
  double jd_tdb, x[3], y[3], z[3], eq[3], az, r_cio;
  short rs;

  if(!ra_cio)
    return novas_error(-1, EINVAL, fn, "NULL output array");

  // Default return value.
  *ra_cio = NAN;

  // Check for valid value of 'accuracy'.
  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  // 'jd_tdb' is the TDB Julian date.
  jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY;

  // Obtain the basis vectors, in the GCRS, for the celestial intermediate
  // system defined by the CIP (in the z direction) and the CIO (in the
  // x direction).
  prop_error(fn, cio_location(jd_tdb, accuracy, &r_cio, &rs), 10);
  prop_error(fn, cio_basis(jd_tdb, r_cio, rs, accuracy, x, y, z), 20);

  // Compute the direction of the true equinox in the GCRS.
  tod_to_gcrs(jd_tdb, accuracy, unitx, eq);

  // Compute the RA-like coordinate of the true equinox in the celestial
  // intermediate system, in radians
  az = atan2(novas_vdot(eq, y), novas_vdot(eq, x));

  // The RA of the CIO is minus this coordinate, cast as hour-angle
  *ra_cio = -az / HOURANGLE;

  return 0;
}

/**
 * Sets the CIO interpolaton data file to use to interpolate CIO locations vs the GCRS.
 * You can specify either the original `CIO_RA.TXT` file included in the distribution
 * (preferred since v1.1), or else a platform-specific binary data file compiled from it
 * via the <code>cio_file</code> utility (the old way).
 *
 * @param filename    Path (preferably absolute path) `CIO_RA.TXT` or else to the binary
 *                    `cio_ra.bin` data.
 * @return            0 if successful, or else -1 if the specified file does not exists or
 *                    we have no permission to read it.
 *
 * @sa cio_location()
 * @sa gcrs_to_cirs()
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 */
int set_cio_locator_file(const char *filename) {
  FILE *old = cio_file;

  // Open new file first to ensure it has a distinct pointer from the old one...
  cio_file = fopen(filename, "r");

  // Close the old file.
  if(old)
    fclose(old);

  return cio_file ? 0 : novas_error(-1, errno, "set_cio_locator_file", "File could not be opened");
}

/**
 * Returns the location of the celestial intermediate origin (CIO) for a given Julian date,
 * as a right ascension with respect to either the GCRS (geocentric ICRS) origin or the true
 * equinox of date. The CIO is always located on the true equator (= intermediate equator)
 * of date.
 *
 * The user may specify an interpolation file to use via set_cio_locator_file() prior to
 * calling this function. In that case the call will return CIO location relative to GCRS.
 * In the absence of the table, it will calculate the CIO location relative to the true
 * equinox. In either case the type of the location is returned alongside the corresponding
 * CIO location value.
 *
 * NOTES:
 * <ol>
 * <li>
 *   Unlike the NOVAS C version of this function, this version will always return a CIO
 *   location as long as the pointer arguments are not NULL. The returned values will be
 *   interpolated from the locator file if possible, otherwise it falls back to calculating
 *   an equinox-based location per default.
 *  </li>
 * </ol>
 *
 * @param jd_tdb           [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param accuracy         NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] ra_cio      [h] Right ascension of the CIO, in hours, or NAN if returning
 *                         with an error.
 * @param[out] loc_type    Pointer in which to return the reference system in which right
 *                         ascension is given, which is either CIO_VS_GCRS (1) if the
 *                         location was obtained via interpolation of the available data
 *                         file, or else CIO_VS_EQUINOX (2) if it was calculated locally.
 *                         It is set to -1 if returning with an error.
 *
 * @return            0 if successful, -1 if one of the pointer arguments is NULL or the
 *                    accuracy is invalid.
 *
 * @sa set_cio_locator_file()
 * @sa cio_ra()
 * @sa gcrs_to_cirs()
 */
short cio_location(double jd_tdb, enum novas_accuracy accuracy, double *ra_cio, short *loc_type) {
  static const char *fn = "cio_location";

  static THREAD_LOCAL enum novas_accuracy acc_last = -1;
  static THREAD_LOCAL short ref_sys_last = -1;
  static THREAD_LOCAL double t_last = 0.0, ra_last = 0.0;
  static THREAD_LOCAL ra_of_cio cio[CIO_INTERP_POINTS];

  const enum novas_debug_mode saved_debug_state = novas_debug_state;

  // Default return values...
  if(ra_cio)
    *ra_cio = NAN;
  if(loc_type)
    *loc_type = -1;

  if(!ra_cio || !loc_type)
    return novas_error(-1, EINVAL, fn, "NULL output poointer: ra_cio=%p, loc_type=%p", ra_cio, loc_type);

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  // Check if previously computed RA value can be used.
  if(time_equals(jd_tdb, t_last) && (accuracy == acc_last)) {
    *ra_cio = ra_last;
    *loc_type = ref_sys_last;
    return 0;
  }

  // We can let slide errors from cio_array since they don't bother us.
  if(novas_debug_state == NOVAS_DEBUG_ON)
    novas_debug(NOVAS_DEBUG_OFF);

  if(cio_array(jd_tdb, CIO_INTERP_POINTS, cio) == 0) {
    int j;

    // Restore the user-selected debug mode.
    novas_debug(saved_debug_state);

    // Perform Lagrangian interpolation for the RA at 'tdb_jd'.
    *ra_cio = 0.0;

    for(j = 0; j < CIO_INTERP_POINTS; j++) {
      double p = 1.0;
      int i;
      for(i = 0; i < CIO_INTERP_POINTS; i++)
        if(i != j)
          p *= (jd_tdb - cio[i].jd_tdb) / (cio[j].jd_tdb - cio[i].jd_tdb);
      *ra_cio += p * cio[j].ra_cio;
    }

    // change units from arcsec to hour-angle (express as arcsec [*], then cast as hour-angle [/])
    *ra_cio *= ARCSEC / HOURANGLE;
    *loc_type = CIO_VS_GCRS;

    return 0;
  }
  else {
    // Restore the user-selected debug mode.
    novas_debug(saved_debug_state);

    // Calculate the equation of origins.
    *ra_cio = -1.0 * ira_equinox(jd_tdb, NOVAS_TRUE_EQUINOX, accuracy);
    *loc_type = CIO_VS_EQUINOX;
  }

  t_last = jd_tdb;
  acc_last = accuracy;
  ra_last = *ra_cio;
  ref_sys_last = *loc_type;

  return 0;
}

/**
 * Computes the orthonormal basis vectors, with respect to the GCRS (geocentric ICRS), of the
 * celestial intermediate system defined by the celestial intermediate pole (CIP) (in the z
 * direction) and the celestial intermediate origin (CIO) (in the x direction).  A TDB Julian
 * date and the right ascension of the CIO at that date is required as input.  The right
 * ascension of the CIO can be with respect to either the GCRS origin or the true equinox of
 * date -- different algorithms are used in the two cases.
 *
 * This function effectively constructs the matrix C in eq. (3) of the reference.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 * </ol>
 *
 * @param jd_tdb      [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param ra_cio      [h] Right ascension of the CIO at epoch (hours).
 * @param loc_type    CIO_VS_GCRS (1) if the cio location is relative to the GCRS or else
 *                    CIO_VS_EQUINOX (2) if relative to the true equinox of date.
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] x      Unit 3-vector toward the CIO, equatorial rectangular coordinates,
 *                    referred to the GCRS.
 * @param[out] y      Unit 3-vector toward the y-direction, equatorial rectangular
 *                    coordinates, referred to the GCRS.
 * @param[out] z      Unit 3-vector toward north celestial pole (CIP), equatorial
 *                    rectangular coordinates, referred to the GCRS.
 * @return            0 if successful, or -1 if any of the output vector arguments are NULL
 *                    or if the accuracy is invalid, or else 1 if 'ref-sys' is invalid.
 *
 * @sa cio_location()
 * @sa gcrs_to_cirs()
 */
short cio_basis(double jd_tdb, double ra_cio, enum novas_cio_location_type loc_type, enum novas_accuracy accuracy, double *x, double *y,
        double *z) {
  static const char *fn = "cio_basis";
  static THREAD_LOCAL enum novas_accuracy acc_last = -1;
  static THREAD_LOCAL double t_last = 0.0;
  static THREAD_LOCAL double zz[3];

  if(!x || !y || !z)
    return novas_error(-1, EINVAL, fn, "NULL output 3-vector: x=%p, y=%p, z=%p", x, y, z);

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  // Compute unit vector z toward celestial pole.
  if(!zz[2] || !time_equals(jd_tdb, t_last) || (accuracy != acc_last)) {
    const double z0[3] = { 0.0, 0.0, 1.0 };
    tod_to_gcrs(jd_tdb, accuracy, z0, zz);
    t_last = jd_tdb;
    acc_last = accuracy;
  }

  // Now compute unit vectors x and y.  Method used depends on the
  // reference system in which right ascension of the CIO is given.
  ra_cio *= HOURANGLE;

  switch(loc_type) {

    case CIO_VS_GCRS: {

      // Compute vector x toward CIO in GCRS.
      const double sinra = sin(ra_cio);
      const double cosra = cos(ra_cio);
      double l;

      x[0] = zz[2] * cosra;
      x[1] = zz[2] * sinra;
      x[2] = -zz[0] * cosra - zz[1] * sinra;

      // Normalize vector x.
      l = novas_vlen(x);
      x[0] /= l;
      x[1] /= l;
      x[2] /= l;

      break;
    }

    case CIO_VS_EQUINOX: {
      static THREAD_LOCAL double last_ra = 0.0;
      static THREAD_LOCAL double xx[3] = { 0.0, 0.0, 1.0 };

      if(xx[2] || fabs(ra_cio - last_ra) > 1e-12) {
        // Construct unit vector toward CIO in equator-and-equinox-of-date
        // system.
        xx[0] = cos(ra_cio);
        xx[1] = sin(ra_cio);
        xx[2] = 0.0;

        // Rotate the vector into the GCRS to form unit vector x.
        tod_to_gcrs(jd_tdb, accuracy, xx, xx);
      }

      memcpy(x, xx, sizeof(xx));

      break;
    }

    default:
      // Invalid value of 'ref_sys'.
      memset(x, 0, XYZ_VECTOR_SIZE);
      memset(y, 0, XYZ_VECTOR_SIZE);
      memset(z, 0, XYZ_VECTOR_SIZE);

      return novas_error(1, EINVAL, fn, "invalid input CIO location type: %d", loc_type);
  }

  // Compute unit vector y orthogonal to x and z (y = z cross x).
  y[0] = zz[1] * x[2] - zz[2] * x[1];
  y[1] = zz[2] * x[0] - zz[0] * x[2];
  y[2] = zz[0] * x[1] - zz[1] * x[0];

  // Load the z array.
  memcpy(z, zz, sizeof(zz));

  return 0;
}

/**
 * Given an input TDB Julian date and the number of data points desired, this function returns
 * a set of Julian dates and corresponding values of the GCRS right ascension of the celestial
 * intermediate origin (CIO).  The range of dates is centered (at least approximately) on the
 * requested date.  The function obtains the data from an external data file.
 *
 * This function assumes that a CIO locator file (`CIO_RA.TXT` or `cio_ra.bin`) exists in the
 * default location (configured at build time), or else was specified via `set_cio_locator_file()`
 * prior to calling this function.
 *
 * NOTES:
 * <ol>
 * <li>This function has been completely re-written by A. Kovacs to provide much more efficient
 * caching and I/O.</li>
 * </ol>
 *
 * @param jd_tdb    [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param n_pts     Number of Julian dates and right ascension values requested (not less than 2
 *                  or more than NOVAS_CIO_CACHE_SIZE).
 * @param[out] cio  A time series (array) of the right ascension of the Celestial Intermediate
 *                  Origin (CIO) with respect to the GCRS.
 * @return          0 if successful, -1 if the output array is NULL or there
 *                  was an I/O error accessing the CIO location data file. Or else 1 if no
 *                  locator data file is available, 2 if 'jd_tdb' not in the range of the CIO
 *                  file, 3 if 'n_pts' out of range, or 6 if 'jd_tdb' is too close to either end
 *                  of the CIO file do we are unable to put 'n_pts' data points into the output
 *
 * @sa set_cio_locator_file()
 * @sa cio_location()
 */
short cio_array(double jd_tdb, long n_pts, ra_of_cio *cio) {
  static const char *fn = "cio_array";

  // Packed struct in case long is not the same width a double
  struct __attribute__ ((packed)) cio_file_header {
    double jd_start;
    double jd_end;
    double jd_interval;
    long n_recs;
  };

  static const FILE *last_file;
  static struct cio_file_header lookup;
  static ra_of_cio cache[NOVAS_CIO_CACHE_SIZE];
  static long index_cache, cache_count;
  static int is_ascii;
  static int header_size, lrec;

  long index_rec;

  if(cio == NULL)
    return novas_error(-1, EINVAL, fn, "NULL output array");

  if(n_pts < 2 || n_pts > NOVAS_CIO_CACHE_SIZE)
    return novas_error(3, ERANGE, fn, "n_pts=%ld is out of bounds [2:%d]", n_pts, NOVAS_CIO_CACHE_SIZE);

  if(cio_file == NULL)
    set_cio_locator_file(DEFAULT_CIO_LOCATOR_FILE);  // Try default locator file.

  if(cio_file == NULL)
    return novas_error(1, ENODEV, fn, "No default CIO locator file");

  // Check if it's a new file
  if(last_file != cio_file) {
    char line[80] = {0};
    int version, tokens;

    last_file = NULL;
    cache_count = 0;

    if(fgets(line, sizeof(line) - 1, cio_file) == NULL)
      return novas_error(1, errno, fn, "empty CIO locator data: %s", strerror(errno));

    tokens = sscanf(line, "CIO RA P%d @ %lfd", &version, &lookup.jd_interval);

    if(tokens == 2) {
      is_ascii = 1;
      header_size = strlen(line);

      if(fgets(line, sizeof(line) - 1, cio_file) == NULL)
        return novas_error(1, errno, fn, "missing ASCII CIO locator data: %s", strerror(errno));

      lrec = strlen(line);

      if(sscanf(line, "%lf", &lookup.jd_start) < 1)
        return novas_error(-1, errno, fn, "incomplete or corrupted ASCII CIO locator record: %s", strerror(errno));

      fseek(cio_file, 0, SEEK_END);

      lookup.n_recs = (ftell(cio_file) - header_size) / lrec;
      lookup.jd_end = lookup.jd_start + lookup.n_recs * lookup.jd_interval;
    }
    else if(tokens) {
      return novas_error(-1, errno, fn, "incomplete or corrupted ASCII CIO locator data header: %s", strerror(errno));
    }
    else {
      is_ascii = 0;
      header_size = sizeof(struct cio_file_header);
      lrec = sizeof(ra_of_cio);

      fseek(cio_file, 0, SEEK_SET);

      // Read the file header
      if(fread(&lookup, sizeof(struct cio_file_header), 1, cio_file) != 1)
        return novas_error(-1, errno, fn, "incomplete or corrupted binary CIO locator data header: %s", strerror(errno));
    }

    last_file = cio_file;
  }

  // Check the input data against limits.
  if((jd_tdb < lookup.jd_start) || (jd_tdb > lookup.jd_end))
    return novas_error(2, EOF, fn, "requested time (JD=%.1f) outside of CIO locator data range (%.1f:%.1f)", jd_tdb, lookup.jd_start,
            lookup.jd_end);

  // Calculate the record index from which data is requested.
  index_rec = (long) ((jd_tdb - lookup.jd_start) / lookup.jd_interval) - (n_pts >> 1);
  if(index_rec < 0)
    return novas_error(6, EOF, fn, "not enough CIO location data points available at the requested time (JD=%.1f)", jd_tdb);

  // Check if the range of data needed is outside the cached range.
  if((index_rec < index_cache) || (index_rec + n_pts > index_cache + cache_count)) {
    // Load cache centered on requested range.
    const long N = lookup.n_recs - index_rec > NOVAS_CIO_CACHE_SIZE ? NOVAS_CIO_CACHE_SIZE : lookup.n_recs - index_rec;

    cache_count = 0;
    index_cache = index_rec - (NOVAS_CIO_CACHE_SIZE >> 1);
    if(index_cache < 0)
      index_cache = 0;

    // Read in cache from the requested position
    fseek(cio_file, header_size + index_cache * lrec, SEEK_SET);

    if(is_ascii) {
      for(cache_count = 0; cache_count < N; cache_count++)
        if(fscanf(cio_file, "%lf %lf\n", &cache[cache_count].jd_tdb, &cache[cache_count].ra_cio) != 2)
          return novas_error(-1, errno, fn, "corrupted ASCII CIO locator data: %s", strerror(errno));
    }
    else {
      if(fread(cache, sizeof(ra_of_cio), N, cio_file) != (size_t) N)
        return novas_error(-1, errno, fn, "corrupted binary CIO locator data: %s", strerror(errno));
      cache_count = N;
    }
  }

  if((index_rec - index_cache) + n_pts > cache_count)
    return novas_error(6, EOF, fn, "not enough CIO location data points available at the requested time (JD=%.1f)", jd_tdb);

  // Copy the requested number of points in to the destination;
  memcpy(cio, &cache[index_rec - index_cache], n_pts * sizeof(ra_of_cio));
  return 0;
}

/**
 * Compute the intermediate right ascension of the equinox at the input Julian date, using an
 * analytical expression for the accumulated precession in right ascension.  For the true
 * equinox, the result is the equation of the origins.
 *
 * NOTES:
 * <ol>
 * <li>Fixes bug in NOVAS C 3.1, which returned the value for the wrong 'equinox' if
 * 'equinox = 1' was requested for the same 'jd_tbd' and 'accuracy' as a the preceding
 * call with 'equinox = 0'. As a result, the caller ended up with the mean instead
 * of the expected true equinox R.A. value.</li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Capitaine, N. et al. (2003), Astronomy and Astrophysics 412, 567-586, eq. (42).</li>
 * </ol>
 *
 * @param jd_tdb      [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param equinox     NOVAS_MEAN_EQUINOX (0) or NOVAS_TRUE_EQUINOX (1; or non-zero)
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @return            [h]  Intermediate right ascension of the equinox, in hours (+ or -).
 *                    If 'equinox' = 1 (i.e true equinox), then the returned value is
 *                    the equation of the origins.
 *
 * @sa cio_location()
 * @sa gcrs_to_cirs()
 */
double ira_equinox(double jd_tdb, enum novas_equinox_type equinox, enum novas_accuracy accuracy) {
  static THREAD_LOCAL enum novas_equinox_type last_type = -999;
  static THREAD_LOCAL enum novas_accuracy acc_last = NOVAS_FULL_ACCURACY;
  static THREAD_LOCAL double t_last = 0.0, last_ra;

  double t, eqeq = 0.0, prec_ra;

  // Fail-safe accuracy
  if(accuracy != NOVAS_REDUCED_ACCURACY)
    accuracy = NOVAS_FULL_ACCURACY;

  if(time_equals(jd_tdb, t_last) && (accuracy == acc_last) && (last_type == equinox)) {
    // Same parameters as last time. Return last calculated value.
    return last_ra;
  }

  // For the true equinox, obtain the equation of the equinoxes in time
  // seconds, which includes the 'complementary terms'.
  if(equinox == NOVAS_TRUE_EQUINOX) {
    e_tilt(jd_tdb, accuracy, NULL, NULL, &eqeq, NULL, NULL);
  }

  // Compute time in Julian centuries from J2000
  t = (jd_tdb - JD_J2000) / JULIAN_CENTURY_DAYS;

  // Precession in RA in arcseconds taken from the reference.
  prec_ra = 0.014506 + ((((-0.0000000368 * t - 0.000029956) * t - 0.00000044) * t + 1.3915817) * t + 4612.156534) * t;
  last_ra = -(prec_ra / 15.0 + eqeq) / 3600.0;

  t_last = jd_tdb;
  last_type = equinox;
  acc_last = accuracy;

  return last_ra;
}

/**
 * Sets the function to use for obtaining position / velocity information for minor planets,
 * or sattelites.
 *
 * @param func   new function to use for accessing ephemeris data for minor planets or satellites.
 * @return       0 if successful, or else -1 if the function argument is NULL.
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa get_ephem_provider()
 * @sa set_planet_provider()
 * @sa set_planet_provider_hp()
 * @sa ephemeris()
 * @sa solsys-ephem.c
 *
 */
int set_ephem_provider(novas_ephem_provider func) {
  readeph2_call = func;
  return 0;
}

/**
 * Returns the user-defined ephemeris accessor function.
 *
 * @return    the currently defined function for accessing ephemeris data for minor planets
 *            or satellites, ot NULL if no function was set via set_ephem_provider() previously.
 *
 * @sa set_ephem_provider()
 * @sa get_planet_provider()
 * @sa get_planet_provider_hp()
 * @sa ephemeris()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
novas_ephem_provider get_ephem_provider() {
  return readeph2_call;
}

/**
 * Retrieves the position and velocity of a solar system body from a fundamental ephemeris.
 *
 * It is recommended that the input structure 'cel_obj' be created using make_object()
 *
 * @param jd_tdb    [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param body      Pointer to structure containing the designation of the body of interest
 * @param origin    NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1)
 * @param accuracy  NOCAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] pos  [AU] Pointer to structure containing the designation of the body of interest
 * @param[out] vel  [AU/day] Velocity vector of the body at 'jd_tdb'; equatorial rectangular
 *                  coordinates in AU/day referred to the ICRS.
 * @return          0 if successful, -1 if the 'jd_tdb' or input object argument is NULL, or
 *                  else 1 if 'origin' is invalid, 2 if <code>cel_obj->type</code> is invalid,
 *                  10 + the error code from solarsystem(), or 20 + the error code from
 *                  readeph().
 *
 * @sa set_planet_provider()
 * @sa set_planet_provider_hp()
 * @sa set_ephem_provider()
 * @sa ephem_open()
 * @sa make_planet()
 * @sa make_ephem_object()
 */
short ephemeris(const double *jd_tdb, const object *body, enum novas_origin origin, enum novas_accuracy accuracy, double *pos, double *vel) {
  static const char *fn = "ephemeris";

  double posvel[6] = {0};
  int error = 0;

  if(!jd_tdb || !body)
    return novas_error(-1, EINVAL, fn, "NULL input pointer: jd_tdb=%p, body=%p", jd_tdb, body);

  if(!pos || !vel)
    return novas_error(-1, EINVAL, fn, "NULL output pointer: pos=%p, vel=%p", pos, vel);

  if(pos == vel)
    return novas_error(-1, EINVAL, fn, "identical output pos and vel 3-vectors @ %p.", pos);

  // Check the value of 'origin'.
  if(origin < 0 || origin >= NOVAS_ORIGIN_TYPES)
    return novas_error(1, EINVAL, fn, "invalid origin type: %d", origin);

  // Invoke the appropriate ephemeris access software depending upon the
  // type of object
  switch(body->type) {

    case NOVAS_PLANET:
      // Get the position and velocity of a major planet, Pluto, Sun, or Moon.
      // When high accuracy is specified, use function 'solarsystem_hp' rather
      // than 'solarsystem'.

      if(accuracy == NOVAS_FULL_ACCURACY)
        error = planet_call_hp(jd_tdb, body->number, origin, pos, vel);
      else
        error = planet_call(jd_tdb[0] + jd_tdb[1], body->number, origin, pos, vel);

      prop_error("ephemeris:planet", error, 10);
      break;

    case NOVAS_EPHEM_OBJECT: {
      enum novas_origin eph_origin = NOVAS_HELIOCENTER;

      if(readeph2_call) {
        // If there is a newstyle epehemeris access routine set, we will prefer it.
        error = readeph2_call(body->name, body->number, jd_tdb[0], jd_tdb[1], &eph_origin, posvel, &posvel[3]);
      }
      else {
#  ifdef DEFAULT_READEPH
        // Use whatever readeph() was compiled or the equivalent user-defined call
        double *res = readeph(body->number, body->name, jd_tdb[0] + jd_tdb[1], &error);
        if(res == NULL) {
          error = 3;
          errno = ENOSYS;
        }
        else {
          memcpy(posvel, res, sizeof(posvel));
          free(res);
        }
#  else
        return novas_error(-1, errno, "ephemeris:ephem_object", "No ephemeris provider was defined. Call set_ephem_provider() prior.");
#  endif
      }

      prop_error("ephemeris:ephem_object", error, 20);

      // Check and adjust the origins as necessary.
      if(origin != eph_origin) {
        double pos0[3] = {0}, vel0[3] = {0};
        enum novas_planet refnum = (origin == NOVAS_BARYCENTER) ? NOVAS_SSB : NOVAS_SUN;
        int i;

        prop_error("ephemeris:origin", planet_call(jd_tdb[0] + jd_tdb[1], refnum, eph_origin, pos0, vel0), 10);

        for(i = 3; --i >= 0;) {
          posvel[i] -= pos[i];
          posvel[i + 3] = novas_add_vel(posvel[i + 3], vel[i]);
        }
      }

      // Break up 'posvel' into separate position and velocity vectors.
      memcpy(pos, posvel, XYZ_VECTOR_SIZE);
      memcpy(vel, &posvel[3], XYZ_VECTOR_SIZE);

      break;
    }

    case NOVAS_ORBITAL_OBJECT: {
      object center;
      double pos0[3] = {0}, vel0[3] = {0};
      int i;

      prop_error(fn, make_planet(body->orbit.system.center, &center), 0);
      prop_error(fn, ephemeris(jd_tdb, &center, origin, accuracy, pos0, vel0), 0);
      prop_error(fn, novas_orbit_posvel(jd_tdb[0] + jd_tdb[1], &body->orbit, accuracy, pos, vel), 0);

      for(i = 3; --i >= 0; ) {
        pos[i] += pos0[i];
        vel[i] += vel0[i];
      }

      break;
    }

    default:
      return novas_error(2, EINVAL, fn, "invalid Solar-system body type: %d", body->type);
  }

  return 0;
}

/**
 * Change xzy vectors to the new polar orientation. &theta, &phi define the orientation of the input pole in the output system.
 *
 * @param in        input 3-vector in the original system (pole = z)
 * @param theta     [deg] polar angle of original pole in the new system
 * @param phi       [deg] azimuthal angle of original pole in the new system
 * @param[out] out  output 3-vector in the new (rotated) system. It may be the same vector as the input.
 * @return          0
 *
 */
static int change_pole(const double *in, double theta, double phi, double *out) {
  double x, y, z;

  x = in[0];
  y = in[1];
  z = in[2];

  theta *= DEGREE;
  phi *= DEGREE;

  double ca = cos(phi);
  double sa = sin(phi);
  double cb = cos(theta);
  double sb = sin(theta);

  out[0] = ca * x - sa * cb * y + sa * sb * z;
  out[1] = sa * x + ca * cb * y - ca * sb * z;
  out[2] = sb * y + cb * z;

  return 0;
}

/**
 * Converts equatorial coordinates of a given type to GCRS equatorial coordinates
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TDB) based Julian Date
 * @param sys       the type of equator assumed for the input (mean, true, or GCRS).
 * @param[in, out] vec  vector to change to GCRS.
 * @return          0 if successful, or else -1 (errno set to EINVAL) if the 'sys'
 *                  argument is invalid.
 *
 * @author Attila Kovacs
 * @since 1.2
 */
static int equ2gcrs(double jd_tdb, enum novas_reference_system sys, double *vec) {
  switch(sys) {
    case NOVAS_GCRS:
    case NOVAS_ICRS:
      return 0;
    case NOVAS_CIRS:
      return cirs_to_gcrs(jd_tdb, NOVAS_REDUCED_ACCURACY, vec, vec);
    case NOVAS_J2000:
      return j2000_to_gcrs(vec, vec);
    case NOVAS_TOD:
      return tod_to_gcrs(jd_tdb, NOVAS_REDUCED_ACCURACY, vec, vec);
    case NOVAS_MOD:
      return mod_to_gcrs(jd_tdb, vec, vec);
    default:
      return novas_error(-1, EINVAL, "equ2gcrs", "invalid reference system: %d", sys);
  }
}

/**
 * Convert coordinates in an orbital system to GCRS equatorial coordinates
 *
 * @param jd_tdb        [day] Barycentric Dynamic Time (TDB) based Julian Date
 * @param sys           Orbital system specification
 * @param accuracy      NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[in, out] vec  Coordinates
 * @return              0 if successful, or else an error from ecl2equ_vec().
 *
 * @author Attila Kovacs
 * @since 1.2
 */
static int orbit2gcrs(double jd_tdb, const novas_orbital_system *sys, enum novas_accuracy accuracy, double *vec) {
  static const char *fn = "orbit2gcrs";

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  if(sys->obl)
    change_pole(vec, sys->obl, sys->Omega, vec);

  if(sys->plane == NOVAS_ECLIPTIC_PLANE) {
    enum novas_equator_type eq;
    double jd = jd_tdb;

    switch(sys->type) {
      case NOVAS_GCRS:
      case NOVAS_ICRS:
        eq = NOVAS_GCRS_EQUATOR;
        jd = NOVAS_JD_J2000;
        break;
      case NOVAS_J2000:
        eq = NOVAS_TRUE_EQUATOR;
        jd = NOVAS_JD_J2000;
        break;
      case NOVAS_TOD:
      case NOVAS_CIRS:
        eq = NOVAS_TRUE_EQUATOR;
        break;
      case NOVAS_MOD:
        eq = NOVAS_MEAN_EQUATOR;
        break;
      default:
        return novas_error(-1, EINVAL, fn, "invalid reference system: %d", sys->type);
    }

    ecl2equ_vec(jd, eq, accuracy, vec, vec);
  }
  else if(sys->plane != NOVAS_EQUATORIAL_PLANE)
    return novas_error(-1, EINVAL, fn, "invalid orbital system reference plane type: %d", sys->type);

  prop_error(fn, equ2gcrs(jd_tdb, sys->type, vec), 0);

  return 0;
}

/**
 * Calculates a rectangular equatorial position and velocity vector for the given orbital elements for the
 * specified time of observation.
 *
 * REFERENCES:
 * <ol>
 * <li>https://ssd.jpl.nasa.gov/planets/approx_pos.html</li>
 * <li>https://en.wikipedia.org/wiki/Orbital_elements</li>
 * <li>https://orbitalofficial.com/</li>
 * <li>https://downloads.rene-schwarz.com/download/M001-Keplerian_Orbit_Elements_to_Cartesian_State_Vectors.pdf</li>
 * </ol>
 *
 * @param jd_tdb    [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param orbit     Orbital parameters
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1).
 * @param[out] pos  [AU] Output ICRS equatorial position vector, or NULL if not required
 * @param[out] vel  [AU/day] Output ICRS equatorial velocity vector, or NULL if not required
 * @return          0 if successful, or else -1 if the orbital parameters are NULL
 *                  or if the position and velocity output vectors are the same or the orbital
 *                  system is ill defined (errno set to EINVAL), or if the calculation did not converge (errno set to
 *                  ECANCELED), or
 *
 * @sa ephemeris()
 * @sa novas_geom_posvel()
 * @sa place()
 * @sa make_orbital_object()
 *
 * @author Attila Kovacs
 * @since 1.2
 */
int novas_orbit_posvel(double jd_tdb, const novas_orbital *orbit, enum novas_accuracy accuracy, double *pos, double *vel) {
  static const char *fn = "novas_orbit_posvel";

  double dt, M, E, nu, r, omega, Omega;
  double cO, sO, ci, si, co, so;
  double xx, yx, zx, xy, yy, zy;
  int i = novas_inv_max_iter;

  if(!orbit)
    return novas_error(-1, EINVAL, fn, "input orbital elements is NULL");

  if(pos == vel)
    return novas_error(-1, EINVAL, fn, "output pos = vel (@ %p)", pos);

  dt = (jd_tdb - orbit->jd_tdb);
  E = M = remainder(orbit->M0 + orbit->n * dt, 360.0) * DEGREE;

  // Iteratively determine E, using Newton-Raphson method...
  while(--i >= 0) {
    double esE = orbit->e * sin(E);
    double ecE = orbit->e * cos(E);
    double dE = (E - esE - M) / (1.0 - ecE);

    E -= dE;
    if(fabs(dE) < EPREC)
      break;
  }

  if(i < 0)
    return novas_error(-1, ECANCELED, fn, "Eccentric anomaly convergence failure");

  nu = 2.0 * atan2(sqrt(1.0 + orbit->e) * sin(0.5 * E), sqrt(1.0 - orbit->e) * cos(0.5 * E));
  r = orbit->a * (1.0 - orbit->e * cos(E));

  omega = orbit->omega * DEGREE;
  if(orbit->apsis_period > 0.0)
    omega += TWOPI * remainder(dt / orbit->apsis_period, 1.0);

  Omega = orbit->Omega * DEGREE;
  if(orbit->node_period > 0.0)
    Omega += TWOPI * remainder(dt / orbit->node_period, 1.0);

  // pos = Rz(-Omega) . Rx(-i) . Rz(-omega) . orb
  cO = cos(Omega);
  sO = sin(Omega);
  ci = cos(orbit->i * DEGREE);
  si = sin(orbit->i * DEGREE);
  co = cos(omega);
  so = sin(omega);

  // Rotation matrix
  // See https://en.wikipedia.org/wiki/Euler_angles
  // (note the Wikipedia has opposite sign convention for angles...)
  xx = cO * co - sO * ci * so;
  yx = sO * co + cO * ci * so;
  zx = si * so;

  xy = -cO * so - sO * ci * co;
  yy = -sO * so + cO * ci * co;
  zy = si * co;

  if(pos) {
    double x = r * cos(nu);
    double y = r * sin(nu);

    // Perform rotation
    pos[0] = xx * x + xy * y;
    pos[1] = yx * x + yy * y;
    pos[2] = zx * x + zy * y;

    prop_error(fn, orbit2gcrs(jd_tdb, &orbit->system, accuracy, pos), 0);
  }

  if(vel) {
    double v = orbit->n * DEGREE * orbit->a * orbit->a / r;    // [AU/day]
    double x = -v * sin(E);
    double y = v * sqrt(1.0 - orbit->e * orbit->e) * cos(E);

    // Perform rotation
    vel[0] = xx * x + xy * y;
    vel[1] = yx * x + yy * y;
    vel[2] = zx * x + zy * y;

    prop_error(fn, orbit2gcrs(jd_tdb, &orbit->system, accuracy, vel), 0);
  }

  return 0;
}

/**
 * Convert Hipparcos catalog data at epoch J1991.25 to epoch J2000.0, for use within NOVAS.
 * To be used only for Hipparcos or Tycho stars with linear space motion.  Both input and
 * output data is in the ICRS.
 *
 * @param hipparcos       An entry from the Hipparcos catalog, at epoch J1991.25, with 'ra' in
 *                        degrees(!) as per Hipparcos catalog units.
 * @param[out] hip_2000   The transformed input entry, at epoch J2000.0, with 'ra' in hours(!)
 *                        as per the NOVAS convention.
 *
 * @return            0 if successful, or -1 if either of the input pointer arguments is NULL.
 *
 * @sa make_cat_entry()
 * @sa NOVAS_JD_HIP
 */
int transform_hip(const cat_entry *hipparcos, cat_entry *hip_2000) {
  static const char *fn = "transform_hip";
  cat_entry scratch;

  if(!hipparcos)
    return novas_error(-1, EINVAL, fn, "NULL Hipparcos input catalog entry");

  // Set up a "scratch" catalog entry containing Hipparcos data in
  // "NOVAS units."
  scratch = *hipparcos;
  strcpy(scratch.catalog, "SCR");

  // Convert right ascension from degrees to hours.
  scratch.ra /= 15.0;

  // Change the epoch of the Hipparcos data from J1991.25 to J2000.0.
  prop_error(fn, transform_cat(1, NOVAS_JD_HIP, &scratch, JD_J2000, "HP2", hip_2000), 0);
  return 0;
}

/**
 * Transform a star's catalog quantities for a change the coordinate system
 * and/or the date for which the positions are calculated.  Also used to
 * rotate catalog quantities on the dynamical equator and equinox of J2000.0
 * to the ICRS or vice versa.
 *
 * 'date_incat' and 'date_newcat' may be specified either as a Julian date
 * (e.g., 2433282.5 or NOVAS_JD_B1950) or a fractional Julian year and
 * fraction (e.g., 1950.0). Values less than 10000 are assumed to be years.
 * You can also use the supplied constants NOVAS_JD_J2000 or
 * NOVAS_JD_B1950. The date arguments are ignored for the ICRS frame
 * conversion options.
 *
 * If 'option' is PROPER_MOTION (1), input data can be in any reference system.
 * If 'option' is PRECESSION (2) or CHANGE_EPOCH (3), input data is assume to be
 * in the dynamical system of 'date_incat' and produces output in the dynamical
 * system of 'date_outcat'. If 'option' is CHANGE_J2000_TO_ICRS (4), the input
 * data should be in the J2000.0 dynamical frame. And if 'option' is
 * CHANGE_ICRS_TO_J2000 (5), the input data must be in the ICRS, and the output
 * will be in the J2000 dynamical frame.
 *
 * This function cannot be properly used to bring data from old star catalogs
 * into the modern system, because old catalogs were compiled using a set of
 * constants that are incompatible with modern values.  In particular, it
 * should not be used for catalogs whose positions and proper motions were
 * derived by assuming a precession constant significantly different
 * from the value implicit in function precession().
 *
 * @param option      Type of transformation
 * @param jd_tt_in    [day|yr] Terrestrial Time (TT) based Julian date, or year, of
 *                    input catalog data. Not used if option is CHANGE_J2000_TO_ICRS (4)
 *                    or CHANGE_ICRS_TO_J2000 (5).
 * @param in          An entry from the input catalog, with units as given in the
 *                    struct definition
 * @param jd_tt_out   [day|yr] Terrestrial Time (TT) based Julian date, or year, of
 *                    output catalog data. Not used if option is CHANGE_J2000_TO_ICRS (4)
 *                    or CHANGE_ICRS_TO_J2000 (5).
 * @param out_id      Catalog identifier (0 terminated). It may also be NULL in which
 *                    case the catalog name is inherited from the input.
 * @param[out] out    The transformed catalog entry, with units as given in the struct
 *                    definition
 * @return            0 if successful, -1 if either vector argument is NULL or if the
 *                    'option' is invalid, or else 2 if 'out_id' is too long.
 *
 * @sa transform_hip()
 * @sa make_cat_entry()
 * @sa NOVAS_JD_J2000
 * @sa NOVAS_JD_B1950
 * @sa NOVAS_JD_HIP
 */
short transform_cat(enum novas_transform_type option, double jd_tt_in, const cat_entry *in, double jd_tt_out, const char *out_id,
        cat_entry *out) {
  static const char *fn = "transform_cat";

  double paralx, dist, r, d, cra, sra, cdc, sdc, k;
  double pos[3], vel[3], term1, pmr, pmd, rvl, xyproj;

  if(!in || !out)
    return novas_error(-1, EINVAL, fn, "NULL parameter: in=%p, out=%p", in, out);

  if(out_id && strlen(out_id) >= sizeof(out->starname))
    return novas_error(2, EINVAL, fn, "output catalog ID is too long (%d > %d)", (int) strlen(out_id), (int) sizeof(out->starname) - 1);

  if(option == CHANGE_J2000_TO_ICRS || option == CHANGE_ICRS_TO_J2000) {
    // ICRS frame ties always assume J2000 for both input and output...
    jd_tt_in = NOVAS_JD_J2000;
    jd_tt_out = NOVAS_JD_J2000;
  }
  else {
    // If necessary, compute Julian dates.

    // This function uses TDB Julian dates internally, but no distinction between TDB and TT is necessary.
    if(jd_tt_in < 10000.0)
      jd_tt_in = JD_J2000 + (jd_tt_in - 2000.0) * JULIAN_YEAR_DAYS;
    if(jd_tt_out < 10000.0)
      jd_tt_out = JD_J2000 + (jd_tt_out - 2000.0) * JULIAN_YEAR_DAYS;
  }

  // Convert input angular components to vectors

  // If parallax is unknown, undetermined, or zero, set it to 1.0e-6
  // milliarcsecond, corresponding to a distance of 1 gigaparsec.
  paralx = in->parallax;
  if(paralx <= 0.0)
    paralx = 1.0e-6;

  // Convert right ascension, declination, and parallax to position
  // vector in equatorial system with units of AU.
  dist = 1.0 / sin(paralx * MAS);
  r = in->ra * HOURANGLE;
  d = in->dec * DEGREE;
  cra = cos(r);
  sra = sin(r);
  cdc = cos(d);
  sdc = sin(d);
  pos[0] = dist * cdc * cra;
  pos[1] = dist * cdc * sra;
  pos[2] = dist * sdc;

  // Compute Doppler factor, which accounts for change in light travel time to star.
  k = 1.0 / (1.0 - in->radialvelocity / C * NOVAS_KM);

  // Convert proper motion and radial velocity to orthogonal components
  // of motion, in spherical polar system at star's original position,
  // with units of AU/day.
  term1 = paralx * JULIAN_YEAR_DAYS;
  pmr = in->promora / term1 * k;
  pmd = in->promodec / term1 * k;
  rvl = in->radialvelocity * DAY / AU_KM * k;

  // Transform motion vector to equatorial system.
  vel[0] = -pmr * sra - pmd * sdc * cra + rvl * cdc * cra;
  vel[1] = pmr * cra - pmd * sdc * sra + rvl * cdc * sra;
  vel[2] = pmd * cdc + rvl * sdc;

  // Update star's position vector for space motion (only if 'option' = 1 or 'option' = 3).
  if((option == PROPER_MOTION) || (option == CHANGE_EPOCH)) {
    int j;
    for(j = 0; j < 3; j++)
      pos[j] += vel[j] * (jd_tt_out - jd_tt_in);
  }

  switch(option) {
    case PROPER_MOTION:
      break;

    case PRECESSION:
    case CHANGE_EPOCH: {
      // Precess position and velocity vectors (only if 'option' = 2 or 'option' = 3).
      prop_error("transform_cat", precession(jd_tt_in, pos, jd_tt_out, pos), 0);
      precession(jd_tt_in, vel, jd_tt_out, vel);
      break;
    }

    case CHANGE_J2000_TO_ICRS:
      // Rotate dynamical J2000.0 position and velocity vectors to ICRS (only if 'option' = 4).
      frame_tie(pos, J2000_TO_ICRS, pos);
      frame_tie(vel, J2000_TO_ICRS, vel);
      break;

    case CHANGE_ICRS_TO_J2000:
      // Rotate ICRS position and velocity vectors to dynamical J2000.0 (only if 'option' = 5).
      frame_tie(pos, ICRS_TO_J2000, pos);
      frame_tie(vel, ICRS_TO_J2000, vel);
      break;

    default:
      if(out != in)
        *out = *in;

      return novas_error(-1, EINVAL, fn, "invalid option %d", option);
  }

  // Convert vectors back to angular components for output.

  // From updated position vector, obtain star's new position expressed as angular quantities.
  xyproj = sqrt(pos[0] * pos[0] + pos[1] * pos[1]);

  r = (xyproj > 0.0) ? atan2(pos[1], pos[0]) : 0.0;
  out->ra = r / HOURANGLE;
  if(out->ra < 0.0)
    out->ra += DAY_HOURS;

  d = atan2(pos[2], xyproj);
  out->dec = d / DEGREE;

  dist = novas_vlen(pos);

  paralx = asin(1.0 / dist) / MAS;

  // Transform motion vector back to spherical polar system at star's new position.
  cra = cos(r);
  sra = sin(r);
  cdc = cos(d);
  sdc = sin(d);
  pmr = -vel[0] * sra + vel[1] * cra;
  pmd = -vel[0] * cra * sdc - vel[1] * sra * sdc + vel[2] * cdc;
  rvl = vel[0] * cra * cdc + vel[1] * sra * cdc + vel[2] * sdc;

  // Convert components of motion to from AU/day to normal catalog units.
  out->promora = pmr * paralx * JULIAN_YEAR_DAYS / k;
  out->promodec = pmd * paralx * JULIAN_YEAR_DAYS / k;
  out->radialvelocity = rvl * (AU_KM / DAY) / k;

  // Set the catalog identification code for the transformed catalog entry.
  if(out_id)
    strncpy(out->catalog, out_id, sizeof(out->catalog));
  else if(out != in)
    strncpy(out->catalog, in->catalog, sizeof(out->catalog));

  out->catalog[sizeof(out->catalog) - 1] = '\0';

  if(out != in) {
    // Take care of zero-parallax case.
    if(in->parallax <= 0.0) {
      out->parallax = 0.0;
      out->radialvelocity = in->radialvelocity;
    }
    else
      out->parallax = in->parallax;

    // Copy unchanged quantities from the input catalog entry to the transformed catalog entry.
    strncpy(out->starname, in->starname, sizeof(out->starname));
    out->starname[sizeof(out->starname) - 1] = '\0';
    out->starnumber = in->starnumber;
  }

  return 0;
}

/**
 * Determines the angle of an object above or below the Earth's limb (horizon).  The geometric
 * limb is computed, assuming the Earth to be an airless sphere (no refraction or oblateness
 * is included).  The observer can be on or above the Earth.  For an observer on the surface
 * of the Earth, this function returns the approximate unrefracted elevation.
 *
 * @param pos_src         [AU] Position 3-vector of observed object, with respect to origin at
 *                        geocenter, components in AU.
 * @param pos_obs         [AU] Position 3-vector of observer, with respect to origin at
 *                        geocenter, components in AU.
 * @param[out] limb_ang   [deg] Angle of observed object above (+) or below (-) limb in degrees,
 *                        or NAN if reurning with an error. It may be NULL if not required.
 * @param[out] nadir_ang  Nadir angle of observed object as a fraction of apparent radius
 *                        of limb: %lt;1.0 if below the limb; 1.0 on the limb; or &gt;1.0 if
 *                        above the limb. Returns NAN in case of an error return. It may be NULL
 *                        if not required.
 *
 * @return    0 if successful, or -1 if either of the input vectors is NULL or if either input
 *            position is a null vector (at the geocenter).
 *
 * @sa place()
 */
int limb_angle(const double *pos_src, const double *pos_obs, double *limb_ang, double *nadir_ang) {
  static const char *fn = "limb_angle";
  double d_src, d_obs, aprad, zdlim, coszd, zdobj;

  // Default return values (in case of error)
  if(limb_ang)
    *limb_ang = NAN;
  if(nadir_ang)
    *nadir_ang = NAN;

  if(!pos_src || !pos_obs)
    return novas_error(-1, EINVAL, fn, "NULL input 3-vector: pos_src=%p, pos_obs=%p", pos_src, pos_obs);

  // Compute the distance to the object and the distance to the observer.
  d_src = novas_vlen(pos_src);
  d_obs = novas_vlen(pos_obs);

  if(!d_src)
    return novas_error(-1, EINVAL, fn, "looking at geocenter");

  if(!d_obs)
    return novas_error(-1, EINVAL, fn, "observer is at geocenter");

  // Compute apparent angular radius of Earth's limb.
  aprad = (d_obs >= ERAD_AU) ? asin(ERAD_AU / d_obs) : HALF_PI;

  // Compute zenith distance of Earth's limb.
  zdlim = M_PI - aprad;

  // Compute zenith distance of observed object.
  coszd = novas_vdot(pos_src, pos_obs) / (d_src * d_obs);

  if(coszd <= -1.0)
    zdobj = M_PI;
  else if(coszd >= 1.0)
    zdobj = 0.0;
  else
    zdobj = acos(coszd);

  // Angle of object wrt limb is difference in zenith distances.
  if(limb_ang)
    *limb_ang = (zdlim - zdobj) / DEGREE;

  // Nadir angle of object as a fraction of angular radius of limb.
  if(nadir_ang)
    *nadir_ang = (M_PI - zdobj) / aprad;

  return 0;
}

/**
 * Computes atmospheric optical refraction for a source at an astrometric zenith distance
 * (e.g. calculated without accounting for an atmosphere). This is suitable for converting
 * astrometric (unrefracted) zenith angles to observed (refracted) zenith angles. See
 * refract() for the reverse correction.
 *
 * The returned value is the approximate refraction for optical wavelengths. This function
 * can be used for planning observations or telescope pointing, but should not be used for
 * precise positioning.
 *
 * REFERENCES:
 * <ol>
 * <li>Explanatory Supplement to the Astronomical Almanac, p. 144.</li>
 * <li>Bennett, G. (1982), Journal of Navigation (Royal Institute) 35, pp. 255-259.</li>
 * </ol>
 *
 * @param location      Pointer to structure containing observer's location. It may also
 *                      contains weather data (optional) for the observer's location.
 * @param option        NOVAS_STANDARD_ATMOSPHERE (1), or NOVAS_WEATHER_AT_LOCATION (2) if
 *                      to use the weather values contained in the 'location' data structure.
 * @param zd_astro      [deg] Astrometric (unrefracted) zenith distance angle of the source.
 * @return              [deg] the calculated optical refraction. (to ~0.1 arcsec accuracy),
 *                      or 0.0 if the location is NULL or the option is invalid.
 *
 * @sa refract()
 * @sa itrs_to_hor()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
double refract_astro(const on_surface *location, enum novas_refraction_model option, double zd_astro) {
  double refr = 0.0;
  int i;

  for(i = 0; i < novas_inv_max_iter; i++) {
    double zd_obs = zd_astro - refr;
    refr = refract(location, option, zd_obs);
    if(fabs(refr - (zd_astro - zd_obs)) < 3.0e-5)
      return refr;
  }

  novas_set_errno(ECANCELED, "refract_astro", "failed to converge");
  return NAN;
}

/**
 * Computes atmospheric optical refraction for an observed (already refracted!) zenith
 * distance through the atmosphere. In other words this is suitable to convert refracted
 * zenith angles to astrometric (unrefracted) zenith angles. For the reverse, see
 * refract_astro().
 *
 * The returned value is the approximate refraction for optical wavelengths. This function
 * can be used for planning observations or telescope pointing, but should not be used for
 * precise positioning.
 *
 * NOTES:
 * <ol>
 * <li>The standard temeperature model includes a very rough estimate of the mean annual
 * temeprature for the ovserver's latitude and elevation, rather than the 10 C everywhere
 * assumption in NOVAS C 3.1.<.li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Explanatory Supplement to the Astronomical Almanac, p. 144.</li>
 * <li>Bennett, G. (1982), Journal of Navigation (Royal Institute) 35, pp. 255-259.</li>
 * </ol>
 *
 * @param location      Pointer to structure containing observer's location. It may also
 *                      contains weather data (optional) for the observer's location.
 * @param option        NOVAS_STANDARD_ATMOSPHERE (1), or NOVAS_WEATHER_AT_LOCATION (2) if
 *                      to use the weather values contained in the 'location' data structure.
 * @param zd_obs        [deg] Observed (already refracted!) zenith distance through the
 *                      atmosphere.
 * @return              [deg] the calculated optical refraction or 0.0 if the location is
 *                      NULL or the option is invalid or the 'zd_obs' is invalid (&lt;90&deg;).
 *
 * @sa refract_astro()
 * @sa hor_to_itrs()
 */
double refract(const on_surface *location, enum novas_refraction_model option, double zd_obs) {
  static const char *fn = "refract";

  // 's' is the approximate scale height of atmosphere in meters.
  const double s = 9.1e3;
  const double ct = 0.065;  // [C/m] averate temperature drop with altitude
  double p, t, h, r;

  if(option == NOVAS_NO_ATMOSPHERE)
    return 0.0;

  if(!location) {
    novas_set_errno(EINVAL, fn, "NULL observer location");
    return 0.0;
  }

  if(option != NOVAS_STANDARD_ATMOSPHERE && option != NOVAS_WEATHER_AT_LOCATION) {
    novas_set_errno(EINVAL, fn, "invalid refraction model option: %d", option);
    return 0.0;
  }

  zd_obs = fabs(zd_obs);

  // Compute refraction up to zenith distance 91 degrees.
  if(zd_obs > 91.0)
    return 0.0;

  // If observed weather data are available, use them.  Otherwise, use
  // crude estimates of average conditions.
  if(option == NOVAS_WEATHER_AT_LOCATION) {
    p = location->pressure;
    t = location->temperature;
  }
  else {
    p = 1010.0 * exp(-location->height / s);
    // AK: A very rough model of mean annual temperatures vs latitude
    t = 30.0 - 30.0 * sin(location->latitude * DEGREE);
    // AK: Estimated temperature drop due to elevation.
    t -= location->height * ct;
  }

  h = 90.0 - zd_obs;
  r = 0.016667 / tan((h + 7.31 / (h + 4.4)) * DEGREE);
  return r * (0.28 * p / (t + 273.0));
}

/**
 * Returns the Julian date for a given Gregorian calendar date. This function makes no checks
 * for a valid input calendar date. Input calendar date must be Gregorian. Input time value
 * can be based on any UT-like time scale (UTC, UT1, TT, etc.) - output Julian date will have
 * the same basis.
 *
 * REFERENCES:
 * <ol>
 *  <li>Fliegel, H. & Van Flandern, T.  Comm. of the ACM, Vol. 11, No. 10, October 1968, p.
 *  657.</li>
 * </ol>
 *
 * @param year    [yr] Gregorian calendar year
 * @param month   [month] Gregorian calendar month [1:12]
 * @param day     [day] Day of month [1:31]
 * @param hour    [hr] Hour of day [0:24]
 * @return        [day] the fractional Julian date for the input calendar date
 *
 * @sa cal_date()
 * @sa get_utc_to_tt()
 * @sa get_ut1_to_tt()
 * @sa tt2tdb()
 *
 */
double julian_date(short year, short month, short day, double hour) {
  long jd12h = day - 32075L + 1461L * (year + 4800L + (month - 14L) / 12L) / 4L + 367L * (month - 2L - (month - 14L) / 12L * 12L) / 12L
          - 3L * ((year + 4900L + (month - 14L) / 12L) / 100L) / 4L;
  return jd12h - 0.5 + hour / DAY_HOURS;
}

/**
 * This function will compute a broken down date on the Gregorian calendar for given the
 * Julian date input. Input Julian date can be based on any UT-like time scale (UTC, UT1,
 * TT, etc.) - output time value will have same basis.
 *
 * REFERENCES:
 * <ol>
 *  <li>Fliegel, H. & Van Flandern, T.  Comm. of the ACM, Vol. 11, No. 10, October 1968,
 *  p. 657.</li>
 * </ol>
 *
 * @param tjd          [day] Julian date
 * @param[out] year    [yr] Gregorian calendar year. It may be NULL if not required.
 * @param[out] month   [month] Gregorian calendat month [1:12]. It may be NULL if not
 *                     required.
 * @param[out] day     [day] Day of the month [1:31]. It may be NULL if not required.
 * @param[out] hour    [h] Hour of day [0:24]. It may be NULL if not required.
 *
 * @return              0
 *
 * @sa julian_date()
 * @sa get_utc_to_tt()
 * @sa get_ut1_to_tt()
 * @sa tt2tdb()
 */
int cal_date(double tjd, short *year, short *month, short *day, double *hour) {
  long jd, k, m, n;
  short y, mo, d;
  double djd, h;

  djd = tjd + 0.5;
  jd = (long) floor(djd);

  h = remainder(djd, 1.0) * DAY_HOURS;
  if(h < 0)
    h += 24;

  k = jd + 68569L;
  n = 4L * k / 146097L;

  k = k - (146097L * n + 3L) / 4L;
  m = 4000L * (k + 1L) / 1461001L;
  k = k - 1461L * m / 4L + 31L;

  mo = (short) (80L * k / 2447L);
  d = (short) (k - 2447L * (long) mo / 80L);
  k = mo / 11L;

  mo = (short) ((long) mo + 2L - 12L * k);
  y = (short) (100L * (n - 49L) + m + k);

  if(year)
    *year = y;
  if(month)
    *month = mo;
  if(day)
    *day = d;
  if(hour)
    *hour = h;

  return 0;
}

/**
 * Returns the normalized angle in the [0:2&pi;) range.
 *
 * @param angle   [rad] an angle in radians.
 * @return        [rad] the normalized angle in the [0:2&pi;) range.
 *
 * @since 1.0
 * @author Attila Kovacs
 */
double norm_ang(double angle) {
  double a;

  a = remainder(angle, TWOPI);
  if(a < 0.0)
    a += TWOPI;

  return a;
}

/**
 * Populates the data structure for a 'catalog' source, such as a star.
 *
 * @param star_name   Object name (less than SIZE_OF_OBJ_NAME in length). It may be NULL
 *                    if not relevant.
 * @param catalog     Catalog identifier (less than SIZE_OF_CAT_NAME in length). E.g.
 *                    'HIP' = Hipparcos, 'TY2' = Tycho-2. It may be NULL if not relevant.
 * @param cat_num     Object number in the catalog.
 * @param ra          [h] Right ascension of the object (hours).
 * @param dec         [deg] Declination of the object (degrees).
 * @param pm_ra       [mas/yr] Proper motion in right ascension (milliarcseconds/year).
 * @param pm_dec      [mas/yr] Proper motion in declination (milliarcseconds/year).
 * @param parallax    [mas] Parallax (milliarcseconds).
 * @param rad_vel     [km/s] Radial velocity (LSR)
 * @param[out] star   Pointer to data structure to populate.
 * @return            0 if successful, or -1 if the output argument is NULL, 1 if the
 *                    'star_name' is too long or 2 if the 'catalog' name is too long.
 *
 * @sa make_object()
 * @sa transform_cat()
 */
short make_cat_entry(const char *star_name, const char *catalog, long cat_num, double ra, double dec, double pm_ra, double pm_dec,
        double parallax, double rad_vel, cat_entry *star) {
  static const char *fn = "make_cat_entry";

  if(!star)
    return novas_error(-1, EINVAL, fn, "NULL input 'star'");

  memset(star, 0, sizeof(*star));

  // Set up the 'star' structure.
  if(star_name) {
    if(strlen(star_name) >= sizeof(star->starname))
      return novas_error(1, EINVAL, fn, "Input star_name is too long (%d > %d)", (int) strlen(star_name), (int) sizeof(star->starname) - 1);
    strncpy(star->starname, star_name, sizeof(star->starname) - 1);
  }

  if(catalog) {
    if(strlen(catalog) >= sizeof(star->catalog))
      return novas_error(2, EINVAL, fn, "Input cataog ID is too long (%d > %d)", (int) strlen(catalog), (int) sizeof(star->catalog) - 1);
    strncpy(star->catalog, catalog, sizeof(star->catalog) - 1);
  }

  star->starnumber = cat_num;
  star->ra = ra;
  star->dec = dec;
  star->promora = pm_ra;
  star->promodec = pm_dec;
  star->parallax = parallax;
  star->radialvelocity = rad_vel;

  return 0;
}

/**
 * Enables or disables case-sensitive processing of the object name. The effect is not
 * retroactive. The setting will only affect the celestial objects that are defined after
 * the call. Note, that catalog names, set via make_cat_entry() are always case sensitive
 * regardless of this setting.
 *
 * @param value   (boolean) TRUE (non-zero) to enable case-sensitive object names, or else
 *                FALSE (0) to convert names to upper case only (NOVAS C compatible
 *                behavior).
 *
 * @sa make_object()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
void novas_case_sensitive(int value) {
  is_case_sensitive = (value != 0);
}

/**
 * Populates an object data structure using the parameters provided. By default (for
 * compatibility with NOVAS C) source names are converted to upper-case internally. You can
 * however enable case-sensitive processing by calling novas_case_sensitive() before.
 *
 * NOTES:
 * <ol>
 * <li>This call does not initialize the `orbit` field (added in v1.2) with zeroes to remain ABI
 * compatible with versions &lt;1.2, and to avoid the possiblity of segfaulting if used to
 * initialize a legacy `object` variable.</li>
 * </ol>
 *
 * @param type          The type of object. NOVAS_PLANET (0), NOVAS_EPHEM_OBJECT (1) or
 *                      NOVAS_CATALOG_OBJECT (2), or NOVAS_ORBITAL_OBJECT (3).
 * @param number        The novas ID number (for solar-system bodies only, otherwise ignored)
 * @param name          The name of the object (case insensitive). It should be shorter than
 *                      SIZE_OF_OBJ_NAME or else an error will be returned. The name is
 *                      converted to upper internally unless novas_case_sensitive() was called
 *                      before to change that.
 * @param star          Pointer to structure to populate with the catalog data for a celestial
 *                      object located outside the solar system. Used only if type is
 *                      NOVAS_CATALOG_OBJECT, otherwise ignored and can be NULL.
 * @param[out] source   Pointer to the celestial object data structure to be populated.
 * @return              0 if successful, or -1 if 'cel_obj' is NULL or when type is
 *                      NOVAS_CATALOG_OBJECT and 'star' is NULL, or else 1 if 'type' is
 *                      invalid, 2 if 'number' is out of legal range or 5 if 'name' is too long.
 *
 * @sa novas_case_sensitive()
 * @sa make_cat_object()
 * @sa make_redshifted_object()
 * @sa make_planet()
 * @sa make_ephem_object()
 * @sa make_orbital_object()
 * @sa novas_geom_posvel()
 * @sa place()
 *
 */
short make_object(enum novas_object_type type, long number, const char *name, const cat_entry *star, object *source) {
  static const char *fn = "make_object";

  if(!source)
    return novas_error(-1, EINVAL, fn, "NULL input source");

  // FIXME for version v2.x initialize the entire structure again...
  memset(source, 0, offsetof(object, orbit));

  // Set the object type.
  if(type < 0 || type >= NOVAS_OBJECT_TYPES)
    return novas_error(1, EINVAL, fn, "invalid input 'type': %d", type);

  source->type = type;

  // Set the object number.
  if(type == NOVAS_PLANET)
    if(number < 0 || number >= NOVAS_PLANETS)
      return novas_error(2, EINVAL, fn, "planet number %ld is out of bounds [0:%d]", number, NOVAS_PLANETS - 1);

  // FIXME will not need special case in v2.x
  if(type == NOVAS_ORBITAL_OBJECT)
    memset(&source->orbit, 0, sizeof(source->orbit));

  source->number = number;

  if(name) {
    int i;

    for(i = 0; name[i]; i++) {
      // if input name is not null-terminated return error;
      if(i == (sizeof(source->name) - 1))
        return novas_error(5, EINVAL, fn, "unterminated source name");

      source->name[i] = is_case_sensitive ? name[i] : toupper(name[i]);
    }
  }

  // Populate the astrometric-data structure if the object is 'type' = 2.
  if(type == NOVAS_CATALOG_OBJECT) {
    if(!star)
      return novas_error(-1, EINVAL, fn, "NULL input 'star'");

    source->star = *star;
  }

  return 0;
}

/**
 * Sets a celestial object to be a major planet, or the Sun, Moon, Solar-system Barycenter, etc.
 *
 * @param num           Planet ID number (NOVAS convention)
 * @param[out] planet   Pointer to structure to populate.
 * @return              0 if successful, or else -1 if the 'planet' pointer is NULL.
 *
 * @sa make_ephem_object()
 * @sa make_cat_entry()
 * @sa place()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int make_planet(enum novas_planet num, object *planet) {
  static const char *fn = "make_planet";
  const char *names[] = NOVAS_PLANET_NAMES_INIT;

  if(num < 0 || num >= NOVAS_PLANETS)
    return novas_error(-1, EINVAL, fn, "planet number %d is out of bounds [%d:%d]", num, 0, NOVAS_PLANETS - 1);

  prop_error(fn, (make_object(NOVAS_PLANET, num, names[num], NULL, planet) ? -1 : 0), 0);
  return 0;
}

/**
 * Populates an 'observer' data structure given the parameters. The output data structure may
 * be used an the the inputs to NOVAS-C function 'place()'.
 *
 * @param where         The location type of the observer
 * @param loc_surface   Pointer to data structure that defines a location on Earth's surface.
 *                      Used only if 'where' is NOVAS_OBSERVER_ON_EARTH, otherwise can be
 *                      NULL.
 * @param loc_space     Pointer to data structure that defines a near-Earth location in space.
 *                      Used only if 'where' is NOVAS_OBSERVER_IN_EARTH_ORBIT, otherwise can
 *                      be NULL.
 * @param[out] obs      Pointer to observer data structure to populate.
 * @return              0 if successful, -1 if a required argument is NULL, or 1 if the 'where'
 *                      argument is invalid.
 *
 * @sa make_observer_at_geocenter()
 * @sa make_observer_on_surface()
 * @sa make_observer_in_space()
 * @sa make_solar_system_observer()
 */
short make_observer(enum novas_observer_place where, const on_surface *loc_surface, const in_space *loc_space, observer *obs) {
  static const char *fn = "make_observer";

  if(!obs)
    return novas_error(-1, EINVAL, fn, "NULL output observer pointer");

  // Initialize the output structure.
  memset(obs, 0, sizeof(*obs));
  obs->where = where;

  // Populate the output structure based on the value of 'where'.
  switch(where) {
    case NOVAS_OBSERVER_AT_GEOCENTER:
      break;

    case NOVAS_AIRBORNE_OBSERVER:
      if(!loc_space)
        return novas_error(-1, EINVAL, fn, "NULL in space location (for velocity)");

      memcpy(&obs->near_earth.sc_vel, loc_space->sc_vel, sizeof(loc_space->sc_vel)); // @suppress("No break at end of case")
      /* fallthrough */

    case NOVAS_OBSERVER_ON_EARTH:
      if(!loc_surface)
        return novas_error(-1, EINVAL, fn, "NULL on surface location");

      memcpy(&obs->on_surf, loc_surface, sizeof(obs->on_surf));
      break;

    case NOVAS_OBSERVER_IN_EARTH_ORBIT:
    case NOVAS_SOLAR_SYSTEM_OBSERVER:
      if(!loc_space)
        return novas_error(-1, EINVAL, fn, "NULL in space location");

      memcpy(&obs->near_earth, loc_space, sizeof(obs->near_earth));
      break;

    default:
      return novas_error(1, EINVAL, fn, "Invalid observer location type (where): %d", where);
  }

  return 0;
}

/**
 * Populates an 'observer' data structure for a hypothetical observer located at Earth's
 * geocenter. The output data structure may be used an the the inputs to NOVAS-C function
 * 'place()'.
 *
 * @param[out] obs    Pointer to data structure to populate.
 * @return          0 if successful, or -1 if the output argument is NULL.
 *
 * @sa make_observer_in_space()
 * @sa make_observer_on_surface()
 * @sa place()
 */
int make_observer_at_geocenter(observer *obs) {
  prop_error("make_observer_at_geocenter", make_observer(NOVAS_OBSERVER_AT_GEOCENTER, NULL, NULL, obs), 0);
  return 0;
}

/**
 * Populates and 'on_surface' data structure with the specified location defining parameters
 * of the observer. The output data structure may be used an the the inputs to NOVAS-C
 * function 'place()'.
 *
 * @param latitude      [deg] Geodetic (ITRS) latitude in degrees; north positive.
 * @param longitude     [deg] Geodetic (ITRS) longitude in degrees; east positive.
 * @param height        [m] Altitude over se level of the observer (meters).
 * @param temperature   [C] Temperature (degrees Celsius).
 * @param pressure      [mbar] Atmospheric pressure (millibars).
 * @param[out] obs      Pointer to the data structure to populate.
 *
 * @return          0 if successful, or -1 if the output argument is NULL.
 *
 * @sa make_observer_in_space()
 * @sa make_observer_at_geocenter()
 * @sa place()
 */
int make_observer_on_surface(double latitude, double longitude, double height, double temperature, double pressure, observer *obs) {
  static const char *fn = "make_observer_on_surface";
  on_surface loc;
  prop_error(fn, make_on_surface(latitude, longitude, height, temperature, pressure, &loc), 0);
  prop_error(fn, make_observer(NOVAS_OBSERVER_ON_EARTH, &loc, NULL, obs), 0);
  return 0;
}

/**
 * Populates an 'observer' data structure, for an observer situated on a near-Earth spacecraft,
 * with the specified geocentric position and velocity vectors. Both input vectors are with
 * respect to true equator and equinox of date. The output data structure may be used an the
 * the inputs to NOVAS-C function 'place()'.
 *
 * @param sc_pos        [km] Geocentric (x, y, z) position vector in km.
 * @param sc_vel        [km/s] Geocentric (x, y, z) velocity vector in km/s.
 * @param[out] obs      Pointer to the data structure to populate
 * @return          0 if successful, or -1 if the output argument is NULL.
 *
 * @sa make_observer_on_surface()
 * @sa make_observer_at_geocenter()
 * @sa place()
 */
int make_observer_in_space(const double *sc_pos, const double *sc_vel, observer *obs) {
  static const char *fn = "make_observer_in_space";
  in_space loc;
  prop_error(fn, make_in_space(sc_pos, sc_vel, &loc), 0);
  prop_error(fn, make_observer(NOVAS_OBSERVER_IN_EARTH_ORBIT, NULL, &loc, obs), 0);
  return 0;
}

/**
 * Populates an 'on_surface' data structure, for an observer on the surface of the Earth, with
 * the given parameters.
 *
 * Note, that because this is an original NOVAS C routine, it does not have an argument to set
 * a humidity value (e.g. for radio refraction). As such, the humidity value remains undefined
 * after this call. To set the humidity, set the output structure's field after calling this
 * funcion. Its unit is [%], and so the range is 0.0--100.0.
 *
 * @param latitude      [deg] Geodetic (ITRS) latitude in degrees; north positive.
 * @param longitude     [deg] Geodetic (ITRS) longitude in degrees; east positive.
 * @param height        [m] Altitude over se level of the observer (meters).
 * @param temperature   [C] Temperature (degrees Celsius).
 * @param pressure      [mbar] Atmospheric pressure (millibars).
 * @param[out] loc      Pointer to Earth location data structure to populate.
 *
 * @return          0 if successful, or -1 if the output argument is NULL.
 *
 * @sa make_observer_on_surface()
 * @sa make_in_space()
 * @sa ON_SURFACE_INIT
 * @sa ON_SURFACE_LOC
 */
int make_on_surface(double latitude, double longitude, double height, double temperature, double pressure, on_surface *loc) {
  if(!loc)
    return novas_error(-1, EINVAL, "make_on_surface", "NULL output location pointer");

  loc->latitude = latitude;
  loc->longitude = longitude;
  loc->height = height;
  loc->temperature = temperature;
  loc->pressure = pressure;

  // FIXME starting v2.0 set humidity to 0.0
  //loc->humidity = 0.0;

  return 0;
}

/**
 * Populates an 'in_space' data structure, for an observer situated on a near-Earth spacecraft,
 * with the provided position and velocity components. Both input vectors are assumed with
 * respect to true equator and equinox of date.
 *
 * @param sc_pos    [km] Geocentric (x, y, z) position vector in km. NULL defaults to the
 *                  origin
 * @param sc_vel    [km/s] Geocentric (x, y, z) velocity vector in km/s. NULL defaults to
 *                  zero speed.
 * @param[out] loc  Pointer to earth-orbit location data structure to populate.
 * @return          0 if successful, or -1 if the output argument is NULL.
 *
 * @sa make_observer_in_space()
 * @sa make_on_surface()
 * @sa IN_SPACE_INIT
 */
int make_in_space(const double *sc_pos, const double *sc_vel, in_space *loc) {
  if(!loc)
    return novas_error(-1, EINVAL, "make_in_space", "NULL output location pointer");

  if(sc_pos)
    memcpy(loc->sc_pos, sc_pos, sizeof(loc->sc_pos));
  else
    memset(loc->sc_pos, 0, sizeof(loc->sc_pos));

  if(sc_vel)
    memcpy(loc->sc_vel, sc_vel, sizeof(loc->sc_vel));
  else
    memset(loc->sc_vel, 0, sizeof(loc->sc_vel));

  return 0;
}


