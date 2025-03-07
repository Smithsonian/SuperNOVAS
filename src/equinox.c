/**
 * @file
 *
 * @date Created  on Mar 6, 2025
 * @author G. Kaplan and Attila Kovacs
 *
 *  Various function for calculating the equator and equinox of date, and related quatities.
 */

#include <string.h>
#include <errno.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"
/// \endcond


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
  static THREAD_LOCAL enum novas_accuracy acc_last = -1;
  static THREAD_LOCAL double t_last = NAN, last_ra;

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
int fund_args(double t, novas_delaunay_args *restrict a) {
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




