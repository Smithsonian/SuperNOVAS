/**
 * @file
 *
 * @date Created  on Mar 6, 2025
 * @author G. Kaplan and Attila Kovacs
 *
 *  Various finctions relating to Earth position and orientation
 */

#include <string.h>
#include <errno.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"
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
int polar_dxdy_to_dpsideps(double jd_tt, double dx, double dy, double *restrict dpsi, double *restrict deps) {
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

/// ==========================================================================

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
int terra(const on_surface *restrict location, double lst, double *restrict pos, double *restrict vel) {
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
int e_tilt(double jd_tdb, enum novas_accuracy accuracy, double *restrict mobl, double *restrict tobl,
        double *restrict ee, double *restrict dpsi, double *restrict deps) {
  static THREAD_LOCAL enum novas_accuracy acc_last = -1;
  static THREAD_LOCAL double jd_last = NAN;
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
        enum novas_earth_rotation_measure erot, enum novas_accuracy accuracy, double *restrict gst) {
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
    static THREAD_LOCAL double jd_last = NAN;
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
 * Corrects a vector in the ITRS (rotating Earth-fixed system) for polar motion, and also
 * corrects the longitude origin (by a tiny amount) to the Terrestrial Intermediate Origin
 * (TIO).  The ITRS vector is thereby transformed to the terrestrial intermediate reference
 * system (TIRS) or Pseudo Earth Fixed (PEF), based on the true (rotational) equator and TIO;
 * or vice versa.  Because the true equator is the plane orthogonal to the direction of the
 * Celestial Intermediate Pole (CIP), the components of the output vector are referred to z
 * and x axes toward the CIP and TIO, respectively.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 * <li>Lambert & Bizouard (2002), Astronomy and Astrophysics 394, 317-321.</li>
 * </ol>
 *
 * @param jd_tt         [day] Terrestrial Time (TT) based Julian date.
 * @param direction     WOBBLE_ITRS_TO_PEF (0) or WOBBLE_PEF_TO_ITRS (nonzero)
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
short geo_posvel(double jd_tt, double ut1_to_tt, enum novas_accuracy accuracy, const observer *restrict obs,
        double *restrict pos, double *restrict vel) {
  static const char *fn = "geo_posvel";
  static THREAD_LOCAL double t_last = NAN;
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
int limb_angle(const double *pos_src, const double *pos_obs, double *restrict limb_ang, double *restrict nadir_ang) {
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
