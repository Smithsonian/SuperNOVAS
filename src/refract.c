/**
 * @file
 *
 * @date Created  on Jun 27, 2024
 * @author Attila Kovacs
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
#include <math.h>
#include "novas.h"

static double novas_refraction(enum novas_refraction_model model, const on_surface *loc, enum novas_refraction_type type, double el) {
  if(!loc) {
    novas_error(-1, EINVAL, "novas_refraction", "NULL on surface observer location");
    return NAN;
  }

  if(type == NOVAS_REFRACT_OBSERVED)
    return refract(loc, model, 90.0 - el);

  if(type == NOVAS_REFRACT_ASTROMETRIC)
    return refract_astro(loc, model, 90.0 - el);

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
 * @param type      Refraction type to use for the original model: REFRACT_OBSERVED (-1) or
 *                  REFRACT_ASTROMETRIC (0).
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
double novas_inv_refract(RefractionModel model, double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el0) {
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
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian data of observation
 * @param loc       Pointer to structure defining the observer's location on earth, and local weather
 * @param type      Whether the input elevation is observed or astrometric: REFRACT_OBSERVED (-1) or
 *                  REFRACT_ASTROMETRIC (0).
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
  if(isnan(dz))
    novas_trace("novas_optical_refraction", -1, 0);
  return dz;
}

/**
 * Returns an optical refraction correction using the weather parameters defined for the observer location.
 *
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian data of observation
 * @param loc       Pointer to structure defining the observer's location on earth, and local weather
 * @param type      Whether the input elevation is observed or astrometric: REFRACT_OBSERVED (-1) or
 *                  REFRACT_ASTROMETRIC (0).
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
  if(isnan(dz))
    novas_trace("novas_optical_refraction", -1, 0);
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
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian data of observation
 * @param loc       Pointer to structure defining the observer's location on earth, and local weather.
 *                  Make sure all weather values, including humidity (added in v1.1), are fully
 *                  populated.
 * @param type      Whether the input elevation is observed or astrometric: REFRACT_OBSERVED (-1) or
 *                  REFRACT_ASTROMETRIC (0).
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
  double y = 1.0, z;
  double poly;
  double fptem;
  double refraction;
  int j;

  if(!loc) {
    novas_error(-1, EINVAL, fn, "NULL on surface observer location");
    return NAN;
  }

  if(type == NOVAS_REFRACT_OBSERVED)
    return novas_inv_refract(novas_radio_refraction, jd_tt, loc, NOVAS_REFRACT_ASTROMETRIC, el);

  if(type != NOVAS_REFRACT_ASTROMETRIC) {
    novas_error(-1, EINVAL, fn, "invalid refraction type: %d", type);
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
