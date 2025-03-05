/**
 * @file
 *
 * @date Created  on Mar 5, 2025
 * @author Attila Kovacs
 *
 *  Various coordinate transformation extensions to novas.c
 */

#include <math.h>
#include <errno.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"
/// \endcond

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
int ecl2equ(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double elon, double elat,
        double *restrict ra, double *restrict dec) {
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
int gal2equ(double glon, double glat, double *restrict ra, double *restrict dec) {
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
 * NOTES:
 * <ol>
 * <li>The accuracy of the output CIRS coordinates depends on how the input TOD coordinates
 * were obtained. If TOD was calculated via the old (pre IAU 2006) method, using the Lieske et
 * al. 1997 nutation model, then the limited accuracy of that model will affect the resulting
 * coordinates. This is the case for the SuperNOVAS functions novas_geom_posvel() and
 * novas_sky_pos() also, when called with `NOVAS_TOD` as the system, as well as all legacy NOVAS
 * C functions that produce TOD coordinates.</li>
 * </ol>
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
int itrs_to_hor(const on_surface *restrict location, const double *restrict itrs, double *restrict az, double *restrict za) {
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
int hor_to_itrs(const on_surface *restrict location, double az, double za, double *restrict itrs) {
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
  ra = remainder(ra + ra_cio, DAY_HOURS);
  if(ra < 0.0)
    ra += DAY_HOURS;

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
  ra = remainder(ra - ra_cio, DAY_HOURS);
  if(ra < 0.0)
    ra += DAY_HOURS;

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
