/**
 * @file
 *
 * @author G. Kaplan and A. Kovacs
 * @version 0.9.0
 *
 *  SuperNOVAS astrometry softwate based on the Naval Observatory Vector Astrometry Software (NOVAS).
 *  It has been modified to fix outstanding issues and to make it easier to use.
 *
 *  Based on the NOVAS C Edition, Version 3.1,  U. S. Naval Observatory
 *  Astronomical Applications Dept.
 *  Washington, DC
 *   <a href="http://www.usno.navy.mil/USNO/astronomical-applications">http://www.usno.navy.mil/USNO/astronomical-applications</a>
 *
 */

#if !COMPAT
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#endif

#include <errno.h>
#include <unistd.h>

#include "novas.h"

/// \cond PRIVATE
#define HALF_PI             (0.5 * M_PI)
#define ERAD_AU             (ERAD/AU)

#define XYZ_VECTOR_SIZE     (3 * sizeof(double))

// Use shorthand definitions for our constants
#define JD_J2000            NOVAS_JD_J2000
#define C                   NOVAS_C
#define AU_SEC              NOVAS_AU_SEC
#define C_AUDAY             NOVAS_C_AU_PER_DAY
#define AU                  NOVAS_AU
#define AU_KM               NOVAS_AU_KM
#define GS                  NOVAS_GS
#define GE                  NOVAS_GE
#define ERAD                NOVAS_ERAD
#define EF                  NOVAS_EFLAT
#define ANGVEL              NOVAS_ANGVEL

// Various locally used physical units
#define DAY                 86400.0         ///< [s] seconds in a day
#define DAY_HOURS           24.0
#define DEG360              360.0
#define JULIAN_YEAR_DAYS    365.25
#define JULIAN_CENTURY_DAYS 36525.0
#define ARCSEC              ASEC2RAD
#define DEGREE              DEG2RAD
#define HOURANGLE           (M_PI / 12.0)
#define MAS                 (1e-3 * ASEC2RAD)

#define CIO_INTERP_POINTS   6     ///< Number of points to load from CIO interpolation table at once.

// On some older platform NAN may not be defined, so define it here if need be
#ifndef NAN
#  define NAN               (0.0/0.0)
#endif

/// \endcond

/**
 * Celestial pole offset &psi; for high-precision applications.
 *
 * @sa EPS_COR
 * @sa cel_pole()
 */
static double PSI_COR = 0.0;

/**
 * Celestial pole offset &epsilon; for high-precision applications.
 *
 * @sa PSI_COR
 * @sa cel_pole()
 */
static double EPS_COR = 0.0;

static FILE *cio_file;            ///< Opened CIO locator data file, or NULL.

static novas_planet_calculator planetcalc = NULL;
static novas_planet_calculator_hp planetcalc_hp = NULL;

static novas_ephem_reader_func readeph2_call = NULL;

static novas_nutate_func nutate_lp = nu2000k;

static double vlen(const double *v) {
  return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

static double vdist(const double *v1, const double *v2) {
  double d2 = 0.0;
  int i;
  for(i = 3; --i >= 0;) {
    const double d = v1[i] - v2[i];
    d2 += d * d;
  }
  return sqrt(d2);
}

static double vdot(const double *v1, const double *v2) {
  return (v1[0] * v2[0]) + (v1[1] * v2[1]) + (v1[2] * v2[2]);
}

/**
 * Computationally efficient implementation of 3D rotation with small angles.
 *
 * @param in            3D vector to rotate
 * @param ax            (rad) rotation angle around x
 * @param ax            (rad) rotation angle around x
 * @param ax            (rad) rotation angle around x
 * @param[out] out      Rotated vector;
 *
 */
static void tiny_rotate(const double *in, double ax, double ay, double az, double *out) {
  const double A[3] = { ax * ax, ay * ay, az * az };
  out[0] = in[0] - 0.5 * (A[1] + A[2]) * in[0] - az * in[1] + ay * in[2];
  out[1] = in[1] - 0.5 * (A[0] + A[2]) * in[1] + az * in[0] - ax * in[2];
  out[2] = in[2] - 0.5 * (A[0] + A[1]) * in[2] - ay * in[0] + ax * in[1];
}

static int time_equals(double jd1, double jd2) {
  return fabs(jd1 - jd2) <= 1.0e-8;
}

int j2000_to_tod(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out) {
  double v[3];

  if(!in || !out) {
    errno = EINVAL;
    return -1;
  }

  if(time_equals(jd_tdb, JD_J2000)) {
    memcpy(out, in, XYZ_VECTOR_SIZE);
    return 0;
  }

  precession(JD_J2000, in, jd_tdb, v);
  nutation(jd_tdb, NUTATE_MEAN_TO_TRUE, accuracy, v, out);
  return 0;
}

int tod_to_j2000(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out) {
  double v[3];

  if(!in || !out) {
    errno = EINVAL;
    return -1;
  }

  if(time_equals(jd_tdb, JD_J2000)) {
    memcpy(out, in, XYZ_VECTOR_SIZE);
    return 0;
  }

  nutation(jd_tdb, NUTATE_TRUE_TO_MEAN, accuracy, in, v);
  precession(jd_tdb, v, JD_J2000, out);

  return 0;
}

int icrs_to_tod(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out) {
  double j2000[3];
  int error = frame_tie(in, TIE_ICRS_TO_J2000, j2000);
  if(error) return error;
  return j2000_to_tod(jd_tdb, accuracy, j2000, out);
}

int tod_to_icrs(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out) {
  double j2000[3];
  int error = tod_to_j2000(jd_tdb, accuracy, in, j2000);
  if(error) return error;
  return frame_tie(j2000, TIE_J2000_TO_ICRS, out);
}

/**
 * Set a custom function to use for regular precision (see NOVAS_REDUCED_PRECISION) ephemeris calculations
 * instead of the default solarsystem() routine.
 *
 * @param f       The function to use for solar system position/velocity calculations.
 *                See solarsystem() for further details on what is required of this
 *                function.
 *
 * @author Attila Kovacs
 * @since 1.0
 *
 * @sa set_planet_calc_hp()
 * @sa solarsystem()
 * @sa NOVAS_REDUCED_PRECISION
 */
int set_planet_calc(novas_planet_calculator f) {
  if(!f) {
    errno = EINVAL;
    return -1;
  }
  planetcalc = f;
  return 0;
}

/**
 * Set a custom function to use for high precision (see NOVAS_FULL_PRECISION) ephemeris calculations
 * instead of the default solarsystem_hp() routine.
 *
 * @param f       The function to use for solar system position/velocity calculations.
 *                See solarsystem_hp() for further details on what is required of this
 *                function.
 *
 * @author Attila Kovacs
 * @since 1.0
 *
 * @sa set_planet_calc()
 * @sa solarsystem_hp()
 * @sa NOVAS_FILL_PRECISION
 */
int set_planet_calc_hp(novas_planet_calculator_hp f) {
  if(!f) {
    errno = EINVAL;
    return -1;
  }
  planetcalc_hp = f;
  return 0;
}

/**
 * Computes the coordinates of a celestial object at the specified time and observer location, returning the position
 * in the celestial reference frame of choice.
 *
 * REFERENCES:
 * <ol>
 *     <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *     <li>Explanatory Supplement to the Astronomical Almanac (1992),Chapter 3.</li>
 * </ol>
 *
 * @param source    Catalog source or solar_system body.
 * @param obs       Observer location (can be NULL if not relevant)
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian date.
 * @param ut1_to_tt [s] Difference TT-UT1 at 'jd_tt', in seconds of time.
 * @param system    The type of coordinate reference system in which coordinates are to be returned.
 * @param accuracy  NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] pos  The astrometric position of the celestial object
 * @return          0 if successful, or -1 if one of the required arguments is NULL, or else 1 if observer location
 *                  is invalid, or an error code &lt;10 if from function place().
 *
 * @sa calc_planet_pos()
 * @sa calc_star_pos()
 *
 *
 * @author Attila Kovacs
 * @since 1.0
 */
int calc_pos(const object *source, const observer *obs, double jd_tt, double ut1_to_tt, enum novas_reference_system system,
        enum novas_accuracy accuracy, sky_pos *pos) {
  object cel_obj;
  observer location = { };
  int error;

  if(!source || !pos) {
    errno = EINVAL;
    return -1;
  }

  if(system < 0 || system >= NOVAS_REFERENCE_SYSTEMS) {
    errno = EINVAL;
    return -1;
  }

  if(accuracy != NOVAS_FULL_ACCURACY) accuracy = NOVAS_REDUCED_ACCURACY;

  // Set up a structure of type 'observer' containing the position of the observer.
  if(obs) {
    error = make_observer(obs->where, &obs->on_surf, &obs->near_earth, &location);
    if(error) return 1;
  }

  // Set up a structure of type 'object' containing the star data.
  if(source->type == NOVAS_DISTANT_OBJECT) {
    error = make_object(source->type, 0, source->star.starname, &source->star, &cel_obj);
  }
  else {
    error = make_object(source->type, source->number, source->name, NULL, &cel_obj);
  }
  if(error) return error + 10;

  // Compute the apparent place with a call to function 'place'.
  error = place(jd_tt, &cel_obj, &location, ut1_to_tt, system, accuracy, pos);
  if(error) {
    memset(pos, 0, sizeof(*pos));
    return error + 20;
  }

  return 0;
}

/**
 * Copmputes the positions of a source in a specific astronomical reference frame
 *
 * @param source      Catalog source or solar_system body.
 * @param frame       Astronomical frame of observation.
 * @param accuracy    NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] pos    Calculated position data structure to populate
 * @return            0 if successful, or -1 if any of the input pointer arguments is NULL,
 *                    or else an error from calc_pos().
 */
int calc_frame_pos(const object *source, const astro_frame *frame, enum novas_accuracy accuracy, sky_pos *pos) {
  if(!frame) {
    errno = EINVAL;
    return -1;
  }
  return calc_pos(source, &frame->location, frame->jd_tdb, frame->ut1_to_tt, frame->basis_system, accuracy, pos);
}

/**
 * Computes the apparent place of a star, referenced to dynamical equator at date 'jd_tt', given its
 * catalog mean place, proper motion, parallax, and radial velocity.
 *
 * REFERENCES:
 * <ol>
 *     <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *     <li>Explanatory Supplement to the Astronomical Almanac (1992),Chapter 3.</li>
 * </ol>
 *
 * @param star      Pointer to catalog entry structure containing catalog data for
 *                  the object in the ICRS.
 * @param obs       Observer location (can be NULL if not relevant)
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian date.
 * @param ut1_to_tt   [s] Difference TT-UT1 at 'jd_tt', in seconds of time.
 * @param system    The type of coordinate reference system in which coordinates are to be returned.
 * @param accuracy  NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] pos  The astrometric position of the celestial object
 * @return          0 if successful, or -1 if one of the required arguments is NULL, or else 1 if observer location
 *                  is invalid, or an error code &lt;10 if from function place().
 *
 * @sa calc_frame_pos()
 *
 * @author Attila Kovacs
 * @since 1.0
 */
int calc_star_pos(const cat_entry *star, const observer *obs, double jd_tt, double ut1_to_tt, enum novas_reference_system system,
        enum novas_accuracy accuracy, sky_pos *pos) {
  object source = { };

  if(!star || !pos) {
    errno = EINVAL;
    return -1;
  }

  source.type = NOVAS_DISTANT_OBJECT;
  source.star = *star;

  return calc_pos(&source, obs, jd_tt, ut1_to_tt, system, accuracy, pos);
}

/**
 * Copmputes the International Celestial Reference System (ICRS) position of a source.
 *
 * @param source      Catalog source or solar_system body.
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date of observation.
 * @param accuracy    NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] pos    Calculated position data structure to populate
 * @return            0 if successful, or -1 if any of the input pointer arguments is NULL,
 *                    or else an error from calc_pos().
 *
 * @sa calc_frame_pos()
 * @sa calc_gcrs_pos()
 * @sa calc_dyncamical_pos()
 */
int calc_icrs_pos(const object *source, double jd_tt, enum novas_accuracy accuracy, sky_pos *pos) {
  return calc_pos(source, NULL, jd_tt, 0.0, NOVAS_ICRS, accuracy, pos);
}

/**
 * Copmputes the Geocentric Celestial Reference System (GCRS) position of a source.
 *
 * @param source      Catalog source or solar_system body.
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date of observation.
 * @param accuracy    NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] pos    Calculated position data structure to populate
 * @return            0 if successful, or -1 if any of the input pointer arguments is NULL,
 *                    or else an error from calc_pos().
 *
 * @sa calc_frame_pos()
 * @sa calc_icrs_pos()
 * @sa calc_dyncamical_pos()
 */
int calc_gcrs_pos(const object *source, double jd_tt, enum novas_accuracy accuracy, sky_pos *pos) {
  return calc_pos(source, NULL, jd_tt, 0.0, NOVAS_GCRS, accuracy, pos);
}

/**
 * Copmputes the Celestial Intermediate Reference System (CIRS) mean equator dynamical position
 * position of a source.
 *
 * @param source      Catalog source or solar_system body.
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date of observation.
 * @param accuracy    NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] pos    Calculated position data structure to populate
 * @return            0 if successful, or -1 if any of the input pointer arguments is NULL,
 *                    or else an error from calc_pos().
 * @sa calc_frame_pos()
 * @sa calc_icrs_pos()
 * @sa calc_gcrs_pos()
 *
 */
int calc_dynamical_pos(const object *source, double jd_tt, enum novas_accuracy accuracy, sky_pos *pos) {
  return calc_pos(source, NULL, jd_tt, 0.0, NOVAS_CIRS, accuracy, pos);
}

/**
 * Computes the apparent place of a star, referenced to dynamical equator at date 'jd_tt', given its
 * catalog mean place, proper motion, parallax, and radial velocity.
 *
 * REFERENCES:
 * <ol>
 *     <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *     <li>Explanatory Supplement to the Astronomical Almanac (1992),Chapter 3.</li>
 * </ol>
 *
 * @param jd_tt     [day] Terretrial Time (TT) based Julian date.
 * @param star      Pointer to catalog entry structure containing catalog data for
 *                  the object in the ICRS.
 * @param accuracy  NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] ra   [h] Apparent right ascension in hours, referred to true equator and equinox of date 'jd_tt' (it may be NULL if not required).
 * @param[out] dec  [deg] Apparent declination in degrees, referred to true equator and equinox of date 'jd_tt' (it may be NULL if not required).
 * @return          0 if successful, or else an error code &lt;10 if from function make_object(), or <20 if from function place().
 *
 * @sa astro_star()
 * @sa local_star()
 * @sa topo_star()
 * @sa virtual_star()
 * @sa app_planet()
 */
short app_star(double jd_tt, const cat_entry *star, enum novas_accuracy accuracy, double *ra, double *dec) {
  sky_pos output = { };
  int status;

  status = calc_star_pos(star, NULL, jd_tt, 0.0, NOVAS_TOD, accuracy, &output);

  if(ra) *ra = output.ra;
  if(dec) *dec = output.dec;
  return status;
}

/**
 * Computes the virtual place of a star, referenced to GCRS, at date 'jd_tt', given its
 * catalog mean place, proper motion, parallax, and radial velocity.
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
 * @param accuracy  NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] ra   [h] Virtual right ascension in hours, referred to the GCRS (it may be NULL if not required).
 * @param[out] dec  [deg] Virtual declination in degrees, referred to the GCRS (it may be NULL if not required).
 * @return          0 if successful, or else an error code  &lt; 10 if from function make_object(), or < 20 if from function place().
 *
 * @sa app_star()
 * @sa astro_star()
 * @sa local_star()
 * @sa topo_star()
 * @sa virtual_planet()
 */
short virtual_star(double jd_tt, const cat_entry *star, enum novas_accuracy accuracy, double *ra, double *dec) {
  sky_pos output = { };
  int status = calc_star_pos(star, NULL, jd_tt, 0.0, NOVAS_GCRS, accuracy, &output);

  if(ra) *ra = output.ra;
  if(dec) *dec = output.dec;
  return status;
}

/**
 * Computes the astrometric place of a star, referred to the ICRS without light deflection or aberration, at date 'jd_tt', given
 * its catalog mean place, proper motion, parallax, and radial velocity.
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
 * @param accuracy  NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] ra   [h] Astrometric right ascension in hours, referred to the ICRS, without light deflection or aberration. (It may be NULL if not required)
 * @param[out] dec  [deg] Astrometric declination in degrees, referred to the ICRS, without light deflection or aberration. (It may be NULL if not required)
 * @return          0 if successful, or else an error code &lt; 10 if from function make_object(), or < 20 if from function place().
 *
 * @sa app_star()
 * @sa local_star()
 * @sa topo_star()
 * @sa virtual_star()
 * @sa astro_planet()
 */
short astro_star(double jd_tt, const cat_entry *star, enum novas_accuracy accuracy, double *ra, double *dec) {
  sky_pos output = { };
  int status = calc_star_pos(star, NULL, jd_tt, 0.0, NOVAS_ICRS, accuracy, &output);

  if(ra) *ra = output.ra;
  if(dec) *dec = output.dec;
  return status;
}

/**
 * Computes the apparent place of a solar system body.
 *
 * REFERENCES:
 * <ol>
 *     <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *     <li>Explanatory Supplement to the Astronomical Almanac (1992),Chapter 3.</li>
 * </ol>
 *
 * @param jd_tt     [day] Terretrial Time (TT) based Julian date.
 * @param ss_body   Pointer to structure containing the body designation for the solar system body.
 * @param accuracy  NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] ra   [h] Apparent right ascension in hours, referred to true equator and equinox of date 'jd_tt'. (It may be NULL if not required)
 * @param[out] dec  [deg] Apparent declination in degrees, referred to true equator and equinox of date 'jd_tt'. (It may be NULL if not required)
 * @param[out] dis  [AU] True distance from Earth to the body at 'jd_tt' in AU (can be NULL if not needed).
 * @return          0 if successful, or else 1 if the value of 'type' in structure 'ss_body' is invalid, or an
 *                  error code &lt;10 if from function place().
 *
 * @sa astro_planet()
 * @sa local_planet()
 * @sa topo_planet()
 * @sa virtual_planet()
 * @sa app_star()
 */
short app_planet(double jd_tt, const object *ss_body, enum novas_accuracy accuracy, double *ra, double *dec, double *dis) {
  sky_pos output = { };
  int status = calc_pos(ss_body, NULL, jd_tt, 0.0, NOVAS_TOD, accuracy, &output);

  if(ra) *ra = output.ra;
  if(dec) *dec = output.dec;
  if(dis) *dis = output.dis;
  return status;
}

/**
 * Computes the virtual place of a solar system body, referenced to the GCRS.
 *
 * REFERENCES:
 * <ol>
 *     <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *     <li>Explanatory Supplement to the Astronomical Almanac (1992),Chapter 3.</li>
 * </ol>
 *
 * @param jd_tt     [day] Terretrial Time (TT) based Julian date.
 * @param ss_body   Pointer to structure containing the body designation for the solar system body.
 * @param accuracy  NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] ra   [h] Virtual right ascension in hours, referred to the GCRS (it may be NULL if not required).
 * @param[out] dec  [deg] Virtual declination in degrees, referred to the GCRS (it may be NULL if not required).
 * @param[out] dis  [AU] True distance from Earth to the body at 'jd_tt' in AU (can be NULL if not needed).
 * @return          0 if successful, or else 1 if the value of 'type' in structure 'ss_body' is invalid, or an
 *                  error code &lt;10 if from function place().
 *
 * @sa app_planet()
 * @sa astro_planet()
 * @sa local_planet()
 * @sa topo_planet()
 * @sa app_star()
 */
short virtual_planet(double jd_tt, const object *ss_body, enum novas_accuracy accuracy, double *ra, double *dec, double *dis) {
  sky_pos output = { };
  int status = calc_pos(ss_body, NULL, jd_tt, 0.0, NOVAS_GCRS, accuracy, &output);

  if(ra) *ra = output.ra;
  if(dec) *dec = output.dec;
  if(dis) *dis = output.dis;
  return status;
}

/**
 * Computes the astrometric place of a solar system body, referenced to the ICRS without light deflection or aberration.
 *
 * REFERENCES:
 * <ol>
 *     <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *     <li>Explanatory Supplement to the Astronomical Almanac (1992),Chapter 3.</li>
 * </ol>
 *
 * @param jd_tt     [day] Terretrial Time (TT) based Julian date.
 * @param ss_body   Pointer to structure containing the body designation for the solar system body.
 * @param accuracy  NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] ra   [h] Astrometric right ascension in hours, referred to the ICRS, without light deflection or aberration. (It may be NULL if not required)
 * @param[out] dec  [deg] Astrometric declination in degrees, referred to the ICRS, without light deflection or aberration. (It may be NULL if not required)
 * @param[out] dis  [AU] True distance from Earth to the body at 'jd_tt' in AU (can be NULL if not needed).
 * @return          0 if successful, or else 1 if the value of 'type' in structure 'ss_body' is invalid, or an
 *                  error code &lt;10 if from function place().
 *
 * @sa app_planet()
 * @sa local_planet()
 * @sa topo_planet()
 * @sa virtual_planet()
 * @sa astro_star()
 */
short astro_planet(double jd_tt, const object *ss_body, enum novas_accuracy accuracy, double *ra, double *dec, double *dis) {
  sky_pos output = { };
  int status = calc_pos(ss_body, NULL, jd_tt, 0.0, NOVAS_ICRS, accuracy, &output);

  if(ra) *ra = output.ra;
  if(dec) *dec = output.dec;
  if(dis) *dis = output.dis;
  return status;
}

/**
 * Computes the topocentric apparent place of a star at date 'jd_tt', given its catalog mean place,
 * proper motion, parallax, and radial velocity.
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
 * @param accuracy  NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] ra   [h] Topocentric right ascension in hours, referred to true equator and equinox of date 'jd_tt'. (It may be NULL if not required)
 * @param[out] dec  [deg] Topocentric declination in degrees, referred to true equator and equinox of date 'jd_tt'. (It may be NULL if not required)
 * @return          0 if successful, or else an error code &lt; 1 if 'where' in structure 'location' is invalid. 10 if from function make_object(), or < 20 if from function place().
 *
 * @sa app_star()
 * @sa local_star()
 * @sa topo_star()
 * @sa virtual_star()
 * @sa astro_planet()
 */
short topo_star(double jd_tt, double ut1_to_tt, const cat_entry *star, const on_surface *position, enum novas_accuracy accuracy,
        double *ra, double *dec) {
  sky_pos output = { };
  observer obs = { };

  obs.where = NOVAS_OBSERVER_ON_EARTH;
  obs.on_surf = *position;

  int status = calc_star_pos(star, &obs, jd_tt, ut1_to_tt, NOVAS_TOD, accuracy, &output);

  if(ra) *ra = output.ra;
  if(dec) *dec = output.dec;
  return status;
}

/**
 * Computes the local apparent place of a star at date 'jd_tt', in the GCRS, given its catalog mean place,
 * proper motion, parallax, and radial velocity.
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
 * @param accuracy  NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] ra   [h] Local right ascension in hours, referred to the GCRS (it may be NULL if not required).
 * @param[out] dec  [deg] Local right ascension in hours, referred to the GCRS (it may be NULL if not required).
 * @return          0 if successful, or else an error code &lt; 1 if 'where' in structure 'location' is invalid;
 *                  10 if from function make_object(), or < 20 if from function place().
 *
 * @sa app_star()
 * @sa astro_star()
 * @sa topo_star()
 * @sa virtual_star()
 * @sa astro_planet()
 */
short local_star(double jd_tt, double ut1_to_tt, const cat_entry *star, const on_surface *position, enum novas_accuracy accuracy,
        double *ra, double *dec) {
  sky_pos output = { };
  observer obs = { };

  obs.where = NOVAS_OBSERVER_ON_EARTH;
  obs.on_surf = *position;

  int status = calc_star_pos(star, &obs, jd_tt, ut1_to_tt, NOVAS_GCRS, accuracy, &output);

  if(ra) *ra = output.ra;
  if(dec) *dec = output.dec;
  return status;
}

/**
 * Computes the topocentric apparent place of a solar system body at the specified time.
 *
 * REFERENCES:
 * <ol>
 *     <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *     <li>Explanatory Supplement to the Astronomical Almanac (1992),Chapter 3.</li>
 * </ol>
 *
 * @param jd_tt     [day] Terretrial Time (TT) based Julian date.
 * @param ss_body   Pointer to structure containing the body designation for the solar system body.
 * @param ut1_to_tt   [s] Difference TT-UT1 at 'jd_tt', in seconds of time.
 * @param position  Position of the observer
 * @param accuracy  NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] ra   [h] Topocentric apparent right ascension in hours, referred to the true equator and equinox of date. (It may be NULL if not required)
 * @param[out] dec  [deg] Topocentric apparent declination in degrees referred to the true equator and equinox of date. (It may be NULL if not required)
 * @param[out] dis  [AU] True distance from Earth to the body at 'jd_tt' in AU (may be NULL if not needed).
 * @return          0 if successful, or else 1 if the value of 'where' in structure 'location' is invalid, or an
 *                  error code &lt;10 if from function place().
 *
 * @sa app_planet()
 * @sa local_planet()
 * @sa topo_planet()
 * @sa virtual_planet()
 * @sa astro_star()
 */
short topo_planet(double jd_tt, const object *ss_body, double ut1_to_tt, const on_surface *position, enum novas_accuracy accuracy,
        double *ra, double *dec, double *dis) {
  sky_pos output;
  observer obs = { };

  obs.where = NOVAS_OBSERVER_ON_EARTH;
  obs.on_surf = *position;

  int status = calc_pos(ss_body, &obs, jd_tt, ut1_to_tt, NOVAS_TOD, accuracy, &output);

  if(ra) *ra = output.ra;
  if(dec) *dec = output.dec;
  if(*dis) *dis = output.dis;
  return status;
}

/**
 * Computes the local apparent place of a solar system body, in the GCRS.
 *
 * REFERENCES:
 * <ol>
 *     <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *     <li>Explanatory Supplement to the Astronomical Almanac (1992),Chapter 3.</li>
 * </ol>
 *
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian date.
 * @param ss_body   Pointer to structure containing the body designation for the solar system body.
 * @param ut1_to_tt   [s] Difference TT-UT1 at 'jd_tt', in seconds of time.
 * @param position  Position of the observer
 * @param accuracy  NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] ra   [h] Local right ascension in hours, referred to the GCRS (it may be NULL if not required).
 * @param[out] dec  [deg] Local right ascension in hours, referred to the GCRS (it may be NULL if not required).
 * @param[out] dis  [AU] True distance from Earth to the body at 'jd_tt' in AU (it may be NULL if not required).
 * @return          0 if successful, or else 1 if the value of 'where' in structure 'location' is invalid, or an
 *                  error code &lt;10 if from function place().
 *
 * @sa astro_planet()
 * @sa topo_planet()
 * @sa virtual_planet()
 * @sa app_star()
 */
short local_planet(double jd_tt, const object *ss_body, double ut1_to_tt, const on_surface *position, enum novas_accuracy accuracy,
        double *ra, double *dec, double *dis) {
  sky_pos output;
  observer obs = { };

  obs.where = NOVAS_OBSERVER_ON_EARTH;
  obs.on_surf = *position;

  int status = calc_pos(ss_body, &obs, jd_tt, ut1_to_tt, NOVAS_GCRS, accuracy, &output);

  if(ra) *ra = output.ra;
  if(dec) *dec = output.dec;
  if(*dis) *dis = output.dis;
  return status;
}

/**
 * Computes the ICRS position of a star, given its apparent place at date 'jd_tt'.  Proper motion, parallax and radial
 * velocity are assumed to be zero.
 *
 * REFERENCES:
 * <ol>
 *     <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *     <li>Explanatory Supplement to the Astronomical Almanac (1992),Chapter 3.</li>
 * </ol>
 *
 * @param jd_tt         [day] Terrestrial Time (TT) based Julian date.
 * @param ra            [h]  Apparent right ascension in hours, referred to true equator and equinox of date.
 * @param dec           [deg] Apparent declination in degrees, referred to true equator and equinox of date.
 * @param accuracy      NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] ira      [h] ICRS right ascension in hours.
 * @param[out] idec     [deg] ICRS declination in degrees.
 * @return          0 if successful; -1 if the supplied output pointers are NULL, 1 if the iterative process did not
 *                  converge after 30 iterations, &gt; 10 if rror from vector2radec(), or else &gt; 20 if error from function app_star().
 *
 */
short mean_star(double jd_tt, double ra, double dec, enum novas_accuracy accuracy, double *ira, double *idec) {
  short error = 0;
  int iter = 0;

  double pos[3], dum[3], pos2[3], newira, newidec, oldira, oldidec, ra2, dec2;

  cat_entry tempstar;

  if(!ira || !idec) {
    errno = EINVAL;
    return -1;
  }

  // Set up the 'tempstar' structure, then use it to create a position
  // vector based on the apparent RA and declination of the star.
  make_cat_entry("dummy", "CAT", 0, ra, dec, 0.0, 0.0, 0.0, 0.0, &tempstar);
  starvectors(&tempstar, pos, dum);

  // Get initial approximation by precessing star position at 'jd_tt'
  // to its position at J2000.0.
  precession(jd_tt, pos, JD_J2000, pos2);
  error = vector2radec(pos2, &newira, &newidec);
  if(error) return (error + 10);

  // Iteratively find ICRS coordinates that produce input apparent place
  // of star at date 'jd_tt'.
  do {
    double deltara, deltadec;

    oldira = newira;
    oldidec = newidec;
    tempstar.ra = oldira;
    tempstar.dec = oldidec;

    error = app_star(jd_tt, &tempstar, accuracy, &ra2, &dec2);
    if(error) {
      *ira = 0.0;
      *idec = 0.0;
      return (error + 20);
    }

    deltara = ra2 - oldira;
    deltadec = dec2 - oldidec;
    if(deltara < -12.0) deltara += DAY_HOURS;
    if(deltara > 12.0) deltara -= DAY_HOURS;
    newira = ra - deltara;
    newidec = dec - deltadec;

    if(iter >= 30) {
      *ira = 0.0;
      *idec = 0.0;
      return 1;
    }
    else {
      iter++;
    }
  }
  while(!(fabs(newira - oldira) <= 1.0e-12) || !(fabs(newidec - oldidec) <= 1.0e-11));

  *ira = newira;
  *idec = newidec;
  if(*ira < 0.0) *ira += DAY_HOURS;
  if(*ira >= DAY_HOURS) *ira -= DAY_HOURS;

  return 0;
}

/**
 * Computes the apparent direction of a star or solar system body at a specified time and in a specified coordinate system.
 *
 * The values of <code>location->where</code> and <code>coord_sys</code> dictate the various standard kinds of place. For example,
 *
 *  <table>
 *  <tr><th><code>location->where</code> </th><th><code>coord_sys</code></th> <th>frame</th></tr>
 *
 *  <tr><td><code>NOVAS_OBSERVER_AT_GEOCENTER</code></td> <td><code>NOVAS_TOD</code></td> <td>apparent place</td></tr>
 *  <tr><td><code>NOVAS_OBSERVER_ON_EARTH</code></td> <td><code>NOVAS_TOD</code></td> <td>topocentric place</td></tr>
 *  <tr><td><code>NOVAS_OBSERVER_AT_GEOCENTER</code></td> <td><code>NOVAS_GCRS</code></td> <td>virtial place</td></tr>
 *  <tr><td><code>NOVAS_OBSERVER_ON_EARTH</code></td> <td><code>NOVAS_GCRS</code></td> <td>local place</td></tr>
 *  <tr><td><code>NOVAS_OBSERVER_AT_GEOCENTER</code></td> <td><code>NOVAS_ICRS</code></td> <td>astrometric place</td></tr>
 *  <tr><td><code>NOVAS_OBSERVER_ON_EARTH</code></td> <td><code>NOVAS_ICRS</code></td> <td>topocentric astrometric place</td></tr>
 *  </table>
 *
 * NOTES:
 * <ol>
 * <li>This version fixes a NOVAS C 3.1 issue that velocities were not antedated for light-travel time.</li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 *     <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *     <li>Explanatory Supplement to the Astronomical Almanac (1992),Chapter 3.</li>
 * </ol>
 *
 * @param jd_tt         [day] Terrestrial Time (TT) based Julian date.
 * @param cel_object    Pointer to a celestrial object data structure
 * @param location      The observer location
 * @param ut1_to_tt     [s] TT - UT1 time difference. Used only when 'location->where' is NOVAS_OBSERVER_ON_EARTH or
 *                      NOVAS_OBSERVER_IN_EARTH_ORBIT.
 * @param coord_sys     The astrometric reference system type in which to return coordinates
 * @param accuracy      NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] output   Data structure to populate with the result.
 * @return              0 if successful, 1 if 'coord_sys' is invalid, 2 if 'accuracy' is invalid,
 *                      3 if Earth is the observed object, and the observer is either at the geocenter or on the Earth's
 *                      surface, 10--40: error is 10 + the error ephemeris(), 40--50: error is 40 + the error from geo_posvel(),
 *                      50--70 error is 50 + error from light_time(), 70--80 error is 70 + error from grav_def(),
 *                      80--90 errro is 80 + error from cio_location(), 90--100 error is 90 + error from cio_basis().
 */
short place(double jd_tt, const object *cel_object, const observer *location, double ut1_to_tt, enum novas_reference_system coord_sys,
        enum novas_accuracy accuracy, sky_pos *output) {
  static object earth, sun;

  static int first_time = 1;
  static enum novas_accuracy acc_last = -1;
  static double tlast1 = 0.0;
  static double tlast2 = 0.0;
  static double peb[3], veb[3], psb[3], px[3], py[3], pz[3];

  enum novas_observer_place loc;
  double x, jd_tdb, jd[2], pog[3], vog[3], pob[3], vob[3], pos1[3], vel1[3], pos2[3], pos3[3], t_light, t_light0, frlimb;
  double pos5[3], pos8[3], r_cio;

  short rs;
  int i, error = 0;

  // Check for invalid value of 'coord_sys' or 'accuracy'.
  if((coord_sys < 0) || (coord_sys >= NOVAS_REFERENCE_SYSTEMS)) return 1;

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY) return 2;

  // Create a null star 'cat_entry' and  Earth and Sun 'object's.
  if(first_time) {
    make_object(NOVAS_MAJOR_PLANET, NOVAS_EARTH, "Earth", NULL, &earth);
    make_object(NOVAS_MAJOR_PLANET, NOVAS_SUN, "Sun", NULL, &sun);
    first_time = 0;
  }

  // ---------------------------------------------------------------------
  // Check on Earth as an observed object.  Earth can only be an observed
  // object when 'location' is a near-Earth satellite.
  // ---------------------------------------------------------------------
  if((cel_object->type == NOVAS_MAJOR_PLANET) && (cel_object->number == NOVAS_EARTH) && (location->where != NOVAS_OBSERVER_IN_EARTH_ORBIT))
    return 3;

  // Compute 'jd_tdb', the TDB Julian date corresponding to 'jd_tt'.
  jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY;

  // ---------------------------------------------------------------------
  // Get position and velocity of Earth (geocenter) and Sun.
  // ---------------------------------------------------------------------
  if(!time_equals(jd_tt, tlast1) || accuracy != acc_last) {
    double vsb[3];

    // Get position and velocity of Earth wrt barycenter of solar system,
    // in ICRS.
    jd[0] = jd_tdb;
    jd[1] = 0.0;

    error = ephemeris(jd, &earth, NOVAS_BARYCENTER, accuracy, peb, veb);
    if(error) return error + 10;

    // Get position and velocity of Sun wrt barycenter of solar system,
    // in ICRS.
    error = ephemeris(jd, &sun, NOVAS_BARYCENTER, accuracy, psb, vsb);
    if(error) return error + 10;

    tlast1 = jd_tt;
    acc_last = accuracy;
  }

  // ---------------------------------------------------------------------
  // Get position and velocity of observer.
  // ---------------------------------------------------------------------
  if((location->where == NOVAS_OBSERVER_ON_EARTH) || (location->where == NOVAS_OBSERVER_IN_EARTH_ORBIT)) {
    // For topocentric place, get geocentric position and velocity vectors
    // of observer (observer is on surface of Earth or in a near-Earth
    // satellite).
    error = geo_posvel(jd_tt, ut1_to_tt, accuracy, location, pog, vog);
    if(error) return error + 40;

    loc = NOVAS_OBSERVER_ON_EARTH;
  }
  else {
    // For geocentric place, there is nothing to do (observer is at
    // geocenter).
    memset(pog, 0, sizeof(pog));
    memset(vog, 0, sizeof(vog));

    loc = NOVAS_OBSERVER_AT_GEOCENTER;
  }

  // Compute position and velocity of observer wrt barycenter of
  // solar system (Galilean transformation).
  for(i = 0; i < 3; i++) {
    pob[i] = peb[i] + pog[i];
    vob[i] = veb[i] + vog[i];
  }


  // ---------------------------------------------------------------------
  // Find geometric position of observed object.
  // ---------------------------------------------------------------------
  if(cel_object->type == NOVAS_DISTANT_OBJECT) // Observed object is star.
  {
    double dt = 0.0;

    // Get position of star updated for its space motion.
    starvectors(&cel_object->star, pos1, vel1);

    dt = d_light(pos1, pob);
    proper_motion(JD_J2000, pos1, vel1, (jd_tdb + dt), pos2);

    // Get position of star wrt observer (corrected for parallax).
    bary2obs(pos2, pob, pos3, &t_light);
    output->dis = 0.0;
  }
  else // Observed object is solar system body.
  {
    // Get position of body wrt barycenter of solar system.
    jd[0] = jd_tdb;
    jd[1] = 0.0;

    error = ephemeris(jd, cel_object, NOVAS_BARYCENTER, accuracy, pos1, vel1);
    if(error) return (error + 10);

    // Get position of body wrt observer, and true (Euclidian) distance.
    bary2obs(pos1, pob, pos2, &t_light0);
    output->dis = t_light0 * C_AUDAY;

    // Get position of body wrt observer, antedated for light-time.
    error = light_time(jd_tdb, cel_object, pob, t_light0, accuracy, pos3, &t_light);
    if(error) return (error + 50);

    // AK: Fix for antedating velocities...
    jd[0] -= t_light;
    error = ephemeris(jd, cel_object, NOVAS_BARYCENTER, accuracy, pos1, vel1);
    if(error) return (error + 50);
  }

  // ---------------------------------------------------------------------
  // Apply gravitational deflection of light and aberration.
  // ---------------------------------------------------------------------
  if(coord_sys == NOVAS_ICRS) {
    // These calculations are skipped for astrometric place.
    memcpy(pos5, pos3, sizeof(pos5));
  }

  else {
    double pos4[3];

    // Variable 'loc' determines whether Earth deflection is included.
    if(loc == NOVAS_OBSERVER_ON_EARTH) {
      limb_angle(pos3, pog, NULL, &frlimb);
      if(frlimb < 0.8) loc = NOVAS_OBSERVER_AT_GEOCENTER;
    }

    // Compute gravitational deflection and aberration.
    error = grav_def(jd_tdb, loc, accuracy, pos3, pob, pos4);
    if(error) return (error + 70);

    aberration(pos4, vob, t_light, pos5);
  }

  // ---------------------------------------------------------------------
  // Transform, if necessary, to output coordinate system.
  // ---------------------------------------------------------------------
  switch(coord_sys) {
    case (NOVAS_TOD): // Transform to equator and equinox of date.
      icrs_to_tod(jd_tdb, accuracy, pos5, pos8);
      break;

    case (NOVAS_CIRS): // Transform to equator and CIO of date.
      if(!time_equals(jd_tdb, tlast2) || accuracy != acc_last) {

        // Obtain the basis vectors, in the GCRS, of the celestial intermediate
        // system.
        error = cio_location(jd_tdb, accuracy, &r_cio, &rs);
        if(error) return (error + 80);
        error = cio_basis(jd_tdb, r_cio, rs, accuracy, px, py, pz);
        if(error) return (error + 90);

        tlast2 = jd_tdb;
        acc_last = accuracy;
      }

      // Transform position vector to celestial intermediate system.
      pos8[0] = vdot(px, pos5);
      pos8[1] = vdot(py, pos5);
      pos8[2] = vdot(pz, pos5);
      break;

    default:
      // No transformation -- keep coordinates in GCRS
      // or ICRS for astrometric coordinates.
      memcpy(pos8, pos5, sizeof(pos8));
  }


  // ---------------------------------------------------------------------
  // Compute radial velocity.
  // ---------------------------------------------------------------------
  rad_vel(cel_object, pos3, vel1, vob, vdist(pob, peb), vdist(pob, psb), vdist(pos1, psb), &output->rv);

  // ---------------------------------------------------------------------
  // Finish up.
  // ---------------------------------------------------------------------
  vector2radec(pos8, &output->ra, &output->dec);

  x = vlen(pos8);

  for(i = 0; i < 3; i++) {
    output->r_hat[i] = pos8[i] / x;
  }

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
 * @param rai         [h] ICRS right ascension in hours.
 * @param deci        [deg] ICRS declination in degrees.
 * @param[out] glon   [deg] Galactic longitude in degrees.
 * @param[out] glat   [deg] Galactic latitude in degrees.
 *
 * @return            0 if successful, or -1 if either of the output pointer arguments are NULL.
 *
 */
int equ2gal(double rai, double deci, double *glon, double *glat) {
  double r, d, pos1[3], pos2[3], xyproj, g;

  // Rotation matrix A_g from Hipparcos documentation eq. 1.5.11.
  // AK: Transposed compared to NOVAS C 3.1 for dot product handling.
  static const double ag[3][3] = { { -0.0548755604, -0.8734370902, -0.4838350155 }, //
          { +0.4941094279, -0.4448296300, +0.7469822445 }, //
          { -0.8676661490, -0.1980763734, +0.4559837762 }, //
          };

  if(!glon || !glat) {
    errno = EINVAL;
    return -1;
  }

  // Form position vector in equatorial system from input coordinates
  r = rai * HOURANGLE;
  d = deci * DEGREE;
  pos1[0] = cos(d) * cos(r);
  pos1[1] = cos(d) * sin(r);
  pos1[2] = sin(d);

  // Rotate position vector to galactic system, using Hipparcos documentation eq. 1.5.13.
  pos2[0] = vdot(ag[0], pos1);
  pos2[1] = vdot(ag[1], pos1);
  pos2[2] = vdot(ag[2], pos1);

  // Decompose galactic vector into longitude and latitude.
  xyproj = sqrt(pos2[0] * pos2[0] + pos2[1] * pos2[1]);

  if(xyproj > 0.0) g = atan2(pos2[1], pos2[0]);
  else g = 0.0;

  *glon = g / DEGREE;
  if(*glon < 0.0) *glon += DEG360;

  *glat = atan2(pos2[2], xyproj) / DEGREE;

  return 0;
}

/**
 * Convert right ascension and declination to ecliptic longitude and latitude.  To convert ICRS RA and dec to ecliptic coordinates
 * (mean ecliptic and equinox of J2000.0), set 'coord_sys' to NOVAS_ICRS_EQUATOR; in this case the value of 'jd_tt' can be set to anything,
 * since J2000.0 is assumed. Otherwise, all input coordinates are dynamical at 'jd_tt'.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date. (Unused if 'coord_sys' is NOVAS_ICRS_EQUATOR)
 * @param coord_sys   The astrometric reference system type in which to return coordinates
 * @param accuracy    NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param ra          [h] Right ascension in hours, referred to specified equator and equinox of date.
 * @param dec         [deg] Declination in degrees, referred to specified equator and equinox of date.
 * @param elon        [deg] Ecliptic longitude in degrees, referred to specified ecliptic and equinox of date.
 * @param elat        [deg] Ecliptic latitude in degrees, referred to specified ecliptic and equinox of date.
 * @return            0 if successful, or else 1 if the value of 'coord_sys' is invalid.
 */
short equ2ecl(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double ra, double dec, double *elon,
        double *elat) {

  double r, d, pos1[3], pos2[3], xyproj, e;
  int error;

  // Form position vector in equatorial system from input coordinates.
  r = ra * HOURANGLE;
  d = dec * DEGREE;
  pos1[0] = cos(d) * cos(r);
  pos1[1] = cos(d) * sin(r);
  pos1[2] = sin(d);

  // Convert the vector from equatorial to ecliptic system.
  error = equ2ecl_vec(jd_tt, coord_sys, accuracy, pos1, pos2);
  if(error) return error;

  // Decompose ecliptic vector into ecliptic longitude and latitude.
  xyproj = sqrt(pos2[0] * pos2[0] + pos2[1] * pos2[1]);

  if(xyproj > 0.0) e = atan2(pos2[1], pos2[0]);
  else e = 0.0;

  *elon = e / DEGREE;
  if(*elon < 0.0) *elon += DEG360;

  e = atan2(pos2[2], xyproj);
  *elat = e / DEGREE;

  return 0;
}

/**
 * Converts an equatorial position vector to an ecliptic position vector. To convert ICRS RA and dec to ecliptic coordinates
 * (mean ecliptic and equinox of J2000.0), set 'coord_sys' to NOVAS_ICRS_EQUATOR; in this case the value of 'jd_tt' can be set to anything,
 * since J2000.0 is assumed. Otherwise, all input coordinates are dynamical at 'jd_tt'.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date. (Unused if 'coord_sys' is NOVAS_ICRS_EQUATOR)
 * @param coord_sys   The astrometric reference system type in which to return coordinates
 * @param accuracy    NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param pos1        Position vector, referred to specified equator and equinox of date.
 * @param[out] pos2   Position vector, referred to specified ecliptic and equinox of date.
 * @return            0 if successful, or else 1 if the value of 'coord_sys' is invalid.
 *
 * @sa ecl2equ_vec()
 * @sa equ2ecl()
 */
short equ2ecl_vec(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, const double *pos1, double *pos2) {
  static enum novas_accuracy acc_last = -1;
  static double t_last = 0.0, oblm, oblt, ob2000;

  const double jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY; // TDB date
  double pos0[3], obl;

  // Get obliquity, depending upon the "system" of the input coordinates.
  switch(coord_sys) {
    case NOVAS_MEAN_EQUATOR: // Input: mean equator and equinox of date
    case NOVAS_TRUE_EQUATOR: // Input: true equator and equinox of date
      memcpy(pos0, pos1, sizeof(pos0));

      if(!time_equals(jd_tt, t_last) || accuracy != acc_last) {
        e_tilt(jd_tdb, accuracy, &oblm, &oblt, NULL, NULL, NULL);
        t_last = jd_tt;
        acc_last = accuracy;
      }

      obl = (coord_sys == NOVAS_MEAN_EQUATOR ? oblm : oblt) * DEGREE;
      break;

    case NOVAS_ICRS_EQUATOR: /* Input: ICRS */
      frame_tie(pos1, TIE_ICRS_TO_J2000, pos0);

      if(ob2000 == 0.0) {
        e_tilt(JD_J2000, accuracy, &oblm, NULL, NULL, NULL, NULL);
        ob2000 = oblm;
      }
      obl = ob2000 * DEGREE;
      break;

    default:
      return 1;
  }

  // Rotate position vector to ecliptic system.
  pos2[0] = pos0[0];
  pos2[1] = pos0[1] * cos(obl) + pos0[2] * sin(obl);
  pos2[2] = -pos0[1] * sin(obl) + pos0[2] * cos(obl);

  return 0;
}

/**
 * Converts an ecliptic position vector to an equatorial position vector. To convert ecliptic coordinates
 * (mean ecliptic and equinox of J2000.0) to ICRS RA and dec to, set 'coord_sys' to NOVAS_ICRS_EQUATOR; in this case the value of
 * 'jd_tt' can be set to anything, since J2000.0 is assumed. Otherwise, all input coordinates are dynamical at 'jd_tt'.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date. (Unused if 'coord_sys' is NOVAS_ICRS_EQUATOR)
 * @param coord_sys   The astrometric reference system type in which to return coordinates
 * @param accuracy    NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param pos1        Position vector, referred to specified ecliptic and equinox of date.
 * @param[out] pos2   Position vector, referred to specified equator and equinox of date.
 * @return            0 if successful, or else 1 if the value of 'coord_sys' is invalid.
 *
 * @sa equ2ecl_vec()
 */
short ecl2equ_vec(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, const double *pos1, double *pos2) {
  static enum novas_accuracy acc_last = -1;
  static double t_last = 0.0, ob2000, oblm, oblt;

  const double jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY;    // TDB date
  double obl = 0.0;

  // Get obliquity, depending upon the "system" of the input coordinates.
  switch(coord_sys) {
    case NOVAS_MEAN_EQUATOR: // Output: mean equator and equinox of date
    case NOVAS_TRUE_EQUATOR: // Output: true equator and equinox of date
      if(!time_equals(jd_tt, t_last) || accuracy != acc_last) {
        e_tilt(jd_tdb, accuracy, &oblm, &oblt, NULL, NULL, NULL);
        t_last = jd_tt;
        acc_last = accuracy;
      }

      obl = (coord_sys == NOVAS_MEAN_EQUATOR ? oblm : oblt) * DEGREE;
      break;

    case NOVAS_ICRS_EQUATOR: /* Output: ICRS */
      if(ob2000 == 0.0) {
        e_tilt(JD_J2000, accuracy, &oblm, NULL, NULL, NULL, NULL);
        ob2000 = oblm;
      }
      obl = ob2000 * DEGREE;
      break;

    default:
      return 1;
  }

  // Rotate position vector to ecliptic system.
  pos2[0] = pos1[0];
  pos2[1] = pos1[1] * cos(obl) - pos1[2] * sin(obl);
  pos2[2] = pos1[1] * sin(obl) + pos1[2] * cos(obl);

  // Case where output vector is to be in ICRS, rotate from dynamical
  // system to ICRS.
  if(coord_sys == NOVAS_ICRS_EQUATOR) {
    double pos0[3];
    memcpy(pos0, pos2, sizeof(pos0));
    frame_tie(pos0, TIE_J2000_TO_ICRS, pos2);
  }

  return 0;
}

/**
 * Transforms topocentric right ascension and declination to zenith distance and azimuth.  It uses a method
 * that properly accounts for polar motion, which is significant at the sub-arcsecond level.  This function can also adjust
 * coordinates for atmospheric refraction.
 *
 * NOTES:
 * <ul>
 *  <li>'xp' and 'yp' can be set to zero if sub-arcsecond accuracy is not needed. 'ra' and 'dec' can be obtained
 *  from tpstar() or tpplan().</li>
 *  <li> The directions 'zd'= 0 (zenith) and 'az'= 0 (north) are here considered fixed in the terrestrial system.
 *  Specifically, the zenith is along the geodetic normal, and north is toward the ITRS pole.</li>
 *  <li>If 'ref_option' is NOVAS_STANDARD_ATMOSPHERE, then 'rar'='ra' and 'decr'='dec'.
 * </ul>
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. (2008). USNO/AA Technical Note of 28 Apr 2008, "Refraction as a Vector."</li>
 * </ol>
 *
 * @param jd_ut1      [day] UT1 based Julian date
 * @param ut1_to_tt   [s] TT - UT1 Time difference in seconds
 * @param accuracy    NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param xp          [arcsec] Conventionally-defined x coordinate of celestial intermediate pole with respect to ITRS reference pole, in arcseconds.
 * @param yp          [arcsec] Conventionally-defined y coordinate of celestial intermediate pole with respect to ITRS reference pole, in arcseconds.
 * @param location    The observer location
 * @param ra          [h] Topocentric right ascension of object of interest, in hours, referred to true equator and equinox of date.
 * @param dec         [deg] Topocentric declination of object of interest, in degrees, referred to true equator and equinox of date.
 * @param ref_option  (boolean) Whether to include refraction correction for the standard atmosphere
 * @param[out] zd     [deg] Topocentric zenith distance in degrees, affected by refraction if 'ref_option' is non-zero.
 * @param[out] az     [deg] Topocentric azimuth (measured east from north) in degrees.
 * @param[out] rar    [h] Topocentric right ascension of object of interest, in hours, referred to true equator and
 *                    equinox of date, affected by refraction if 'ref_option' is non-zero. (It may be NULL if not required)
 * @param[out] decr   [deg] Topocentric declination of object of interest, in degrees, referred to true equator and equinox of date.
 *                    (It may be NULL if not required)
 * @return            0 if successful, or -1 if one of the 'zd' or 'az' output pointers are NULL.
 */
int equ2hor(double jd_ut1, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const on_surface *location, double ra,
        double dec, short ref_option, double *zd, double *az, double *rar, double *decr) {

  double sinlat, coslat, sinlon, coslon, sindc, cosdc, sinra, cosra;
  double uze[3], une[3], uwe[3], uz[3], un[3], uw[3], p[3];
  double pz, pn, pw, proj, pr[3];

  if(!zd || !az) {
    errno = EINVAL;
    return -1;
  }

  if(rar) *rar = ra;
  if(decr) *decr = dec;

  sinlat = sin(location->latitude * DEGREE);
  coslat = cos(location->latitude * DEGREE);
  sinlon = sin(location->longitude * DEGREE);
  coslon = cos(location->longitude * DEGREE);

  ra *= HOURANGLE;
  dec *= DEGREE;

  sindc = sin(dec);
  cosdc = cos(dec);
  sinra = sin(ra);
  cosra = cos(ra);

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
  ter2cel(jd_ut1, 0.0, ut1_to_tt, 1, accuracy, 1, xp, yp, uze, uz);
  ter2cel(jd_ut1, 0.0, ut1_to_tt, 1, accuracy, 1, xp, yp, une, un);
  ter2cel(jd_ut1, 0.0, ut1_to_tt, 1, accuracy, 1, xp, yp, uwe, uw);

  // Define unit vector 'p' toward object in celestial system
  // (wrt equator and equinox of date).
  p[0] = cosdc * cosra;
  p[1] = cosdc * sinra;
  p[2] = sindc;

  // Compute coordinates of object wrt orthonormal basis.

  // Compute components of 'p' - projections of 'p' onto rotated
  // Earth-fixed basis vectors.
  pz = vdot(p, uz);
  pn = vdot(p, un);
  pw = vdot(p, uw);

  // Compute azimuth and zenith distance.
  proj = sqrt(pn * pn + pw * pw);
  if(proj > 0.0) *az = -atan2(pw, pn) / DEGREE;

  if(*az < 0.0) *az += DEG360;
  else if(*az >= DEG360) *az -= DEG360;

  *zd = atan2(proj, pz) / DEGREE;

  // Apply atmospheric refraction if requested.
  if(ref_option) {

    // Get refraction in zenith distance.

    // Iterative process is required because refraction algorithms are
    // always a function of observed (not computed) zenith distance.
    // Require convergence to 0.1 arcsec (actual accuracy less).
    double refr;

    const double zd0 = *zd;
    double zd1;

    do {
      zd1 = *zd;
      refr = refract(location, ref_option, *zd);
      *zd = zd0 - refr;
    }
    while(fabs(*zd - zd1) > 3.0e-5);

    // Apply refraction to celestial coordinates of object.

    if((refr > 0.0) && (*zd > 3.0e-4)) {
      // Shift position vector of object in celestial system to account
      // for refraction (see USNO/AA Technical Note 1998-09).
      const double sinzd = sin(*zd * DEGREE);
      const double coszd = cos(*zd * DEGREE);
      const double sinzd0 = sin(zd0 * DEGREE);
      const double coszd0 = cos(zd0 * DEGREE);

      int j;

      // Compute refracted position vector.
      for(j = 0; j < 3; j++)
        pr[j] = ((p[j] - coszd0 * uz[j]) / sinzd0) * sinzd + uz[j] * coszd;

      // Compute refracted right ascension and declination.
      proj = sqrt(pr[0] * pr[0] + pr[1] * pr[1]);

      if(rar) {
        if(proj > 0.0) *rar = atan2(pr[1], pr[0]) / HOURANGLE;
        if(*rar < 0.0) *rar += DAY_HOURS;
        if(*rar >= DAY_HOURS) *rar -= DAY_HOURS;
      }

      if(decr) *decr = atan2(pr[2], proj) / DEGREE;
    }
  }
  return 0;
}

/**
 * Converts GCRS right ascension and declination to coordinates with respect to the equator of date (mean or true).
 * For coordinates with respect to the true equator of date, the origin of right ascension can be either the true equinox or the
 * celestial intermediate origin (CIO). This function only supports the CIO-based method.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date. (Unused if 'coord_sys' is NOVAS_ICRS_EQUATOR)
 * @param coord_sys   Dynamical equatorial system type
 * @param accuracy    NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY (unused if 'coord_sys' is not NOVAS_ICRS)
 * @param rag         [h] GCRS right ascension in hours.
 * @param decg        [deg] GCRS declination in degrees.
 * @param[out] ra     [h] Right ascension in hours, referred to specified equator and right ascension origin of date.
 * @param[out] dec    [deg] Declination in degrees, referred to specified equator of date.
 * @return            0 if successful, or -1 with errno set to EINVAL if the output pointers are NULL, otherwise
 *                    <0 if an error from vector2radec(), 10--20 error is  10 + error cio_location(); or else
 *                    20 + error from cio_basis()
 */
short gcrs2equ(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double rag, double decg, double *ra,
        double *dec) {
  double t1, r, d, pos1[3], pos2[3], r_cio, x[3], y[3], z[3];
  short rs;
  int error = 0;

  if(!ra || !dec) {
    errno = EINVAL;
    return -1;
  }

  // 't1' is the TDB Julian date.
  t1 = jd_tt + tt2tdb(jd_tt) / DAY;

  // Form position vector in equatorial system from input coordinates.
  r = rag * 15.0 * DEG2RAD;
  d = decg * DEG2RAD;

  pos1[0] = cos(d) * cos(r);
  pos1[1] = cos(d) * sin(r);
  pos1[2] = sin(d);


  // Transform the position vector based on the value of 'coord_sys'.

  if(coord_sys <= 1) {
    // Transform the position vector from GCRS to mean equator and equinox
    // of date.

    // If requested, transform further to true equator and equinox of date.
    if(coord_sys == NOVAS_TRUE_EQUATOR) {
      icrs_to_tod(t1, accuracy, pos1, pos2);
    }
    else {
      double pos3[3];
      frame_tie(pos1, TIE_ICRS_TO_J2000, pos3);
      precession(JD_J2000, pos3, t1, pos2);
    }
  }
  else {
    // Obtain the basis vectors, in the GCRS, of the celestial intermediate
    // system.
    error = cio_location(t1, accuracy, &r_cio, &rs);
    if(error) return (error + 10);

    error = cio_basis(t1, r_cio, rs, accuracy, x, y, z);
    if(error) return (error + 20);

    // Transform position vector to the celestial intermediate system
    // (which has the CIO as its origin of right ascension).
    pos2[0] = vdot(x, pos1);
    pos2[1] = vdot(y, pos1);
    pos2[2] = vdot(z, pos1);
  }

  // Convert the position vector to equatorial spherical coordinates.
  error = vector2radec(pos2, ra, dec);
  if(error) {
    *ra = 0.0;
    *dec = 0.0;
    return (-error);
  }

  return 0;
}

/**
 * Computes the Greenwich sidereal time, either mean or apparent, at the specified Julian date. The Julian date
 * can be broken into two parts if convenient, but for the highest precision, set 'jd_high' to be the integral
 * part of the Julian date, and set 'jd_low' to be the fractional part.
 *
 * NOTES:
 * <ol>
 * <li>Contains fix for known <a href="https://aa.usno.navy.mil/software/novas_faq">sidereal time units bug.</a></li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 * </ol>
 *
 * @param jd_high     [day] High-order part of UT1 Julian date.
 * @param jd_low      [day] Low-order part of UT1 Julian date. (You can leave it at zero if 'jd_high' specified
 *                    the date with sufficient precision)
 * @param ut1_to_tt   [s] TT - UT1 Time difference in seconds
 * @param gst_type    NOVAS_MEAN_EQUINOX or NOVAS_TRUE_EQUINOX, depending on whether wanting mean or apparent
 *                    GST, respectively.
 * @param method      EROT_GST or EROT_ERA, depending on whether to use GST relative to equinox of date (pre
 *                    IAU 2006) or ERA relative to the CIO (IAU 2006 standard).
 * @param accuracy    NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] gst    [h] Greenwich (mean or apparent) sidereal time, in hours. (In case the returned error
 *                    code is &gt;1 the gst value will be set to NAN.
 * @return            0 if successful, or -1 if the 'gst' argument is NULL, 1 if 'accuracy' is invalid
 *                    2 if 'method' is invalid, or else 10--30 with 10 + the error from cio_rai().
 */
short sidereal_time(double jd_high, double jd_low, double ut1_to_tt, enum novas_equinox_type gst_type,
        enum novas_earth_rotation_measure method, enum novas_accuracy accuracy, double *gst) {

  double jd_ut, jd_tt, jd_tdb, t, theta, st, eqeq;

  if(!gst) {
    errno = EINVAL;
    return -1;
  }

  *gst = NAN;

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY) return 1;

  // Time argument for precession and nutation components of sidereal
  // time is TDB.  First approximation is TDB = TT, then refine.
  jd_ut = jd_high + jd_low;
  jd_tt = jd_ut + (ut1_to_tt / DAY);
  jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY;

  t = (jd_tdb - JD_J2000) / JULIAN_CENTURY_DAYS;

  // Compute the Earth Rotation Angle.  Time argument is UT1.
  theta = era(jd_high, jd_low);

  // Compute the equation of the equinoxes if needed, depending upon the
  // input values of 'gst_type' and 'method'.  If not needed, set to zero.
  if(((gst_type == NOVAS_MEAN_EQUINOX) && (method == EROT_GST)) || /* GMST; CIO-TIO */
  ((gst_type == NOVAS_TRUE_EQUINOX) && (method == EROT_ERA))) /* GAST; equinox */
  {
    static enum novas_accuracy acc_last = -1;
    static double jd_last = -1e100;
    static double ee;

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

  // Compute Greenwich sidereal time depending upon input values of
  // method' and 'gst_type'.
  switch(method) {
    case (EROT_GST): {
      // Use 'CIO-TIO-theta' method.  See Circular 179, Section 6.5.4.
      const double ux[3] = { 1.0, 0.0, 0.0 };
      double ra_cio, ha_eq, x[3], y[3], z[3], w1[3], w2[3], eq[3];
      short ref_sys;

      // Obtain the basis vectors, in the GCRS, of the celestial intermediate system.
      int error = cio_location(jd_tdb, accuracy, &ra_cio, &ref_sys);
      if(error) {
        *gst = 99.0;
        return (error + 10);
      }

      cio_basis(jd_tdb, ra_cio, ref_sys, accuracy, x, y, z);

      // Compute the direction of the true equinox in the GCRS.
      nutation(jd_tdb, NUTATE_TRUE_TO_MEAN, accuracy, ux, w1);
      precession(jd_tdb, w1, JD_J2000, w2);
      frame_tie(w2, TIE_J2000_TO_ICRS, eq);

      // Compute the hour angle of the equinox wrt the TIO meridian
      // (near Greenwich, but passes through the CIP and TIO).
      ha_eq = theta - atan2(vdot(eq, y), vdot(eq, x)) / DEGREE;

      // For mean sidereal time, subtract the equation of the equinoxes.

      // AK: Fix for documented bug in NOVAS 3.1 --> 3.1.1
      ha_eq -= (eqeq / 3600.0);

      ha_eq = fmod(ha_eq, DEG360) / 15.0;
      if(ha_eq < 0.0) ha_eq += DAY_HOURS;
      *gst = ha_eq;
      return 0;
    }

    case (EROT_ERA):

      // Use equinox method.  See Circular 179, Section 2.6.2.

      // Precession-in-RA terms in mean sidereal time taken from third
      // reference, eq. (42), with coefficients in arcseconds.
      st = eqeq + 0.014506 + ((((-0.0000000368 * t - 0.000029956) * t - 0.00000044) * t + 1.3915817) * t + 4612.156534) * t;

      // Form the Greenwich sidereal time.
      *gst = fmod((st / 3600.0 + theta), DEG360) / 15.0;

      if(*gst < 0.0) *gst += DAY_HOURS;
      return 0;

    default:        // Invalid value of 'method'.
      return 2;
  }

  return 2;
}

/**
 * Returns the value of the Earth Rotation Angle (theta) for a given UT1 Julian date.  The expression used is
 * taken from the note to IAU Resolution B1.8 of 2000. The input Julian date cane be split into an into high
 * and low order parts (e.g. integer and fractional parts) for improved accuracy, or else one of the
 * components (e.g. the low part) can be set to zero if no split is desired.
 *
 * The algorithm used here is equivalent to the canonical theta = 0.7790572732640 + 1.00273781191135448 * t,
 * where t is the time in days from J2000 (t = jd_high + jd_low - JD_J2000), but it avoids many two-PI 'wraps' that
 * decrease precision (adopted from SOFA Fortran routine iau_era00; see also expression at top of page 35 of
 * IERS Conventions (1996)).
 *
 *  REFERENCES:
 *  <ol>
 *   <li>IAU Resolution B1.8, adopted at the 2000 IAU General Assembly, Manchester, UK.</li>
 *   <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 *  </ol>
 *
 * @param jd_high   [day] High-order part of UT1 Julian date.
 * @param jd_low    [day] Low-order part of UT1 Julian date.
 * @return          [deg] The Earth Rotation Angle (theta) in degrees.
 */
double era(double jd_high, double jd_low) {
  double theta, thet1, thet2, thet3;

  thet1 = 0.7790572732640 + 0.00273781191135448 * (jd_high - JD_J2000);
  thet2 = 0.00273781191135448 * jd_low;
  thet3 = fmod(jd_high, 1.0) + fmod(jd_low, 1.0);

  theta = fmod(thet1 + thet2 + thet3, 1.0) * DEG360;
  if(theta < 0.0) theta += DEG360;

  return theta;
}

/**
 * Rotates a vector from the terrestrial to the
 * celestial system.  Specifically, it transforms a vector in the
 * ITRS (rotating earth-fixed system) to the GCRS (a local space-
 * fixed system) by applying rotations for polar motion, Earth
 * rotation, nutation, precession, and the dynamical-to-GCRS
 * frame tie.
 *
 * If both 'xp' and 'yp' are set to 0 no polar motion is included
 * in the transformation.
 *
 *  REFERENCES:
 *  <ol>
 *   <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *   <li>Kaplan, G. H. (2003), 'Another Look at Non-Rotating Origins', Proceedings of IAU XXV Joint Discussion 16.</li>
 *  </ol>
 *
 * @param jd_ut_high    [day] High-order part of UT1 Julian date.
 * @param jd_ut_low     [day] Low-order part of UT1 Julian date.
 * @param ut1_to_tt     [s] TT - UT1 Time difference in seconds
 * @param method        EROT_GST or EROT_ERA, depending on whether to use GST relative to equinox of date (pre
 *                      IAU 2006) or ERA relative to the CIO (IAU 2006 standard).
 * @param accuracy      NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param option        NOVAS_GCRS or NOVAS_TOD. Used only when method is EROT_GST.
 * @param xp            [arcsec] Conventionally-defined X coordinate of celestial intermediate
 *                      pole with respect to ITRS pole, in arcseconds.
 * @param yp            [arcsec] Conventionally-defined Y coordinate of celestial intermediate
 *                      pole with respect to ITRS pole, in arcseconds.
 * @param vec1          Position vector, geocentric equatorial rectangular coordinates,
 *                      referred to ITRS axes (terrestrial system) in the normal case
 *                      where 'option' is NOVAS_GCRS.
 * @param[out] vec2     Position vector, geocentric equatorial rectangular coordinates,
 *                      referred to GCRS axes (celestial system) or with respect to
 *                      the equator and equinox of date, depending on 'option'.
 * @return              0 if successful, -1 if either of the vector arguments is NULL, 1 if 'accuracy' is invalid,
 *                      2 if 'method' is invalid 10--20 the error is 10 + the error from cio_location(),
 *                      or else 20 + error from cio_basis().
 */
short ter2cel(double jd_ut_high, double jd_ut_low, double ut1_to_tt, enum novas_earth_rotation_measure method,
        enum novas_accuracy accuracy, enum novas_reference_system option, double xp, double yp, const double *vec1, double *vec2) {
  double jd_ut1, jd_tt, jd_tdb, gast, r_cio, v1[3], v2[3];
  short rs;

  if(!vec1 || !vec2) {
    errno = EINVAL;
    return -1;
  }

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY) return 1;

  // Compute the TT Julian date corresponding to the input UT1 Julian
  // date.
  jd_ut1 = jd_ut_high + jd_ut_low;
  jd_tt = jd_ut1 + (ut1_to_tt / DAY);

  // Compute the TDB Julian date corresponding to the input UT1 Julian
  // date.
  jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY;

  // Apply polar motion
  if((xp == 0.0) && (yp == 0.0)) memcpy(v1, vec1, sizeof(v1));
  else wobble(jd_tdb, WOBBLE_ITRS_TO_TIRS, xp, yp, vec1, v1);

  switch(method) {
    case (EROT_GST): {
      // 'CIO-TIO-THETA' method. See second reference, eq. (3) and (4).
      double x[3], y[3], z[3];
      double theta;
      int error;

      // Obtain the basis vectors, in the GCRS, of the celestial intermediate
      // system.
      error = cio_location(jd_tdb, accuracy, &r_cio, &rs);
      if(error) return (error + 10);

      error = cio_basis(jd_tdb, r_cio, rs, accuracy, x, y, z);
      if(error) return (error + 20);

      // Compute and apply the Earth rotation angle, 'theta', transforming the
      // vector to the celestial intermediate system.
      theta = era(jd_ut_high, jd_ut_low);
      spin(-theta, v1, v2);

      // Transform the vector from the celestial intermediate system to the
      // GCRS.
      vec2[0] = x[0] * v2[0] + y[0] * v2[1] + z[0] * v2[2];
      vec2[1] = x[1] * v2[0] + y[1] * v2[1] + z[1] * v2[2];
      vec2[2] = x[2] * v2[0] + y[2] * v2[1] + z[2] * v2[2];

      break;
    }
    case (EROT_ERA):
      sidereal_time(jd_ut_high, jd_ut_low, ut1_to_tt, NOVAS_TRUE_EQUINOX, EROT_ERA, accuracy, &gast);
      spin(-gast * 15.0, v1, v2);

      if(option == NOVAS_TOD) {
        memcpy(vec2, v2, sizeof(v2));         // skips remaining transformations.
      }
      else {
        tod_to_icrs(jd_tdb, accuracy, v2, vec2);
      }
      break;

    default:
      // Invalid value of 'method'.
      return 2;
  }

  return 0;
}

/**
 * Rotates a vector from the celestial to the terrestrial system.  Specifically, it transforms a vector in the
 * GCRS (a local space-fixed system) to the ITRS (a rotating earth-fixed system) by applying rotations for the
 * GCRS-to-dynamical frame tie, precession, nutation, Earth rotation, and polar motion.
 *
 * If both 'xp' and 'yp' are set to 0 no polar motion is included
 * in the transformation.
 *
 * REFERENCES:
 *  <ol>
 *   <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *   <li>Kaplan, G. H. (2003), 'Another Look at Non-Rotating Origins', Proceedings of IAU XXV Joint Discussion 16.</li>
 *  </ol>
 *
 * @param jd_ut_high    [day] High-order part of UT1 Julian date.
 * @param jd_ut_low     [day] Low-order part of UT1 Julian date.
 * @param ut1_to_tt     [s] TT - UT1 Time difference in seconds
 * @param method        EROT_GST or EROT_ERA, depending on whether to use GST relative to equinox of date (pre
 *                      IAU 2006) or ERA relative to the CIO (IAU 2006 standard).
 * @param accuracy      NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param option        NOVAS_GCRS or NOVAS_TOD. Used only when method is EROT_GST.
 * @param xp            [arcsec] Conventionally-defined X coordinate of celestial intermediate
 *                      pole with respect to ITRS pole, in arcseconds.
 * @param yp            [arcsec] Conventionally-defined Y coordinate of celestial intermediate
 *                      pole with respect to ITRS pole, in arcseconds.
 * @param vec1          Position vector, geocentric equatorial rectangular coordinates,
 *                      referred to GCRS axes (celestial system) or with respect to
 *                      the equator and equinox of date, depending on 'option'
 * @param[out] vec2     Position vector, geocentric equatorial rectangular coordinates,
 *                      referred to ITRS axes (terrestrial system)
 *
 * Position vector, geocentric equatorial rectangular coordinates,
 *                      referred to GCRS axes (celestial system) or with respect to
 *                      the equator and equinox of date, depending on 'option'.
 * @return              0 if successful, -1 if either of the vector arguments is NULL, 1 if 'accuracy' is invalid,
 *                      2 if 'method' is invalid 10--20 the error is 10 + the error from cio_location(),
 *                      or else 20 + error from cio_basis().
 */
short cel2ter(double jd_ut_high, double jd_ut_low, double ut1_to_tt, enum novas_earth_rotation_measure method,
        enum novas_accuracy accuracy, enum novas_reference_system option, double xp, double yp, const double *vec1, double *vec2) {

  double jd_ut1, jd_tt, jd_tdb, gast, r_cio, theta, v1[3], v2[3], v3[3], v4[3], x[3], y[3], z[3];
  short rs, error = 0;

  if(!vec1 || !vec2) {
    errno = EINVAL;
    return -1;
  }

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY) return 1;

  // Compute the TT Julian date corresponding to the input UT1 Julian date.
  jd_ut1 = jd_ut_high + jd_ut_low;
  jd_tt = jd_ut1 + (ut1_to_tt / DAY);

  // Compute the TDB Julian date corresponding to the input UT1 Julian date
  jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY;

  switch(method) {
    case (EROT_GST):
      // 'CIO-TIO-THETA' method.
      // See second reference, eq. (3) and (4).

      // Obtain the basis vectors, in the GCRS, of the celestial intermediate system.
      error = cio_location(jd_tdb, accuracy, &r_cio, &rs);
      if(error) return (error + 10);

      error = cio_basis(jd_tdb, r_cio, rs, accuracy, x, y, z);
      if(error) return (error + 20);

      // Transform the vector from the GCRS to the celestial intermediate system.
      v1[0] = vdot(x, vec1);
      v1[1] = vdot(y, vec1);
      v1[2] = vdot(z, vec1);

      // Compute and apply the Earth rotation angle, 'theta', transforming the
      // vector to the terrestrial intermediate system.
      theta = era(jd_ut_high, jd_ut_low);
      spin(theta, v1, v2);

      break;

    case (EROT_ERA):
      // IAU 2006 standard method

      if(option == NOVAS_TOD) {
        memcpy(v3, vec1, sizeof(v3));
      }
      else {
        icrs_to_tod(jd_tdb, accuracy, vec1, v3);
      }

      // Apply Earth rotation.
      sidereal_time(jd_ut_high, jd_ut_low, ut1_to_tt, NOVAS_TRUE_EQUINOX, EROT_ERA, accuracy, &gast);
      spin(gast * 15.0, v3, v4);

      break;

    default:
      // Invalid value of 'method'.
      return 2;
  }

  // Apply polar motion, transforming the vector to the ITRS.
  if((xp == 0.0) && (yp == 0.0)) memcpy(vec2, v2, sizeof(v2));
  else wobble(jd_tdb, WOBBLE_TIRS_TO_ITRS, xp, yp, v2, vec2);

  return 0;
}

/**
 * Transforms a vector from one coordinate system to another with same origin and axes rotated about the z-axis.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 * </ol>
 *
 * @param angle   [deg] Angle of coordinate system rotation, positive counterclockwise when viewed from +z, in degrees.
 * @param pos1    Input position vector.
 * @param[out] pos2   Position vector expressed in new coordinate system rotated about z by 'angle'.
 *
 * @return        0 if successful, or -1 if the output vector is NULL.
 */
int spin(double angle, const double *pos1, double *pos2) {
  static double ang_last = -999.0;
  static double xx, yx, xy, yy, zz = 1.0;

  if(!pos2) {
    errno = EINVAL;
    return -1;
  }

  if(fmod(fabs(angle - ang_last), DEG360) >= 1.0e-12) {
    const double angr = angle * DEGREE;
    const double cosang = cos(angr);
    const double sinang = sin(angr);

    // Rotation matrix (non-zero elements only).
    xx = cosang;
    yx = sinang;
    xy = -sinang;
    yy = cosang;

    ang_last = angle;
  }

  // Perform rotation.
  pos2[0] = xx * pos1[0] + yx * pos1[1];
  pos2[1] = xy * pos1[0] + yy * pos1[1];
  pos2[2] = zz * pos1[2];

  return 0;
}

/**
 * Corrects a vector in the ITRS (rotating Earth-fixed system)
 * for polar motion, and also corrects the longitude origin
 * (by a tiny amount) to the Terrestrial Intermediate Origin
 * (TIO).  The ITRS vector is thereby transformed to the terrestrial
 * intermediate system, based on the true (rotational) equator and
 * TIO.  Because the true equator is the plane orthogonal to the
 * direction of the Celestial Intermediate Pole (CIP), the components
 * of the output vector are referred to z and x axes toward the CIP
 * and TIO, respectively.
 *
 * NOTES:
 * <ol>
 * <li>More efficient 3D rotation implementation for small angles by A. Kovacs,
 * which also exposed a small error in the output 'x' coordinate value as
 * calculated by NOVAS C 3.1, typically at the 10 uas level.</li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 * <li>Lambert & Bizouard (2002), Astronomy and Astrophysics 394, 317-321.</li>
 * </ol>
 *
 * @param tjd           [day] Terrestrial Time (TT) based Julian date.
 * @param direction     WOBBLE_ITRS_TO_TIRS or WOBBLE_TIRS_TO_ITRS
 * @param xp            [arcsec] Conventionally-defined X coordinate of Celestial Intermediate Pole
 *                      with respect to ITRS pole, in arcseconds.
 * @param yp            [arcsec] Conventionally-defined Y coordinate of Celestial Intermediate Pole
 *                      with respect to ITRS pole, in arcseconds.
 * @param pos1          Input position vector, geocentric equatorial rectangular coordinates, in the
 *                      original system defined by 'direction'
 * @param[out] pos2     Output Position vector, geocentric equatorial rectangular coordinates, int the
 *                      final system defined by 'direction'
 *
 * @return              0 if successful, or -1 if the output vector argument is NULL.
 */
int wobble(double tjd, enum novas_wobble_direction direction, double xp, double yp, const double *pos1, double *pos2) {
  double xpole, ypole, t, s1;

  if(!pos2) {
    errno = EINVAL;
    return -1;
  }

  xpole = xp * ARCSEC;
  ypole = yp * ARCSEC;

  // Compute approximate longitude of TIO (s'), using eq. (10) of the second reference
  t = (tjd - JD_J2000) / JULIAN_CENTURY_DAYS;
  s1 = -47.0e-6 * ARCSEC * t;

  // Compute elements of rotation matrix.
  // Equivalent to R3(-s')R2(x)R1(y) as per IERS Conventions (2003).
  if(direction == WOBBLE_ITRS_TO_TIRS) tiny_rotate(pos1, -ypole, -xpole, s1, pos2);
  else tiny_rotate(pos1, ypole, xpole, -s1, pos2);

  // Second-order correction for the non-negligible xp, yp product...
  pos2[0] += xpole * ypole * pos1[1];

  return 0;
}

/**
 * Computes the position and velocity vectors of a terrestrial observer with respect to the center of the Earth.
 *
 * This function ignores polar motion, unless the observer's longitude and latitude have been corrected for it,
 * and variation in the length of day (angular velocity of earth).
 *
 * The true equator and equinox of date do not form an inertial system.  Therefore, with respect to an inertial
 * system, the very small velocity component (several meters/day) due to the precession and nutation of the Earth's
 * axis is not accounted for here.
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
 *                    and equinox of date, components in AU/day. (It may be NULL if
 *                    no velocity data is required).
 *
 * @return            0 if successful, or -1 if both output arguments are NULL.
 */
int terra(const on_surface *location, double lst, double *pos, double *vel) {
  double df, df2, phi, sinphi, cosphi, c, s, ach, ash, stlocl, sinst, cosst;
  double ht_km;
  int j;

  if(!pos && !vel) {
    errno = EINVAL;
    return -1;
  }

  // Compute parameters relating to geodetic to geocentric conversion.
  df = 1.0 - EF;
  df2 = df * df;

  phi = location->latitude * DEGREE;
  sinphi = sin(phi);
  cosphi = cos(phi);
  c = 1.0 / sqrt(cosphi * cosphi + df2 * sinphi * sinphi);
  s = df2 * c;
  ht_km = location->height / 1000.0;
  ach = 1e-3 * ERAD * c + ht_km;
  ash = 1e-3 * ERAD * s + ht_km;

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
    if(pos) pos[j] /= AU_KM;
    if(vel) vel[j] /= AU_KM / DAY;
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
 * @param accuracy  NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] mobl      [deg] Mean obliquity of the ecliptic in degrees
 * @param[out] tobl      [deg] True obliquity of the ecliptic in degrees
 * @param[out] ee        [deg] Equation of the equinoxes in seconds of time
 * @param[out] dpsi      [arcsec] Nutation in longitude in arcseconds
 * @param[out] deps      [arcsec] Nutation in obliquity in arcseconds
 *
 * @return          0
 *
 * @sa cel_pole()
 */
int e_tilt(double jd_tdb, enum novas_accuracy accuracy, double *mobl, double *tobl, double *ee, double *dpsi, double *deps) {
  static enum novas_accuracy accuracy_last = -1;
  static double jd_last = 0;
  static double d_psi, d_eps, mean_ob, true_ob, eq_eq;

  // Compute the nutation angles (arcseconds) if the input Julian date
  // is significantly different from the last Julian date, or the
  // accuracy mode has changed from the last call.
  if(!time_equals(jd_tdb, jd_last) || (accuracy != accuracy_last)) {
    // Compute time in Julian centuries from epoch J2000.0.
    const double t = (jd_tdb - JD_J2000) / JULIAN_CENTURY_DAYS;
    double dp, de, c_terms;

    nutation_angles(t, accuracy, &dp, &de);

    // Obtain complementary terms for equation of the equinoxes in arcseconds.
    c_terms = ee_ct(jd_tdb, 0.0, accuracy) / ARCSEC;

    // Apply observed celestial pole offsets.
    d_psi = dp + PSI_COR;
    d_eps = de + EPS_COR;

    // Compute mean obliquity of the ecliptic in arcseconds.
    mean_ob = mean_obliq(jd_tdb);

    // Compute true obliquity of the ecliptic in arcseconds.
    true_ob = mean_ob + d_eps;

    // Convert obliquity values to degrees.
    mean_ob /= 3600.0;
    true_ob /= 3600.0;

    // Compute equation of the equinoxes in seconds of time.
    eq_eq = d_psi * cos(mean_ob * DEGREE) + c_terms;
    eq_eq /= 15.0;

    // Reset the values of the last Julian date and last mode.
    jd_last = jd_tdb;
    accuracy_last = accuracy;
  }

  // Set output values.
  if(dpsi) *dpsi = d_psi;
  if(deps) *deps = d_eps;
  if(ee) *ee = eq_eq;
  if(mobl) *mobl = mean_ob;
  if(tobl) *tobl = true_ob;

  return 0;
}

/**
 * specifies the celestial pole offsets for high-precision applications.  Each set of offsets is
 * a correction to the modeled position of the pole for a specific date, derived from observations
 * and published by the IERS.
 *
 * The variables 'PSI_COR' and 'EPS_COR' are used only in NOVAS function e_tilt().
 *
 * This function, if used, should be called before any other
 * NOVAS functions for a given date.  Values of the pole offsets
 * specified via a call to this function will be used until
 * explicitly changed.
 *
 * 'tjd' is used only if 'type' is POLE_OFFSETS_X_Y, to transform dx and dy to
 * the equivalent delta-delta-psi and delta-delta-epsilon values.
 *
 * If 'type' is POLE_OFFSETS_X_Y, dx and dy are unit vector component
 * corrections, but are expressed in milliarcseconds simply by
 * multiplying by 206264806, the number of milliarcseconds in one radian.
 *
 * REFERENCES:
 * <ol>
 *  <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 *  <li>Kaplan, G. (2003), USNO/AA Technical Note 2003-03.</li>
 * </ol>
 *
 * @param tjd       [day] Terrestrial Time (TT) based Julian date.
 * @param type      POLE_OFFSETS_DPSI_DEPS or POLE_OFFSETS_X_Y
 * @param dpole1    [mas] Value of celestial pole offset in first coordinate, (delta-delta-psi or dx) in milliarcseconds.
 * @param dpole2    [mas] Value of celestial pole offset in second coordinate, (delta-delta-epsilon or dy) in milliarcseconds.
 * @return          0 if successful, or else 1 if 'type' is invalid.
 */
short cel_pole(double tjd, enum novas_pole_offset_type type, double dpole1, double dpole2) {
  double dx, dy, t, mean_ob, sin_e, x, dz, dp1[3], dp2[3], dp3[3];

  switch(type) {
    case (POLE_OFFSETS_DPSI_DEPS):

      // Angular coordinates of modeled pole referred to mean ecliptic of
      // date, that is,delta-delta-psi and delta-delta-epsilon.
      PSI_COR = dpole1 * 1.0e-3;
      EPS_COR = dpole2 * 1.0e-3;
      break;

    case (POLE_OFFSETS_X_Y):

      // Components of modeled pole unit vector referred to GCRS axes, that is, dx and dy.
      dx = dpole1;
      dy = dpole2;

      t = (tjd - JD_J2000) / JULIAN_CENTURY_DAYS;

      // Compute sin_e of mean obliquity of date.
      mean_ob = mean_obliq(tjd);
      sin_e = sin(mean_ob * ARCSEC);

      // The following algorithm, to transform dx and dy to
      // delta-delta-psi and delta-delta-epsilon, is from eqs. (7)-(9) of the
      // second reference.
      //
      // Trivial model of pole trajectory in GCRS allows computation of dz.
      x = (2004.190 * t) * ARCSEC;
      dz = -(x + 0.5 * x * x * x) * dx;

      // Form pole offset vector (observed - modeled) in GCRS.
      dp1[0] = dx * MAS;
      dp1[1] = dy * MAS;
      dp1[2] = dz * MAS;

      // Precess pole offset vector to mean equator and equinox of date.
      frame_tie(dp1, TIE_ICRS_TO_J2000, dp2);
      precession(JD_J2000, dp2, tjd, dp3);

      // Compute delta-delta-psi and delta-delta-epsilon in arcseconds.
      PSI_COR = (dp3[0] / sin_e) / ARCSEC;
      EPS_COR = dp3[1] / ARCSEC;
      break;

    default:
      // Invalid value of 'type'.
      return 1;
  }

  return 0;
}

/**
 * Computes the "complementary terms" of the equation of the equinoxes.
 * The input Julian date can be split into high and low order parts for
 * improved accuracy. Typically, the split is into integer and
 * fractiona parts. If the precision of a single part is sufficient,
 * you may sey the lowe order part to 0.
 *
 * The series used in this function was derived from the first
 * reference.  This same series was also adopted for use in the IAU's
 * Standards of Fundamental Astronomy (SOFA) software (i.e.,
 * subroutine eect00.for and function <code>eect00.c</code>).
 *
 * The low-accuracy series used in this function is a simple
 * implementation derived from the first reference, in which terms
 * smaller than 2 microarcseconds have been omitted.
 *
 * REFERENCES:
 * <ol>
 * <li>Capitaine, N., Wallace, P.T., and McCarthy, D.D. (2003). Astron. & Astrophys. 406, p. 1135-1149. Table 3.</li>
 * <li>IERS Conventions (2010), Chapter 5, p. 60, Table 5.2e.<br>
 * (Table 5.2e presented in the printed publication is a truncated
 * series. The full series, which is used in NOVAS, is available
 * on the IERS Conventions Center website:
 * <a href="ftp://tai.bipm.org/iers/conv2010/chapter5/tab5.2e.txt">
 * ftp://tai.bipm.org/iers/conv2010/chapter5/tab5.2e.txt</a>)
 * </li>
 * </ol>
 *
 * @param jd_high     [day] High-order part of TT based Julian date.
 * @param jd_low      [day] Low-order part of TT based Julian date.
 * @param accuracy    NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @return            [rad] Complementary terms, in radians.
 */
double ee_ct(double jd_high, double jd_low, enum novas_accuracy accuracy) {

  // Argument coefficients for t^0.
  const char ke0_t[33][14] = { { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, //
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
  const float se0_t[33][2] = { { +2640.96e-6, -0.39e-6 }, //
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
  const double se1[2] = //
          { -0.87e-6, +0.00e-6 };

  novas_fundamental_args fa2;
  double fa[14];

  // Interval between fundamental epoch J2000.0 and current date.
  const double t = ((jd_high - JD_J2000) + jd_low) / JULIAN_CENTURY_DAYS;

  // High accuracy mode.
  if(accuracy == NOVAS_FULL_ACCURACY) {
    double s0 = 0.0, s1 = 0.0;
    double a = 0.0;
    int i;

    // Fill the 5 Earth-Sun-Moon fundamental args
    fund_args(t, (novas_fundamental_args*) fa);

    // Add planet longitudes
    for(i = NOVAS_MERCURY; i <= NOVAS_NEPTUNE; i++) {
      int j = i - NOVAS_MERCURY;
      fa[5 + j] = planet_lon(t, i);
    }

    // General accumulated precession longitude
    fa[13] = accum_prec(t);

    // Evaluate the complementary terms.
    for(i = 32; --i >= 0;) {
      const char *ke = &ke0_t[i][0];
      const float *se = &se0_t[i][0];
      int j;

      a = 0.0;
      for(j = 14; --j >= 0;)
        if(ke[j]) a += ke[j] * fa[j];

      s0 += se[0] * sin(a);
      if(se[1]) s0 += se[1] * cos(a);
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
 * Transforms a vector from the dynamical reference system to the
 * International Celestial Reference System (ICRS), or vice versa.
 * The dynamical reference system is based on the dynamical mean
 * equator and equinox of J2000.0.  The ICRS is based on the space-
 * fixed ICRS axes defined by the radio catalog positions of several
 * hundred extragalactic objects.
 *
 * For geocentric coordinates, the same transformation is used
 * between the dynamical reference system and the GCRS.
 *
 * NOTES:
 * <ol>
 * <li>More efficient 3D rotation implementation for small angles by A. Kovacs</li>
 * </ol>
 *
 * REFERENCES:
 *  <ol>
 *   <li>Hilton, J. and Hohenkerk, C. (2004), Astronomy and Astrophysics 413, 765-770, eq. (6) and (8).</li>
 *   <li>IERS (2003) Conventions, Chapter 5.</li>
 *  </ol>
 *
 * @param pos1        Position vector, equatorial rectangular coordinates.
 * @param direction   <0 for for dynamical to ICRS transformation, or else &gt;=0
 *                    for ICRS to dynamical transformation. Alternatively you may
 *                    used the constants TIE_J2000_TO_ICRS or TIE_ICRS_TO_J2000.
 * @param[out] pos2   Position vector, equatorial rectangular coordinates.
 * @return            0 if successfor or -1 if either of the vector arguments is NULL.
 */
int frame_tie(const double *pos1, enum novas_frametie_direction direction, double *pos2) {

  // 'xi0', 'eta0', and 'da0' are ICRS frame biases in arcseconds taken
  // from IERS (2003) Conventions, Chapter 5.
  static const double xi0 = -0.0166170 * ARCSEC;
  static const double eta0 = -0.0068192 * ARCSEC;
  static const double da0 = -0.01460 * ARCSEC;

  if(!pos1 || !pos2) {
    errno = EINVAL;
    return -1;
  }

  if(direction >= 0) tiny_rotate(pos1, eta0, -xi0, -da0, pos2);
  else tiny_rotate(pos1, -eta0, xi0, da0, pos2);

  return 0;
}

/**
 * Applies proper motion, including foreshortening effects, to a
 * star's position.
 *
 * REFERENCES:
 *  <ol>
 *  <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *  </ol>
 *
 * @param jd_tdb1   [day] Barycentric Dynamical Time (TDB) based Julian date of the first epoch.
 * @param pos       Position vector at first epoch.
 * @param vel       Velocity vector at first epoch.
 * @param jd_tdb2   [day] Barycentric Dynamical Time (TDB) based Julian date of the second epoch.
 * @param pos2      Position vector at second epoch.
 * @return          0 if successful, or -1 if any of the vector areguments is NULL.
 */
int proper_motion(double jd_tdb1, const double *pos, const double *vel, double jd_tdb2, double *pos2) {
  int j;

  if(!pos || !vel || !pos2) {
    errno = EINVAL;
    return -1;
  }

  for(j = 0; j < 3; j++) {
    pos2[j] = pos[j] + (vel[j] * (jd_tdb2 - jd_tdb1));
  }

  return 0;
}

/**
 * Moves the origin of coordinates from the barycenter
 * of the solar system to the observer (or the geocenter); i.e.,
 * this function accounts for parallax (annual+geocentric or just
 * annual).
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 * </ol>
 *
 * @param pos             [AU] Position vector, referred to origin at solar system barycenter, components in AU.
 * @param pos_obs         [AU] Position vector of observer (or the geocenter), with respect to origin at solar system barycenter, components in AU.
 * @param[out] pos2       [AU] Position vector, referred to origin at center of mass of the Earth, components in AU.(It may be NULL if not required).
 * @param[out] lighttime  [day] Light time from object to Earth in days.
 * @return                0 if successful, or -1 if any of the essential pointer arguments is NULL.
 */
int bary2obs(const double *pos, const double *pos_obs, double *pos2, double *lighttime) {
  int j;

  if(!pos || !pos_obs || !pos2) {
    errno = EINVAL;
    return -1;
  }

  // Translate vector to geocentric coordinates.
  for(j = 0; j < 3; j++) {
    pos2[j] = pos[j] - pos_obs[j];
  }

  // Calculate length of vector in terms of light time.
  if(lighttime) *lighttime = vlen(pos2) / C_AUDAY;

  return 0;
}

/**
 * Computes the geocentric position and velocity
 * of an observer on the surface of the earth or on a near-earth
 * spacecraft.  The final vectors are expressed in the GCRS.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date.
 * @param ut1_to_tt   [s] TT - UT1 time difference in seconds
 * @param accuracy    NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param obs         Observer location
 * @param[out] pos    [AU] Position 3-vector of observer, with respect to origin at geocenter, referred to GCRS axes, components in AU.
 *                    (It may be NULL if not required.)
 * @param[out] vel    [AU/day] Velocity 3-vector of observer, with respect to origin at geocenter, referred to GCRS axes, components in AU/day.
 *                    (It may be NULL if not required.)
 * @return            0 if successful, if the 'observer' is NULL, or 1 if 'accuracy' is invalid, or 2 if '<code>obserrver->where</code>'
 *                    is invalid.
 */
short geo_posvel(double jd_tt, double ut1_to_tt, enum novas_accuracy accuracy, const observer *obs, double *pos, double *vel) {
  static double t_last = 0;
  static enum novas_accuracy acc_last = -1;
  static double gast;

  double gmst, x1, x2, x3, x4, eqeq, pos1[3], vel1[3], jd_tdb, jd_ut1;

  if(!obs) {
    errno = EINVAL;
    return -1;
  }

  // Invalid value of 'accuracy'.
  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY) return 1;

  // Compute 'jd_tdb', the TDB Julian date corresponding to 'jd_tt'.
  jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY;

  switch(obs->where) {

    case (NOVAS_OBSERVER_AT_GEOCENTER):                   // Observer at geocenter.  Trivial case.
      if(pos) memset(pos, 0, XYZ_VECTOR_SIZE);
      if(vel) memset(vel, 0, XYZ_VECTOR_SIZE);
      return 0;

      // Other two cases: Get geocentric position and velocity vectors of
      // observer wrt equator and equinox of date.

    case (NOVAS_OBSERVER_ON_EARTH):                       // Observer on surface of Earth.

      // Compute UT1 and sidereal time.
      jd_ut1 = jd_tt - (ut1_to_tt / DAY);
      if(!time_equals(jd_ut1, t_last) || accuracy != acc_last) {
        sidereal_time(jd_ut1, 0.0, ut1_to_tt, NOVAS_MEAN_EQUINOX, EROT_ERA, accuracy, &gmst);
        e_tilt(jd_tdb, accuracy, &x1, &x2, &eqeq, &x3, &x4);
        gast = gmst + eqeq / 3600.0;
        t_last = jd_ut1;
        acc_last = accuracy;
      }

      // Function 'terra' does the hard work, given sidereal time.
      terra(&obs->on_surf, gast, pos1, vel1);
      break;

    case (NOVAS_OBSERVER_IN_EARTH_ORBIT): {               // Observer on near-earth spacecraft.
      int i;
      double ivu = DAY / AU_KM;

      // Convert units to AU and AU/day.
      for(i = 0; i < 3; i++) {
        if(pos) pos1[i] = obs->near_earth.sc_pos[i] / AU_KM;
        if(vel) vel1[i] = obs->near_earth.sc_vel[i] * ivu;
      }

      break;
    }

    default:
      // AK: Added check for valid observer
      return 2;
  }

  // Transform geocentric position vector of observer to GCRS.
  if(pos) tod_to_icrs(jd_tdb, accuracy, pos1, pos);

  // Transform geocentric velocity vector of observer to GCRS.
  if(vel) tod_to_icrs(jd_tdb, accuracy, vel1, vel);

  return 0;
}

/**
 * Computes the geocentric position of a solar system body, as antedated for light-time.
 *
 *
 * @param jd_tdb      [day] Barycentric Dynamical Time (TDB) based Julian date
 * @param ss_object   Pointer to structure containing the designation for the solar system body
 * @param pos_obs     [AU] Position 3-vector of observer (or the geocenter), with respect to
 *                    origin at solar system barycenter, referred to ICRS axes,
 *                    components in AU.
 * @param tlight0     [day] First approximation to light-time, in days (can be set to 0.0 if unknown).
 * @param accuracy    NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] pos    [AU] Position 3-vector of body, with respect to origin at observer (or
 *                    the geocenter), referred to ICRS axes, components in AU.
 * @param[out] tlight [day] Calculated light time
 *
 * @return            0 if successful, -1 if any of the poiinter arguments is NULL, 1 if the algorithm failed
 *                    to converge after 10 iterations, or 10 + the error from solarsystem().
 */
short light_time(double jd_tdb, const object *ss_object, const double *pos_obs, double tlight0, enum novas_accuracy accuracy,
        double *pos, double *tlight) {
  int iter = 0;

  double tol, jd[2] = { }, t1, t2, t3 = 0.0, pos1[3], vel1[3];

  if(!ss_object || !pos_obs) {
    errno = EINVAL;
    return -1;
  }

  if(tlight) *tlight = 0.0;

  // Set light-time convergence tolerance.  If full-accuracy option has
  // been selected, split the Julian date into whole days + fraction of
  // day.
  if(accuracy == NOVAS_FULL_ACCURACY) {
    tol = 1.0e-12;

    jd[0] = floor(jd_tdb);
    t1 = jd_tdb - jd[0];
    t2 = t1 - tlight0;
  }
  else {
    tol = 1.0e-9;

    t1 = jd_tdb;
    t2 = jd_tdb - tlight0;
  }

  // Iterate to obtain correct light-time (usually converges rapidly).
  for(iter = 0; iter < 10; iter++) {
    int error;

    if(iter > 0) {
      t2 = t3;
    }

    jd[1] = t2;
    error = ephemeris(jd, ss_object, 0, accuracy, pos1, vel1);

    if(error != 0) return (error + 10);

    bary2obs(pos1, pos_obs, pos, tlight);

    t3 = t1 - *tlight;
    iter++;

    if(fabs(t3 - t2) <= tol) return 0;
  }

  return 1;
}

/**
 * Teturns the difference in light-time, for a star,
 * between the barycenter of the solar system and the observer (or
 * the geocenter).
 *
 * Alternatively, this function returns the light-time from the
 * observer (or the geocenter) to a point on a light ray that is
 * closest to a specific solar system body.  For this purpose, 'pos1'
 * is the position vector toward observed object, with respect to
 * origin at observer (or the geocenter); 'pos_obs' is the position
 * vector of solar system body, with respect to origin at observer
 * (or the geocenter), components in AU; and the returned value is
 * the light time to point on line defined by 'pos1' that is closest
 * to solar system body (positive if light passes body before hitting
 * observer, i.e., if 'pos1' is within 90 degrees of 'pos_obs').
 *
 * @param pos1      Position vector of star, with respect to origin at solar system barycenter.
 * @param pos_obs   [AU] Position vector of observer (or the geocenter), with respect to origin at solar system barycenter, components in AU.
 * @return          [day] Difference in light time, in the sense star to barycenter minus
 *                  star to earth, in days, or NAN if either of the input arguments is NULL.
 */
double d_light(const double *pos1, const double *pos_obs) {
  if(!pos1 || !pos_obs) {
    errno = EINVAL;
    return NAN;
  }

  // Light-time returned is the projection of vector 'pos_obs' onto the
  // unit vector 'u1' (formed from 'pos1'), divided by the speed of light.
  return vdot(pos_obs, pos1) / vlen(pos1) / C_AUDAY;
}

/**
 * Computes the total gravitational deflection of
 * light for the observed object due to the major gravitating bodies
 * in the solar system.  This function valid for an observed body
 * within the solar system as well as for a star.
 *
 * If 'accuracy' is set to zero (full accuracy), three bodies
 * (Sun, Jupiter, and Saturn) are used in the calculation.  If
 * the reduced-accuracy option is set, only the Sun is used in the
 * calculation.  In both cases, if the observer is not at the
 * geocenter, the deflection due to the Earth is included.
 *
 * The number of bodies used at full and reduced accuracy can be
 * set by making a change to the code in this function as indicated
 * in the comments.
 *
 * REFERENCES:
 * <ol>
 * <li>Klioner, S. (2003), Astronomical Journal 125, 1580-1597, Section 6.</li>
 * </ol>
 *
 * @param jd_tdb      [day] Barycentric Dynamical Time (TDB) based Julian date
 * @param loc_code    The type of observer frame
 * @param accuracy    NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param pos1        [AU] Position 3-vector of observed object, with respect to origin at
 *                    observer (or the geocenter), referred to ICRS axes, components
 *                    in AU.
 * @param pos_obs     [AU] Position 3-vector of observer (or the geocenter), with respect to
 *                    origin at solar system barycenter, referred to ICRS axes,
 *                    components in AU.
 * @param[out] pos2   [AU] Position vector of observed object, with respect to origin at
 *                    observer (or the geocenter), referred to ICRS axes, corrected
 *                    for gravitational deflection, components in AU.
 * @return            0 if successful, -1 if any of the pointer arguments is NULL,
 *                    10--30 the error is 10 + the error from ephemeris(), or else
 *                    30 + the error from make_object().
 *
 */
short grav_def(double jd_tdb, enum novas_observer_place loc_code, enum novas_accuracy accuracy, const double *pos1,
        const double *pos_obs, double *pos2) {

  // The following list of body numbers identifies which gravitating bodies (aside from the Earth)
  // are potentially used -- list is taken from Klioner's table 1, the order based on area of sky
  // affected (col 2).  Order is Sun, Jupiter, Saturn, Moon, Venus, Uranus, Neptune.
  static const enum novas_planet body_num[7] = { NOVAS_SUN, NOVAS_JUPITER, NOVAS_SATURN, NOVAS_MOON, NOVAS_VENUS, NOVAS_URANUS,
          NOVAS_NEPTUNE };

  static const double rmass[] = NOVAS_RMASS_INIT;

  static int first_time = 1;
  static object body[7], earth;


  // Set the number of bodies -- and hence the bodies used -- based on the value of the 'accuracy' flag.

  // Change value of 'nbodies' to include or exclude gravitating bodies
  // ('nbodies' <= 0 means no deflection calculated, 'nbodies' = 1 means
  // Sun only, 'nbodies' = 2 means Sun + Jupiter, etc.)  Default is
  // 'nbodies' = 3: Sun + Jupiter + Saturn.
  const int nbodies = (accuracy == NOVAS_FULL_ACCURACY) ? 3 : 1;

  double jd[2] = { }, tlt, pbody[3], vbody[3], pbodyo[3], x;
  int i, error;

  if(!pos1 || !pos_obs || !pos2) {
    errno = EINVAL;
    return -1;
  }

  // Set up the structures of type 'object' containing the body information.
  if(first_time) {
    // Body names correspondig to their major planet ID numbers
    const char *name[] = NOVAS_PLANET_NAMES_INIT;
    static int nbodies_last = 0;

    make_object(NOVAS_MAJOR_PLANET, NOVAS_EARTH, name[NOVAS_EARTH], NULL, &earth);

    if(nbodies > nbodies_last) for(i = 0; i < nbodies; i++) {
      int num = body_num[i];
      error = make_object(NOVAS_MAJOR_PLANET, num, name[num], NULL, &body[i]);
      if(error) return (error + 30);
      nbodies_last = nbodies;
    }
    first_time = 0;
  }

  // Initialize output vector of observed object to equal input vector.
  memcpy(pos2, pos1, XYZ_VECTOR_SIZE);

  // Compute light-time to observed object.
  tlt = vlen(pos1) / C_AUDAY;

  // Cycle through gravitating bodies.
  for(i = 0; i < nbodies; i++) {
    double dlt, tclose;

    jd[0] = jd_tdb;

    // Get position of gravitating body wrt ss barycenter at time 'jd_tdb'.
    error = ephemeris(jd, &body[i], NOVAS_BARYCENTER, accuracy, pbody, vbody);
    if(error) return (10 + error);

    // Get position of gravitating body wrt observer at time 'jd_tdb'.
    bary2obs(pbody, pos_obs, pbodyo, &x);

    // Compute light-time from point on incoming light ray that is closest to gravitating body.
    dlt = d_light(pos2, pbodyo);

    // Get position of gravitating body wrt ss barycenter at time when
    // incoming photons were closest to it.
    tclose = jd_tdb;

    if(dlt > 0.0) tclose = jd_tdb - dlt;
    if(tlt < dlt) tclose = jd_tdb - tlt;

    jd[0] = tclose;
    error = ephemeris(jd, &body[i], NOVAS_BARYCENTER, accuracy, pbody, vbody);
    if(error) return 10 + error;

    // Compute deflection due to gravitating body.
    grav_vec(pos2, pos_obs, pbody, rmass[body_num[i]], pos2);
  }

  // If observer is not at geocenter, add in deflection due to Earth.
  if(loc_code != NOVAS_OBSERVER_AT_GEOCENTER) {

    // Get position of Earth wrt solar system barycenter at time 'jd_tdb'.
    jd[0] = jd_tdb;
    error = ephemeris(jd, &earth, NOVAS_BARYCENTER, accuracy, pbody, vbody);
    if(error) return 10 + error;

    // Compute deflection due to Earth.
    grav_vec(pos2, pos_obs, pbody, rmass[NOVAS_EARTH], pos2);
  }

  return 0;
}

/**
 * Corrects position vector for the deflection
 * of light in the gravitational field of an arbitrary body.  This
 * function valid for an observed body within the solar system as
 * well as for a star.
 *
 * REFERENCES:
 * <ol>
 * <li>Murray, C.A. (1981) Mon. Notices Royal Ast. Society 195, 639-648.</li>
 * <li>See also formulae in Section B of the Astronomical Almanac, or</li>
 * <li>Kaplan, G. et al. (1989) Astronomical Journal 97, 1197-1210, section iii f.</li>
 * </ol>
 *
 * @param pos1      [AU] Position 3-vector of observed object, with respect to origin at
 *                  observer (or the geocenter), components in AU.
 * @param pos_obs   [AU] Position vector of gravitating body, with respect to origin
 *                  at solar system barycenter, components in AU.
 * @param pos_body  [AU] Position 3-vector of gravitating body, with respect to origin
 *                  at solar system barycenter, components in AU.
 * @param rmass     [1/Msun] Reciprocal mass of gravitating body in solar mass units, that is, Sun mass / body mass.
 *
 * @param[out] pos2 [AU]  Position 3-vector of observed object, with respect to origin at
 *                  observer (or the geocenter), corrected for gravitational
 *                  deflection, components in AU.
 * @return          0 if successful, or -1 if any of the input vectors is NULL.
 */
int grav_vec(const double *pos1, const double *pos_obs, const double *pos_body, double rmass, double *pos2) {
  double pq[3], pe[3], pmag, emag, qmag, phat[3], ehat[3], qhat[3], pdotq, edotp, qdote;
  int i;

  if(!pos1 || !pos_obs || !pos_body || !pos2) {
    errno = EINVAL;
    return -1;
  }

  // Construct vector 'pq' from gravitating body to observed object and
  // construct vector 'pe' from gravitating body to observer.
  for(i = 0; i < 3; i++) {
    pq[i] = pos_obs[i] + pos1[i] - pos_body[i];
    pe[i] = pos_obs[i] - pos_body[i];
  }

  // Compute vector magnitudes and unit vectors.
  pmag = vlen(pos1);
  emag = vlen(pe);
  qmag = vlen(pq);

  for(i = 0; i < 3; i++) {
    phat[i] = pos1[i] / pmag;
    ehat[i] = pe[i] / emag;
    qhat[i] = pq[i] / qmag;
  }

  // Compute dot products of vectors.
  pdotq = vdot(phat, qhat);
  edotp = vdot(ehat, phat);
  qdote = vdot(qhat, ehat);

  // If gravitating body is observed object, or is on a straight line
  // toward or away from observed object to within 1 arcsec, deflection
  // is set to zero; set 'pos2' equal to 'pos1'.
  if(fabs(edotp) > 0.99999999999) {
    memcpy(pos2, pos1, XYZ_VECTOR_SIZE);
  }

  else {
    // Compute scalar factors.
    const double fac1 = 2.0 * GS / (C * C * emag * AU * rmass);
    const double fac2 = 1.0 + qdote;

    // Construct corrected position vector 'pos2'.
    for(i = 0; i < 3; i++) {
      const double p2i = phat[i] + fac1 * (pdotq * ehat[i] - edotp * qhat[i]) / fac2;
      pos2[i] = p2i * pmag;
    }
  }

  return 0;
}

/**
 * Corrects position vector for aberration of light.  Algorithm
 * includes relativistic terms.
 *
 * REFERENCES:
 * <ol>
 * <li>Murray, C. A. (1981) Mon. Notices Royal Ast. Society 195, 639-648.</li>
 * <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 * </ol>
 *
 * @param pos         [AU]  Position vector, referred to origin at center of mass of the
 *                    Earth, components in AU.
 * @param ve          [AU/day]  Velocity vector of center of mass of the Earth, referred to
 *                    origin at solar system barycenter, components in AU/day.
 * @param lighttime   [day] Light time from object to Earth in days. If set to 0, this function will compute it.
 * @param[out] pos2   [AU] Position vector, referred to origin at center of mass of the
 *                    Earth, corrected for aberration, components in AU
 *
 * @return            0 if successful, or -1 if any of the vector arguments are NULL.
 */
int aberration(const double *pos, const double *ve, double lighttime, double *pos2) {
  double p1mag, vemag, beta, cosd, gammai, p, q, r;

  if(!pos || !ve || !pos2) {
    errno = EINVAL;
    return -1;
  }

  if(lighttime == 0.0) {
    p1mag = vlen(pos);
    lighttime = p1mag / C_AUDAY;
  }
  else p1mag = lighttime * C_AUDAY;

  vemag = vlen(ve);
  beta = vemag / C_AUDAY;

  cosd = vdot(pos, ve) / (p1mag * vemag);
  gammai = sqrt(1.0 - beta * beta);
  p = beta * cosd;
  q = (1.0 + p / (1.0 + gammai)) * lighttime;
  r = 1.0 + p;

  pos2[0] = (gammai * pos[0] + q * ve[0]) / r;
  pos2[1] = (gammai * pos[1] + q * ve[1]) / r;
  pos2[2] = (gammai * pos[2] + q * ve[2]) / r;

  return 0;
}

/**
 * Predicts the radial velocity of the observed object
 * as it would be measured by spectroscopic means.  Radial
 * velocity is here defined as the radial velocity measure (z)
 * times the speed of light.  For a solar system body, it applies
 * to a fictitious emitter at the center of the observed object,
 * assumed massless (no gravitational red shift), and does not
 * in general apply to reflected light.  For stars, it includes
 * all effects, such as gravitational red shift, contained
 * in the catalog barycentric radial velocity measure, a scalar
 * derived from spectroscopy.  Nearby stars with a known kinematic
 * velocity vector (obtained independently of spectroscopy) can be
 * treated like solar system objects.
 *
 * All the input arguments are BCRS quantities, expressed
 * with respect to the ICRS axes.  'vel' and 'vel_obs' are kinematic
 * velocities - derived from geometry or dynamics, not spectroscopy.
 *
 * If the object is outside the solar system, the algorithm
 * used will be consistent with the IAU definition of stellar
 * radial velocity, specifically, the barycentric radial velocity
 * measure, which is derived from spectroscopy.  In that case,
 * the vector 'vel' can be very approximate -- or, for distant stars
 * or galaxies, zero -- as it will be used only for a small geometric
 * correction that is proportional to proper motion.
 *
 * Any of the distances (last three input arguments) can be set to
 * zero (0.0) if the corresponding general relativistic gravitational
 * potential term is not to be evaluated.  These terms generally
 * are important only at the meter/second level.  If 'd_obs_geo' and
 * 'd_obs_sun' are both zero, an average value will be used for the
 * relativistic term for the observer, appropriate for an observer on
 * the surface of the Earth.  'd_obj_sun', if given, is used only for
 * solar system objects.
 *
 * REFERENCES:
 * <ol>
 * <li>Lindegren & Dravins (2003), Astronomy & Astrophysics 401, 1185-1201.</li>
 * </ol>
 *
 * @param cel_object    Celestial object observed
 * @param pos           [AU|*] Geometric position vector of object with respect to observer.
 *                      For solar system sources it should be corrected for light-time, and
 *                      expressed in AU. For non-solar-system objects, the position vector
 *                      defines a direction only, with arbitrary magnitude.
 * @param vel           [AU/day] Velocity vector of object with respect to solar system barycenter, in AU/day.
 * @param vel_obs       [AU/day] Velocity vector of observer with respect to solar system barycenter, in AU/day.
 * @param d_obs_geo     [AU] Distance from observer to geocenter, in AU, or 0.0 if gravitational deflection can be ignored.
 * @param d_obs_sun     [AU] Distance from observer to Sun, in AU, or 0.0 if gravitational deflection can be ignored.
 * @param d_obj_sun     [AU] Distance from object to Sun, in AU, or 0.0 if gravitational deflection can be ignored.
 * @param[out] rv            [km/s] The observed radial velocity measure times the speed of light, in kilometers/second.
 *
 * @return              0 if successful, or -1 if any of the pointer arguments is NULL.
 */
int rad_vel(const object *cel_object, const double *pos, const double *vel, const double *vel_obs, double d_obs_geo, double d_obs_sun,
        double d_obj_sun, double *rv) {
  static int first_call = 1;
  static double c2, toms, toms2;

  double v[3], ra, dec, radvel, posmag, uk[3], v2, vo2, r, phigeo, phisun, rel, rar, dcr, cosdec, du[3], zc, kv, zb1, kvobs, zobs1;
  int i;

  if(!cel_object || !pos || !vel || !vel_obs || !rv) {
    errno = EINVAL;
    return -1;
  }

  // Set up local constants the first time this function is called.
  if(first_call) {
    c2 = C * C;
    toms = AU / DAY;
    toms2 = toms * toms;

    first_call = 0;
  }

  // Initialize variables needed for radial velocity calculation.
  memcpy(v, vel, sizeof(v));

  if(cel_object->type == NOVAS_DISTANT_OBJECT) {
    /* Data for objects outside the solar system. */
    ra = cel_object->star.ra;
    dec = cel_object->star.dec;
    radvel = cel_object->star.radialvelocity;

    if(cel_object->star.parallax <= 0.0) memset(v, 0, sizeof(v));
  }
  else {
    ra = 0.0;
    dec = 0.0;
    radvel = 0.0;
  }

  // Compute length of position vector = distance to object in AU.
  posmag = vlen(pos);

  // Compute unit vector toward object.
  for(i = 0; i < 3; i++) {
    uk[i] = pos[i] / posmag;
  }

  // Compute velocity-squared factors.
  v2 = vdot(v, v) * toms2;
  vo2 = vdot(vel_obs, vel_obs) * toms2;

  // Compute geopotential at observer, unless observer is geocentric.
  r = d_obs_geo * AU;
  phigeo = (r > 1.0e6) ? GE / r : 0.0;

  // Compute solar potential at observer.
  r = d_obs_sun * AU;
  phisun = (r > 1.0e8) ? GS / r : 0.0;

  // Compute relativistic potential and velocity factor for observer.
  if((d_obs_geo != 0.0) || (d_obs_sun != 0.0)) {
    // Lindegren & Dravins eq. (41), second factor in parentheses.
    rel = 1.0 - (phigeo + phisun) / c2 - 0.5 * vo2 / c2;
  }
  else {
    // Lindegren & Dravins eq. (42), inverse.
    rel = 1.0 - 1.550e-8;
  }

  // Complete radial velocity calculation.
  switch(cel_object->type) {
    case 2: // Objects outside the solar system.
      // For stars, update barycentric radial velocity measure for change
      // in view angle.
      rar = ra * HOURANGLE;
      dcr = dec * DEGREE;
      cosdec = cos(dcr);
      du[0] = uk[0] - (cosdec * cos(rar));
      du[1] = uk[1] - (cosdec * sin(rar));
      du[2] = uk[2] - sin(dcr);
      zc = radvel * 1.0e3 + vdot(v, du) * toms;

      // Compute observed radial velocity measure of a star (inverse of
      // Lindegren & Dravins eq. (41)).
      zb1 = 1.0 + zc / C;
      kvobs = vdot(uk, vel_obs) * toms;
      zobs1 = zb1 * rel / (1.0 + kvobs / C);
      break;

    case NOVAS_MAJOR_PLANET: /* Objects in the solar system */
    case NOVAS_MINOR_PLANET:
    default:
      // Compute solar potential at object, if within solar system.
      r = d_obj_sun * AU;
      phisun = ((r > 1.0e8) && (r < 1.0e16)) ? GS / r : 0.0;

      // Compute observed radial velocity measure of a planet or other
      // object -- including a nearby star -- where kinematic barycentric
      // velocity vector is known and gravitational red shift is negligible
      // (Lindegren & Dravins eq. (40), applied as per S. Klioner private
      // communication (2006)).
      kv = vdot(uk, vel) * toms;
      zb1 = (1.0 + kv / C) / (1.0 - phisun / c2 - 0.5 * v2 / c2);
      kvobs = vdot(uk, vel_obs) * toms;
      zobs1 = zb1 * rel / (1.0 + kvobs / C);
  }

  // Convert observed radial velocity measure to kilometers/second.
  *rv = (zobs1 - 1.0) * C / 1000.0;

  return 0;
}

/**
 * Precesses equatorial rectangular coordinates from one epoch to
 * another. Unlike the original NOVAS routine, this routine works
 * for any pairing of the time arguments.
 *
 * REFERENCES:
 * <ol>
 * <li>Explanatory Supplement To The Astronomical Almanac, pp. 103-104.</li>
 * <li>Capitaine, N. et al. (2003), Astronomy And Astrophysics 412, pp. 567-586.</li>
 * <li>Hilton, J. L. et al. (2006), IAU WG report, Celest. Mech., 94, pp. 351-367.</li>
 * </ol>
 *
 * @param jd_tdb1     [day] Barycentric Dynamic Time (TDB) based Julian date of the initial epoch
 * @param pos1        Position 3-vector, geocentric equatorial rectangular coordinates,
 *                    referred to mean dynamical equator and equinox of the initial epoch.
 * @param jd_tdb2     [day] Barycentric Dynamic Time (TDB) based Julian date of the final epoch
 * @param[out] pos2   Position 3-vector, geocentric equatorial rectangular coordinates,
 *                    referred to mean dynamical equator and equinox of the final epoch.
 * @return            0 if successful, or -1 if either of the position vectors is NULL.
 */
short precession(double jd_tdb1, const double *pos1, double jd_tdb2, double *pos2) {
  static int first_time = 1;

  static double t_last = 0.0;
  static double xx, yx, zx, xy, yy, zy, xz, yz, zz;
  double t;

  if(!pos1 || !pos2) {
    errno = EINVAL;
    return -1;
  }

  if(jd_tdb1 == jd_tdb2) {
    memcpy(pos2, pos1, XYZ_VECTOR_SIZE);
    return 0;
  }

  // Check to be sure that either 'jd_tdb1' or 'jd_tdb2' is equal to JD_J2000.
  if((jd_tdb1 != JD_J2000) && (jd_tdb2 != JD_J2000)) {
    double pos0[3];
    precession(jd_tdb1, pos1, JD_J2000, pos0);
    precession(JD_J2000, pos0, jd_tdb2, pos2);
    return 0;
  }

  // 't' is time in TDB centuries between the two epochs.
  t = (jd_tdb2 - jd_tdb1) / JULIAN_CENTURY_DAYS;

  if(jd_tdb2 == JD_J2000) t = -t;

  if((fabs(t - t_last) >= 1.0e-15) || (first_time == 1)) {
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
    first_time = 0;
  }

  if(jd_tdb2 == JD_J2000) {
    // Perform rotation from epoch to J2000.0.
    pos2[0] = xx * pos1[0] + xy * pos1[1] + xz * pos1[2];
    pos2[1] = yx * pos1[0] + yy * pos1[1] + yz * pos1[2];
    pos2[2] = zx * pos1[0] + zy * pos1[1] + zz * pos1[2];
  }
  else {
    // Perform rotation from J2000.0 to epoch.
    pos2[0] = xx * pos1[0] + yx * pos1[1] + zx * pos1[2];
    pos2[1] = xy * pos1[0] + yy * pos1[1] + zy * pos1[2];
    pos2[2] = xz * pos1[0] + yz * pos1[1] + zz * pos1[2];
  }

  return 0;
}

/**
 * Nutates equatorial rectangular coordinates from mean equator and
 * equinox of epoch to true equator and equinox of epoch. Inverse
 * transformation may be applied by setting flag 'direction'.
 *
 * REFERENCES:
 * <ol>
 * <li>Explanatory Supplement To The Astronomical Almanac, pp. 114-115.</li>
 * </ol>
 *
 *
 * @param jd_tdb      [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param direction   NUTATE_MEAN_TO_TRUE or NUTATE_TRUE_TO_MEAN
 * @param accuracy    NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param pos         Position 3-vector, geocentric equatorial rectangular coordinates,
 *                    referred to mean equator and equinox of epoch.
 * @param[out] pos2   Position vector, geocentric equatorial rectangular coordinates,
 *                    referred to true equator and equinox of epoch.
 *
 * @return            0 if successful, or -1 if one of the vector arguments is NULL.
 */
int nutation(double jd_tdb, enum novas_nutation_direction direction, enum novas_accuracy accuracy, const double *pos, double *pos2) {
  double oblm, oblt, psi;

  double cm, sm, ct, st, cp, sp;
  double xx, yx, zx, xy, yy, zy, xz, yz, zz;

  if(!pos || !pos2) {
    errno = EINVAL;
    return -1;
  }

  if(time_equals(jd_tdb, JD_J2000)) {
    memcpy(pos2, pos, XYZ_VECTOR_SIZE);
    return 0;
  }

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
    // Perform rotation.
    pos2[0] = xx * pos[0] + yx * pos[1] + zx * pos[2];
    pos2[1] = xy * pos[0] + yy * pos[1] + zy * pos[2];
    pos2[2] = xz * pos[0] + yz * pos[1] + zz * pos[2];
  }
  else {
    // Perform inverse rotation.
    pos2[0] = xx * pos[0] + xy * pos[1] + xz * pos[2];
    pos2[1] = yx * pos[0] + yy * pos[1] + yz * pos[2];
    pos2[2] = zx * pos[0] + zy * pos[1] + zz * pos[2];
  }

  return 0;
}

/**
 * Set the function to use for low-precision IAU 2000 nutation calculations instead of the default nu2000k().
 *
 * @param f     the new function to use for low-precision IAU 2000 nutation calculations
 * @return      0 if successful, or -1 if the input argument is NULL
 *
 * @sa nutation_angles()
 */
int nutation_set_lp(novas_nutate_func f) {
  if(!f) {
    errno = EINVAL;
    return -1;
  }
  nutate_lp = f;
  return 0;
}

/**
 * Returns the values for nutation in longitude and
 * nutation in obliquity for a given TDB Julian date.  The nutation
 * model selected depends upon the input value of 'accuracy'.  See
 * notes below for important details.
 *
 * This function selects the nutation model depending first upon
 * the input value of 'accuracy'.  If 'accuracy' is NOVAS_FILL_ACCURACY,
 * the IAU 2000A nutation model is used.  Otherwise,
 * the model used depends upon the value of function local
 * variable 'low_acc_choice'. If local variable 'low_acc_choice' = 1
 * (the default), a specially truncated version of IAU 2000A, called
 * 'NU2000K' is used.  If 'low_acc_choice' = 2, the IAU 2000B nutation
 * model is used.
 *
 * See the prologs of the nutation functions in file 'nutation.c'
 * for details concerning the models.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 * </ol>
 *
 * @param t           [cy] TDB time in Julian centuries since J2000.0
 * @param accuracy    NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] dpsi   [arcsec] Nutation in longitude in arcseconds.
 * @param[out] deps   [arcsec] Nutation in obliquity in arcseconds.
 *
 * @return            0 if successful, or -1 if the output pointer arguments are NULL
 *
 * @sa nutation_set_lp()
 */
int nutation_angles(double t, enum novas_accuracy accuracy, double *dpsi, double *deps) {
  if(!dpsi || !deps) {
    errno = EINVAL;
    return -1;
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
 * @param[out] a  [rad] Fundamental arguments data to populate (5 doubles)
 *
 * @return        0 if successful, or -1 if the utput pointer argument is NULL.
 */
int fund_args(double t, novas_fundamental_args *a) {
  if(!a) {
    errno = EINVAL;
    return -1;
  }

  a->l = 485868.249036 + t * 1717915923.2178;
  a->l1 = 1287104.79305 + t * 129596581.0481;
  a->F = 335779.526232 + t * 1739527262.8478;
  a->D = 1072260.70369 + t * 1602961601.2090;
  a->Omega = 450160.398036 - t * 6962890.5431;

  // higher order terms (for uas precision) only if |t| > 0.001
  if(fabs(t) > 1e-3) {
    const double t2 = t * t;
    a->l += t2 * (31.8792 + t * (0.051635 + t * (-0.00024470)));
    a->l1 += t2 * (-0.5532 + t * (0.000136 + t * (-0.00001149)));
    a->F += t2 * (-12.7512 + t * (-0.001037 + t * (0.00000417)));
    a->D += t2 * (-6.3706 + t * (0.006593 + t * (-0.00003169)));
    a->Omega += t2 * (7.4722 + t * (0.007702 + t * (-0.00005939)));
  }


  a->l = norm_ang(a->l * ARCSEC);
  a->l1 = norm_ang(a->l1 * ARCSEC);
  a->F = norm_ang(a->F * ARCSEC);
  a->D = norm_ang(a->D * ARCSEC);
  a->Omega = norm_ang(a->Omega * ARCSEC);

  return 0;
}

double planet_lon(double t, enum novas_planet planet) {
  // Planetary longitudes, Mercury through Neptune, wrt mean dynamical
  // ecliptic and equinox of J2000, with high order terms omitted
  // (Simon et al. 1994, 5.8.1-5.8.8).
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
      errno = EINVAL;
      return NAN;
  }

  return fmod(lon, TWOPI);
}

double accum_prec(double t) {
  // General precession in longitude (Simon et al. 1994), equivalent
  // to 5028.8200 arcsec/cy at J2000.
  return fmod((0.024380407358 + 0.000005391235 * t) * t, TWOPI);
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
 * Converts an vector in equatorial rectangular coordinates to
 * equatorial spherical coordinates.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 * </ol>
 *
 * @param pos   Position 3-vector, equatorial rectangular coordinates.
 * @param ra    [h] Right ascension in hours.
 * @param dec   [deg] Declination in degrees.
 * @return      0 if successful, -1 of any of the arguments are NULL, or
 *              1 if all input components are 0 so 'ra' and 'dec' are indeterminate,
 *              or else 2 if both pos[0] and pos[1] are zero, but pos[2] is nonzero,
 *              and so 'ra' is indeterminate.
 */
short vector2radec(const double *pos, double *ra, double *dec) {
  double xyproj;

  if(!pos || !ra || !dec) {
    errno = EINVAL;
    return -1;
  }

  xyproj = sqrt(pos[0] * pos[0] + pos[1] * pos[1]);
  if((xyproj == 0.0) && (pos[2] == 0)) {
    *ra = 0.0;
    *dec = 0.0;
    return 1;
  }
  else if(xyproj == 0.0) {
    *ra = 0.0;
    if(pos[2] < 0.0) *dec = -90.0;
    else *dec = 90.0;
    return 2;
  }
  else {
    *ra = atan2(pos[1], pos[0]) / HOURANGLE;
    *dec = atan2(pos[2], xyproj) / DEGREE;

    if(*ra < 0.0) *ra += DAY_HOURS;
  }
  return 0;
}

/**
 * Converts equatorial spherical coordinates to a vector (equatorial
 * rectangular coordinates).
 *
 * @param ra          [h] Right ascension (hours).
 * @param dec         [deg] Declination (degrees).
 * @param dist        [AU] Distance (AU)
 * @param[out] vector [AU] Position 3-vector, equatorial rectangular coordinates (AU).
 *
 * @return            0 if successful, or -1 if the vector argument is NULL.
 */
int radec2vector(double ra, double dec, double dist, double *vector) {
  double cosdec;

  if(!vector) {
    errno = EINVAL;
    return -1;
  }

  dec *= DEGREE;
  ra *= HOURANGLE;

  cosdec = cos(dec);

  vector[0] = dist * cosdec * cos(ra);
  vector[1] = dist * cosdec * sin(ra);
  vector[2] = dist * sin(dec);

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
 *                     components in AU.
 * @param[out] vel     [AU/day] Velocity vector, equatorial rectangular coordinates,
 *                     components in AU/Day.
 *
 * @return             0 if successful, or -1 f the output vectors are NULL.
 */
int starvectors(const cat_entry *star, double *pos, double *vel) {
  double paralx, dist, r, d, cra, sra, cdc, sdc, pmr, pmd, rvl, k;

  if(!pos || !vel) {
    errno = EINVAL;
    return -1;
  }

  // If parallax is unknown, undetermined, or zero, set it to 1e-6
  // milliarcsecond, corresponding to a distance of 1 gigaparsec.
  paralx = star->parallax;

  if(star->parallax <= 0.0) paralx = 1.0e-6;

  // Convert right ascension, declination, and parallax to position vector
  // in equatorial system with units of AU.
  dist = 1.0 / sin(paralx * MAS);
  r = (star->ra) * HOURANGLE;
  d = (star->dec) * DEGREE;
  cra = cos(r);
  sra = sin(r);
  cdc = cos(d);
  sdc = sin(d);

  pos[0] = dist * cdc * cra;
  pos[1] = dist * cdc * sra;
  pos[2] = dist * sdc;

  // Compute Doppler factor, which accounts for change in
  // light travel time to star.
  k = 1.0 / (1.0 - star->radialvelocity / C * 1000.0);

  // Convert proper motion and radial velocity to orthogonal components of
  // motion with units of AU/Day.
  pmr = star->promora / (paralx * JULIAN_YEAR_DAYS) * k;
  pmd = star->promodec / (paralx * JULIAN_YEAR_DAYS) * k;
  rvl = star->radialvelocity * DAY / AU_KM * k;

  // Transform motion vector to equatorial system.
  vel[0] = -pmr * sra - pmd * sdc * cra + rvl * cdc * cra;
  vel[1] = pmr * cra - pmd * sdc * sra + rvl * cdc * sra;
  vel[2] = pmd * cdc + rvl * sdc;

  return 0;
}

/**
 * Computes the Terrestrial Time (TT) or Terrestrial Dynamical Time
 * (TDT) Julian date corresponding to a Barycentric Dynamical Time
 * (TDB) Julian date.
 *
 * Expression used in this function is a truncated form of a
 * longer and more precise series given in the first reference.  The
 * result is good to about 10 microseconds.
 *
 * @deprecated Use the less computationally intensive an more accurate tt2tdb()
 *            routine instead.
 *
 * REFERENCES:
 * <ol>
 * <li>Fairhead, L. & Bretagnon, P. (1990) Astron. & Astrophys. 229, 240.</li>
 * <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 * </ol>
 *
 * @param tdb_jd         [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param[out] tt_jd     [day] Terrestrial Time (TT) based Julian date
 * @param[out] secdiff   [s] Difference 'tdb_jd'-'tt_jd', in seconds. (It may be NULL if not required)
 * @return               0 if successful, or -1 if the tt_jd pointer argument is NULL.
 *
 * @sa tt2tdb()
 */
int tdb2tt(double tdb_jd, double *tt_jd, double *secdiff) {
  double t, d;

  if(!tt_jd) {
    errno = EINVAL;
    return -1;
  }

  t = (tdb_jd - JD_J2000) / JULIAN_CENTURY_DAYS;

  // Expression given in USNO Circular 179, eq. 2.6.
  d = 0.001657 * sin(628.3076 * t + 6.2401) + 0.000022 * sin(575.3385 * t + 4.2970) + 0.000014 * sin(1256.6152 * t + 6.1969)
          + 0.000005 * sin(606.9777 * t + 4.0212) + 0.000005 * sin(52.9691 * t + 0.4444) + 0.000002 * sin(21.3299 * t + 5.5431)
          + 0.000010 * t * sin(628.3076 * t + 4.2490);

  *tt_jd = tdb_jd - d / DAY;

  if(secdiff) *secdiff = d;

  return 0;
}

/**
 * Returns the TDB - TT time difference in seconds for a given TT date.
 *
 *
 * REFERENCES
 * <ol>
 * <li><a><href="https://gssc.esa.int/navipedia/index.php/Transformations_between_Time_Systems">
 * https://gssc.esa.int/navipedia/index.php/Transformations_between_Time_Systems</a>
 * </li>
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
  double t = (jd_tt - JD_J2000) / JULIAN_CENTURY_DAYS;
  double g = TWOPI * (357.528 + 35999.050 * t) * DEGREE;
  return 0.001658 * sin(g + 0.0167 * sin(g));
}

/**
 * Computes the true right ascension of the celestial
 * intermediate origin (CIO) at a given TT Julian date.  This is
 * the negative value for the equation of the origins.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 * </ol>
 *
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian date
 * @param accuracy  NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param ra_cio    [h] Right ascension of the CIO, with respect to the true equinox
 *                  of date, in hours (+ or -).
 * @return          0 if successful, -1 if the output pointer argument is NULL,
 *                  1 if 'accuracy' is invalid, 10--20: 10 + error code from cio_location(),
 *                  or else 20 + error from cio_basis().
 */
short cio_ra(double jd_tt, enum novas_accuracy accuracy, double *ra_cio) {
  short rs, error;

  const double unitx[3] = { 1.0, 0.0, 0.0 };
  double jd_tdb, x[3], y[3], z[3], eq[3], az, r_cio;

  if(!ra_cio) {
    errno = EINVAL;
    return -1;
  }

  // Check for valid value of 'accuracy'.
  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY) return 1;

  // 'jd_tdb' is the TDB Julian date.
  jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY;

  // Obtain the basis vectors, in the GCRS, for the celestial intermediate
  // system defined by the CIP (in the z direction) and the CIO (in the
  // x direction).
  error = cio_location(jd_tdb, accuracy, &r_cio, &rs);
  if(error) {
    *ra_cio = 0.0;
    return (error + 10);
  }

  error = cio_basis(jd_tdb, r_cio, rs, accuracy, x, y, z);
  if(error) {
    return (error + 20);
  }

  // Compute the direction of the true equinox in the GCRS.
  tod_to_icrs(jd_tdb, accuracy, unitx, eq);

  // Compute the RA-like coordinate of the true equinox in the celestial
  // intermediate system.
  az = atan2(vdot(eq, y), vdot(eq, x)) / DEGREE;

  // The RA of the CIO is minus this coordinate.
  *ra_cio = -az / 15.0;

  return 0;
}

/**
 * Sets the CIO interpolaton data file to use to interpolate CIO locations vs the GCRS.
 * The necessary binary data file may be obtained via the <code>cio_file.c</code> utility
 * provided in this distribution under <code>tools/</code>.
 *
 * @param filename    Path (preferably absolute path) to binary data file generated
 *                    by the <code>cio_file.c</code> utility from the <code>CIO_RA.TXT</code>
 *                    data file.
 * @return            0 if successful, or else -1 if the specified file does not exists or
 *                    we have no permission to read it.
 *
 * @sa cio_location()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int cio_set_locator_file(const char *filename) {
  if(access(filename, R_OK) != 0) return -1;

  if(cio_file) fclose(cio_file);
  cio_file = fopen(filename, "rb");

  return cio_file ? 0 : -1;
}

/**
 * Returns the location of the celestial
 * intermediate origin (CIO) for a given Julian date, as a
 * right ascension with respect to either the GCRS (geocentric ICRS)
 * origin or the true equinox of date.  The CIO is always located on
 * the true equator (= intermediate equator) of date.
 *
 * The user may specify an interpolation file to use via cio_set_locator_file() prior
 * to calling this function. In that case the call will return CIO location
 * relative to GCRS. In the absence of the table, it will calculate the CIO location
 * relative to the true equinox. In either case the type of the location is returned
 * alogside the CIO location value.
 *
 *
 * @param jd_tdb      [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param accuracy    NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] ra_cio      [h] Right ascension of the CIO, in hours.
 * @param[out] ref_sys     Pointer in which to return the reference system in which right ascension is given,
 *                         which is either CIO_VS_GCRS (1) if the location was obtained
 *                         via interpolation of the available data file, or else CIO_VS_EQUINOX (2)
 *                         if it was calculated locally.
 *
 * @return            0 if successful, -1 if one of the pointer arguments is NULL, or else
 *                    10 + the error from cio_array().
 *
 * @sa cio_set_locator_file()
 */
short cio_location(double jd_tdb, enum novas_accuracy accuracy, double *ra_cio, short *ref_sys) {
  static enum novas_accuracy acc_last = -1;
  static short ref_sys_last = -1;
  static double t_last = 0.0, ra_last = 0.0;
  static ra_of_cio cio[CIO_INTERP_POINTS];

  if(!ra_cio || !ref_sys) {
    errno = EINVAL;
    return -1;
  }

  // Check if previously computed RA value can be used.
  if(time_equals(jd_tdb, t_last) && (accuracy == acc_last)) {
    *ra_cio = ra_last;
    *ref_sys = ref_sys_last;
    return 0;
  }

  if(cio_file) {
    int j;

    int error = cio_array(jd_tdb, CIO_INTERP_POINTS, cio);
    if(error) {
      *ra_cio = 0.0;
      return (error + 10);
    }

    // Perform Lagrangian interpolation for the RA at 'tdb_jd'.
    *ra_cio = 0.0;
    for(j = 0; j < CIO_INTERP_POINTS; j++) {
      double p = 1.0;
      int i;
      for(i = 0; i < CIO_INTERP_POINTS; i++)
        if(i != j) p *= (jd_tdb - cio[i].jd_tdb) / (cio[j].jd_tdb - cio[i].jd_tdb);
      *ra_cio += p * cio[j].ra_cio;
    }

    *ra_cio /= 54000.0;
    *ref_sys = CIO_VS_GCRS;
  }
  else {
    // Calculate the equation of origins.
    *ra_cio = -1.0 * ira_equinox(jd_tdb, NOVAS_TRUE_EQUINOX, accuracy);
    *ref_sys = CIO_VS_EQUINOX;
  }

  t_last = jd_tdb;
  ra_last = *ra_cio;
  ref_sys_last = *ref_sys;

  return 0;
}

/**
 * Computes the orthonormal basis vectors, with
 * respect to the GCRS (geocentric ICRS), of the celestial
 * intermediate system defined by the celestial intermediate pole
 * (CIP) (in the z direction) and the celestial intermediate origin
 * (CIO) (in the x direction).  A TDB Julian date and the right
 * ascension of the CIO at that date is required as input.  The
 * right ascension of the CIO can be with respect to either the
 * GCRS origin or the true equinox of date -- different algorithms
 * are used in the two cases.
 *
 * This function effectively constructs the matrix C in eq. (3)
 * of the reference.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 * </ol>
 *
 * @param jd_tdb      [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param ra_cio      [h] Right ascension of the CIO at epoch (hours).
 * @param ref_sys     CIO_VS_GCRS if the cio location is relative to the GCRS or else
 *                    CIO_VS_EQUINOX if relative to the true equinox of date.
 * @param accuracy    NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[out] x      Unit 3-vector toward the CIO, equatorial rectangular
 *                    coordinates, referred to the GCRS.
 * @param[out] y      Unit 3-vector toward the y-direction, equatorial rectangular
 *                    coordinates, referred to the GCRS.
 * @param[out] z      Unit 3-vector toward north celestial pole (CIP), equatorial
 *                    rectangular coordinates, referred to the GCRS.
 * @return            0 if successful, or -1 if any of the output vector arguments are NULL,
 *                    or else 1 if 'ref-sys' is invalid.
 */
short cio_basis(double jd_tdb, double ra_cio, enum novas_cio_location_type ref_sys, enum novas_accuracy accuracy, double *x, double *y,
        double *z) {
  static enum novas_cio_location_type ref_sys_last = -1;
  static double t_last = 0.0, ra_last;
  static double xx[3], yy[3], zz[3];
  const double z0[3] = { 0.0, 0.0, 1.0 };
  double w0[3], sinra, cosra, xmag;

  if(!x || !y || !z) {
    errno = EINVAL;
    return -1;
  }

  // Compute unit vector z toward celestial pole.
  if(ra_cio != ra_last || !time_equals(jd_tdb, t_last) || (ref_sys != ref_sys_last)) {
    tod_to_icrs(jd_tdb, accuracy, z0, zz);
    t_last = jd_tdb;
    ra_last = ra_cio;
    ref_sys_last = ref_sys;
  }
  else {
    memcpy(x, xx, sizeof(xx));
    memcpy(y, yy, sizeof(yy));
    memcpy(z, zz, sizeof(zz));
    return 0;
  }

  // Now compute unit vectors x and y.  Method used depends on the
  // reference system in which right ascension of the CIO is given.
  ra_cio *= HOURANGLE;

  switch(ref_sys) {

    case CIO_VS_GCRS:

      // Compute vector x toward CIO in GCRS.
      sinra = sin(ra_cio);
      cosra = cos(ra_cio);
      xx[0] = zz[2] * cosra;
      xx[1] = zz[2] * sinra;
      xx[2] = -zz[0] * cosra - zz[1] * sinra;

      // Normalize vector x.
      xmag = vlen(xx);
      xx[0] /= xmag;
      xx[1] /= xmag;
      xx[2] /= xmag;

      // Compute unit vector y orthogonal to x and z (y = z cross x).
      yy[0] = zz[1] * xx[2] - zz[2] * xx[1];
      yy[1] = zz[2] * xx[0] - zz[0] * xx[2];
      yy[2] = zz[0] * xx[1] - zz[1] * xx[0];

      break;

    case CIO_VS_EQUINOX:

      // Construct unit vector toward CIO in equator-and-equinox-of-date
      // system.
      w0[0] = cos(ra_cio);
      w0[1] = sin(ra_cio);
      w0[2] = 0.0;

      // Rotate the vector into the GCRS to form unit vector x.
      tod_to_icrs(jd_tdb, accuracy, w0, xx);

      // Compute unit vector y orthogonal to x and z (y = z cross x).
      yy[0] = zz[1] * xx[2] - zz[2] * xx[1];
      yy[1] = zz[2] * xx[0] - zz[0] * xx[2];
      yy[2] = zz[0] * xx[1] - zz[1] * xx[0];

      break;

    default:
      // Invalid value of 'ref_sys'.
      memset(x, 0, XYZ_VECTOR_SIZE);
      memset(y, 0, XYZ_VECTOR_SIZE);
      memset(z, 0, XYZ_VECTOR_SIZE);
      return 1;
  }

  // Load the x, y, and z arrays.
  memcpy(x, xx, sizeof(xx));
  memcpy(y, yy, sizeof(yy));
  memcpy(z, zz, sizeof(zz));

  return 0;
}

/**
 * Given an input TDB Julian date and the number of data points
 * desired, this function returns a set of Julian dates and
 * corresponding values of the GCRS right ascension of the celestial
 * intermediate origin (CIO).  The range of dates is centered (at
 * least approximately) on the requested date.  The function obtains
 * the data from an external data file.
 *
 * This function assumes that binary, random-access file
 * has been created and is either in the same directory as
 * your executable; or you set its location previously via cio_set_locator_file().
 * This file is created by program 'cio_file.c'.
 *
 * NOTES:
 * <ol>
 * <li>This function has been completely re-written by A. Kovacs to provide
 * much more efficient caching and I/O.</li>
 * </ol>
 *
 * @param jd_tdb    [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param n_pts     Number of Julian dates and right ascension values requested
 *                  (not less than 2 or more than 20).
 * @param[out] cio  A time series (array) of the right ascension of the Celestial
 *                  Intermediate Origin (CIO) with respect to the GCRS.
 * @return          0 if successful, -1 if the output array is NULL or there
 *                  was an I/O error accessing the CIO location data file. Or else 1 if no
 *                  locator data file is available, 2 if 'jd_tdb' not in the range of the CIO file,
 *                  3 if 'n_pts' out of range, 4 if  unable to allocate memory for the internal 't' array,
 *                  5 if unable to allocate memory for the internal 'ra' array, or 6
 *                  if 'jd_tdb' is too close to either end of the CIO file do we are unable to put
 *                  'n_pts' data points into the output
 *
 * @sa cio_set_locator_file()
 * @sa cio_location()
 */
short cio_array(double jd_tdb, long n_pts, ra_of_cio *cio) {
  static FILE *last_file;
  static ra_of_cio cache[NOVAS_CIO_CACHE_SIZE];
  static long index_cache, cache_count, n_recs;
  static double jd_beg, jd_end, t_int;

  long index_rec;

  if(cio == NULL) {
    errno = EINVAL;
    return -1;
  }

  if(n_pts < 2 || n_pts > NOVAS_CIO_CACHE_SIZE) {
    errno = ERANGE;
    return 3;               // n_pts is out of bounds
  }

  if(cio_file == NULL) cio_set_locator_file(DEFAULT_CIO_LOCATOR_FILE);  // Try default locator file.
  if(cio_file == NULL) {
    errno = ENODEV;
    cache_count = 0;
    return 1;
  }

  // Check if it's a new file.
  if(last_file != cio_file) {
    cache_count = 0;

    // Read the file header
    if(fread(&jd_beg, sizeof(double), 1, cio_file) != sizeof(double)) return -1;
    if(fread(&jd_end, sizeof(double), 1, cio_file) != sizeof(double)) return -1;
    if(fread(&t_int, sizeof(double), 1, cio_file) != sizeof(double)) return -1;
    if(fread(&n_recs, sizeof(long), 1, cio_file) != sizeof(double)) return -1;
  }

  //Check the input data against limits.
  if((jd_tdb < jd_beg) || (jd_tdb > jd_end)) {
    errno = EOF;
    return 2;
  }

  // Calculate the record index from which data is requested.
  index_rec = (long) ((jd_tdb - jd_beg) / t_int) - (n_pts >> 1);
  if(index_rec < 0) {
    errno = EOF;
    return 6;  // Data requested before file...
  }

  // Check if the range of data needed is outside the cached range.
  if((index_rec < index_cache) || (index_rec + n_pts > index_cache + cache_count)) {
    // Load cache centered on requested range.
    const size_t header_size = 3 * sizeof(double) + sizeof(long);
    const long N = n_recs - index_rec > NOVAS_CIO_CACHE_SIZE ? NOVAS_CIO_CACHE_SIZE : n_recs - index_rec;

    cache_count = 0;
    index_cache = index_rec - (N >> 1);
    if(index_cache < 0) index_cache = 0;

    // Read in cache from the requested position.
    if(fseek(cio_file, header_size + index_cache * sizeof(ra_of_cio), SEEK_SET) < 0) return -1;
    if(fread(&cache, sizeof(ra_of_cio), n_pts, cio_file) != n_pts * sizeof(ra_of_cio)) return -1;

    cache_count = N;
  }

  if(n_pts < cache_count) {
    errno = EOF;
    return 6; // Data requested beyond file...
  }

  // Copy the requested number of points in to the destination;
  memcpy(cio, &cache[index_rec - index_cache], n_pts * sizeof(ra_of_cio));

  return 0;
}

/**
 * Compute the intermediate right ascension of the equinox at
 * the input Julian date, using an analytical expression for the
 * accumulated precession in right ascension.  For the true equinox,
 * the result is the equation of the origins.
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
 * @param equinox     NOVAS_MEAN_EQUINOX or NOVAS_TRUE_EQUINOX
 * @param accuracy    NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @return            [h]  Intermediate right ascension of the equinox, in hours (+ or -).
 *                    If 'equinox' = 1 (i.e true equinox), then the returned value is
 *                    the equation of the origins.
 */
double ira_equinox(double jd_tdb, enum novas_equinox_type equinox, enum novas_accuracy accuracy) {
  static enum novas_equinox_type last_type = -999;
  static enum novas_accuracy acc_last = NOVAS_FULL_ACCURACY;
  static double t_last = 0.0, last_ra;

  double t, eqeq = 0.0, prec_ra;

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

  return (last_ra);
}

/**
 * Sets the function to use for obtaining position / velocity information for minor planets, or sattelites.
 *
 * @param f   new function to use for accessing ephemeris data for minor planets or satellites.
 * @return    0 if successful, or else -1 if the function argument is NULL.
 *
 * @since 1.0
 * @author Attila Kovacs
 *
 * @sa get_ephem_reasder()
 * @sa ephemeris()
 *
 */
int set_ephem_reader(novas_ephem_reader_func f) {
  if(!f) {
    errno = EINVAL;
    return -1;
  }
  readeph2_call = f;
  return 0;
}

/**
 * Returns the user-defined ephemeris accessor function.
 *
 * @return    the currently defined function for accessing ephemeris data for minor planets or satellites, ot NULL if no function was set
 *            via set_ephem_reader() previously.
 *
 * @sa set_ephem_reader()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
novas_ephem_reader_func get_ephem_reader() {
  return readeph2_call;
}

/**
 * Retrieves the position and velocity of a solar system body from
 * a fundamental ephemeris.
 *
 * It is recommended that the input structure 'cel_obj' be
 * created using make_object()
 *
 * @param jd_tdb    [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param cel_obj   Pointer to structure containing the designation of the body of interest
 * @param origin    NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1)
 * @param accuracy  NOCAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] pos  [AU] Pointer to structure containing the designation of the body of interest
 * @param[out] vel  [AU/day] Velocity vector of the body at 'jd_tdb'; equatorial rectangular
 *                  coordinates in AU/day referred to the ICRS.
 * @return          0 if successful, -1 if the input object argument is NULL, or else
 *                  1 if 'origin' is invalid, 2 if <code>cel_obj->type</code> is invalid,
 *                  10 + the error code from solarsystem(), or
 *                  20 + the error code from readeph().
 *
 * @sa ephem_open()
 * @sa set_planet_calc()
 * @sa set_planet_calc_hp()
 * @sa set_ephem_reader()
 * @sa make_object()
 */
short ephemeris(const double jd_tdb[2], const object *cel_obj, enum novas_origin origin, enum novas_accuracy accuracy, double *pos,
        double *vel) {

  double posvel[6] = { };
  int error = 0;

  if(!cel_obj) {
    errno = EINVAL;
    return -1;
  }

  // Check the value of 'origin'.
  if((origin < 0) || (origin >= NOVAS_ORIGIN_TYPES)) return 1;

  // Invoke the appropriate ephemeris access software depending upon the
  // type of object
  switch(cel_obj->type) {

    case NOVAS_MAJOR_PLANET:
      // Get the position and velocity of a major planet, Pluto, Sun, or Moon.
      // When high accuracy is specified, use function 'solarsystem_hp' rather
      // than 'solarsystem'.

      if(accuracy == NOVAS_FULL_ACCURACY) {
#       ifdef DEFAULT_SOLSYS
        extern novas_planet_calculator_hp default_planetcalc_hp;
        if(!planetcalc_hp) set_planet_calc_hp(default_planetcalc_hp);
#       endif
        error = planetcalc_hp(jd_tdb, cel_obj->number, origin, pos, vel);
      }
      else {
#       ifdef DEFAULT_SOLSYS
        extern novas_planet_calculator default_planetcalc;
        if(!planetcalc) set_planet_calc(default_planetcalc);
#       endif
        error = planetcalc(jd_tdb[0] + jd_tdb[1], cel_obj->number, origin, pos, vel);
      }

      if(error) return error + 10;
      break;

    case NOVAS_MINOR_PLANET: {
      enum novas_origin eph_origin = NOVAS_HELIOCENTER;

      error = -1;
      if(readeph2_call) {
        // If there is a newstyle epehemeris access routine set, we will prefer it.
        error = readeph2_call(cel_obj->number, cel_obj->name, jd_tdb[0], jd_tdb[1], &eph_origin, posvel, &posvel[3]);
      }
#     ifdef DEFAULT_READEPH
      else {
        // Use whatever readeph() was compiled or the equivalent user-defined call
        double *res = readeph(cel_obj->number, cel_obj->name, jd_tdb[0] + jd_tdb[1], &error);
        if(res == NULL) return 3;
        memcpy(posvel, res, sizeof(posvel));
        free(res);
      }
#     endif

      if(error) return (20 + error);

      // Check and adjust the origins as necessary.
      if(origin != eph_origin) {
        double pos0[3] = { }, vel0[3] = { };
        enum novas_planet refnum = (origin == NOVAS_BARYCENTER) ? NOVAS_BARYCENTER_POS : NOVAS_SUN;
        int i;

        error = planetcalc(jd_tdb[0] + jd_tdb[1], refnum, eph_origin, pos0, vel0);
        if(error) return (error + 10);

        for(i = 0; i < 3; i++) {
          posvel[i] -= pos[i];
          posvel[i + 3] -= vel[i];
        }
      }

      // Break up 'posvel' into separate position and velocity vectors.
      memcpy(pos, posvel, XYZ_VECTOR_SIZE);
      memcpy(vel, &posvel[3], XYZ_VECTOR_SIZE);

      break;
    }

    default:
      // Invalid type of object.
      return 2;

  }

  return 0;
}

/**
 * Convert Hipparcos catalog data at epoch J1991.25 to epoch
 * J2000.0, for use within NOVAS.  To be used only for Hipparcos or
 * Tycho stars with linear space motion.  Both input and output data
 * is in the ICRS.
 *
 * @param hipparcos       An entry from the Hipparcos catalog, at epoch J1991.25, with
 *                        'ra' in degrees(!) as per Hipparcos catalog units.
 * @param[out] hip_2000   The transformed input entry, at epoch J2000.0, with 'ra' in hours(1) as
 *                        per the NOVAS convention.
 *
 * @return            0 if successful, or -1 if either of the input pointer arguments is NULL.
 */
int transform_hip(const cat_entry *hipparcos, cat_entry *hip_2000) {
  const double epoch_hip = 2448349.0625;

  cat_entry scratch;

  if(!hipparcos || !hip_2000) {
    errno = EINVAL;
    return -1;
  }

  // Set up a "scratch" catalog entry containing Hipparcos data in
  // "NOVAS units."
  strcpy(scratch.starname, hipparcos->starname);
  scratch.starnumber = hipparcos->starnumber;
  scratch.dec = hipparcos->dec;
  scratch.promora = hipparcos->promora;
  scratch.promodec = hipparcos->promodec;
  scratch.parallax = hipparcos->parallax;
  scratch.radialvelocity = hipparcos->radialvelocity;

  strcpy(scratch.catalog, "SCR");

  // Convert right ascension from degrees to hours.
  scratch.ra = hipparcos->ra / 15.0;

  // Change the epoch of the Hipparcos data from J1991.25 to J2000.0.
  transform_cat(1, epoch_hip, &scratch, JD_J2000, "HP2", hip_2000);

  return 0;
}

/**
 *  Transform a star's catalog quantities for a change of epoch
 * and/or equator and equinox.  Also used to rotate catalog
 * quantities on the dynamical equator and equinox of J2000.0 to the
 * ICRS or vice versa.
 *
 * 'date_incat' and 'date_newcat' may be specified either as a
 * Julian date (e.g., 2433282.5) or a Julian year and fraction
 * (e.g., 1950.0).  Values less than 10000 are assumed to be years.
 * For 'option' = 2 or 'option' = 3, either 'date_incat' or
 * 'date_newcat' must be 2451545.0 or 2000.0 (J2000.0).  For
 * 'option' = 4 and 'option' = 5, 'date_incat' and 'date_newcat'
 * are ignored.
 *
 * If 'option' is CHANGE_EPOCH, input data can be in any fixed reference
 * system. If 'option' is CHANGE_EQUATOR_EQUINOX or CHANGE_EQUATOR_EQUINOX_EPOCH, this function assumes
 * the input data is in the dynamical system and produces output in
 * the dynamical system. If 'option' is CHANGE_J2000_TO_ICRS, the input data must be
 * on the dynamical equator and equinox of J2000.0. And if 'option' is CHANGE_ICRS_TO_J2000,
 * the input data must be in the ICRS.
 *
 * This function cannot be properly used to bring data from
 * old star catalogs into the modern system, because old catalogs
 * were compiled using a set of constants that are
 * incompatible with modern values.  In particular, it should not
 * be used for catalogs whose positions and proper motions were
 * derived by assuming a precession constant significantly different
 * from the value implicit in function 'precession'.
 *
 * @param option          Type of transformation
 * @param date_incat      [day|yr] Terrestrial Time (TT) based Julian date, or year, of input catalog data.
 * @param incat           An entry from the input catalog, with units as given in the struct definition
 * @param date_newcat     [day|yr] Terrestrial Time (TT) based Julian date, or year, of output catalog data.
 * @param newcat_id       Catalog identifier (0 terminated)
 * @param[out] newcat     The transformed catalog entry, with units as given in the struct definition
 * @return                0 if successful, -1 if any of the pointer arguments is NULL,
 *                        1 if the input date is invalid for for option CHANGE_EQUATOR_EQUINOX or
 *                        CHANGE_EQUATOR_EQUINOX_EPOCH, or 2 if 'newcat_id' out of bounds.
 */
short transform_cat(enum novas_transform_type option, double date_incat, const cat_entry *incat, double date_newcat,
        const char *newcat_id, cat_entry *newcat) {

  double jd_incat, jd_newcat, paralx, dist, r, d, cra, sra, cdc, sdc, k;
  double pos1[3], term1, pmr, pmd, rvl, vel1[3], pos2[3], vel2[3], xyproj;

  if(!incat || !newcat || !newcat_id) {
    errno = EINVAL;
    return -1;
  }

  if(strlen(newcat_id) >= sizeof(newcat->starname)) return 2;

  // If necessary, compute Julian dates.

  // This function uses TDB Julian dates internally, but no distinction between TDB and TT is necessary.
  if(date_incat < 10000.0) jd_incat = JD_J2000 + (date_incat - 2000.0) * JULIAN_YEAR_DAYS;
  else jd_incat = date_incat;

  if(date_newcat < 10000.0) jd_newcat = JD_J2000 + (date_newcat - 2000.0) * JULIAN_YEAR_DAYS;
  else jd_newcat = date_newcat;

  // Convert input angular components to vectors

  // If parallax is unknown, undetermined, or zero, set it to 1.0e-6
  // milliarcsecond, corresponding to a distance of 1 gigaparsec.
  paralx = incat->parallax;
  if(paralx <= 0.0) paralx = 1.0e-6;

  // Convert right ascension, declination, and parallax to position
  // vector in equatorial system with units of AU.
  dist = 1.0 / sin(paralx * MAS);
  r = incat->ra * HOURANGLE;
  d = incat->dec * DEGREE;
  cra = cos(r);
  sra = sin(r);
  cdc = cos(d);
  sdc = sin(d);
  pos1[0] = dist * cdc * cra;
  pos1[1] = dist * cdc * sra;
  pos1[2] = dist * sdc;

  // Compute Doppler factor, which accounts for change in light travel time to star.
  k = 1.0 / (1.0 - incat->radialvelocity / C * 1000.0);

  // Convert proper motion and radial velocity to orthogonal components
  // of motion, in spherical polar system at star's original position,
  // with units of AU/day.
  term1 = paralx * JULIAN_YEAR_DAYS;
  pmr = incat->promora / term1 * k;
  pmd = incat->promodec / term1 * k;
  rvl = incat->radialvelocity * DAY / AU_KM * k;

  // Transform motion vector to equatorial system.
  vel1[0] = -pmr * sra - pmd * sdc * cra + rvl * cdc * cra;
  vel1[1] = pmr * cra - pmd * sdc * sra + rvl * cdc * sra;
  vel1[2] = pmd * cdc + rvl * sdc;

  memcpy(vel2, vel1, sizeof(vel2));

  // Update star's position vector for space motion (only if 'option' = 1 or 'option' = 3).
  if((option == CHANGE_EPOCH) || (option == CHANGE_EQUATOR_EQUINOX_EPOCH)) {
    int j;
    for(j = 0; j < 3; j++)
      pos2[j] = pos1[j] + vel1[j] * (jd_newcat - jd_incat);
  }
  else memcpy(pos2, pos1, sizeof(pos2));

  switch(option) {

    case CHANGE_EQUATOR_EQUINOX:
    case CHANGE_EQUATOR_EQUINOX_EPOCH: {
      // Precess position and velocity vectors (only if 'option' = 2 or 'option' = 3).
      int error;

      memcpy(pos1, pos2, sizeof(pos1));
      memcpy(vel1, vel2, sizeof(vel1));

      error = precession(jd_incat, pos1, jd_newcat, pos2);
      if(error) return (error);

      precession(jd_incat, vel1, jd_newcat, vel2);
      break;
    }

    case CHANGE_J2000_TO_ICRS:
      // Rotate dynamical J2000.0 position and velocity vectors to ICRS (only if 'option' = 4).
      frame_tie(pos1, TIE_J2000_TO_ICRS, pos2);
      frame_tie(vel1, TIE_J2000_TO_ICRS, vel2);
      break;

    case CHANGE_ICRS_TO_J2000:
      // Rotate ICRS position and velocity vectors to dynamical J2000.0 (only if 'option' = 5).
      frame_tie(pos1, TIE_ICRS_TO_J2000, pos2);
      frame_tie(vel1, TIE_ICRS_TO_J2000, vel2);
      break;

    default:
      // Nothing to do here for other options...
  }

  // Convert vectors back to angular components for output.

  // From updated position vector, obtain star's new position expressed as angular quantities.
  xyproj = sqrt(pos2[0] * pos2[0] + pos2[1] * pos2[1]);

  r = (xyproj > 0.0) ? atan2(pos2[1], pos2[0]) : 0.0;
  newcat->ra = r / HOURANGLE;
  if(newcat->ra < 0.0) newcat->ra += DAY_HOURS;
  if(newcat->ra >= DAY_HOURS) newcat->ra -= DAY_HOURS;

  d = atan2(pos2[2], xyproj);
  newcat->dec = d / DEGREE;

  dist = vlen(pos2);

  paralx = asin(1.0 / dist) / MAS;
  newcat->parallax = paralx;

  // Transform motion vector back to spherical polar system at star's new position.
  cra = cos(r);
  sra = sin(r);
  cdc = cos(d);
  sdc = sin(d);
  pmr = -vel2[0] * sra + vel2[1] * cra;
  pmd = -vel2[0] * cra * sdc - vel2[1] * sra * sdc + vel2[2] * cdc;
  rvl = vel2[0] * cra * cdc + vel2[1] * sra * cdc + vel2[2] * sdc;

  // Convert components of motion to from AU/day to normal catalog units.
  newcat->promora = pmr * paralx * JULIAN_YEAR_DAYS / k;
  newcat->promodec = pmd * paralx * JULIAN_YEAR_DAYS / k;
  newcat->radialvelocity = rvl * (AU_KM / DAY) / k;

  // Take care of zero-parallax case.
  if(newcat->parallax <= 1.01e-6) {
    newcat->parallax = 0.0;
    newcat->radialvelocity = incat->radialvelocity;
  }

  // Set the catalog identification code for the transformed catalog entry.
  strncpy(newcat->catalog, newcat_id, sizeof(newcat->catalog) - 1);

  // Copy unchanged quantities from the input catalog entry to the transformed catalog entry.
  strncpy(newcat->starname, incat->starname, sizeof(newcat->starname) - 1);
  newcat->starnumber = incat->starnumber;

  return 0;
}

/**
 * determines the angle of an object above or below the Earth's limb (horizon).  The geometric limb is computed,
 * assuming the Earth to be an airless sphere (no refraction or oblateness is included).  The observer can be on or
 * above the Earth.  For an observer on the surface of the Earth, this function returns the approximate unrefracted altitude.
 *
 * @param pos_obj         [AU] Position 3-vector of observed object, with respect to origin at geocenter, components in AU.
 * @param pos_obs         [AU] Position 3-vector of observer, with respect to origin at geocenter, components in AU.
 * @param[out] limb_ang   [deg] Angle of observed object above (+) or below (-) limb in degrees. It may be NULL if not required.
 * @param[out] nadir_ang  [deg] Nadir angle of observed object as a fraction of apparent radius of limb:
 *                        <1.0 if below the limb; 1.0 on the limb; or &gt1.0 if above the limb. It may be NULL if not required.
 *
 * @return    0 if successful, or -1 if either of the input vectors is NULL.
 */
int limb_angle(const double *pos_obj, const double *pos_obs, double *limb_ang, double *nadir_ang) {
  double disobj, disobs, aprad, zdlim, coszd, zdobj;

  if(!pos_obj || !pos_obs) {
    errno = EINVAL;
    return -1;
  }
  // Compute the distance to the object and the distance to the observer.
  disobj = vlen(pos_obj);
  disobs = vlen(pos_obs);

  // Compute apparent angular radius of Earth's limb.
  aprad = (disobs >= ERAD_AU) ? asin(ERAD_AU / disobs) : HALF_PI;

  // Compute zenith distance of Earth's limb.
  zdlim = M_PI - aprad;

  // Compute zenith distance of observed object.
  coszd = vdot(pos_obj, pos_obs) / (disobj * disobs);

  if(coszd <= -1.0) zdobj = M_PI;
  else if(coszd >= 1.0) zdobj = 0.0;
  else zdobj = acos(coszd);

  // Angle of object wrt limb is difference in zenith distances.
  if(limb_ang) *limb_ang = (zdlim - zdobj) / DEGREE;

  // Nadir angle of object as a fraction of angular radius of limb.
  if(nadir_ang) *nadir_ang = (M_PI - zdobj) / aprad;

  return 0;
}

/**
 * computes atmospheric refraction in zenith distance.  This version computes approximate refraction for
 * optical wavelengths.  This function can be used for planning observations or telescope pointing, but should not be used
 * for precise positioning.
 *
 * @param location      Pointer to structure containing observer's location. It may also contains weather data (optional)
 *                      for the observer's location.
 * @param ref_option    NOVAS_STANDARD_ATMOSPHERE, or NOVAS_WEATHER_AT_LOCATION if to use the weather values contained in
 *                      the 'location' data structure.
 * @param zd_obs        [deg] Zenith distance angle of the direction of observation.
 * @return              [deg] the calculated optical refraction.
 */
double refract(const on_surface *location, enum novas_refraction_model ref_option, double zd_obs) {
  // 's' is the approximate scale height of atmosphere in meters.
  const double s = 9.1e3;
  double p, t, h, r;

  // Compute refraction only for zenith distances between 0.1 and 91 degrees.
  if((zd_obs < 0.1) || (zd_obs > 91.0)) return 0.0;

  // If observed weather data are available, use them.  Otherwise, use
  // crude estimates of average conditions.
  if(ref_option == 2) {
    p = location->pressure;
    t = location->temperature;
  }
  else {
    p = 1010.0 * exp(-location->height / s);
    t = 10.0;
  }

  h = 90.0 - zd_obs;
  r = 0.016667 / tan((h + 7.31 / (h + 4.4)) * DEGREE);
  return r * (0.28 * p / (t + 273.0));
}

/**
 * Returns the Julian date for a given Gregorian calendar date. This function makes no checks for a valid input calendar date.
 * Input calendar date must be Gregorian. Input time value can be based on any UT-like time scale (UTC, UT1, TT, etc.) -
 * output Julian date will have the same basis.
 *
 * @param year    [yr] Gregorian calendar year
 * @param month   [month] Gregorian calendar month [1:12]
 * @param day     [day] Day of month [1:31]
 * @param hour    [hr] Hour of day [0:24]
 * @return        [day] the fractional Julian date for the input calendar date
 *
 * @sa calendar_date()
 */
double julian_date(short year, short month, short day, double hour) {
  long jd12h = day - 32075L + 1461L * (year + 4800L + (month - 14L) / 12L) / 4L
          + 367L * (month - 2L - (month - 14L) / 12L * 12L) / 12L
          - 3L * ((year + 4900L + (month - 14L) / 12L) / 100L) / 4L;
  return jd12h - 0.5 + hour / DAY_HOURS;
}

/**
 * This function will compute a broken down date on the Gregorian calendar for given the Julian date input.
 * This routine valid for any 'jd' greater than zero. Input Julian date can be based on any UT-like time scale
 * (UTC, UT1, TT, etc.) - output time value will have same basis.
 *
 * REFERENCES:
 *
 * <ol>
 *  <li>Fliegel, H. & Van Flandern, T.  Comm. of the ACM, Vol. 11, No. 10, October 1968, p. 657.</li>
 * </ol>
 *
 * @param tjd     [day] Julian date
 * @param[out] year    [yr] Gregorian calendar year. It may be NULL if not required.
 * @param[out] month   [month] Gregorian calendat month [1:12]. It may be NULL if not required.
 * @param[out] day     [day] Day of the month [1:31]. It may be NULL if not required.
 * @param[out] hour    [h] Hour of day [0:24]. It may be NULL if not required.
 *
 * @return              0
 *
 * @sa julian_date()
 */
int cal_date(double tjd, short *year, short *month, short *day, double *hour) {
  long jd, k, m, n;
  short y, mo, d, h;
  double djd;

  djd = tjd + 0.5;
  jd = (long) floor(djd);

  h = fmod(djd, 1.0) * DAY_HOURS;

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

  if(year) *year = y;
  if(month) *month = mo;
  if(day) *day = d;
  if(hour) *hour = h;

  return 0;
}

/**
 * Returns the normalized angle in the [0:2&pi;) range.
 *
 * @param angle   [rad] an angle in radians.
 * @return        [rad] the normalized angle in the [0:2&pi;) range.
 */
double norm_ang(double angle) {
  double a;

  a = remainder(angle, TWOPI);
  if(a < 0.0) a += TWOPI;

  return a;
}

/**
 *
 *
 * @param star_name   Object name (less than SIZE_OF_OBJ_NAME in length).
 * @param catalog     Catalog identifier (less than SIZE_OF_CAT_NAME in length). E.g.
 *                    'HIP' = Hipparcos, 'TY2' = Tycho-2.
 * @param star_num    Object number in the catalog.
 * @param ra          [h] Right ascension of the object (hours).
 * @param dec         [deg] Declination of the object (degrees).
 * @param pm_ra       [mas/yr] Proper motion in right ascension (milliarcseconds/year).
 * @param pm_dec      [mas/yr] Proper motion in declination (milliarcseconds/year).
 * @param parallax    [mas] Parallax (milliarcseconds).
 * @param rad_vel     [km/s] Radial velocity (LSR)
 * @param[out] star   Pointer to data structure to populate.
 * @return            0 if successful, or -1 if the output argument is NULL, 1 if the 'star_name' is too long
 *                    or 2 if the 'catalog' name is too long.
 *
 */
short make_cat_entry(const char *star_name, const char *catalog, long star_num, double ra, double dec, double pm_ra, double pm_dec,
        double parallax, double rad_vel, cat_entry *star) {

  if(!star) {
    errno = EINVAL;
    return -1;
  }

  memset(star, 0, sizeof(*star));

  // Set up the 'star' structure.
  if(!star_name) return 1;
  if(!catalog) return 2;

  if(strlen(star_name) >= sizeof(star->starname)) return 1;
  strncpy(star->starname, star_name, sizeof(star->starname) - 1);

  if(strlen(catalog) >= sizeof(star->catalog)) return 2;
  strncpy(star->catalog, catalog, sizeof(star->catalog) - 1);

  star->starnumber = star_num;
  star->ra = ra;
  star->dec = dec;
  star->promora = pm_ra;
  star->promodec = pm_dec;
  star->parallax = parallax;
  star->radialvelocity = rad_vel;

  return 0;
}

/**
 * Populates and object data structure using the parameters provided.
 *
 * @param type          The type of object. NOVAS_MAJOR_PLANET, NOVAS_MINOR_PLANET or NOVAS_DISTANT_OBJECT
 * @param number        The novas ID number (for solar-system bodies only, otherwise ignored)
 * @param name          The name of the object (case insensitive). It should be shorter than SIZE_OF_OBJ_NAME
 *                      or else an error will be returned.
 * @param star_data     Pointer to structure to populate with the catalog data for a celestial object located outside the solar system. Used only if type is NOVAS_DISTANT_OBJECT, otherwise ignored and can be NULL.
 * @param[out] cel_obj  Pointer to the celestial object data structure to be populated. Used only if 'type' is NOVAS_MAJOR_PLANET or NOVAS_MINOR_PLANET, otherwise ignored and may be NULL.
 * @return              0 if successful, or else 1 if 'type' is invalid, 2 if 'number' is out of range,
 *                      3 if cel_obj is NULL, 4 if star_data is NULL, or 5 if 'name' is too long.
 *                      (The return values 3 and 4 are somewhat different than in the original NOVAS. However
 *                      they are produced in very similar contexts, and for similar reasons).
 *
 * @sa place()
 */
short make_object(enum novas_object_type type, int number, const char *name, const cat_entry *star_data, object *cel_obj) {

  if(!cel_obj) {
    errno = EINVAL;
    return 3;
  }

  memset(cel_obj, 0, sizeof(*cel_obj));

  // Set the object type.
  if((type < 0) || (type >= NOVAS_OBJECT_TYPES)) return 1;
  else cel_obj->type = type;

  // Set the object number.
  if(type == NOVAS_MAJOR_PLANET) {
    if((number < 0) || (number >= NOVAS_PLANETS)) return 2;
  }
  else if(type == NOVAS_MINOR_PLANET) {
    if(number <= 0) {
      errno = ERANGE;
      return 2;
    }
  }
  else number = 0;

  cel_obj->number = number;

  if(name) {
    int i;

    for(i = 0; name[i]; i++) {
      // if input name is not null-terminated return error;
      if(i == (sizeof(cel_obj->name) - 1)) {
        errno = EFAULT;
        return 5;
      }

      cel_obj->name[i] = (char) toupper(name[i]);
    }
  }

  // Populate the astrometric-data structure if the object is 'type' = 2.
  if(type == NOVAS_DISTANT_OBJECT) {
    if(!star_data) {
      errno = EINVAL;
      return 4;      // TODO different meaning than original...
    }
    cel_obj->star = *star_data;
  }

  return 0;
}

/**
 * Populates an 'observer' data structure given the parameters. The output data structure may be used an the the inputs to NOVAS-C
 * function 'place()'.
 *
 * @param where         The location type of the observer
 * @param obs_surface   Pointer to data structure that defines a location on Earth's surface. Used only if 'where' is NOVAS_OBSERVER_AT_GEOCENTER, otherwise can be NULL.
 * @param obs_space     Pointer to data structure that defines a near-Earth location in space. Used only if 'where' is NOVAS_OBSERVER_IN_EARTH_ORBIT, otherwise can be NULL.
 * @param[out] obs      Pointer to observer data structure to populate.
 * @return              0 if successful, -1 if a required argument is NULL, or 1 if the 'where' argument is invalid.
 *
 * @sa make_observer_at_geocenter()
 * @sa make_observer_on_surface()
 * @sa make_observer_in_space()
 */
short make_observer(enum novas_observer_place where, const on_surface *obs_surface, const in_space *obs_space, observer *obs) {
  if(!obs) {
    errno = EINVAL;
    return -1;
  }

  // Initialize the output structure.
  memset(obs, 0, sizeof(*obs));

  // Populate the output structure based on the value of 'where'.
  switch(where) {
    case (NOVAS_OBSERVER_AT_GEOCENTER): /* Geocentric */
      break;

    case (NOVAS_OBSERVER_ON_EARTH): /* On surface of Earth */
      if(!obs_surface) {
        errno = EINVAL;
        return -1;
      }
      memcpy(&obs->on_surf, obs_surface, sizeof(obs->on_surf));
      break;

    case NOVAS_OBSERVER_IN_EARTH_ORBIT: /* In near-Earth spacecraft */
      if(!obs_space) {
        errno = EINVAL;
        return -1;
      }
      memcpy(&obs->near_earth, obs_space, sizeof(obs->near_earth));
      break;

    default:
      return 1;
  }

  return 0;
}

/**
 * Populates an 'observer' data structure for a hypothetical observer located at Earth's geocenter.
 * The output data structure may be used an the the inputs to NOVAS-C
 * function 'place()'.
 *
 * @param[out] obs_at_geocenter    Pointer to data structure to populate.
 * @return          0 if successful, or -1 if the output argument is NULL.
 *
 * @sa make_observer_in_space()
 * @sa make_observer_on_surface()
 * @sa place()
 */
int make_observer_at_geocenter(observer *obs_at_geocenter) {
  if(!obs_at_geocenter) {
    errno = EINVAL;
    return -1;
  }
  memset(obs_at_geocenter, 0, sizeof(*obs_at_geocenter));
  obs_at_geocenter->where = NOVAS_OBSERVER_AT_GEOCENTER;
  return 0;
}

/**
 * Populates and 'on_surface' data structure with the specified location defining parameters of the observer.
 * The output data structure may be used an the the inputs to NOVAS-C
 * function 'place()'.
 *
 * @param latitude      [deg] Geodetic (ITRS) latitude in degrees; north positive.
 * @param longitude     [deg] Geodetic (ITRS) longitude in degrees; east positive.
 * @param height        [m] Altitude over se level of the observer (meters).
 * @param temperature   [C] Temperature (degrees Celsius).
 * @param pressure      [mbar] Atmospheric pressure (millibars).
 * @param[out] obs_on_surface   Pointer to the data structure to populate.
 *
 * @return          0 if successful, or -1 if the output argument is NULL.
 *
 * @sa make_observer_in_space()
 * @sa make_observer_at_geocenter()
 * @sa place()
 */
int make_observer_on_surface(double latitude, double longitude, double height, double temperature, double pressure,
        observer *obs_on_surface) {
  if(!obs_on_surface) {
    errno = EINVAL;
    return -1;
  }

  memset(obs_on_surface, 0, sizeof(*obs_on_surface));
  obs_on_surface->where = NOVAS_OBSERVER_ON_EARTH;
  make_on_surface(latitude, longitude, height, temperature, pressure, &obs_on_surface->on_surf);
  return 0;
}

/**
 * Populates an 'observer' data structure, for an observer situated on a near-Earth spacecraft, with
 * the specified geocentric position and velocity vectors. Both input vectors are with respect to true
 * equator and equinox of date. The output data structure may be used an the the inputs to NOVAS-C
 * function 'place()'.
 *
 * @param sc_pos        [km] Geocentric (x, y, z) position vector in km.
 * @param sc_vel        [km/s] Geocentric (x, y, z) velocity vector in km/s.
 * @param[out] obs_in_space  Pointer to the data structure to populate
 * @return          0 if successful, or -1 if the output argument is NULL.
 *
 * @sa make_observer_on_surface()
 * @sa make_observer_at_geocenter()
 * @sa place()
 */
int make_observer_in_space(const double *sc_pos, const double *sc_vel, observer *obs_in_space) {
  if(!obs_in_space) {
    errno = EINVAL;
    return -1;
  }

  memset(obs_in_space, 0, sizeof(*obs_in_space));
  obs_in_space->where = NOVAS_OBSERVER_IN_EARTH_ORBIT;
  make_in_space(sc_pos, sc_vel, &obs_in_space->near_earth);
  return 0;
}

/**
 * Populates an 'on_surface' data structure, for an observer on the surface of the Earth, with the given parameters.
 *
 * @param latitude      [deg] Geodetic (ITRS) latitude in degrees; north positive.
 * @param longitude     [deg] Geodetic (ITRS) longitude in degrees; east positive.
 * @param height        [m] Altitude over se level of the observer (meters).
 * @param temperature   [C] Temperature (degrees Celsius).
 * @param pressure      [mbar] Atmospheric pressure (millibars).
 * @param[out] obs_surface   Pointer to the data structure to populate.
 *
 * @return          0 if successful, or -1 if the output argument is NULL.
 *
 * @sa make_in_space()
 */
int make_on_surface(double latitude, double longitude, double height, double temperature, double pressure, on_surface *obs_surface) {
  if(!obs_surface) {
    errno = EINVAL;
    return -1;
  }

  obs_surface->latitude = latitude;
  obs_surface->longitude = longitude;
  obs_surface->height = height;
  obs_surface->temperature = temperature;
  obs_surface->pressure = pressure;

  return 0;
}

/**
 * Populates an 'in_space' data structure, for an observer situated on a near-Earth spacecraft, with the provided position and velocity components.
 * Both input vectors are assumed with respect to true equator and equinox of date.
 *
 * @param sc_pos    [km] Geocentric (x, y, z) position vector in km. NULL defaults to the origin
 * @param sc_vel    [km/s] Geocentric (x, y, z) velocity vector in km/s. NULL defaults to zero speed.
 * @param[out] obs_space    Pointer to data structure to populate.
 * @return          0 if successful, or -1 if the output argument is NULL.
 *
 * @sa make_on_surface()
 */
int make_in_space(const double *sc_pos, const double *sc_vel, in_space *obs_space) {
  if(!obs_space) {
    errno = EINVAL;
    return -1;
  }

  if(sc_pos) memcpy(obs_space->sc_pos, sc_pos, sizeof(obs_space->sc_pos));
  else memset(obs_space->sc_pos, 0, sizeof(obs_space->sc_pos));

  if(sc_vel) memcpy(obs_space->sc_vel, sc_vel, sizeof(obs_space->sc_vel));
  else memset(obs_space->sc_vel, 0, sizeof(obs_space->sc_vel));

  return 0;
}