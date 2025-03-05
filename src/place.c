/**
 * @file
 *
 * @date Created  on Mar 5, 2025
 * @author Attila Kovacs
 *
 *  Various convenience variants of the place() function in novas.c
 */

#include <math.h>
#include <errno.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"
/// \endcond

/**
 * Computes the International Celestial Reference System (ICRS) position of a source.
 * (from the geocenter). Unlike `place_gcrs()`, this version does not include
 * aberration or gravitational deflection corrections.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date of observation.
 * @param source      Catalog source or solar_system body.
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] pos    Structure to populate with the calculated geocentric ICRS position
 *                    data (Unlike place_gcrs(), the calculated coordinates do not account
 *                    for aberration or gravitational deflection).
 * @return            0 if successful, or -1 if any of the input pointer arguments is NULL,
 *                    or else an error from place().
 *
 * @sa place_gcrs()
 * @sa place_cirs()
 * @sa place_tod()
 * @sa mean_star()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int place_icrs(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos) {
  prop_error("place_icrs", place(jd_tt, source, NULL, 0.0, NOVAS_ICRS, accuracy, pos), 0);
  return 0;
}

/**
 * Computes the Geocentric Celestial Reference System (GCRS) position of a source (as 'seen'
 * from the geocenter) at the given time of observation. Unlike `place_icrs()`, this includes
 * aberration for the moving frame of the geocenter as well as gravitational deflections
 * calculated for a virtual observer located at the geocenter. See `place()` for more information.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date of observation.
 * @param source      Catalog source or solar_system body.
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] pos    Structure to populate with the calculated GCRS position data
 * @return            0 if successful, or -1 if any of the input pointer arguments is NULL,
 *                    or else an error from place().
 *
 * @sa place_icrs()
 * @sa place_cirs()
 * @sa place_tod()
 * @sa virtual_star()
 * @sa virtual_planet()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int place_gcrs(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos) {
  prop_error("place_gcrs", place(jd_tt, source, NULL, 0.0, NOVAS_GCRS, accuracy, pos), 0);
  return 0;
}

/**
 * Computes the Celestial Intermediate Reference System (CIRS) dynamical position
 * position of a source as 'seen' from the geocenter at the given time of observation. See
 * `place()` for more information.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date of observation.
 * @param source      Catalog source or solar_system body.
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] pos    Structure to populate with the calculated CIRS position data
 * @return            0 if successful, or -1 if any of the input pointer arguments is NULL,
 *                    or else an error from place().
 *
 * @sa place_tod()
 * @sa place_gcrs()
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 */
int place_cirs(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos) {
  prop_error("place_cirs", place(jd_tt, source, NULL, 0.0, NOVAS_CIRS, accuracy, pos), 0);
  return 0;
}

/**
 * Computes the True of Date (TOD) dynamical position position of a source as 'seen' from the
 * geocenter at the given time of observation. See `place()` for more information.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date of observation.
 * @param source      Catalog source or solar_system body.
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] pos    Structure to populate with the calculated CIRS position data
 * @return            0 if successful, or -1 if any of the input pointer arguments is NULL,
 *                    or else an error from place().
 *
 * @sa place_cirs()
 * @sa place_gcrs()
 * @sa app_star()
 * @sa app_planet()
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 */
int place_tod(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos) {
  prop_error("place_tod", place(jd_tt, source, NULL, 0.0, NOVAS_TOD, accuracy, pos), 0);
  return 0;
}

/**
 * Computes the Mean of Date (MOD) dynamical position position of a source as 'seen' from the
 * geocenter at the given time of observation. See `place()` for more information.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date of observation.
 * @param source      Catalog source or solar_system body.
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] pos    Structure to populate with the calculated CIRS position data
 * @return            0 if successful, or -1 if any of the input pointer arguments is NULL,
 *                    or else an error from place().
 *
 * @sa place_cirs()
 * @sa place_gcrs()
 * @sa app_star()
 * @sa app_planet()
 *
 * @since 1.1
 * @author Attila Kovacs
 *
 */
int place_mod(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos) {
  prop_error("place_mod", place(jd_tt, source, NULL, 0.0, NOVAS_MOD, accuracy, pos), 0);
  return 0;
}

/**
 * Computes the J2000 dynamical position position of a source as 'seen' from the
 * geocenter at the given time of observation. See `place()` for more information.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date of observation.
 * @param source      Catalog source or solar_system body.
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] pos    Structure to populate with the calculated CIRS position data
 * @return            0 if successful, or -1 if any of the input pointer arguments is NULL,
 *                    or else an error from place().
 *
 * @sa place_cirs()
 * @sa place_gcrs()
 * @sa app_star()
 * @sa app_planet()
 *
 * @since 1.1
 * @author Attila Kovacs
 *
 */
int place_j2000(double jd_tt, const object *restrict source, enum novas_accuracy accuracy, sky_pos *restrict pos) {
  prop_error("place_j2000", place(jd_tt, source, NULL, 0.0, NOVAS_J2000, accuracy, pos), 0);
  return 0;
}
