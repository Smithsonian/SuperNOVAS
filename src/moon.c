/**
 * @file
 *
 * @date Created  on Dec 11, 2025
 * @author Attila Kovacs
 *
 *  This module implements self-contained calculations for the Moon's position, such as via
 *  Keplerian orbital approximation, and through a semi-analytical model by Chapront-Touze &amp;
 *  Chapront 1988. The latter can predict the Moon's position to cm-level precision, but is quite
 *  expensive to calculate with around 35,000 sinusoidal terms. However, one may opto to omit the
 *  less significant terms to arrive at a less precise result faster if needed.
 *
 *  REFERENCES:
 *  <ol>
 *   <li>Chapront-Touze, M., &amp; Chapront, J., A&amp;A, 190, 342 (1988)</li>
 *   <li>Chapront J., Francou G., 2003, A&amp;A, 404, 735</li>
 *   <li>Chapront-Touze, M., &amp; Francou, G., "LUNAR SOLUTION ELP version ELP/MPP02", (October 2002),
 *       https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/</li>
 *  </ol>
 *
 *  @sa solsys-calceph.c, solsys-cspice.c, ephemeris.c
 */

#include <string.h>
#include <stdint.h>
#include <errno.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
namespace novas {
#  endif
#endif

#define ELP_DELTA       0.01      ///< [day] for cord estimate

typedef struct {
  uint8_t D;
  uint8_t F;
  uint8_t l;
  uint8_t l1;
  double A;             ///< [arcsec|km] amplitude
} elp_main_coeffs;

typedef struct {
  float A;              ///< [arcsec|km] amplitude
  float phi;            ///< [deg] phase
  uint8_t delaunay[4];  ///< [rad] D, F, l, l1
  uint8_t planets[8];   ///< [rad] Mercury -> Neptune
  uint8_t precession;   ///< [rad] Multiple for general precession term
} elp_pert_coeffs;


typedef struct {
  double W1;            /// [rad]
  double W2;            /// [rad]
  double W3;            /// [rad]
  double T;             /// [rad]
  double omega;         /// [rad]
} elp_mean_args;

#include "elp2000/elp-lon.tab.c"
#include "elp2000/elp-pert-lon.tab.c"

/// \endcond


static void elp_args(double t, elp_mean_args *restrict elp, novas_delaunay_args *restrict delaunay) {
  // From Chapront-Touze, M., & Francou, G., "LUNAR SOLUTION ELP version ELP/MPP02", (October 2002),
  // https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/</li>

  elp->W1      = 218.316654363889 + t * (481266.4843711222 + t * (-0.0018912   + t * ( 1.8344e-6  - t * 8.803e-9)));
  elp->W2      =  83.353242986111 + t * (  4067.61675475   + t * (-0.0106286   + t * (-1.25131e-5 + t * 5.9169e-8)));
  elp->W3      = 125.044555044444 + t * ( -1935.5332050833 + t * ( 0.0017664   + t * ( 2.1181e-6  - t * 9.9611e-9)));
  elp->T       = 100.466427458333 + t * ( 35999.3728591667 + t * (-5.61e-6     + t * ( 2.5e-9     + t * 4.2e-11)));
  elp->omega   = 102.93734935     + t * (     0.3225676167 + t * ( 1.470181e-4 + t * (-3.2816e-8  + t * 3.16083e-9)));

  // [deg] -> [rad] in [-pi:pi) range.
  elp->W1      = remainder(elp->W1 * DEGREE, TWOPI);
  elp->W2      = remainder(elp->W2 * DEGREE, TWOPI);
  elp->W3      = remainder(elp->W3 * DEGREE, TWOPI);
  elp->T       = remainder(elp->T * DEGREE, TWOPI);
  elp->omega   = remainder(elp->omega * DEGREE, TWOPI);

  // Chapront-Touze & Francou (2002), Eq. 3.
  delaunay->D = elp->W1 - elp->T + M_PI;
  delaunay->F = elp->W1 - elp->W3;
  delaunay->l = elp->W1 - elp->W2;
  delaunay->l1 = elp->T - elp->omega;
  delaunay->Omega = elp->W3;
}

static double elp_sin(const novas_delaunay_args *restrict args, const elp_main_coeffs *restrict coeffs, int n, double limit) {
  int i;
  double sum = 0.0;

  for(i = 0; i < n; i++) {
    const elp_main_coeffs *c = &coeffs[i];
    double arg;

    if(fabs(c->A) < limit) return sum;

    arg = c->D * args->D + c->F * args->F + c->l * args->l + c->l1 * args->l1;
    sum += c->A * sin(arg);
  }

  return sum;
}

static double elp_cos(const novas_delaunay_args *restrict args, const elp_main_coeffs *restrict coeffs, int n, double limit) {
  int i;
  double sum = 0.0;

  for(i = 0; i < n; i++) {
    const elp_main_coeffs *c = &coeffs[i];
    double arg;

    if(fabs(c->A) < limit) return sum;

    arg = c->D * args->D + c->F * args->F + c->l * args->l + c->l1 * args->l1;
    sum += c->A * cos(arg);
  }

  return sum;
}

static double elp_pert(double t, const novas_delaunay_args *restrict args, const double *restrict planets, double prec,
        const elp_pert_coeffs **restrict coeffs, const int *restrict n, double limit) {
  int i, k;
  double sum = 0.0;
  double tfact = 1.0;

  for(k = 0; n[k] > 0; k++) {
    const elp_pert_coeffs *ck = coeffs[k];
    double sum1 = 0.0;

    for(i = 0; i < n[k]; i++) {
      const elp_pert_coeffs *c = &ck[i];
      double arg;
      int m;

      if(fabs(tfact * c->A) < limit) break;

      arg = c->delaunay[0] * args->D + c->delaunay[1] * args->F + c->delaunay[2] * args->l + c->delaunay[3] * args->l1;
      for(m = NOVAS_MERCURY; m <= NOVAS_NEPTUNE; m++)
        arg += c->planets[m - NOVAS_MERCURY] * planets[m];
      arg += c->precession * prec;

      sum1 += c->A * sin(arg + c->phi * DEGREE);
    }

    sum += tfact * sum1;
    tfact *= t;
  }

  return sum;
}

#if !CPPCHECK
/**
 * Calculates the Moon's geocentric position using the ELP/MPP02 model by Chapront-Touze &amp;
 * Francou (2003), in the ELP reference plane (i.e. the Ecliptic of date), down to the specified
 * limiting term amplitude.
 *
 * REFERENCES:
 * <ol>
 * <li>Chapront-Touze, M., &amp; Chapront, J., A&amp;A, 190, 342 (1988)</li>
 * <li>Chapront J., Francou G., 2003, A&amp;A, 404, 735</li>
 * <li>Chapront-Touze, M., &amp; Francou, G., "LUNAR SOLUTION ELP version ELP/MPP02", (October 2002),
 * https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/</li>
 * </ol>
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TDB) based Julian date.
 * @param limit     [arcsec|km] Sum only the harmonic terms with amplitudes larger than this
 *                  limit. The resulting accuracy is typically an order-of-magnitude above
 *                  the set limiting amplitude.
 * @return          [deg] Ecliptic longitude of Moon vs geoncenter (on ecliptic of date).
 */
double novas_moon_elp_ecl_lon(double jd_tdb, double limit) {
  const elp_pert_coeffs *leading = &elp_plon0[0];
  const double t = (jd_tdb - NOVAS_JD_J2000) / JULIAN_CENTURY_DAYS;

  elp_mean_args elp = {};
  novas_delaunay_args args = {};
  double lon;

  // Delaunay args of ELP2000 (Chapront & Francou 2003).
  elp_args(t, &elp, &args);

  lon = elp.W1 + elp_sin(&args, elp_lon, elp_n_lon, limit) * ARCSEC;

  if(limit < fabs(leading->A)) {
    double planets[NOVAS_NEPTUNE + 1] = {0.0};
    double prec = accum_prec(t);
    int i;

    for(i = NOVAS_MERCURY; i <= NOVAS_NEPTUNE; i++)
      planets[i] = (i == NOVAS_EARTH) ? elp.T  : planet_lon(t, (enum novas_planet) i);

    lon += elp_pert(t, &args, planets, prec, elp_plon, elp_n_plon, limit) * ARCSEC;
  }

  return lon;
}
#endif // CPPCHECK

#if !CPPCHECK
/**
 * Calculates the Moon's geocentric position using the ELP/MPP02 model by Chapront-Touze &amp;
 * Francou (2003), in the ELP reference plane (i.e. the Ecliptic of date), down to the specified
 * limiting term amplitude.
 *
 * REFERENCES:
 * <ol>
 * <li>Chapront-Touze, M., &amp; Chapront, J., A&amp;A, 190, 342 (1988)</li>
 * <li>Chapront J., Francou G., 2003, A&amp;A, 404, 735</li>
 * <li>Chapront-Touze, M., &amp; Francou, G., "LUNAR SOLUTION ELP version ELP/MPP02", (October 2002),
 * https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/</li>
 * </ol>
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TDB) based Julian date.
 * @param limit     [arcsec|km] Sum only the harmonic terms with amplitudes larger than this
 *                  limit.
 * @param[out] pos  [AU] Output geocentric position vector in the Ecliptic of J2000.
 * @return          0 if successful, or else -1 if there was an error (errno will indicate the
 *                  type of error.
 */
int novas_moon_elp_ecl_pos(double jd_tdb, double limit, double *pos) {
/// \cond PRIVATE
#  include "elp2000/elp-lat.tab.c"
#  include "elp2000/elp-dis.tab.c"

#  include "elp2000/elp-pert-lat.tab.c"
#  include "elp2000/elp-pert-dis.tab.c"
/// \endcond

  const elp_pert_coeffs *leading = &elp_plon0[0];
  const double t = (jd_tdb - NOVAS_JD_J2000) / JULIAN_CENTURY_DAYS;

  elp_mean_args elp = {};
  novas_delaunay_args args = {};
  double lon, lat, dis;
  double clat;

  if(!pos)
    return novas_error(-1, EINVAL, "novas_moon_elp_ecl_pos", "output position vector is NULL");

  // Delaunay args of ELP2000 (Chapront & Francou 2003).
  elp_args(t, &elp, &args);

  lon = elp.W1 + elp_sin(&args, elp_lon, elp_n_lon, limit) * ARCSEC;
  lat = elp_sin(&args, elp_lat, elp_n_lat, limit) * ARCSEC;
  dis = elp_cos(&args, elp_dis, elp_n_dis, limit) * NOVAS_KM / NOVAS_AU;

  if(limit < fabs(leading->A)) {
    double planets[NOVAS_NEPTUNE + 1] = {0.0};
    double prec = accum_prec(t);
    int i;

    for(i = NOVAS_MERCURY; i <= NOVAS_NEPTUNE; i++)
      planets[i] = (i == NOVAS_EARTH) ? elp.T  : planet_lon(t, (enum novas_planet) i);

    lon += elp_pert(t, &args, planets, prec, elp_plon, elp_n_plon, limit) * ARCSEC;
    lat += elp_pert(t, &args, planets, prec, elp_plat, elp_n_plat, limit) * ARCSEC;
    dis += elp_pert(t, &args, planets, prec, elp_pdis, elp_n_pdis, limit) * NOVAS_KM / NOVAS_AU;
  }

  clat = cos(lat);

  pos[0] = dis * clat * cos(lon);
  pos[1] = dis * clat * sin(lon);
  pos[2] = dis * sin(lat);

  return 0;
}
#endif // CPPCHECK

/**
 * Returns the Moon's ICRS geometric position and velocity, relative to an Earth-based observer
 * (or the geocenter), using the ELP/MPP02 model by Chapront-Touze &amp; Francou (2003). Only terms
 * larger than the specified limit are used to provide a result with the desired precision.
 *
 * REFERENCES:
 * <ol>
 * <li>Chapront-Touze, M., &amp; Chapront, J., A&amp;A, 190, 342 (1988)</li>
 * <li>Chapront J., Francou G., 2003, A&amp;A, 404, 735</li>
 * <li>Chapront-Touze, M., &amp; Francou, G., "LUNAR SOLUTION ELP version ELP/MPP02", (October 2002),
 * https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/</li>
 * </ol>
 *
 * @param time      Astrometric time of observation.
 * @param obs       Earth-based observer location, or NULL for geocentric.
 * @param limit     [arcsec|km] Sum only terms with amplitudes larger than this limit. The
 *                  resulting accuracy is typically an order-of-magnitude above the set limiting
 *                  amplitude.
 * @param[out] pos  [AU] The Moon's position vector relative to the observer (or geocenter), or
 *                  NULL if not required.
 * @param[out] vel  [AU/day] The Moon's velocity vector relative to the observer (or geocenter),
 *                  or NULL if not required.
 * @return          0 if successful, or else -1 if there was an error (errno will indicate the
 *                  type of error.
 *
 * @since 1.6
 * @author Attila Kovacs
 *
 * @sa novas_moon_elp_posvel()
 * @sa novas_moon_elp_sky_pos_fp()
 * @sa novas_make_moon_orbit()
 * @sa novas_geom_posvel()
 */
int novas_moon_elp_posvel_fp(const novas_timespec *restrict time, const on_surface *restrict obs, double limit,
        double *restrict pos, double *restrict vel) {
  static const char *fn = "novas_moon_elp_posvel_fp";

  double tdb = novas_get_time(time, NOVAS_TDB);
  double tt = novas_get_time(time, NOVAS_TT);
  double opos[3] = {0.0}, ovel[3] = {0.0};
  enum novas_accuracy acc = limit < 1e-3 ? NOVAS_FULL_ACCURACY : NOVAS_REDUCED_ACCURACY;

  if(!pos && !vel)
    return novas_error(-1, EINVAL, fn, "both output pos and vel are NULL");

  if(isnan(tdb))
    novas_trace(fn, -1, 0);

  if(obs)
    terra(obs, novas_time_gst(time, NOVAS_REDUCED_ACCURACY), opos, ovel);

  if(pos) {
    int k;

    novas_moon_elp_ecl_pos(tdb, limit, pos);

    for(k = 3; --k >= 0; )
      pos[k] -= opos[k];

    equ2ecl_vec(tt, NOVAS_MEAN_EQUATOR, acc, pos, pos);
    mod_to_gcrs(tdb, pos, pos);
  }

  if(vel) {
    double p1[3] = {0.0}, p2[3] = {0.0};
    int k;

    novas_moon_elp_ecl_pos(tdb - ELP_DELTA, limit, p1);
    novas_moon_elp_ecl_pos(tdb + ELP_DELTA, limit, p2);

    for(k = 3; --k >= 0; )
      vel[k] = (p2[k] - p1[k]) / (2.0 * ELP_DELTA) - ovel[k];

    equ2ecl_vec(tt, NOVAS_MEAN_EQUATOR, acc, vel, vel);
    mod_to_gcrs(tdb, vel, vel);
  }

  return 0;
}

/**
 * Returns the Moon's ICRS geometric position and velocity, relative to an Earth-based observer
 * (or the geocenter), using the ELP/MPP02 model by Chapront-Touze &amp; Francou (2003).
 *
 * REFERENCES:
 * <ol>
 * <li>Chapront-Touze, M., &amp; Chapront, J., A&amp;A, 190, 342 (1988)</li>
 * <li>Chapront J., Francou G., 2003, A&amp;A, 404, 735</li>
 * <li>Chapront-Touze, M., &amp; Francou, G., "LUNAR SOLUTION ELP version ELP/MPP02", (October 2002),
 * https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/</li>
 * </ol>
 *
 * @param time      Astrometric time of observation.
 * @param obs       Earth-based observer location, or NULL for geocentric.
 * @param accuracy  NOVAS_REDUCED_ACCURACY (1) to sum only terms larger than 0.1 arcsec or km (for
 *                  a precision at the arc-second or km level), or NOVAS_FULL_ACCURACY (0) to sum
 *                  all terms for the full precision at the 0.1 mas / 10 cm accuracy.
 * @param[out] pos  [AU] The Moon's position vector relative to the observer (or geocenter), on
 *                  NULL if not required.
 * @param[out] vel  [AU/day] The Moon's velocity vector relative to the observer (or geocenter),
 *                  or NULL if not required.
 * @return          0 if successful, or else -1 if there was an error (errno will indicate the
 *                  type of error.
 *
 * @since 1.6
 * @author Attila Kovacs
 *
 * @sa novas_moon_elp_posvel_fp()
 * @sa novas_moon_elp_sky_pos()
 * @sa novas_make_moon_orbit()
 * @sa novas_geom_posvel()
 */
int novas_moon_elp_posvel(const novas_timespec *restrict time, const on_surface *restrict obs, enum novas_accuracy accuracy,
        double *restrict pos, double *restrict vel) {
  double limit = (accuracy == NOVAS_REDUCED_ACCURACY) ? 0.1 : 0.0;
  prop_error("novas_moon_elp_posvel", novas_moon_elp_posvel_fp(time, obs, limit, pos, vel), 0);
  return 0;
}

/**
 * Returns the Moon's ICRS apparent place, relative to an Earth-based observer (or the geocenter),
 * using the ELP/MPP02 model by Chapront-Touze &amp; Francou (2003). Only terms larger than the
 * specified limit are used to provide a result with the desired precision.
 *
 * REFERENCES:
 * <ol>
 * <li>Chapront-Touze, M., &amp; Chapront, J., A&amp;A, 190, 342 (1988)</li>
 * <li>Chapront J., Francou G., 2003, A&amp;A, 404, 735</li>
 * <li>Chapront-Touze, M., &amp; Francou, G., "LUNAR SOLUTION ELP version ELP/MPP02", (October 2002),
 * https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/</li>
 * </ol>
 *
 * @param time      Astrometric time of observation.
 * @param obs       Earth-based observer location, or NULL for geocentric.
 * @param limit     [arcsec|km] Sum only terms with amplitudes larger than this limit. The
 *                  resulting accuracy is typically an order-of-magnitude above the set limiting
 *                  amplitude.
 * @param[out] pos  The Moon's position, relative to the true equator and equinox of date.
 * @return          0 if successful, or else -1 if there was an error (errno will indicate the
 *                  type of error.
 *
 * @since 1.6
 * @author Attila Kovacs
 *
 * @sa novas_moon_elp_sky_pos_fp()
 * @sa novas_moon_elp_posvel()
 * @sa novas_make_moon_orbit()
 * @sa novas_sky_pos()
 */
int novas_moon_elp_sky_pos_fp(const novas_timespec *restrict time, const on_surface *restrict obs, double limit,
        sky_pos *restrict pos) {
  static const char *fn = "novas_moon_elp_skypos_fp";

  double p[3] = {0.0}, v[3] = {0.0};
  int k;

  if(!pos)
    return novas_error(-1, EINVAL, fn, "output position is NULL.");

  prop_error(fn, novas_moon_elp_posvel_fp(time, obs, limit, p, v), 0);

  vector2radec(p, &pos->ra, &pos->dec);
  pos->dis = novas_vlen(p);

  for(k = 3; --k >= 0; )
    pos->r_hat[k] = p[k] / pos->dis;

  pos->rv = novas_vdot(p, v) / pos->dis * (NOVAS_AU / NOVAS_DAY) / NOVAS_KMS;

  return 0;
}

/**
 * Returns the Moon's ICRS apparent place, relative to an Earth-based observer (or the geocenter),
 * using the ELP/MPP02 model by Chapront-Touze &amp; Francou (2003).
 *
 * REFERENCES:
 * <ol>
 * <li>Chapront-Touze, M., &amp; Chapront, J., A&amp;A, 190, 342 (1988)</li>
 * <li>Chapront J., Francou G., 2003, A&amp;A, 404, 735</li>
 * <li>Chapront-Touze, M., &amp; Francou, G., "LUNAR SOLUTION ELP version ELP/MPP02", (October 2002),
 * https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/</li>
 * </ol>
 *
 * @param time      Astrometric time of observation.
 * @param obs       Earth-based observer location, or NULL for geocentric.
 * @param accuracy  NOVAS_REDUCED_ACCURACY (1) to sum only terms larger than 0.1 arcsec or km (for
 *                  a precision at the arc-second or km level), or NOVAS_FULL_ACCURACY (0) to sum
 *                  all terms for the full precision at the 0.1 mas / 10 cm accuracy.
 * @param[out] pos  The Moon's position, relative to the true equator and equinox of date.
 * @return          0 if successful, or else -1 if there was an error (errno will indicate the
 *                  type of error.
 *
 * @since 1.6
 * @author Attila Kovacs
 *
 * @sa novas_moon_elp_sky_pos_fp()
 * @sa novas_moon_elp_posvel()
 * @sa novas_make_moon_orbit()
 * @sa novas_sky_pos()
 */
int novas_moon_elp_sky_pos(const novas_timespec *restrict time, const on_surface *restrict obs, enum novas_accuracy accuracy,
        sky_pos *restrict pos) {
  double limit = (accuracy == NOVAS_REDUCED_ACCURACY) ? 1e-4 : 0.0;
  prop_error("novas_moon_elp_skypos", novas_moon_elp_sky_pos_fp(time, obs, limit, pos), 0);
  return 0;
}



/**
 * Gets mean orbital elements for the Moon relative to the geocenter for the specified epoch
 * of observation. It is based on the ELP2000-85 model, but not including the perturbation
 * series. As such it has accuracy at the few degrees level only, however it is 'valid' for
 * long-term projections (i.e. for years around the orbit's reference epoch).
 *
 * For the short-term , `novas_make_moon_orbit()` can provide somewhat more accurate
 * predictions for up to a day or so around the reference epoch of the orbit.
 *
 * REFERENCES:
 * <ol>
 *  <li>Chapront, J. et al., 2002, A&amp;A 387, 700–709</li>
 *  <li>Chapront-Touze, M, and Chapront, J. 1988, Astronomy and Astrophysics,
 *      vol. 190, p. 342-352.</li>
 *  <li>Chapront J., Francou G., 2003, A&amp;A, 404, 735</li>
 *  <li>Laskar J., 1986, A&amp;A, 157, 59</li>
 * </ol>
 *
 * @param jd_tdb      [day] Barycentric Dynamical Time (TDB) based Julian Date.
 * @param[out] orbit  Orbital elements data structure to populate.
 * @return            0 if successful, or else -1 (`errno` set to `EINVAL`).
 *
 * @since 1.4
 * @author Attila Kovacs
 *
 * @sa novas_make_moon_orbit(), novas_make_planet_orbit(), make_orbital_object()
 */
int novas_make_moon_mean_orbit(double jd_tdb, novas_orbital *restrict orbit) {
  novas_orbital def = NOVAS_ORBIT_INIT;
  double W1, t, dot;

  if(!orbit)
    return novas_error(-1, EINVAL, "novas_make_moon_orbit", "output orbital is NULL");

  // Default ecliptic orbital...
  memcpy(orbit, &def, sizeof(novas_orbital));

  orbit->system.center = NOVAS_EARTH;
  orbit->system.plane = NOVAS_ECLIPTIC_PLANE;
  orbit->system.type = NOVAS_J2000;

  // Values expressed for instant
  orbit->jd_tdb = jd_tdb;

  t = (jd_tdb - NOVAS_JD_J2000) / JULIAN_CENTURY_DAYS;

  // Mean inclination (leading latitude term of ELP02 series)
  orbit->i = 5.12816666666667;

  // eccentricity (from the leading term of the ELP03 series)
  orbit->e = 0.0542994634645866;

  // Chapront & Francou 2003
  // ELP/MPP02
  // https://cyrano-se.obspm.fr/pub/2_lunar_solutions/2_elpmpp02/elpmpp02.pdf
  W1           = 218.316654363889 + t * (481266.4843711222 + t * (-0.0018912 + t * ( 1.8344e-6  - t * 8.803e-9)));   // W1
  orbit->omega =  83.353242986111 + t * (  4067.61675475   + t * (-0.0106286 + t * (-1.25131e-5 + t * 5.9169e-8)));  // W2
  orbit->Omega = 125.044555044444 + t * ( -1935.5332050833 + t * ( 0.0017664 + t * ( 2.1181e-6  - t * 9.9611e-9)));  // W3

  orbit->M0 = W1 - orbit->omega;

  // apsis from rising node (omega = Omega - omega_bar)
  orbit->omega -= orbit->Omega;

  // differentiate M0 above to get mean motion
  orbit->n = 477198.867616372 + t * (0.001747498 + t * (4.30425e-6 - t * 2.71888e-7));

  // From Chapront-Touze, M, and Chapront, J. 1983, A&A, 124, 1, p. 50-62.
  // (n^2 a^3 = constant).
  orbit->a = 3.84747980645e8 / NOVAS_AU * pow(477198.86763133 / orbit->n, 2.0 / 3.0);

  orbit->n /= JULIAN_CENTURY_DAYS;

  // differentiate omega above to get apsis motion
  dot = 4067.61675475 + t * (-0.0212572 + t * (-3.75393e-05 + t * 2.36676e-07));
  orbit->apsis_period = JULIAN_CENTURY_DAYS * TWOPI / dot;

  // differentiate Omega above to get node motion
  dot = -1935.53320508333 + t * (0.0035328 + t * (6.3543e-06 - t * 3.98444e-08));
  orbit->node_period = JULIAN_CENTURY_DAYS * TWOPI / dot;

  // apsis w.r.t. the node.
  orbit->apsis_period -= orbit->node_period;

  // Transform from the mean ecliptic of date to the mean ecliptic of J2000
  // Laskar 1986, A&A, 157, 59
  if(fabs(t) > 1e-4) {
    double P = t *   0.10180391e-4 + t * ( 0.47020439e-6 + t * (-0.5417367e-9 + t * (-0.2507948e-11 + t * 0.463486e-14)));
    double Q = t * -0.113469002e-3 + t * ( 0.12372674e-6 + t * ( 0.1265417e-8 + t * (-0.1371808e-11 - t * 0.320334e-14)));
    double pole[3] = {0.0};
    double dOmega;

    pole[1] = -sin(orbit->i * DEGREE);
    pole[2] =  cos(orbit->i * DEGREE);

    novas_Rx(-P, pole);
    novas_Ry(Q, pole);

    orbit->i = atan2(sqrt(pole[0] * pole[0] + pole[1] * pole[1]), pole[2]) / DEGREE;
    dOmega = atan2(pole[0], -pole[1]) / DEGREE;

    orbit->Omega += dOmega;
    orbit->omega -= dOmega;
  }

  return 0;
}

/**
 * Gets an approximation of the `current` Keplerian orbital elements for the Moon relative to the
 * geocenter for the specified epoch of observation. The orbit includes the most dominant Solar
 * perturbation terms to produce results with an accuracy at the ~10 arcmin level for +- 0.5 days
 * around the reference time argument for the orbit. It is based on the ELP/MPP02 model.
 *
 * While, the ELP model itself can be highly precise, the Moon's orbit is far from Keplerian, and
 * so any attempt to describe it in purely Keplerian terms is inherently flawed, which is the
 * reason for the generally poor accuracy of this model.
 *
 * REFERENCES:
 * <ol>
 *  <li>Chapront, J. et al., 2002, A&amp;A 387, 700–709</li>
 *  <li>Chapront-Touze, M, and Chapront, J. 1988, Astronomy and Astrophysics, vol. 190, p. 342-352.</li>
 *  <li>Chapront J., Francou G., 2003, A&amp;A, 404, 735</li>
 * </ol>
 *
 * @param jd_tdb      [day] Barycentric Dynamical Time (TDB) based Julian Date.
 * @param[out] orbit  Orbital elements data structure to populate.
 * @return            0 if successful, or else -1 (`errno` set to `EINVAL`).
 *
 * @since 1.5.1
 * @author Attila Kovacs
 *
 * @sa novas_make_moon_mean_orbit(), novas_make_planet_orbit(), make_orbital_object()
 */
int novas_make_moon_orbit(double jd_tdb, novas_orbital *restrict orbit) {
  typedef struct {
    int8_t iD;      // multiple of D
    int8_t il1;     // multiple of l1
    int8_t il;      // multiple of l
    int8_t iF;      // multiple of F
    float A;        // [arcsec,km] amplitude
  } elp_coeffs;


  // From ELP01: https://cyrano-se.obspm.fr/pub/2_lunar_solutions/1_elp82b/elp_series/ELP01
  static const elp_coeffs clon[8] = {
    {  0,  0,  1,  2,      -45.10032 }, //
    {  0,  0,  1, -2,       39.53393 }, //
    {  1,  0, -1,  0,      -18.58467 }, //
    {  2,  0,  0, -2,       55.17801 }, //
    {  2,  0,  2,  0,       14.37964 }, //
    {  4,  0, -2,  0,       30.77247 }, //
    {  2,  2, -1,  0,       -9.36601 },  //

    // Principal terms not included
    // (These degrade the Keplerian model)
    //{  0,  0,  0,  2,     -411.60287 }, //
    //{  2,  0, -2,  0,      211.65487 }, // T
    //{  2,  0, -3,  0,       13.19400 }, //

    // The following elongate the orbit, and are at least partly degenerate with eccentric
    // deformation...
    //{  0,  1,  1,  0,     -109.38419 }, // E
    //{  0,  1, -1,  0,     -147.32654 }, // E-
    //{  2,  0, -1,  0,     4586.43061 }, // E
    //{  2,  0,  1,  0,      191.95575 }, // E
    //{  2,  1, -1,  0,      -28.39810 }  // E?
    //{  2, -1,  1,  0,       14.53078 }, // E
    //{  2, -1, -1,  0,      205.44315 }, // E
    //{  4,  0, -1,  0,       38.42974 }, // E
  };

  // From ELP01: https://cyrano-se.obspm.fr/pub/2_lunar_solutions/1_elp82b/elp_series/ELP01
  static const elp_coeffs comega[7] = {
    {  0,  1,  0,  0,     -666.44186 },
    {  1,  0,  0,  0,     -124.98806 }, //
    {  1,  1,  0,  0,       17.95512 }, //
    {  2,  0,  0,  0,     2369.91227 }, //
    {  2,  1,  0,  0,      -24.35910 }, //
    {  2, -1,  0,  0,      164.73458 }, //
    {  4,  0,  0,  0,       13.89903 }, //
  };

  // From ELP02: https://cyrano-se.obspm.fr/pub/2_lunar_solutions/1_elp82b/elp_series/ELP02
  static const elp_coeffs clat[7] = {
    {  0,  0,  2, -1,       31.75985 }, //
    {  2,  0,  0, -1,      623.65783 }, //
    {  2,  0,  1, -1,       33.35743 }, // *
    {  2,  1,  0, -1,      -12.09470 }, //
    {  0,  1,  1, -1,       -5.07614 }, // *
    {  0,  1,  1,  1,       -5.31151 }, //
    {  2,  0,  1,  1,       15.12165 }  //

    // Principal terms not included
    // (These degrade the Keplerian model)
    //{  0,  0,  1, -1,      999.70079 }, // *
    //{  0,  0,  1,  1,     1010.17430 }, //
    //{  0,  0,  2,  1,       61.91229 }, //
    //{  2, -1,  0, -1,       29.57794 }, // ?
    //{  2,  0, -1, -1,      166.57528 }, //
    //{  2,  0, -1,  1,      199.48515 }, // *
    //{  2,  0,  0,  1,      117.26161 }, //
  };


  // From ELP03: https://cyrano-se.obspm.fr/pub/2_lunar_solutions/1_elp82b/elp_series/ELP03
  static const elp_coeffs ce[12] = {
    {  0,  1, -1,  0,     0.0}, //-129.62476 }, // E
    {  0,  1,  1,  0,      104.75896 }, // E
    {  2,  0, -1,  0,    -3699.10468 }, // E
    {  2,  0,  1,  0,     -170.73274 }, // E
    {  2,  1, -1,  0,       24.20935 }, // E
    {  2, -1,  1,  0,      -12.83185 }, // E
    {  2, -1, -1,  0,     -152.14314 }, // E
    {  4,  0, -1,  0,      -34.78245 }, // E

    // Tidal terms, which we crudely approximate with elliptical flattening
    {  2, -1, -2,  0,       10.05654 }, // T
    {  2,  0, -2,  0,      246.15768 }, // T
    {  2,  0,  2,  0,      -10.44472 }, // T
    {  4,  0, -2,  0,      -21.63627 }  // T

    // Principal terms not included
    // (These degrade the Keplerian model)
  };

  // From ELP03: https://cyrano-se.obspm.fr/pub/2_lunar_solutions/1_elp82b/elp_series/ELP03
  static const elp_coeffs cdis[7] = {
    {  0,  1,  0,  0,       48.89010 }, //
    {  1,  0,  0,  0,      108.74265 }, //
    {  1,  1,  0,  0,      -16.67533 }, //
    {  2, -1,  0,  0,     -204.59357 }, //
    {  2,  0,  0,  0,    -2955.96651 }, //
    {  2,  1,  0,  0,       30.82498 }, //
    {  4,  0,  0,  0,      -11.64993 },  //

    // Principal terms not included
    // These terms are not consistent with a Keplerian orbit, with |iL| != 1
    //{  2,  0,  0, -2,       10.32129 }, //
  };

  // [arcsec] Eccentric series for ecliptic longitude vs mean anomaly.
  static const float AE[8] = { 22639.55000, 769.02326, 36.12364, 1.93367, 0.11100, 0.00665, 0.00041, 0.00003 };

  const double t = (jd_tdb - NOVAS_JD_J2000) / JULIAN_CENTURY_DAYS;
  double M1, dE = 0.0, dL = 0.0, dL1 = 0.0, dY = 0.0, dOmega;
  double pole[3] = {0.0}, E[2] = {0.0}, domega;
  int i;

  elp_mean_args elp = {};
  novas_delaunay_args args = {};

  prop_error("novas_make_moon_orbit", novas_make_moon_mean_orbit(jd_tdb, orbit), 0);

  // Delaunay args for Solar perturbations.
  // (Chapront & Francou 2003).
  elp_args(t, &elp, &args);

  // Perturb longitude...
  for(i = 0; i < 7; i++) {
    const elp_coeffs *c = &clon[i];
    const double arg = (c->iD * args.D + c->il1 * args.l1 + c->il * args.l + c->iF * args.F);
    dL += c->A * sin(arg);
    dL1 = c->il * c->A * cos(arg);   // dL/dl * dl/dt = dL / dt
  }

  // Calculate the ecliptic vs mean longitude differential for eccentric orbit.
  for(i = 0; i < 8; i++)
    dE += (i + 1) * AE[i] * cos((i + 1) * args.l);
  dE *= ARCSEC;

  // Project longitude perturbation into orbital mean anomaly.
  orbit->M0 += (dL / 3600.0) * (1.0 + dE);

  // dl / dt - d(W1 - W2) / dt
  M1      = 477198.867616372 + t * (0.0087374 + t * (1.43475e-05 - t * 6.7972e-08));

  // Projected local current mean motion.
  orbit->n += (dL1 * ARCSEC) * (1.0 + dE) * (M1 / 3600.0) / JULIAN_CENTURY_DAYS;

  // Perturb omega (apsis location vs node)
  dL = 0.0;
  for(i = 0; i < 7; i++) {
    const elp_coeffs *c = &comega[i];
    double arg = (c->iD * args.D + c->il1 * args.l1); // (semi)annual variations only...
    dL += c->A * sin(arg);
  }

  // Project longitude perturbation into shift in periapsis.
  orbit->omega += (dL / 3600.0);

  pole[1] = -sin(orbit->i * DEGREE);
  pole[2] =  cos(orbit->i * DEGREE);

  // Perturb pole...
  for(i = 0; i < 7; i++) {
    const elp_coeffs *c = &clat[i];

    if(c->il) {
      double arg = (c->iD * args.D + c->il1 * args.l1 + c->il * args.l + c->iF * args.F);
      dY += c->A * sin(arg);
    }
    else {
      double arg = (c->iD * args.D + c->il1 * args.l1);
      novas_Rz(arg, pole);
      novas_Rx(c->A * ARCSEC, pole);
      novas_Rz(-arg, pole);
    }
  }

  novas_Rz(args.F, pole);
  novas_Ry(dY * ARCSEC, pole);
  novas_Rz(-args.F, pole);

  orbit->i = atan2(sqrt(pole[0] * pole[0] + pole[1] * pole[1]), pole[2]) / DEGREE;
  dOmega = atan2(pole[0], -pole[1]) / DEGREE;

  orbit->Omega += dOmega;
  orbit->omega -= dOmega;

  // Perturb eccentricity
  E[0] = -orbit->e * orbit->a * NOVAS_AU;

  for(i = 0; i < 12; i ++) {
    const elp_coeffs *c = &ce[i];
    double arg = -(c->iD * args.D + c->il1 * args.l1); // (semi)annual variations only...
    double A = c->A * NOVAS_KM;

    if(abs(c->il) == 2)
      A = -2.0 * A;       // Tidal terms as excess eccentricity (a crude approximation...)

    E[0] -= A * cos(arg);
    E[1] -= A * sin(arg);
  }

  orbit->e = sqrt(E[0] * E[0] + E[1] * E[1]) / (orbit->a * NOVAS_AU);
  domega = -atan2(-E[1], -E[0]) / DEGREE;

  orbit->omega += domega;
  orbit->M0 -= domega;

  // Perturb mean distance
  for(i = 0; i < 7; i++) {
    const elp_coeffs *c = &cdis[i];
    double arg = (c->iD * args.D + c->il1 * args.l1); // (semi)annual variations only...
    orbit->a += c->A * NOVAS_KM / NOVAS_AU * cos(arg);
  }

  return 0;
}


/**
 * Calculates the Moon's phase at a given time. It uses orbital models for Earth (E.M. Standish
 * and J.G. Williams 1992), and for the Moon (Chapront, J. et al., 2002), and takes into account
 * the slightly eccentric nature of both orbits.
 *
 * NOTES:
 * <ol>
 * <li>The Moon's phase here follows the definition by the Astronomical Almanac, as the excess
 * ecliptic longitude of the Moon over that of the Sun seen from the geocenter.</li>
 * <li>There are other definitions of the phase too, depending on which you might find slightly
 * different answers, but regardless of the details most phase calculations should match to within
 * a few degrees.</li>
 * </ol>
 *
 * NOTES:
 * <ol>
 * <li>This function caches the result of the last calculation.</li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 *  <li>The Explanatory Supplement to the Astronomical Almanac, University Science Books, 3rd ed.,
 *      p. 507</li>
 *  <li>E.M. Standish and J.G. Williams 1992.</li>
 *  <li>https://ssd.jpl.nasa.gov/planets/approx_pos.html</li>
 *  <li>Chapront, J. et al., 2002, A&amp;A 387, 700–709</li>
 *  <li>Chapront-Touze, M, and Chapront, J. 1983, Astronomy and Astrophysics (ISSN 0004-6361),
 *      vol. 124, no. 1, July 1983, p. 50-62.</li>
 * </ol>
 *
 * @param jd_tdb      [day] Barycentric Dynamical Time (TDB) based Julian Date.
 * @return            [deg] The Moon's phase, or more precisely the ecliptic longitude difference
 *                    between the Sun and the Moon, as seen from the geocenter. 0: New Moon, 90:
 *                    1st quarter, +/- 180 Full Moon, -90: 3rd quarter or NAN if the solution
 *                    failed to converge (errno will be set to ECANCELED), or if the JD date is
 *                    outside the range of the orbital model (errno set to EINVAL).
 *
 * @since 1.4
 * @author Attila Kovacs
 *
 * @sa novas_next_moon_phase(), novas_make_moon_orbit(), novas_solar_illum()
 */
double novas_moon_phase(double jd_tdb) {
  static const char *fn = "novas_moon_phase";

  static THREAD_LOCAL double last_tdb = NAN, last_phase;

  if(novas_time_equals(jd_tdb, last_tdb))
    return last_phase;

  novas_orbital orbit = NOVAS_ORBIT_INIT;
  double pos[3] = {0.0};
  double he, hm;

  // EMB ecliptic pos around Sun
  prop_nan(fn, novas_make_planet_orbit(NOVAS_EMB, jd_tdb, &orbit));
  prop_nan(fn, novas_orbit_native_posvel(jd_tdb, &orbit, pos, NULL));
  vector2radec(pos, &he, NULL);

  // Moon ecliptic pos around Earth
  novas_make_moon_orbit(jd_tdb, &orbit);
  prop_nan(fn, novas_orbit_native_posvel(jd_tdb, &orbit, pos, NULL));
  vector2radec(pos, &hm, NULL);

  //hm = novas_moon_elp_ecl_lon(jd_tdb, 1.0) / 15.0;

  last_phase = remainder(12.0 + hm - he, 24.0) * 15.0;
  last_tdb = jd_tdb;

  return last_phase;
}


/**
 * Calculates the date / time at which the Moon will reach the specified phase next, _after_ the
 * specified time. It uses orbital models for Earth (E.M. Standish and J.G. Williams 1992), and
 * for the Moon (Chapront, J. et al., 2002), and takes into account the slightly eccentric nature
 * of both orbits.
 *
 * NOTES:
 * <ol>
 * <li>The Moon's phase here follows the definition by the Astronomical Almanac, as the excess
 * ecliptic longitude of the Moon over that of the Sun seen from the geocenter.</li>
 * <li>There are other definitions of the phase too, depending on which you might find slightly
 * different answers, but regardless of the details most phase calculations should match give or
 * take a few hours.</li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 *  <li>The Explanatory Supplement to the Astronomical Almanac, University Science Books, 3rd ed.,
 *      p. 507</li>
 *  <li>E.M. Standish and J.G. Williams 1992.</li>
 *  <li>https://ssd.jpl.nasa.gov/planets/approx_pos.html</li>
 *  <li>Chapront, J. et al., 2002, A&amp;A 387, 700–709</li>
 *  <li>Chapront-Touze, M, and Chapront, J. 1983, Astronomy and Astrophysics (ISSN 0004-6361),
 *      vol. 124, no. 1, July 1983, p. 50-62.</li>
 * </ol>
 *
 * @param phase   [deg] The Moon's phase, or more precisely the ecliptic longitude difference
 *                between the Sun and the Moon, as seen from the geocenter. 0: New Moon, 90: 1st
 *                quarter, +/- 180 Full Moon, -90: 3rd quarter.
 * @param jd_tdb  [day] The lower bound date for the phase, as a Barycentric Dynamical Time (TDB)
 *                based Julian Date.
 * @return        [day] The Barycentric Dynamical Time (TDB) based Julian Date when the Moon will
 *                be in the desired phase next after the input date; or NAN if the solution failed
 *                to converge (errno will be set to ECANCELED).
 *
 * @since 1.4
 * @author Attila Kovacs
 *
 * @sa novas_moon_phase(), novas_make_moon_orbit()
 */
double novas_next_moon_phase(double phase, double jd_tdb) {
  static const char *fn = "novas_next_moon_phase";
  int i;

  double t = (jd_tdb - NOVAS_JD_J2000) / JULIAN_CENTURY_DAYS;

  // [deg/day] Differential motion of the Moon w.r.t. Earth.
  // Moon motion from Chapront-Touze, M, and Chapront, J. 1983, A&A, 124, 1, p. 50-62.
  // Earth motion from E.M. Standish and J.G. Williams 1992. Table 8.10.3. Valid for 3000 BC to 3000 AD.
  double rate = 445266.793243221 + t * (0.021258 + t * (3.75393e-05 - t * 2.366776e-07));
  rate /= JULIAN_CENTURY_DAYS;

  for(i = 0; i < novas_inv_max_iter; i++) {
    double phi = novas_moon_phase(jd_tdb);

    if(isnan(phi))
      return novas_trace_nan(fn);

    phi = remainder(phase - phi, DEG360); // [deg]

    if(fabs(phi) < 1e-6)
      return jd_tdb;

    if(i == 0 && phi < 0.0)
      phi += DEG360;  // initial phase shift must be positive to ensure it is after input date.

    // Date when mean elongation changes by phi...
    jd_tdb += phi / rate;
  }

  novas_error(-1, ECANCELED, fn, "Failed to converge");
  return NAN;
}

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
} // namespace novas
#  endif
#endif


#if __cplusplus
#  ifdef NOVAS_NAMESPACE
} // namespace novas
#  endif
#endif

