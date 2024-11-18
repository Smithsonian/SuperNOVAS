/**
 * @file
 *
 * @date Created  on Aug 24, 2024
 * @author Attila Kovacs
 *
 *  SuperNOVAS only functions, which are not integral to the functionality of novas.c, and thus
 *  can live in a separate, more manageably sized, module.
 */

// We'll use gcc major version as a proxy for the glibc library to decide which feature macro to use.
// gcc 5.1 was released 2015-04-22...
#if __GNUC__ >= 5
#  define _DEFAULT_SOURCE        ///< strcasdecmp() feature macro starting glibc 2.20 (2014-09-08)
#else
#  define _BSD_SOURCE           ///< strcasecmp() feature macro for glibc <= 2.19
#endif

#include <math.h>
#include <errno.h>
#include <string.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"
/// \endcond

/**
 * Returns the difference between Terrestrial Time (TT) and Universal Coordinated Time (UTC)
 *
 * @param leap_seconds  [s] The current leap seconds (see IERS Bulletins)
 * @return              [s] The TT - UTC time difference
 *
 * @sa get_ut1_to_tt()
 * @sa julian_date()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
double get_utc_to_tt(int leap_seconds) {
  return leap_seconds + NOVAS_TAI_TO_TT;
}

/**
 * Returns the TT - UT1 time difference given the leap seconds and the actual UT1 - UTC time
 * difference as measured and published by IERS.
 *
 * NOTES:
 * <ol>
 * <li>The current UT1 - UTC time difference, and polar offsets, historical data and near-term
 * projections are published in the
 <a href="https://www.iers.org/IERS/EN/Publications/Bulletins/bulletins.html>IERS Bulletins</a>
 * </li>
 * </ol>
 *
 * @param leap_seconds  [s] Leap seconds at the time of observations
 * @param dut1          [s] UT1 - UTC time difference [-0.5:0.5]
 * @return              [s] The TT - UT1 time difference that is suitable for used with all
 *                      calls in this library that require a <code>ut1_to_tt</code> argument.
 *
 * @sa get_utc_to_tt()
 * @sa place()
 * @sa cel_pole()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
double get_ut1_to_tt(int leap_seconds, double dut1) {
  return get_utc_to_tt(leap_seconds) + dut1;
}

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
int place_icrs(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos) {
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
int place_gcrs(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos) {
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
int place_cirs(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos) {
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
int place_tod(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos) {
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
int place_mod(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos) {
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
int place_j2000(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos) {
  prop_error("place_j2000", place(jd_tt, source, NULL, 0.0, NOVAS_J2000, accuracy, pos), 0);
  return 0;
}

/**
 * Convert ecliptic longitude and latitude to right ascension and declination.  To convert
 * GCRS ecliptic coordinates (mean ecliptic and equinox of J2000.0), set 'coord_sys' to
 * NOVAS_GCRS_EQUATOR(2); in this case the value of 'jd_tt' can be set to anything, since
 * J2000.0 is assumed. Otherwise, all input coordinates are dynamical at'jd_tt'.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date. (Unused if 'coord_sys'
 *                    is NOVAS_GCRS_EQUATOR[2])
 * @param coord_sys   The astrometric reference system of the coordinates. If 'coord_sys' is
 *                    NOVAS_GCRS_EQUATOR(2), the input GCRS coordinates are converted to
 *                    J2000 ecliptic coordinates.
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param elon        [deg] Ecliptic longitude in degrees, referred to specified ecliptic and
 *                    equinox of date.
 * @param elat        [deg] Ecliptic latitude in degrees, referred to specified ecliptic and
 *                    equinox of date.
 * @param[out] ra     [h] Right ascension in hours, referred to specified equator and equinox
 *                    of date.
 * @param[out] dec    [deg] Declination in degrees, referred to specified equator and equinox
 *                    of date.

 * @return            0 if successful, or else 1 if the value of 'coord_sys' is invalid.
 *
 * @sa ecl2equ_vec()
 * @sa equ2ecl()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int ecl2equ(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double elon, double elat, double *ra,
        double *dec) {
  static const char *fn = "ecl2equ";
  double coslat, pos[3], xyproj;

  if(!ra || !dec)
    return novas_error(-1, EINVAL, fn, "NULL output pointer: ra=%p, dec=%p", ra, dec);

  // Form position vector in equatorial system from input coordinates.
  elon *= DEGREE;
  elat *= DEGREE;
  coslat = cos(elat);

  pos[0] = coslat * cos(elon);
  pos[1] = coslat * sin(elon);
  pos[2] = sin(elat);

  // Convert the vector from equatorial to ecliptic system.
  prop_error(fn, ecl2equ_vec(jd_tt, coord_sys, accuracy, pos, pos), 0);

  // Decompose ecliptic vector into ecliptic longitude and latitude.
  xyproj = sqrt(pos[0] * pos[0] + pos[1] * pos[1]);
  *ra = xyproj ? atan2(pos[1], pos[0]) / HOURANGLE : 0.0;
  if(*ra < 0.0)
    *ra += DAY_HOURS;

  *dec = atan2(pos[2], xyproj) / DEGREE;

  return 0;
}

/**
 * Converts galactic longitude and latitude to ICRS right ascension and declination.
 *
 * REFERENCES:
 * <ol>
 * <li>Hipparcos and Tycho Catalogues, Vol. 1, Section 1.5.3.</li>
 * </ol>
 *
 * @param glon        [deg] Galactic longitude in degrees.
 * @param glat        [deg] Galactic latitude in degrees.
 * @param[out] ra     [h] ICRS right ascension in hours.
 * @param[out] dec    [deg] ICRS declination in degrees.

 *
 * @return            0 if successful, or -1 if either of the output pointer arguments
 *                    are NULL.
 *
 * @sa equ2gal()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int gal2equ(double glon, double glat, double *ra, double *dec) {
  double pos1[3], pos2[3], xyproj, coslat;

  // Rotation matrix A_g from Hipparcos documentation eq. 1.5.11.
  // AK: Transposed compared to NOVAS C 3.1 for dot product handling.
  static const double ag[3][3] = { //
          { -0.0548755604, +0.4941094279, -0.8676661490 }, //
          { -0.8734370902, -0.4448296300, -0.1980763734 }, //
          { -0.4838350155, +0.7469822445, +0.4559837762 } };

  if(!ra || !dec)
    return novas_error(-1, EINVAL, "gal2equ", "NULL output pointer: ra=%p, dec=%p", ra, dec);

  // Form position vector in equatorial system from input coordinates
  glon *= DEGREE;
  glat *= DEGREE;
  coslat = cos(glat);

  pos1[0] = coslat * cos(glon);
  pos1[1] = coslat * sin(glon);
  pos1[2] = sin(glat);

  // Rotate position vector to galactic system, using Hipparcos documentation eq. 1.5.13.
  pos2[0] = novas_vdot(ag[0], pos1);
  pos2[1] = novas_vdot(ag[1], pos1);
  pos2[2] = novas_vdot(ag[2], pos1);

  // Decompose galactic vector into longitude and latitude.
  xyproj = sqrt(pos2[0] * pos2[0] + pos2[1] * pos2[1]);
  *ra = xyproj ? atan2(pos2[1], pos2[0]) / HOURANGLE : 0.0;
  if(*ra < 0.0)
    *ra += DAY_HOURS;

  *dec = atan2(pos2[2], xyproj) / DEGREE;

  return 0;
}

/**
 * Change J2000 coordinates to GCRS coordinates. Same as frame_tie() called with J2000_TO_ICRS
 *
 * @param in        J2000 input 3-vector
 * @param[out] out  GCRS output 3-vector
 * @return          0 if successful, or else an error from frame_tie()
 *
 * @sa j2000_to_tod()
 * @sa gcrs_to_j2000()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int j2000_to_gcrs(const double *in, double *out) {
  prop_error("j2000_to_gcrs", frame_tie(in, J2000_TO_ICRS, out), 0);
  return 0;
}

/**
 * Transforms a rectangular equatorial (x, y, z) vector from the Celestial Intermediate
 * Reference System (CIRS) at the given epoch to the True of Date (TOD) reference
 * system.
 *
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian date that defines
 *                  the output epoch. Typically it does not require much precision, and
 *                  Julian dates in other time measures will be unlikely to affect the
 *                  result
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in        CIRS Input (x, y, z) position or velocity vector
 * @param[out] out  Output position or velocity 3-vector in the True of Date (TOD) frame.
 *                  It can be the same vector as the input.
 * @return          0 if successful, or -1 if either of the vector arguments is NULL
 *                  or the accuracy is invalid, or 10 + the error from cio_location(), or
 *                  else 20 + the error from cio_basis().
 *
 * @sa tod_to_cirs()
 * @sa cirs_to_app_ra()
 * @sa cirs_to_gcrs()
 * @sa cirs_to_itrs()
 *
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int cirs_to_tod(double jd_tt, enum novas_accuracy accuracy, const double *in, double *out) {
  static const char *fn = "cirs_to_tod";

  double ra_cio;  // [h] R.A. of the CIO (from the true equinox) we'll calculate

  // Obtain the R.A. [h] of the CIO at the given date
  prop_error(fn, cio_ra(jd_tt, accuracy, &ra_cio), 0);
  prop_error(fn, spin(-15.0 * ra_cio, in, out), 0);

  return 0;
}

/**
 * Transforms a rectangular equatorial (x, y, z) vector from the True of Date (TOD) reference
 * system to the Celestial Intermediate Reference System (CIRS) at the given epoch to the .
 *
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian date that defines
 *                  the output epoch. Typically it does not require much precision, and
 *                  Julian dates in other time measures will be unlikely to affect the
 *                  result
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in        CIRS Input (x, y, z) position or velocity vector
 * @param[out] out  Output position or velocity 3-vector in the True of Date (TOD) frame.
 *                  It can be the same vector as the input.
 * @return          0 if successful, or -1 if either of the vector arguments is NULL
 *                  or the accuracy is invalid, or 10 + the error from cio_location(), or
 *                  else 20 + the error from cio_basis().
 *
 * @sa cirs_to_tod()
 * @sa app_to_cirs_ra()
 * @sa tod_to_gcrs()
 * @sa tod_to_j2000()
 * @sa tod_to_itrs()
 *
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int tod_to_cirs(double jd_tt, enum novas_accuracy accuracy, const double *in, double *out) {
  static const char *fn = "tod_to_cirs";

  double ra_cio;  // [h] R.A. of the CIO (from the true equinox) we'll calculate

  // Obtain the R.A. [h] of the CIO at the given date
  prop_error(fn, cio_ra(jd_tt, accuracy, &ra_cio), 0);
  prop_error(fn, spin(15.0 * ra_cio, in, out), 0);

  return 0;
}


/**
 * Converts a position vector in the Earth-fixed ITRS frame to astrometric (unrefracted) azimuth
 * and zenith angles at the specified observer location.
 *
 * @param location   Observer location on Earth
 * @param itrs       3-vector position in Earth-fixed ITRS frame
 * @param[out] az    [deg] astrometric azimuth angle at observer location [0:360]. It may be
 *                   NULL if not required.
 * @param[out] za    [deg] astrometric zenith angle at observer location [0:180]. It may be NULL
 *                   if not required.
 * @return           0 if successful, or else -1 if the location or the input vector is NULL.
 *
 * @sa hor_to_itrs()
 * @sa cirs_to_itrs()
 * @sa tod_to_itrs()
 * @sa refract_astro()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int itrs_to_hor(const on_surface *location, const double *itrs, double *az, double *za) {
  double uze[3], une[3], uwe[3];
  double lat, lon, coslat, sinlat, coslon, sinlon;
  double pn, pw, pz, proj;

  // Default output values in case of error return.
  if(az)
    *az = NAN;
  if(za)
    *za = NAN;

  if(!location || !itrs)
    return novas_error(-1, EINVAL, "itrs_to_hor", "NULL input location=%p or ITRS pos=%p", location, itrs);

  lat = location->latitude * DEGREE;
  lon = location->longitude * DEGREE;

  coslat = cos(lat);
  sinlat = sin(lat);
  coslon = cos(lon);
  sinlon = sin(lon);

  // Define vector toward local north in Earth-fixed system (x axis).
  une[0] = -sinlat * coslon;
  une[1] = -sinlat * sinlon;
  une[2] = coslat;

  // Define vector toward local west in Earth-fixed system (y axis).
  uwe[0] = sinlon;
  uwe[1] = -coslon;
  uwe[2] = 0.0;

  // Define vector toward local zenith in Earth-fixed system (z axis).
  uze[0] = coslat * coslon;
  uze[1] = coslat * sinlon;
  uze[2] = sinlat;

  // Obtain vectors in celestial system.

  // Compute coordinates of object w.r.t orthonormal basis.

  // Compute components of 'p' - projections of 'p' onto rotated
  // Earth-fixed basis vectors.
  pn = novas_vdot(itrs, une);
  pw = novas_vdot(itrs, uwe);
  pz = novas_vdot(itrs, uze);

  // Compute azimuth and zenith distance.
  proj = sqrt(pn * pn + pw * pw);

  if(az) {
    *az = proj > 0.0 ? -atan2(pw, pn) / DEGREE : 0.0;
    if(*az < 0.0)
      *az += DEG360;
  }

  if(za)
    *za = atan2(proj, pz) / DEGREE;

  return 0;
}

/**
 * Converts astrometric (unrefracted) azimuth and zenith angles at the specified observer location to a
 * unit position vector in the Earth-fixed ITRS frame.
 *
 * @param location   Observer location on Earth
 * @param az         [deg] astrometric azimuth angle at observer location [0:360]. It may be
 *                   NULL if not required.
 * @param za         [deg] astrometric zenith angle at observer location [0:180]. It may be NULL
 *                   if not required.
 * @param[out] itrs  Unit 3-vector direction in Earth-fixed ITRS frame
 * @return           0 if successful, or else -1 if the location or the input vector is NULL.
 *
 * @sa itrs_to_hor()
 * @sa itrs_to_cirs()
 * @sa itrs_to_tod()
 * @sa refract()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int hor_to_itrs(const on_surface *location, double az, double za, double *itrs) {
  double in[3], uze[3], une[3], uwe[3];
  double sinza;
  double lat, lon, coslat, sinlat, coslon, sinlon;

  if(!location || !itrs)
    return novas_error(-1, EINVAL, "hor_to_itrs", "NULL input location=%p or output itrs=%p position", location, itrs);

  az *= -DEGREE;
  za *= DEGREE;

  sinza = sin(za);
  in[0] = sinza * cos(az);
  in[1] = sinza * sin(az);
  in[2] = cos(za);

  lat = location->latitude * DEGREE;
  lon = location->longitude * DEGREE;

  coslat = cos(lat);
  sinlat = sin(lat);
  coslon = cos(lon);
  sinlon = sin(lon);

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

  // Calculate ITRS positions from NWZ components at ITRS origin
  itrs[0] = une[0] * in[0] + uwe[0] * in[1] + uze[0] * in[2];
  itrs[1] = une[1] * in[0] + uwe[1] * in[1] + uze[1] * in[2];
  itrs[2] = une[2] * in[0] + uwe[2] * in[1] + uze[2] * in[2];

  return 0;
}

/**
 * Converts a CIRS right ascension coordinate (measured from the CIO) to an apparent R.A.
 * measured from the true equinox of date.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param ra          [h] The CIRS right ascension coordinate, measured from the CIO.
 * @return            [h] the apparent R.A. coordinate measured from the true equinox of date [0:24],
 *                    or NAN if the accuracy is invalid, or if there wan an error from cio_ra().
 *
 * @sa app_to_cirs_ra()
 * @sa cirs_to_tod()
 *
 * @since 1.0.1
 * @author Attila Kovacs
 */
double cirs_to_app_ra(double jd_tt, enum novas_accuracy accuracy, double ra) {
  double ra_cio;  // [h] R.A. of the CIO (from the true equinox) we'll calculate

  // Obtain the R.A. [h] of the CIO at the given date
  int stat = cio_ra(jd_tt, accuracy, &ra_cio);
  if(stat)
    return novas_trace_nan("cirs_to_app_ra");

  // Convert CIRS R.A. to true apparent R.A., keeping the result in the [0:24] h range
  ra = remainder(ra + ra_cio, 24.0);
  if(ra < 0.0)
    ra += 24.0;

  return ra;
}

/**
 * Converts an apparent right ascension coordinate (measured from the true equinox of date) to a
 * CIRS R.A., measured from the CIO.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param ra          [h] the apparent R.A. coordinate measured from the true equinox of date.
 * @return            [h] The CIRS right ascension coordinate, measured from the CIO [0:24],
 *                    or NAN if the accuracy is invalid, or if there wan an error from cio_ra().
 *
 * @sa cirs_to_app_ra()
 * @sa tod_to_cirs()
 *
 * @since 1.0.1
 * @author Attila Kovacs
 */
double app_to_cirs_ra(double jd_tt, enum novas_accuracy accuracy, double ra) {
  double ra_cio;  // [h] R.A. of the CIO (from the true equinox) we'll calculate

  // Obtain the R.A. [h] of the CIO at the given date
  int stat = cio_ra(jd_tt, accuracy, &ra_cio);
  if(stat)
    return novas_trace_nan("app_to_cirs_ra");

  // Convert CIRS R.A. to true apparent R.A., keeping the result in the [0:24] h range
  ra = remainder(ra - ra_cio, 24.0);
  if(ra < 0.0)
    ra += 24.0;

  return ra;
}

/**
 * Rotates a position vector from the Earth-fixed ITRS frame to the dynamical CIRS frame of
 * date (IAU 2000 standard method).
 *
 * If both 'xp' and 'yp' are set to 0 no polar motion is included in the transformation.
 *
 * If extreme (sub-microarcsecond) accuracy is not required, you can use UT1-based Julian date
 * instead of the TT-based Julian date and set the 'ut1_to_tt' argument to 0.0. and you can
 * use UTC-based Julian date the same way.for arcsec-level precision also.
 *
 * REFERENCES:
 *  <ol>
 *   <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *   <li>Kaplan, G. H. (2003), 'Another Look at Non-Rotating Origins', Proceedings of IAU
 *   XXV Joint Discussion 16.</li>
 *  </ol>
 *
 * @param jd_tt_high    [day] High-order part of Terrestrial Time (TT) based Julian date.
 * @param jd_tt_low     [day] Low-order part of Terrestrial Time (TT) based Julian date.
 * @param ut1_to_tt     [s] TT - UT1 Time difference in seconds
 * @param accuracy      NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param xp            [arcsec] Conventionally-defined X coordinate of celestial intermediate
 *                      pole with respect to ITRS pole, in arcseconds.
 * @param yp            [arcsec] Conventionally-defined Y coordinate of celestial intermediate
 *                      pole with respect to ITRS pole, in arcseconds.
 * @param in            Position vector, geocentric equatorial rectangular coordinates,
 *                      referred to ITRS axes (terrestrial system)
 * @param[out] out      Position vector, geocentric equatorial rectangular coordinates,
 *                      referred to CIRS axes (celestial system).
 * @return              0 if successful, -1 if either of the vector arguments is NULL, 1 if
 *                      'accuracy' is invalid, or else 10 + the error from cio_location(), or
 *                      20 + error from cio_basis().
 *
 * @sa itrs_to_tod()
 * @sa cirs_to_itrs()
 * @sa cirs_to_gcrs()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int itrs_to_cirs(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp,
        const double *in, double *out) {
  prop_error("itrs_to_cirs",
          ter2cel(jd_tt_high, jd_tt_low - ut1_to_tt / DAY, ut1_to_tt, EROT_ERA, accuracy, NOVAS_DYNAMICAL_CLASS, xp, yp, in, out), 0);
  return 0;
}

/**
 * Rotates a position vector from the Earth-fixed ITRS frame to the dynamical True of Date
 * (TOD) frame of date (pre IAU 2000 method).
 *
 * If both 'xp' and 'yp' are set to 0 no polar motion is included in the transformation.
 *
 * If extreme (sub-microarcsecond) accuracy is not required, you can use UT1-based Julian date
 * instead of the TT-based Julian date and set the 'ut1_to_tt' argument to 0.0. and you can
 * use UTC-based Julian date the same way.for arcsec-level precision also.
 *
 * REFERENCES:
 *  <ol>
 *   <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *   <li>Kaplan, G. H. (2003), 'Another Look at Non-Rotating Origins', Proceedings of IAU
 *   XXV Joint Discussion 16.</li>
 *  </ol>
 *
 * @param jd_tt_high    [day] High-order part of Terrestrial Time (TT) based Julian date.
 * @param jd_tt_low     [day] Low-order part of Terrestrial Time (TT) based Julian date.
 * @param ut1_to_tt     [s] TT - UT1 Time difference in seconds
 * @param accuracy      NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param xp            [arcsec] Conventionally-defined X coordinate of celestial intermediate
 *                      pole with respect to ITRS pole, in arcseconds.
 * @param yp            [arcsec] Conventionally-defined Y coordinate of celestial intermediate
 *                      pole with respect to ITRS pole, in arcseconds.
 * @param in            Position vector, geocentric equatorial rectangular coordinates,
 *                      referred to ITRS axes (terrestrial system)
 * @param[out] out      Position vector, geocentric equatorial rectangular coordinates,
 *                      referred to True of Date (TOD) axes (celestial system)
 * @return              0 if successful, -1 if either of the vector arguments is NULL, 1 if
 *                      'accuracy' is invalid, or else 10 + the error from cio_location(), or
 *                      20 + error from cio_basis().
 *
 * @sa itrs_to_cirs()
 * @sa tod_to_itrs()
 * @sa tod_to_j2000()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int itrs_to_tod(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const double *in,
        double *out) {
  prop_error("itrs_to_tod",
          ter2cel(jd_tt_high, jd_tt_low - ut1_to_tt / DAY, ut1_to_tt, EROT_GST, accuracy, NOVAS_DYNAMICAL_CLASS, xp, yp, in, out), 0);
  return 0;
}

/**
 * Rotates a position vector from the dynamical CIRS frame of date to the Earth-fixed ITRS frame
 * (IAU 2000 standard method).
 *
 * If both 'xp' and 'yp' are set to 0 no polar motion is included in the transformation.
 *
 * If extreme (sub-microarcsecond) accuracy is not required, you can use UT1-based Julian date
 * instead of the TT-based Julian date and set the 'ut1_to_tt' argument to 0.0. and you can
 * use UTC-based Julian date the same way.for arcsec-level precision also.
 *
 *
 * REFERENCES:
 *  <ol>
 *   <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *   <li>Kaplan, G. H. (2003), 'Another Look at Non-Rotating Origins', Proceedings of IAU XXV
 *   Joint Discussion 16.</li>
 *  </ol>
 *
 * @param jd_tt_high    [day] High-order part of Terrestrial Time (TT) based Julian date.
 * @param jd_tt_low     [day] Low-order part of Terrestrial Time (TT) based Julian date.
 * @param ut1_to_tt     [s] TT - UT1 Time difference in seconds
 * @param accuracy      NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param xp            [arcsec] Conventionally-defined X coordinate of celestial intermediate
 *                      pole with respect to ITRS pole, in arcseconds.
 * @param yp            [arcsec] Conventionally-defined Y coordinate of celestial intermediate
 *                      pole with respect to ITRS pole, in arcseconds.
 * @param in            Position vector, geocentric equatorial rectangular coordinates,
 *                      referred to CIRS axes (celestial system).
 * @param[out] out      Position vector, geocentric equatorial rectangular coordinates,
 *                      referred to ITRS axes (terrestrial system).
 * @return              0 if successful, -1 if either of the vector arguments is NULL, 1 if
 *                      'accuracy' is invalid, 2 if 'method' is invalid 10--20, 3 if the method
 *                      and option are mutually incompatible, or else 10 + the error from
 *                      cio_location(), or 20 + error from cio_basis().
 *
 * @sa tod_to_itrs()
 * @sa itrs_to_cirs()
 * @sa gcrs_to_cirs()
 * @sa cirs_to_gcrs()
 * @sa cirs_to_tod()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int cirs_to_itrs(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp,
        const double *in, double *out) {
  prop_error("cirs_to_itrs",
          cel2ter(jd_tt_high, jd_tt_low - ut1_to_tt / DAY, ut1_to_tt, EROT_ERA, accuracy, NOVAS_DYNAMICAL_CLASS, xp, yp, in, out), 0);
  return 0;
}

/**
 * Rotates a position vector from the dynamical True of Date (TOD) frame of date the Earth-fixed
 * ITRS frame (pre IAU 2000 method).
 *
 * If both 'xp' and 'yp' are set to 0 no polar motion is included in the transformation.
 *
 * If extreme (sub-microarcsecond) accuracy is not required, you can use UT1-based Julian date
 * instead of the TT-based Julian date and set the 'ut1_to_tt' argument to 0.0. and you can
 * use UTC-based Julian date the same way.for arcsec-level precision also.
 *
 * REFERENCES:
 *  <ol>
 *   <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *   <li>Kaplan, G. H. (2003), 'Another Look at Non-Rotating Origins', Proceedings of IAU XXV
 *   Joint Discussion 16.</li>
 *  </ol>
 *
 * @param jd_tt_high    [day] High-order part of Terrestrial Time (TT) based Julian date.
 * @param jd_tt_low     [day] Low-order part of Terrestrial Time (TT) based Julian date.
 * @param ut1_to_tt     [s] TT - UT1 Time difference in seconds.
 * @param accuracy      NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param xp            [arcsec] Conventionally-defined X coordinate of celestial intermediate
 *                      pole with respect to ITRS pole, in arcseconds.
 * @param yp            [arcsec] Conventionally-defined Y coordinate of celestial intermediate
 *                      pole with respect to ITRS pole, in arcseconds.
 * @param in            Position vector, geocentric equatorial rectangular coordinates,
 *                      referred to True of Date (TOD) axes (celestial system).
 * @param[out] out      Position vector, geocentric equatorial rectangular coordinates,
 *                      referred to ITRS axes (terrestrial system).
 * @return              0 if successful, -1 if either of the vector arguments is NULL, 1 if
 *                      'accuracy' is invalid, 2 if 'method' is invalid 10--20, 3 if the method
 *                      and option are mutually incompatible, or else 10 + the error from
 *                      cio_location(), or 20 + error from cio_basis().
 *
 * @sa cirs_to_itrs()
 * @sa itrs_to_tod()
 * @sa j2000_to_tod()
 * @sa tod_to_gcrs()
 * @sa tod_to_j2000()
 * @sa tod_to_cirs()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int tod_to_itrs(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const double *in,
        double *out) {
  prop_error("tod_to_itrs",
          cel2ter(jd_tt_high, jd_tt_low - ut1_to_tt / DAY, ut1_to_tt, EROT_GST, accuracy, NOVAS_DYNAMICAL_CLASS, xp, yp, in, out), 0);
  return 0;
}

/**
 * Computes the gravitationally undeflected position of an observed source position due to the
 * specified Solar-system bodies.
 *
 * REFERENCES:
 * <ol>
 * <li>Klioner, S. (2003), Astronomical Journal 125, 1580-1597, Section 6.</li>
 * </ol>
 *
 * @param pos_app     [AU] Apparent position 3-vector of observed object, with respect to origin at
 *                    observer (or the geocenter), referred to ICRS axes, components
 *                    in AU.
 * @param pos_obs     [AU] Position 3-vector of observer (or the geocenter), with respect to
 *                    origin at solar system barycenter, referred to ICRS axes,
 *                    components in AU.
 * @param planets     Apparent planet data containing positions and velocities for the major
 *                    gravitating bodies in the solar-system.
 * @param[out] out    [AU] Nominal position vector of observed object, with respect to origin at
 *                    observer (or the geocenter), referred to ICRS axes, without gravitational
 *                    deflection, components in AU. It can be the same vector as the input, but not
 *                    the same as pos_obs.
 * @return            0 if successful, -1 if any of the pointer arguments is NULL.
 *
 * @sa obs_planets()
 * @sa grav_planets()
 * @sa novas_app_to_geom()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int grav_undo_planets(const double *pos_app, const double *pos_obs, const novas_planet_bundle *planets, double *out) {
  static const char *fn = "grav_undo_planets";

  const double tol = 1e-13;
  double pos_def[3] = {0}, pos0[3] = {0};
  double l;
  int i;

  if(!pos_app || !pos_obs)
    return novas_error(-1, EINVAL, fn, "NULL input 3-vector: pos_app=%p, pos_obs=%p", pos_app, pos_obs);

  if(!planets)
    return novas_error(-1, EINVAL, fn, "NULL input planet data");

  if(!out)
    return novas_error(-1, EINVAL, fn, "NULL output 3-vector: out=%p", out);

  l = novas_vlen(pos_app);
  if(l == 0.0) {
    if(out != pos_app)
      memcpy(out, pos_app, XYZ_VECTOR_SIZE);
    return 0;        // Source is same as observer. No deflection.
  }

  memcpy(pos0, pos_app, sizeof(pos0));

  for(i = 0; i < novas_inv_max_iter; i++) {
    int j;

    prop_error(fn, grav_planets(pos0, pos_obs, planets, pos_def), 0);

    if(novas_vdist(pos_def, pos_app) / l < tol) {
      memcpy(out, pos0, sizeof(pos0));
      return 0;
    }

    for(j = 3; --j >= 0;)
      pos0[j] -= pos_def[j] - pos_app[j];
  }

  return novas_error(-1, ECANCELED, fn, "failed to converge");
}

/**
 * Computes the gravitationally undeflected position of an observed source position due to the
 * major gravitating bodies in the solar system.  This function valid for an observed body within
 * the solar system as well as for a star.
 *
 * If 'accuracy' is set to zero (full accuracy), three bodies (Sun, Jupiter, and Saturn) are
 * used in the calculation.  If the reduced-accuracy option is set, only the Sun is used in
 * the calculation.  In both cases, if the observer is not at the geocenter, the deflection
 * due to the Earth is included.
 *
 * The number of bodies used at full and reduced accuracy can be set by making a change to
 * the code in this function as indicated in the comments.
 *
 * REFERENCES:
 * <ol>
 * <li>Klioner, S. (2003), Astronomical Journal 125, 1580-1597, Section 6.</li>
 * </ol>
 *
 * @param jd_tdb      [day] Barycentric Dynamical Time (TDB) based Julian date
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param pos_app     [AU] Apparent position 3-vector of observed object, with respect to origin at
 *                    observer (or the geocenter), referred to ICRS axes, components
 *                    in AU.
 * @param pos_obs     [AU] Position 3-vector of observer (or the geocenter), with respect to
 *                    origin at solar system barycenter, referred to ICRS axes,
 *                    components in AU.
 * @param[out] out    [AU] Nominal position vector of observed object, with respect to origin at
 *                    observer (or the geocenter), referred to ICRS axes, without gravitational
 *                    deflection, components in AU. It can be the same vector as the input, but not
 *                    the same as pos_obs.
 * @return            0 if successful, -1 if any of the pointer arguments is NULL (errno = EINVAL)
 *                    or if the result did not converge (errno = ECANCELED), or else an error from
 *                    obs_planets().
 *
 * @sa grav_def()
 * @sa novas_app_to_geom()
 * @sa set_planet_provider()
 * @sa set_planet_provider_hp()
 * @sa grav_bodies_full_accuracy
 * @sa grav_bodies_reduced_accuracy
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int grav_undef(double jd_tdb, enum novas_accuracy accuracy, const double *pos_app, const double *pos_obs, double *out) {
  static const char *fn = "grav_undef";

  novas_planet_bundle planets = {0};
  int pl_mask = (accuracy == NOVAS_FULL_ACCURACY) ? grav_bodies_full_accuracy : grav_bodies_reduced_accuracy;

  if(!pos_app || !out)
    return novas_error(-1, EINVAL, fn, "NULL source position 3-vector: pos_app=%p, out=%p", pos_app, out);

  prop_error(fn, obs_planets(jd_tdb, accuracy, pos_obs, pl_mask, &planets), 0);
  prop_error(fn, grav_undo_planets(pos_app, pos_obs, &planets, out), 0);
  return 0;
}

/**
 * Populates and object data structure with the data for a catalog source.
 *
 * @param star          Pointer to structure to populate with the catalog data for a celestial
 *                      object located outside the solar system.
 * @param[out] source   Pointer to the celestial object data structure to be populated.
 * @return              0 if successful, or -1 if 'cel_obj' is NULL or when type is
 *                      NOVAS_CATALOG_OBJECT and 'star' is NULL, or else 1 if 'type' is
 *                      invalid, 2 if 'number' is out of legal range or 5 if 'name' is too long.
 *
 * @sa make_cat_entry()
 * @sa make_planet()
 * @sa make_ephem_object()
 * @sa novas_geom_posvel()
 * @sa place()
 *
 * @since 1.1
 * @author Attila Kovacs
 *
 */
int make_cat_object(const cat_entry *star, object *source) {
  if(!star || !source)
    return novas_error(-1, EINVAL, "make_cat_object", "NULL parameter: star=%p, source=%p", star, source);
  make_object(NOVAS_CATALOG_OBJECT, star->starnumber, star->starname, star, source);
  return 0;
}

/**
 * Sets a celestial object to be a Solar-system ephemeris body. Typically this would be used to define
 * minor planets, asteroids, comets and planetary satellites.
 *
 * @param name          Name of object. By default converted to upper-case, unless novas_case_sensitive()
 *                      was called with a non-zero argument. Max. SIZE_OF_OBJ_NAME long, including
 *                      termination. If the ephemeris provider uses names, then the name should match
 *                      those of the ephemeris provider -- otherwise it is not important.
 * @param num           Solar-system body ID number (e.g. NAIF). The number should match the needs of the
 *                      ephemeris provider used with NOVAS. (If the ephemeris provider is by name and not
 *                      ID number, then the number here is not important).
 * @param[out] body     Pointer to structure to populate.
 * @return              0 if successful, or else -1 if the 'body' pointer is NULL or the name
 *                      is too long.
 *
 *
 * @sa set_ephem_provider()
 * @sa make_planet()
 * @sa make_cat_entry()
 * @sa novas_geom_posvel()
 * @sa place()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int make_ephem_object(const char *name, long num, object *body) {
  prop_error("make_ephem_object", (make_object(NOVAS_EPHEM_OBJECT, num, name, NULL, body) ? -1 : 0), 0);
  return 0;
}

/**
 * Sets a celestial object to be a Solar-system orbital body. Typically this would be used to define
 * minor planets, asteroids, comets, or even planetary satellites.
 *
 * @param name          Name of object. It may be NULL if not relevant.
 * @param num           Solar-system body ID number (e.g. NAIF). It is not required and can be set e.g. to
 *                      -1 if not relevant to the caller.
 * @param orbit         The orbital parameters to adopt. The data will be copied, not referenced.
 * @param[out] body     Pointer to structure to populate.
 * @return              0 if successful, or else -1 if the 'orbit' or 'body' pointer is NULL or the name
 *                      is too long.
 *
 *
 * @sa novas_orbit_posvel()
 * @sa make_planet()
 * @sa make_ephem_object()
 * @sa novas_geom_posvel()
 * @sa place()
 *
 * @since 1.2
 * @author Attila Kovacs
 */
int make_orbital_object(const char *name, long num, const novas_orbital *orbit, object *body) {
  static const char *fn = "make_orbital_object";
  if(!orbit)
    return novas_error(-1, EINVAL, fn, "Input orbital elements is NULL");
  prop_error(fn, (make_object(NOVAS_ORBITAL_OBJECT, num, name, NULL, body) ? -1 : 0), 0);
  body->orbit = *orbit;
  return 0;
}

/**
 * Populates a celestial object data structure with the parameters for a redhifted catalog
 * source, such as a distant quasar or galaxy. It is similar to `make_cat_object()` except
 * that it takes a Doppler-shift (z) instead of radial velocity and it assumes no parallax
 * and no proper motion (appropriately for a distant redshifted source). The catalog name
 * is set to `EXT` to indicate an extragalactic source, and the catalog number defaults to 0.
 * The user may change these default field values as appropriate afterwards, if necessary.
 *
 * @param name        Object name (less than SIZE_OF_OBJ_NAME in length). It may be NULL.
 * @param ra          [h] Right ascension of the object (hours).
 * @param dec         [deg] Declination of the object (degrees).
 * @param z           Redhift value (&lambda;<sub>obs</sub> / &lambda;<sub>rest</sub> - 1 =
 *                    f<sub>rest</sub> / f<sub>obs</sub> - 1).
 * @param[out] source Pointer to structure to populate.
 * @return            0 if successful, or 5 if 'name' is too long, else -1 if the 'source'
 *                    pointer is NULL.
 *
 * @sa make_cat_object()
 * @sa novas_v2z()
 *
 * @since 1.2
 * @author Attila Kovacs
 */
int make_redshifted_object(const char *name, double ra, double dec, double z, object *source) {
  static const char *fn = "make_redshifted_source";

  cat_entry c;
  double v = novas_z2v(z);

  if(isnan(v))
    return novas_error(-1, EINVAL, fn, "invalid redshift value: %f\n", z);

  prop_error(fn, make_cat_entry(name, "EXT", 0, ra, dec, 0.0, 0.0, 0.0, v, &c), 0);
  return make_cat_object(&c, source);
}

/**
 * Populates an 'observer' data structure for an observer moving relative to the surface of Earth,
 * such as an airborne observer. Airborne observers have an earth fixed momentary location,
 * defined by longitude, latitude, and altitude, the same was as for a stationary observer on
 * Earth, but are moving relative to the surface, such as in an aircraft or balloon observatory.
 *
 * @param location    Current longitude, latitude and altitude, and local weather (temperature and pressure)
 * @param vel         [km/s] Surface velocity.
 * @param[out] obs    Pointer to data structure to populate.
 * @return            0 if successful, or -1 if the output argument is NULL.
 *
 * @sa make_observer_at geocenter()
 * @sa make_observer_in_space()
 * @sa make_observer_on_surface()
 * @sa make_solar_system_observer()
 * @sa novas_calc_geometric_position()
 * @sa place()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int make_airborne_observer(const on_surface *location, const double *vel, observer *obs) {
  in_space motion = IN_SPACE_INIT;

  if(!vel)
    return novas_error(-1, EINVAL, "make_airborne_observer", "NULL velocity");

  memcpy(motion.sc_vel, vel, sizeof(motion.sc_vel));

  prop_error("make_airborne_observer", make_observer(NOVAS_AIRBORNE_OBSERVER, location, &motion, obs), 0);
  return 0;
}

/**
 * Populates an 'observer' data structure, for an observer situated on a near-Earth spacecraft,
 * with the specified geocentric position and velocity vectors. Solar-system observers are similar
 * to observers in Earth-orbit but their momentary position and velocity is defined relative to
 * the Solar System Barycenter, instead of the geocenter.
 *
 * @param sc_pos        [AU] Solar-system barycentric (x, y, z) position vector in ICRS.
 * @param sc_vel        [AU/day] Solar-system barycentric (x, y, z) velocity vector in ICRS.
 * @param[out] obs      Pointer to the data structure to populate
 * @return          0 if successful, or -1 if the output argument is NULL.
 *
 * @sa make_observer_in_space()
 * @sa make_observer_on_surface()
 * @sa make_observer_at_geocenter()
 * @sa make_airborne_observer()
 * @sa novas_calc_geometric_position()
 * @sa place()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int make_solar_system_observer(const double *sc_pos, const double *sc_vel, observer *obs) {
  static const char *fn = "make_observer_in_space";
  in_space loc;
  prop_error(fn, make_in_space(sc_pos, sc_vel, &loc), 0);
  prop_error(fn, make_observer(NOVAS_SOLAR_SYSTEM_OBSERVER, NULL, &loc, obs), 0);
  return 0;
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
  vel *= NOVAS_KM / C;   // [km/s] -> beta
  if(fabs(vel) > 1.0) {
    novas_error(-1, EINVAL, "novas_v2z", "velocity exceeds speed of light v=%g km/s", vel * C / NOVAS_KM);
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

/**
 * Returns the NOVAS planet ID for a given name (case insensitive), or -1 if no match is found.
 *
 * @param name    The planet name, or that for the "Sun", "Moon" or "SSB" (case insensitive).
 *                The spelled out "Solar System Barycenter" is also recognized with either spaces,
 *                hyphens ('-') or underscores ('_') separating the case insensitive words.
 * @return        The NOVAS major planet ID, or -1 (errno set to EINVAL) if the input name is
 *                NULL or if there is no match for the name provided.
 *
 * @author Attila Kovacs
 * @since 1.2
 *
 * @sa make_planet()
 */
enum novas_planet novas_planet_for_name(const char *name) {
  static const char *fn = "novas_planet_for_name()";
  static const char *names[] = NOVAS_PLANET_NAMES_INIT;

  char *tok;
  int i;

  if(!name)
    return novas_error(-1, EINVAL, fn, "Input name is NULL");

  if(!name[0])
    return novas_error(-1, EINVAL, fn, "Input name is empty");

  for(i = 0; i < NOVAS_PLANETS; i++)
    if(strcasecmp(name, (const char *) names[i]) == 0)
      return i;

  // Check for Solar System Barycenter (and variants)
  tok = strtok(strdup(name), " \t-_");
  if(strcasecmp("solar", tok) == 0) {
    tok = strtok(NULL, " \t-_");
    if(tok && strcasecmp("system", tok) == 0) {
      tok = strtok(NULL, " \t-_");
      if(tok && strcasecmp("barycenter", tok) == 0)
        return NOVAS_SSB;
    }
  }

  return novas_error(-1, EINVAL, fn, "No match for name: '%s'", name);
}

/**
 * Sets the orientation of an orbital system using the RA and DEC coordinates of the pole
 * of the Laplace (or else equatorial) plane relative to which the orbital elements are
 * defined. Orbital parameters of planetary satellites normally include the R.A. and
 * declination of the pole of the local Laplace plane in which the Keplerian orbital elements
 * are referenced.
 *
 * The system will become referenced to the equatorial plane, the relative obliquity is set
 * to (90&deg; - `dec`), while the argument of the ascending node ('Omega') is set to
 * (90&deg; + `ra`).
 *
 * NOTES:
 * <ol>
 * <li>You should not expect much precision from the long-range orbital approximations for
 * planetary satellites. For applications that require precision at any level, you should rely
 * on appropriate ephemerides, or else on up-to-date short-term orbital elements.</li>
 * </ol>
 *
 * @param type  Coordinate reference system in which `ra` and `dec` are defined (e.g. NOVAS_GCRS).
 * @param ra    [h] the R.A. of the pole of the oribtal reference plane.
 * @param dec   [deg] the declination of the pole of the oribtal reference plane.
 * @param[out]  sys   The orbital system
 * @return      0 if successful, or else -1 (errno will be set to EINVAL) if the output `sys`
 *              pointer is NULL.
 *
 * @author Attila Kovacs
 * @since 1.2
 *
 * @sa make_orbital_object()
 */
int novas_set_orbsys_pole(enum novas_reference_system type, double ra, double dec, novas_orbital_system *sys) {
  if (!sys)
    return novas_error(-1, EINVAL, "novas_set_orbsys_pole", "input system is NULL");

  sys->plane = NOVAS_EQUATORIAL_PLANE;
  sys->type = type;
  sys->obl = remainder(90.0 - dec, 360.0);
  sys->Omega = remainder(15.0 * ra + 90.0, 360.0);

  return 0;
}
