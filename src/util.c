/**
 * @file
 *
 * @date Created  on Aug 24, 2024
 * @author Attila Kovacs
 *
 *  Miscellaneous utility function, which are not called in novas.c and hence do not need to
 *  bloat the already big source there.
 */

#include <math.h>
#include <errno.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"
/// \endcond


/**
 * Converts a radial recession velocity to a redshift value (z = &delta;f / f<sub>rest</sub>).
 * It is based on the relativistic formula:
 * <pre>
 *  1 + z = sqrt((1 + &beta;) / (1 - &beta;))
 * </pre>
 * where &beta; = v / c.
 *
 * @param vel   [km/s] velocity (i.e. rate) of recession.
 * @return      the corresponding redshift value (&delta;&lambda; / &lambda;<sub>rest</sub>), or NAN if
 *              the input velocity is invalid (i.e., it exceeds the speed of light).
 *
 * @sa novas_z2v()
 * @sa novas_z_add()
 *
 * @author Attila Kovacs
 * @since 1.2
 */
double novas_v2z(double vel) {
  vel *= 1e3 / C;   // [km/s] -> beta
  if(fabs(vel) > 1.0) {
    novas_error(-1, EINVAL, "novas_v2z", "velocity exceeds speed of light v=%g km/s", 1e-3 * vel * C);
    return NAN;
  }
  return sqrt((1.0 + vel) / (1.0 - vel)) - 1.0;
}


/**
 * Returns the gravitational redshift (_z_) for light emitted near a massive spherical body
 * at some distance from its center, and observed at some very large (infinite) distance away.
 *
 * @param M_kg    [kg] Mass of gravitating body that is contained inside the emitting radius.
 * @param r_m     [m] Radius at which light is emitted.
 * @return        The gravitational redshift (_z_) for an observer at very large  (infinite)
 *                distance from the gravitating body.
 *
 * @sa redshift_vrad()
 * @sa unredshift_vrad()
 * @sa novas_z_add()
 *
 * @since 1.2
 * @author Attila Kovacs
 */
double grav_redshift(double M_kg, double r_m) {
  static const double G = 6.6743e-11; // G in SI units.
  static const double c2 = C * C;

  const double rs = 2 * G * M_kg / c2;

  return 1.0 / sqrt(1.0 - rs / r_m) - 1.0;
}

/**
 * Applies an incremental redshift correction to a radial velocity. For example, you may
 * use this function to correct a radial velocity calculated by `rad_vel()` or `rad_vel2()`
 * for a Solar-system body to account for the gravitational redshift for light originating
 * at a specific distance away from the body. For the Sun, you may want to undo the redshift
 * correction applied for the photosphere using `unredshift_vrad()` first.
 *
 * @param vrad    [km/s] Radial velocity
 * @param z       Redshift correction to apply
 * @return        [km/s] The redshift corrected radial velocity or NAN if the redshift value
 *                is invalid (errno will be set to EINVAL).
 *
 * @sa unredshift_vrad()
 * @sa grav_redshift()
 * @sa novas_z_add()
 *
 * @since 1.2
 * @author Attila Kovacs
 */
double redshift_vrad(double vrad, double z) {
  static const char *fn = "redshift_vrad";
  double z0;
  if(z <= -1.0) {
    novas_error(-1, EINVAL, fn, "invalid redshift value: z=%g", z);
    return NAN;
  }
  z0 = novas_v2z(vrad);
  if(isnan(z0)) novas_trace(fn, -1, 0);
  return novas_z2v((1.0 + z0) * (1.0 + z) - 1.0);
}

/**
 * Undoes an incremental redshift correction that was applied to radial velocity.
 *
 * @param vrad    [km/s] Radial velocity
 * @param z       Redshift correction to apply
 * @return        [km/s] The radial velocity without the redshift correction or NAN if the
 *                redshift value is invalid. (errno will be set to EINVAL)
 *
 * @sa redshift_vrad()
 * @sa grav_redshift()
 *
 * @since 1.2
 * @author Attila Kovacs
 */
double unredshift_vrad(double vrad, double z) {
  static const char *fn = "unredshift_vrad";
  double z0;
  if(z <= -1.0) {
    novas_error(-1, EINVAL, fn, "invalid redshift value: z=%g", z);
    return NAN;
  }
  z0 = novas_v2z(vrad);
  if(isnan(z0)) novas_trace(fn, -1, 0);
  return novas_z2v((1.0 + z0) / (1.0 + z) - 1.0);
}

/**
 * Compounds two redshift corrections, e.g. to apply (or undo) a series gravitational redshift
 * corrections and/or corrections for a moving observer. It's effectively using
 * (1 + z) = (1 + z1) * (1 + z2).
 *
 * @param z1    One of the redshift values
 * @param z2    The other redshift value
 * @return      The compound redshift value, ot NAN if either input redshift is invalid (errno
 *              will be set to EINVAL).
 *
 * @sa grav_redshift()
 * @sa redshift_vrad()
 * @sa unredshift_vrad()
 *
 * @since 1.2
 * @author Attila Kovacs
 */
double novas_z_add(double z1, double z2) {
  if(z1 <= -1.0 || z2 <= -1.0) {
    novas_error(-1, EINVAL, "novas_z_add", "invalid redshift value: z1=%g, z2=%g", z1, z2);
    return NAN;
  }
  return z1 + z2 + z1 * z2;
}

/**
 * Returns the inverse of a redshift value, that is the redshift for a body moving with the same
 * velocity as the original but in the opposite direction.
 *
 * @param z     A redhift value
 * @return      The redshift value for a body moving in the opposite direction with the
 *              same speed, or NAN if the input redshift is invalid.
 *
 * @sa novas_z_add()
 *
 * @since 1.2
 * @author Attila Kovacs
 */
double novas_z_inv(double z) {
  if(z <= -1.0) {
    novas_error(-1, EINVAL, "novas_z_inv", "invalid redshift value: z=%g", z);
    return NAN;
  }
  return 1.0 / (1.0 + z) - 1.0;
}
