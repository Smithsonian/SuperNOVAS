/**
 * @file
 *
 * @date Created  on Mar 5, 2025
 * @author Attila Kovacs
 *
 *  Various spectral / velocity related extensions to novas.c
 */

#include <math.h>
#include <errno.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"
/// \endcond

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
  static const double twoGoverC2 = 2.0 * 6.6743e-11 / (C * C); // 2G/c^2 in SI units.
  return 1.0 / sqrt(1.0 - twoGoverC2 * M_kg / r_m) - 1.0;
}


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
  vel *= NOVAS_KMS / C;   // [km/s] -> beta
  if(fabs(vel) > 1.0) {
    novas_error(-1, EINVAL, "novas_v2z", "velocity exceeds speed of light v=%g km/s", vel * C / NOVAS_KMS);
    return NAN;
  }
  return sqrt((1.0 + vel) / (1.0 - vel)) - 1.0;
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


static int convert_lsr_ssb_vel(const double *vLSR, int sign, double *vSSB) {
  static const double betaSSB[3] = { 11.1 * NOVAS_KMS / C, 12.24 * NOVAS_KMS / C, 7.25 * NOVAS_KMS / C };
  int i;

  for(i = 3; --i >= 0; )
    vSSB[i] = novas_add_beta(vLSR[i] * NOVAS_KMS / C, sign * betaSSB[i]) * C / NOVAS_KMS;

  return 0;
}


/**
 * Returns a Solar System Baricentric (SSB) radial velocity for a radial velocity that is referenced to the
 * Local Standard of Rest (LSR). Internally, NOVAS always uses barycentric radial velocities, but it
 * is just as common to have catalogs define radial velocities referenced to the LSR.
 *
 * The SSB motion w.r.t. the barycenter is assumed to be (11.1, 12.24, 7.25) km/s in ICRS (Shoenrich et al.
 * 2010).
 *
 * REFERENCES:
 * <ol>
 * <li>Ralph Schoenrich, James Binney, Walter Dehnen, Monthly Notices of the Royal Astronomical Society,
 * Volume 403, Issue 4, April 2010, Pages 1829–1833, https://doi.org/10.1111/j.1365-2966.2010.16253.x</li>
 * </ol>
 *
 * @param epoch     [yr] Coordinate epoch in which the coordinates and velocities are defined. E.g. 2000.0.
 * @param ra        [h] Right-ascenscion of source at given epoch.
 * @param dec       [deg] Declination of source at given epoch.
 * @param vLSR      [km/s] radial velocity defined against the Local Standard of Rest (LSR), at given epoch.
 *
 * @return          [km/s] Equivalent Solar-System Barycentric radial velocity.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa make_cat_entry()
 * @sa novas_ssb_to_lsr_vel()
 */
double novas_lsr_to_ssb_vel(double epoch, double ra, double dec, double vLSR) {
  double u[3] = {}, v[3];
  double jd = NOVAS_JD_J2000 + 365.25 * (epoch - 2000.0);
  int i;

  radec2vector(ra, dec, 1.0, u);
  for(i = 3; --i >= 0; )
    v[i] = vLSR * u[i];

  precession(jd, v, NOVAS_JD_J2000, v);
  convert_lsr_ssb_vel(v, -1, v);
  precession(NOVAS_JD_J2000, v, jd, v);

  return novas_vdot(u, v);
}


/**
 * Returns a radial-velocity referenced to the Local Standard of Rest (LSR) for a given Solar-System
 * Barycentric (SSB) radial velocity. Internally, NOVAS always uses barycentric radial velocities, but it
 * is just as common to have catalogs define radial velocities referenced to the LSR.
 *
 * The SSB motion w.r.t. the barycenter is assumed to be (11.1, 12.24, 7.25) km/s in ICRS (Shoenrich et al.
 * 2010).
 *
 * REFERENCES:
 * <ol>
 * <li>Ralph Schoenrich, James Binney, Walter Dehnen, Monthly Notices of the Royal Astronomical Society,
 * Volume 403, Issue 4, April 2010, Pages 1829–1833, https://doi.org/10.1111/j.1365-2966.2010.16253.x</li>
 * </ol>
 *
 * @param epoch     [yr] Coordinate epoch in which the coordinates and velocities are defined. E.g. 2000.0.
 * @param ra        [h] Right-ascenscion of source at given epoch.
 * @param dec       [deg] Declination of source at given epoch.
 * @param vLSR      [km/s] radial velocity defined against the Local Standard of Rest (LSR), at given epoch.
 *
 * @return          [km/s] Equivalent Solar-System Barycentric radial velocity.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa make_cat_entry()
 * @sa novas_lsr_to_ssb_vel()
 */
double novas_ssb_to_lsr_vel(double epoch, double ra, double dec, double vLSR) {
  double u[3] = {}, v[3];
  double jd = NOVAS_JD_J2000 + 365.25 * (epoch - 2000.0);
  int i;

  radec2vector(ra, dec, 1.0, u);
  for(i = 3; --i >= 0;)
    v[i] = vLSR * u[i];

  precession(jd, v, NOVAS_JD_J2000, v);
  convert_lsr_ssb_vel(v, 1, v);
  precession(NOVAS_JD_J2000, v, jd, v);

  return novas_vdot(u, v);
}
