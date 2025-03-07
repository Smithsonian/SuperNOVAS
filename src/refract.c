/**
 * @file
 *
 * @date Created  on Jun 27, 2024
 * @author Attila Kovacs and G. Kaplan
 *
 *  A collection of refraction models and utilities to use with novas_app_to_hor() or
 *  novas_hor_to_app().
 *
 * @sa novas_app_to_hor()
 * @sa novas_hor_to_app().
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "novas.h"


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
double refract_astro(const on_surface *restrict location, enum novas_refraction_model option, double zd_astro) {
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
 *
 */
double refract(const on_surface *restrict location, enum novas_refraction_model option, double zd_obs) {
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

static double novas_refraction(enum novas_refraction_model model, const on_surface *loc, enum novas_refraction_type type, double el) {
  static const char *fn = "novas_refraction";

  if(!loc) {
    novas_set_errno(EINVAL, fn, "NULL on surface observer location");
    return NAN;
  }

  if(type == NOVAS_REFRACT_OBSERVED)
    return refract(loc, model, 90.0 - el);

  if(type == NOVAS_REFRACT_ASTROMETRIC)
    return refract_astro(loc, model, 90.0 - el);

  novas_set_errno(EINVAL, fn, "NULL on surface observer location");
  return NAN;
}

/**
 * Computes the reverse atmospheric refraction for a given refraction model. Thus if a refraction
 * model takes observed elevation as an input, the reverse refraction takes astrometric elevation
 * as its input, and vice versa.
 *
 * @param model     The original refraction model
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian data of observation
 * @param loc       Pointer to structure defining the observer's location on earth, and local weather
 * @param type      Refraction type to use for the original model: NOVAS_REFRACT_OBSERVED (-1) or
 *                  NOVAS_REFRACT_ASTROMETRIC (0).
 * @param el0       [deg] input elevation for the inverse refraction model.
 * @return          [deg] Estimated refraction, or NAN if there was an error (it should also
 *                  set errno to indicate the type of error).
 *
 * @sa refract_astro()
 * @sa itrs_to_hor()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
double novas_inv_refract(RefractionModel model, double jd_tt, const on_surface *restrict loc, enum novas_refraction_type type, double el0) {
  double refr = 0.0;
  const int dir = (type == NOVAS_REFRACT_OBSERVED ? 1 : -1);
  int i;

  for(i = 0; i < novas_inv_max_iter; i++) {
    double el1 = el0 + dir * refr;
    refr = model(jd_tt, loc, type, el1);

    if(fabs(refr - dir * (el1 - el0)) < 1e-7)
      return refr;
  }

  novas_set_errno(ECANCELED, "refract_astro", "failed to converge");
  return NAN;
}

/**
 * Returns an optical refraction correction for a standard atmosphere.
 *
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian data of observation (unused in this implementation of RefractionModel)
 * @param loc       Pointer to structure defining the observer's location on earth, and local weather
 * @param type      Whether the input elevation is observed or astrometric: NOVAS_REFRACT_OBSERVED (-1) or
 *                  NOVAS_REFRACT_ASTROMETRIC (0).
 * @param el        [deg] Astrometric (unrefracted) source elevation
 * @return          [deg] Estimated refraction, or NAN if there was an error (it should also
 *                  set errno to indicate the type of error).
 *
 * @sa novas_app_to_hor()
 * @sa novas_optical_refraction()
 * @sa NOVAS_STANDARD_ATMOSPHERE()
 * @sa refract()
 * @sa refract_astro()
 */
double novas_standard_refraction(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el) {
  double dz = novas_refraction(NOVAS_STANDARD_ATMOSPHERE, loc, type, el);
  (void) jd_tt;

  if(isnan(dz))
    return novas_trace_nan("novas_optical_refraction");
  return dz;
}

/**
 * Returns an optical refraction correction using the weather parameters defined for the observer location.
 *
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian data of observation (unused in this implementation of RefractionModel)
 * @param loc       Pointer to structure defining the observer's location on earth, and local weather
 * @param type      Whether the input elevation is observed or astrometric: NOVAS_REFRACT_OBSERVED (-1) or
 *                  NOVAS_REFRACT_ASTROMETRIC (0).
 * @param el        [deg] Astrometric (unrefracted) source elevation
 * @return          [arcsec] Estimated refraction, or NAN if there was an error (it should also
 *                  set errno to indicate the type of error).
 *
 * @sa novas_app_to_hor()
 * @sa novas_optical_refraction()
 * @sa NOVAS_STANDARD_ATMOSPHERE()
 * @sa refract()
 * @sa refract_astro()
 */
double novas_optical_refraction(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el) {
  double dz = novas_refraction(NOVAS_WEATHER_AT_LOCATION, loc, type, el);
  (void) jd_tt;

  if(isnan(dz))
    return novas_trace_nan("novas_optical_refraction");
  return dz;
}

/**
 * Atmospheric refraction model for radio wavelengths (Berman &amp; Rockwell 1976). It uses the
 * weather parameters defined for the location, including humidity. As such make sure the weather
 * data is fully defined, and that the humidity was explicitly set after calling
 * `make_on_surface()`.
 *
 * Adapted from FORTAN code provided by Berman &amp; Rockwell 1976.
 *
 * REFERENCES:
 * <ol>
 * <li>Berman, Allan L., and Rockwell, Stephen T. (1976), NASA JPL Technical Report 32-1601</li>
 * </ol>
 *
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian data of observation (unused in this implementation of RefractionModel)
 * @param loc       Pointer to structure defining the observer's location on earth, and local weather.
 *                  Make sure all weather values, including humidity (added in v1.1), are fully
 *                  populated.
 * @param type      Whether the input elevation is observed or astrometric: NOVAS_REFRACT_OBSERVED (-1) or
 *                  NOVAS_REFRACT_ASTROMETRIC (0).
 * @param el        [deg] source elevation of the specified type.
 * @return          [deg] Estimated refraction, or NAN if there was an error (it should also
 *                  set errno to indicate the type of error).
 *
 * @sa novas_optical_refraction()
 * @sa make_on_surface()
 * @sa on_surface
 */
double novas_radio_refraction(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el) {
  static const char *fn = "novas_radio_refraction";
  // Various coefficients...
  static const double E[] = { 0.0, 46.625, 45.375, 4.1572, 1.4468, 0.25391, 2.2716, -1.3465, -4.3877, 3.1484, 4.520, -1.8982, 0.8900 };

  double E0, TK;
  double y, z;
  double poly;
  double fptem;
  double refraction;
  int j;

  (void) jd_tt;

  if(!loc) {
    novas_set_errno(EINVAL, fn, "NULL on surface observer location");
    return NAN;
  }

  if(loc->humidity < 0.0 || loc->humidity > 100.0) {
    novas_set_errno(EINVAL, fn, "invalid humidity value: %g", loc->humidity);
    return NAN;
  }

  if(type == NOVAS_REFRACT_OBSERVED)
    return novas_inv_refract(novas_radio_refraction, jd_tt, loc, NOVAS_REFRACT_ASTROMETRIC, el);

  if(type != NOVAS_REFRACT_ASTROMETRIC) {
    novas_set_errno(EINVAL, fn, "invalid refraction type: %d", type);
    return NAN;
  }

  // Zenith angle in degrees
  z = 90.0 - el;

  // Temperature in Kelvin
  TK = loc->temperature + 273.16;
  fptem = (loc->pressure / 1000.) * (273.16 / TK);
  E0 = (z - E[1]) / E[2];
  poly = E[11];

  for(j = 1; j <= 8; j++)
    poly = poly * E0 + E[11 - j];

  if(poly <= -80.) poly = 0.;

  poly = exp(poly) - E[12];
  refraction = poly * fptem / 3600.0;
  y = exp(((TK * 17.149) - 4684.1) / (TK - 38.45));

  return refraction * (1.0 + (y * loc->humidity * 71.) / (TK * loc->pressure * 0.760));
}
