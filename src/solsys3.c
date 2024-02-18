/**
 * @file
 *
 * @author G. Kaplan and A. Kovacs
 *
 *  SuperNOVAS plane calculator functions for the Earth and Sun only, with an orbital model
 *  based on the DE405 ephemerides by JPL.
 *
 *  Based on the NOVAS C Edition, Version 3.1,  U. S. Naval Observatory
 *  Astronomical Applications Dept.
 *  Washington, DC
 *  <a href="http://www.usno.navy.mil/USNO/astronomical-applications">http://www.usno.navy.mil/USNO/astronomical-applications</a>
 *
 *  @sa solsys-ephem.c
 */

#include <math.h>
#include <string.h>
#include <errno.h>

#include "novas.h"


/// \cond PRIVATE
#define T0        NOVAS_JD_J2000
/// \endcond

// Additional local function prototypes
void sun_eph(double jd, double *ra, double *dec, double *dis);
void earth_sun_enable_hp(int value);

/**
 * Whether the high-precision call is allowed to return a low-precision result. If set to 0
 * (false) solarsystem_earth_sun_hp() will return with an error code 3 indicating that a
 * high-precision calculation is not possible. Otherise, a non-zero value (true) will let the
 * function to be used without errors, returning the low-precison result of
 * solarsystem_earth_sun() instead.
 *
 * @sa solarsystem_earth_sun_hp()
 */
static int allow_lp_for_hp = 0;

/**
 * Specify whether the high-precision call is allowed to return a low-precision result. If set
 * to 0 (false) solarsystem_earth_sun_hp() will return with an error code 3 indicating that a
 * high-precision calculation is not possible. Otherise, a non-zero value (true) will let the
 * function to be used without errors, returning the low-precison result of
 * solarsystem_earth_sun() instead.
 *
 * @param value   (boolean) A non-zero value enables the error-free use of the earth_sun_calc_hp()
 *                by allowing to return the low-precision result. Otherwise, earth_sun_calc_hp()
 *                will return an error code 3 indicating that the high-precision result is not
 *                available (this latter is the default behavior).
 *
 * @sa earth_sun_calc_hp()
 */
void earth_sun_enable_hp(int value) {
  allow_lp_for_hp = (value != 0);
}

/**
 * Provides the position and velocity of the Earth and Sun only
 * at epoch 'jd_tdb' by evaluating a closed-form theory without reference to an
 * external file.  This function can also provide the position
 * and velocity of the Sun.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. H. "NOVAS: Naval Observatory Vector Astrometry
 *     Subroutines"; USNO internal document dated 20 Oct 1988;
 *     revised 15 Mar 1990.</li>
 * <li>Explanatory Supplement to The Astronomical Almanac (1992).</li>
 * </ol>
 *
 * @param jd_tdb        [day] Barycentric Dynamical Time (TDB) based Julian date
 * @param body          NOVAS_EARTH (3) or NOVAS_SUN (10) only.
 * @param origin        NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1) relative to which to return
 *                      positions and velocities. (For compatibility with existing NOVAS C
 *                      compatible user implementations, we keep the original NOVAS C argument
 *                      type here).
 * @param[out] position [AU] Position vector of 'body' at 'tjd'; equatorial rectangular
 *                      coordinates in AU referred to the mean equator and equinox of J2000.0.
 * @param[out] velocity [AU/day] Velocity vector of 'body' at 'tjd'; equatorial rectangular
 *                      system referred to the mean equator and equinox of J2000.0, in AU/Day.
 * @return              0 if successful, -1 if there is a required function is not provided
 *                      (errno set to ENOSYS)
 *                      or if one of the output pointer arguments is NULL (errno set to EINVAL).
 *                      1 if the input Julian date ('tjd') is out of range, 2 if 'body' is
 *                      invalid.
 *
 * @sa earth_sun_calc_hp()
 * @sa set_planet_provider()
 * @sa solarsystem()
 * @sa novas_planet_provider
 *
 */
short earth_sun_calc(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position, double *velocity) {
  int i;

  // The arrays below contain masses and orbital elements for the four
  // largest planets -- Jupiter, Saturn, Uranus, and Neptune --  (see
  // Explanatory Supplement (1992), p. 316) with angles in radians.  These
  // data are used for barycenter computations only.

  static const float pm[4] = { 1047.349, 3497.898, 22903.0, 19412.2 };
  static const float pa[4] = { 5.203363, 9.537070, 19.191264, 30.068963 };
  static const float pe[4] = { 0.048393, 0.054151, 0.047168, 0.008586 };
  static const float pj[4] = { 0.022782, 0.043362, 0.013437, 0.030878 };
  static const float po[4] = { 1.755036, 1.984702, 1.295556, 2.298977 };
  static const float pw[4] = { 0.257503, 1.613242, 2.983889, 0.784898 };
  static const float pl[4] = { 0.600470, 0.871693, 5.466933, 5.321160 };
  static const float pn[4] = { 1.450138e-3, 5.841727e-4, 2.047497e-4, 1.043891e-4 };

  // 'obl' is the obliquity of ecliptic at epoch J2000.0 in degrees.

  static const double obl = 23.4392794444;
  static double tmass, a[3][4], b[3][4];

  // Initialize constants.
  // Initial value of 'tmass' is mass of Sun plus four inner planets.

  if(!position || !velocity) {
    errno = EINVAL;
    return -1;
  }

  if(!tmass) {
    const double oblr = obl * TWOPI / 360.0;
    const double se = sin(oblr);
    const double ce = cos(oblr);

    tmass = 1.0 + 5.977e-6;

    for(i = 4; --i >= 0; ) {
      // Compute sine and cosine of orbital angles.
      const double si = sin(pj[i]);
      const double ci = cos(pj[i]);
      const double sn = sin(po[i]);
      const double cn = cos(po[i]);
      const double sw = sin(pw[i] - po[i]);
      const double cw = cos(pw[i] - po[i]);

      // Compute p and q vectors (see Brouwer & Clemence (1961), Methods of
      // Celestial Mechanics, pp. 35-36.)
      const double p1 = cw * cn - sw * sn * ci;
      const double p2 = (cw * sn + sw * cn * ci) * ce - sw * si * se;
      const double p3 = (cw * sn + sw * cn * ci) * se + sw * si * ce;
      const double q1 = -sw * cn - cw * sn * ci;
      const double q2 = (-sw * sn + cw * cn * ci) * ce - cw * si * se;
      const double q3 = (-sw * sn + cw * cn * ci) * se + cw * si * ce;
      const double roote = sqrt(1.0 - pe[i] * pe[i]);

      a[0][i] = pa[i] * p1;
      a[1][i] = pa[i] * p2;
      a[2][i] = pa[i] * p3;
      b[0][i] = pa[i] * roote * q1;
      b[1][i] = pa[i] * roote * q2;
      b[2][i] = pa[i] * roote * q3;

      tmass += 1.0 / pm[i];
    }
  }

  // Check if input Julian date is within range (within 3 centuries of J2000).
  if((jd_tdb < 2340000.5) || (jd_tdb > 2560000.5)) {
    errno = EDOM;
    return 1;
  }

  // Form heliocentric coordinates of the Sun or Earth, depending on
  // 'body'.  Velocities are obtained from crude numerical differentiation.
  if((body == 0) || (body == 1) || (body == NOVAS_SUN)) { // Sun
    for(i = 0; i < 3; i++) position[i] = velocity[i] = 0.0;
  }
  else if((body == 2) || (body == NOVAS_EARTH)) { /* Earth */
    double p[3][3];

    for(i = 3; --i >= 0; ) {
      const double qjd = jd_tdb + (double) (i - 1) * 0.1;
      double ras, decs, diss;

      sun_eph(qjd, &ras, &decs, &diss);
      radec2vector(ras, decs, diss, position);
      precession(qjd, position, T0, position);

      p[i][0] = -position[0];
      p[i][1] = -position[1];
      p[i][2] = -position[2];
    }

    for(i = 3; --i >= 0; ) {
      position[i] = p[1][i];
      velocity[i] = (p[2][i] - p[0][i]) / 0.2;
    }
  }
  else {
    errno = EINVAL;
    return 2;
  }

  // If 'origin' = 0, move origin to solar system barycenter.
  // Solar system barycenter coordinates are computed from Keplerian
  // approximations of the coordinates of the four largest planets.
  if(origin == NOVAS_BARYCENTER) {
    static double tlast;
    static double pbary[3], vbary[3];

    if(fabs(jd_tdb - tlast) >= 1.0e-06) {
      memset(pbary, 0, sizeof(pbary));
      memset(vbary, 0, sizeof(vbary));

      // The following loop cycles once for each of the four planets.
      for(i = 4; --i >= 0; ) {
        // Compute mass factor and add in to total displacement.
        const double f = 1.0 / (pm[i] * tmass);

        // Compute mean longitude, mean anomaly, and eccentric anomaly.
        const double e = pe[i];
        const double mlon = pl[i] + pn[i] * (jd_tdb - T0);
        const double ma = remainder((mlon - pw[i]), TWOPI);
        const double u = ma + e * sin(ma) + 0.5 * e * e * sin(2.0 * ma);
        const double sinu = sin(u);
        const double cosu = cos(u);

        // Compute velocity factor.
        const double anr = pn[i] / (1.0 - e * cosu);
        int k;

        for (k = 3; --k >= 0; ) {
          // Compute planet's position and velocity wrt eq & eq J2000.
          const double p = a[k][i] * (cosu - e) + b[k][i] * sinu;
          const double v = anr * (-a[k][i] * sinu + b[k][i] * cosu);

          pbary[k] += f * p;
          vbary[k] += f * v;
        }
      }

      tlast = jd_tdb;
    }

    for(i = 3; --i >= 0; ) {
      position[i] -= pbary[i];
      velocity[i] -= vbary[i];
    }
  }

  return 0;
}



/**
 * It may provide the position and velocity of the Earth and Sun, the same as
 * solarsystem_earth_sun(), if earth_sun_enable_hp() is set to true (non-zero). Otherwise,
 * it will return with an error code of 3, indicating that high-precision calculations are
 * not provided by this implementation.
 *
 * NOTES:
 * <ol>
 * <li>This implementation will always propulate the output position and velocity vectors
 * with the low-precision result, regardless of the return error code, in order to reduce
 * the chance of unpredictable behavior, even if the user does not check the returned error
 * status (which of course they should).
 * </li>
 * </ol>
 *
 *
 * @param jd_tdb        [day] Barycentric Dynamical Time (TDB) based Julian date
 * @param body          NOVAS_EARTH (3) or NOVAS_SUN (10) only.
 * @param origin        NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1) relative to which to
 *                      return positions and velocities. (For compatibility with existing
 *                      NOVAS C compatible user implementations, we keep the original
 *                      NOVAS C argument type here).
 * @param[out] position [AU] Position vector of 'body' at 'tjd'; equatorial rectangular
 *                      coordinates in AU referred to the mean equator and equinox
 *                      of J2000.0.
 * @param[out] velocity [AU/day] Velocity vector of 'body' at 'tjd'; equatorial rectangular
 *                      system referred to the mean equator and equinox of J2000.0,
 *                      in AU/Day.
 * @return              0 if successful, -1 if there is a required function is not provided
 *                      (errno set to ENOSYS), or if one of the output pointer arguments is
 *                      NULL (errno set to EINVAL). 1 if the input Julian date ('tjd') is
 *                      out of range, 2 if 'body' is invalid, or 3 if the high-precision
 *                      orbital data cannot be produced (default return value).
 *
 * @sa earth_sun_enable_hp()
 * @sa earth_sun_calc()
 * @sa set_planet_provider()
 * @sa solarsystem_hp()
 * @sa novas_planet_provider_hp
 *
 */
short earth_sun_calc_hp(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin, double *position,
        double *velocity) {

  int error = earth_sun_calc(jd_tdb[0] + jd_tdb[1], body, origin, position, velocity);
  if(error) return error;

  return allow_lp_for_hp ? 0 : 3;
}

/**
 * Computes equatorial spherical coordinates of Sun referred to the mean equator and equinox
 * of date.
 *
 * Quoted accuracy is 2.0 + 0.03 * T<sup>2</sup> arcsec, where T is measured in units of
 * 1000 years from J2000.0.  See reference.
 *
 * The obliquity equation is updated to equation 5.12 of the second reference.
 *
 * The linear fit to DE405 primarily corrects for the difference between "old" (Lieske) and
 * "new" (IAU 2006) precession.  The difference, new - old, is -0.3004 arcsec/cy.
 *
 * REFERENCES:
 * <ol>
 * <li>Bretagnon, P. and Simon, J.L. (1986).  Planetary Programs and
 * Tables from -4000 to + 2800. (Richmond, VA: Willmann-Bell).</li>
 * <li>Kaplan, G.H. (2005). US Naval Observatory Circular 179.</li>
 * </ol>
 *
 * @param jd           [day] jd (double) Julian date on TDT or ET time scale.
 * @param[out] ra      [h] Right ascension referred to mean equator and equinox of date (hours).
 * @param[out] dec     [deg] Declination referred to mean equator and equinox of date (degrees).
 * @param[out] dis     [AU] Geocentric distance (AU).
 *
 * @sa earth_sun_calc()
 */
void sun_eph(double jd, double *ra, double *dec, double *dis) {
  struct sun_con {
    int l;
    int r;
    float alpha;
    double nu;
  };

  double sum_lon = 0.0;
  double sum_r = 0.0;
  const double factor = 1.0e-07;
  double u, lon, t, emean, sin_lon;

  int i;

  static const struct sun_con con[50] = {
          { 403406, 0, 4.721964, 1.621043 }, //
          { 195207, -97597, 5.937458, 62830.348067 }, //
          { 119433, -59715, 1.115589, 62830.821524 }, //
          { 112392, -56188, 5.781616, 62829.634302 }, //
          { 3891, -1556, 5.5474, 125660.5691 }, //
          { 2819, -1126, 1.5120, 125660.9845 }, //
          { 1721, -861, 4.1897, 62832.4766 }, //
          { 0, 941, 1.163, 0.813 }, //
          { 660, -264, 5.415, 125659.310 }, //
          { 350, -163, 4.315, 57533.850 }, //
          { 334, 0, 4.553, -33.931 }, //
          { 314, 309, 5.198, 777137.715 }, //
          { 268, -158, 5.989, 78604.191 }, //
          { 242, 0, 2.911, 5.412 }, //
          { 234, -54, 1.423, 39302.098 }, //
          { 158, 0, 0.061, -34.861 }, //
          { 132, -93, 2.317, 115067.698 }, //
          { 129, -20, 3.193, 15774.337 }, //
          { 114, 0, 2.828, 5296.670 }, //
          { 99, -47, 0.52, 58849.27 }, //
          { 93, 0, 4.65, 5296.11 }, //
          { 86, 0, 4.35, -3980.70 }, //
          { 78, -33, 2.75, 52237.69 }, //
          { 72, -32, 4.50, 55076.47 }, //
          { 68, 0, 3.23, 261.08 }, //
          { 64, -10, 1.22, 15773.85 }, //
          { 46, -16, 0.14, 188491.03 }, //
          { 38, 0, 3.44, -7756.55 }, //
          { 37, 0, 4.37, 264.89 }, //
          { 32, -24, 1.14, 117906.27 }, //
          { 29, -13, 2.84, 55075.75 }, //
          { 28, 0, 5.96, -7961.39 }, //
          { 27, -9, 5.09, 188489.81 },
          { 27, 0, 1.72, 2132.19 }, //
          { 25, -17, 2.56, 109771.03 }, //
          { 24, -11, 1.92, 54868.56 }, //
          { 21, 0, 0.09, 25443.93 }, //
          { 21, 31, 5.98, -55731.43 }, //
          { 20, -10, 4.03, 60697.74 }, //
          { 18, 0, 4.27, 2132.79 }, //
          { 17, -12, 0.79, 109771.63 }, //
          { 14, 0, 4.24, -7752.82 }, //
          { 13, -5, 2.01, 188491.91 }, //
          { 13, 0, 2.65, 207.81 }, //
          { 13, 0, 4.98, 29424.63 }, //
          { 12, 0, 0.93, -7.99 }, //
          { 10, 0, 2.21, 46941.14 }, //
          { 10, 0, 3.59, -68.29 }, //
          { 10, 0, 1.50, 21463.25 }, //
          { 10, -9, 2.55, 157208.40 } };

  // Define the time units 'u', measured in units of 10000 Julian years
  // from J2000.0, and 't', measured in Julian centuries from J2000.0.
  u = (jd - T0) / 3652500.0;
  t = u * 100.0;

  // Compute longitude and distance terms from the series.
  for(i = 50; --i >= 0; ) {
    const struct sun_con *c = &con[i];
    const double arg = c->alpha + c->nu * u;
    sum_lon += c->l * sin(arg);
    if(c->r) sum_r += c->r * cos(arg);
  }

  // Compute longitude, latitude, and distance referred to mean equinox
  // and ecliptic of date.  Apply correction to longitude based on a
  // linear fit to DE405 in the interval 1900-2100.
  lon = 4.9353929 + 62833.1961680 * u + factor * sum_lon;
  lon += ((-0.1371679461 - 0.2918293271 * t) * ASEC2RAD);

  lon = remainder(lon, TWOPI);
  *dis = 1.0001026 + factor * sum_r;

  // Compute mean obliquity of the ecliptic.
  emean = (84381.406 + (-46.836769 + (-0.0001831 + 0.00200340 * t) * t) * t) * ASEC2RAD;

  // Compute equatorial spherical coordinates referred to the mean equator
  // and equinox of date.
  sin_lon = sin(lon);
  *ra = atan2((cos(emean) * sin_lon), cos(lon)) * RAD2DEG;
  if(*ra < 0.0) *ra += 360.0;
  *ra = *ra / 15.0;

  *dec = asin(sin(emean) * sin_lon) * RAD2DEG;

  return;
}

#if DEFAULT_SOLSYS == 3
  novas_planet_provider default_planetcalc = earth_sun_calc;
  novas_planet_provider_hp default_planetcalc_hp = earth_sun_calc_hp;
#elif !BUILTIN_SOLSYS3
short solarsystem(double jd_tdb, short body, short origin, double *position, double *velocity) {
  return earth_sun_calc(jd_tdb, body, origin, position, velocity);
}

short solarsystem_hp(const double jd_tdb[2], short body, short origin, double *position, double *velocity) {
  return earth_sun_calc_hp(jd_tdb, body, origin, position, velocity);
}
#endif

