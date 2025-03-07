/**
 * @file
 *
 * @date Created  on Mar 5, 2025
 * @author G. Kaplan and Attila Kovacs
 *
 *  Various transformations between different coordinate systems
 */

#include <string.h>
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
 * Converts ICRS right ascension and declination to galactic longitude and latitude.
 *
 * REFERENCES:
 * <ol>
 * <li>Hipparcos and Tycho Catalogues, Vol. 1, Section 1.5.3.</li>
 * </ol>
 *
 * @param ra          [h] ICRS right ascension in hours.
 * @param dec         [deg] ICRS declination in degrees.
 * @param[out] glon   [deg] Galactic longitude in degrees.
 * @param[out] glat   [deg] Galactic latitude in degrees.
 *
 * @return            0 if successful, or -1 if either of the output pointer arguments
 *                    are NULL.
 *
 * @sa gal2equ()
 */
int equ2gal(double ra, double dec, double *restrict glon, double *restrict glat) {
  double pos1[3], pos2[3], xyproj, cosd;

  // Rotation matrix A_g from Hipparcos documentation eq. 1.5.11.
  // AK: Transposed compared to NOVAS C 3.1 for dot product handling.
  static const double ag[3][3] = { //
          { -0.0548755604, -0.8734370902, -0.4838350155 }, //
          { +0.4941094279, -0.4448296300, +0.7469822445 }, //
          { -0.8676661490, -0.1980763734, +0.4559837762 } };

  if(!glon || !glat)
    return novas_error(-1, EINVAL, "equ2gal", "NULL output pointer: glon=%p, glat=%p", glon, glat);

  // Form position vector in equatorial system from input coordinates
  ra *= HOURANGLE;
  dec *= DEGREE;
  cosd = cos(dec);

  pos1[0] = cosd * cos(ra);
  pos1[1] = cosd * sin(ra);
  pos1[2] = sin(dec);

  // Rotate position vector to galactic system, using Hipparcos documentation eq. 1.5.13.
  pos2[0] = novas_vdot(ag[0], pos1);
  pos2[1] = novas_vdot(ag[1], pos1);
  pos2[2] = novas_vdot(ag[2], pos1);

  // Decompose galactic vector into longitude and latitude.
  xyproj = sqrt(pos2[0] * pos2[0] + pos2[1] * pos2[1]);
  *glon = xyproj ? atan2(pos2[1], pos2[0]) / DEGREE : 0.0;
  if(*glon < 0.0)
    *glon += DEG360;

  *glat = atan2(pos2[2], xyproj) / DEGREE;

  return 0;
}

/**
 * Convert right ascension and declination to ecliptic longitude and latitude.  To convert
 * GCRS RA and dec to ecliptic coordinates (mean ecliptic and equinox of J2000.0), set
 * 'coord_sys' to NOVAS_GCRS_EQUATOR(2); in this case the value of 'jd_tt' can be set to
 * anything, since J2000.0 is assumed. Otherwise, all input coordinates are dynamical at
 * 'jd_tt'.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date. (Unused if 'coord_sys'
 *                    is NOVAS_GCRS_EQUATOR[2])
 * @param coord_sys   The astrometric reference system of the coordinates. If 'coord_sys' is
 *                    NOVAS_GCRS_EQUATOR(2), the input GCRS coordinates are converted to
 *                    J2000 ecliptic coordinates.
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param ra          [h] Right ascension in hours, referred to specified equator and equinox
 *                    of date.
 * @param dec         [deg] Declination in degrees, referred to specified equator and equinox
 *                    of date.
 * @param[out] elon   [deg] Ecliptic longitude in degrees, referred to specified ecliptic and
 *                    equinox of date.
 * @param[out] elat   [deg] Ecliptic latitude in degrees, referred to specified ecliptic and
 *                    equinox of date.
 * @return            0 if successful, or else 1 if the value of 'coord_sys' is invalid.
 *
 * @sa equ2ecl_vec()
 * @sa ecl2equ()
 *
 */
short equ2ecl(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double ra, double dec,
        double *restrict elon, double *restrict elat) {
  static const char *fn = "equ2ecl";
  double cosd, pos[3], xyproj;

  if(!elon || !elat)
    return novas_error(-1, EINVAL, fn, "NULL output pointer: elon=%p, elat=%p", elon, elat);

  // Form position vector in equatorial system from input coordinates.
  ra *= HOURANGLE;
  dec *= DEGREE;
  cosd = cos(dec);

  pos[0] = cosd * cos(ra);
  pos[1] = cosd * sin(ra);
  pos[2] = sin(dec);

  // Convert the vector from equatorial to ecliptic system.
  prop_error(fn, equ2ecl_vec(jd_tt, coord_sys, accuracy, pos, pos), 0);

  // Decompose ecliptic vector into ecliptic longitude and latitude.
  xyproj = sqrt(pos[0] * pos[0] + pos[1] * pos[1]);

  *elon = (xyproj > 0.0) ? atan2(pos[1], pos[0]) / DEGREE : 0.0;
  if(*elon < 0.0)
    *elon += DEG360;

  *elat = atan2(pos[2], xyproj) / DEGREE;

  return 0;
}

/**
 * Converts an equatorial position vector to an ecliptic position vector. To convert
 * ICRS RA and dec to ecliptic coordinates (mean ecliptic and equinox of J2000.0), set
 * 'coord_sys' to NOVAS_GCRS_EQUATOR(2); in this case the value of 'jd_tt' can be set
 * to anything, since J2000.0 is assumed. Otherwise, all input coordinates are dynamical
 * at 'jd_tt'.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date. (Unused if 'coord_sys'
 *                    is NOVAS_GCRS_EQUATOR[2])
 * @param coord_sys   The astrometric reference system type of the coordinates.
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in          Position vector, referred to specified equator and equinox of date.
 * @param[out] out    Position vector, referred to specified ecliptic and equinox of date.
 *                    It can be the same vector as the input. If 'coord_sys' is
 *                    NOVAS_GCRS_EQUATOR(2), the input GCRS coordinates are converted to
 *                    J2000 ecliptic coordinates.
 * @return            0 if successful, -1 if either vector argument is NULL or the accuracy
 *                    is invalid, or else 1 if the value of 'coord_sys' is invalid.
 *
 * @sa equ2ecl()
 * @sa ecl2equ_vec()
 */
short equ2ecl_vec(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy,
        const double *in, double *out) {
  static const char *fn = "equ2ecl_vec";
  double pos0[3], obl, c, s;

  if(!in || !out)
    return novas_error(-1, EINVAL, fn, "NULL input or output 3-vector: in=%p, out=%p", in, out);

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  // Get obliquity, depending upon the "system" of the input coordinates.
  switch(coord_sys) {
    case NOVAS_MEAN_EQUATOR:      // Input: mean equator and equinox of date
    case NOVAS_TRUE_EQUATOR: {    // Input: true equator and equinox of date
      static THREAD_LOCAL enum novas_accuracy acc_last = -1;
      static THREAD_LOCAL double t_last = NAN, oblm, oblt;

      memcpy(pos0, in, sizeof(pos0));

      if(!time_equals(jd_tt, t_last) || accuracy != acc_last) {
        const double jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY; // TDB date
        e_tilt(jd_tdb, accuracy, &oblm, &oblt, NULL, NULL, NULL);
        t_last = jd_tt;
        acc_last = accuracy;
      }

      obl = (coord_sys == NOVAS_MEAN_EQUATOR ? oblm : oblt) * DEGREE;
      break;
    }

    case NOVAS_GCRS_EQUATOR: /* Input: GCRS */{
      static THREAD_LOCAL enum novas_accuracy acc_2000 = -1;
      static THREAD_LOCAL double ob2000;

      frame_tie(in, ICRS_TO_J2000, pos0);

      if(accuracy != acc_2000) {
        ob2000 = mean_obliq(JD_J2000);
        acc_2000 = accuracy;
      }

      obl = ob2000 * ARCSEC;
      break;
    }

    default:
      return novas_error(1, EINVAL, fn, "invalid equator type: %d", coord_sys);
  }

  c = cos(obl);
  s = sin(obl);

  // Rotate position vector to ecliptic system.
  out[0] = pos0[0];
  out[1] = pos0[1] * c + pos0[2] * s;
  out[2] = -pos0[1] * s + pos0[2] * c;

  return 0;
}

/**
 * Converts an ecliptic position vector to an equatorial position vector. To convert
 * ecliptic coordinates (mean ecliptic and equinox of J2000.0) to GCRS RA and dec to, set
 * 'coord_sys' to NOVAS_GCRS_EQUATOR(2); in this case the value of 'jd_tt' can be set to
 * anything, since J2000.0 is assumed. Otherwise, all input coordinates are dynamical at
 * 'jd_tt'.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date. (Unused if 'coord_sys'
 *                    is NOVAS_GCRS_EQUATOR[2])
 * @param coord_sys   The astrometric reference system type of the coordinates
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in          Position vector, referred to specified ecliptic and equinox of date.
 * @param[out] out    Position vector, referred to specified equator and equinox of date. It
 *                    can be the same vector as the input.
 * @return            0 if successful, -1 if either vector argument is NULL or the accuracy
 *                    is invalid, or else 1 if the value of 'coord_sys' is invalid.
 *
 * @sa ecl2equ()
 * @sa equ2ecl_vec()
 */
short ecl2equ_vec(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy,
        const double *in, double *out) {
  static const char *fn = "ecl2equ_vec";
  double obl = 0.0, c, s, x, y, z;

  if(!in || !out)
    return novas_error(-1, EINVAL, fn, "NULL input or output 3-vector: in=%p, out=%p", in, out);

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  // Get obliquity, depending upon the "system" of the input coordinates.
  switch(coord_sys) {

    case NOVAS_MEAN_EQUATOR:        // Output: mean equator and equinox of date
    case NOVAS_TRUE_EQUATOR: {      // Output: true equator and equinox of date
      static THREAD_LOCAL enum novas_accuracy acc_last = -1;
      static THREAD_LOCAL double t_last = NAN, oblm, oblt;

      if(!time_equals(jd_tt, t_last) || accuracy != acc_last) {
        const double jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY;    // TDB date
        e_tilt(jd_tdb, accuracy, &oblm, &oblt, NULL, NULL, NULL);
        t_last = jd_tt;
        acc_last = accuracy;
      }

      obl = (coord_sys == NOVAS_MEAN_EQUATOR ? oblm : oblt) * DEGREE;
      break;
    }

    case NOVAS_GCRS_EQUATOR: {      // Output: GCRS
      static THREAD_LOCAL double ob2000;

      if(ob2000 == 0.0) {
        ob2000 = mean_obliq(JD_J2000);
      }

      obl = ob2000 * ARCSEC;
      break;
    }

    default:
      return novas_error(1, EINVAL, fn, "invalid equator type: %d", coord_sys);
  }

  x = in[0];
  y = in[1];
  z = in[2];

  c = cos(obl);
  s = sin(obl);

  // Rotate position vector to ecliptic system.
  out[0] = x;
  out[1] = y * c - z * s;
  out[2] = y * s + z * c;

  // Case where output vector is to be in ICRS, rotate from dynamical
  // system to ICRS.
  if(coord_sys == NOVAS_GCRS_EQUATOR) {
    frame_tie(out, J2000_TO_ICRS, out);
  }

  return 0;
}

/**
 * Transforms topocentric (TOD) right ascension and declination to zenith distance and
 * azimuth. This method should not be used to convert CIRS apparent coordinates (IAU
 * 2000 standard) -- for those you should use cirs_to_itrs() followed by itrs_to_hor()
 * instead.
 *
 * It uses a method that properly accounts for polar motion, which is significant at the
 * sub-arcsecond level. This function can also adjust coordinates for atmospheric
 * refraction.
 *
 * @deprecated  The name of this function does not reveal what type of equatorial
 *              coordinates it requires. To make it less ambiguous, you should use
 *              tod_to_itrs() followed by itrs_to_hor() instead, possibly following it
 *              with refract_astro() if you also want to apply optical refraction.
 *
 * NOTES:
 * <ul>
 *  <li>'xp' and 'yp' can be set to zero if sub-arcsecond accuracy is not needed.</li>
 *  <li> The directions 'zd'= 0 (zenith) and 'az'= 0 (north) are here considered fixed in
 *  the terrestrial system. Specifically, the zenith is along the geodetic normal, and north
 *  is toward the ITRS pole.</li>
 *  <li>If 'ref_option' is NOVAS_STANDARD_ATMOSPHERE (1), then 'rar'='ra' and 'decr'='dec'.
 * </ul>
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. (2008). USNO/AA Technical Note of 28 Apr 2008, "Refraction as a Vector."</li>
 * </ol>
 *
 * @param jd_ut1      [day] UT1 based Julian date
 * @param ut1_to_tt   [s] TT - UT1 Time difference in seconds
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param xp          [arcsec] Conventionally-defined x coordinate of celestial intermediate
 *                    pole with respect to ITRS reference pole, in arcseconds.
 * @param yp          [arcsec] Conventionally-defined y coordinate of celestial intermediate
 *                    pole with respect to ITRS reference pole, in arcseconds.
 * @param location    The observer location
 * @param ra          [h] Topocentric right ascension of object of interest, in hours, referred
 *                    to true equator and equinox of date.
 * @param dec         [deg] Topocentric declination of object of interest, in degrees, referred
 *                    to true equator and equinox of date.
 * @param ref_option  NOVAS_STANDARD_ATMOSPHERE (1), or NOVAS_WEATHER_AT_LOCATION (2) if to use
 *                    the weather
 * @param[out] zd     [deg] Topocentric zenith distance in degrees (unrefracted).
 * @param[out] az     [deg] Topocentric azimuth (measured east from north) in degrees.
 * @param[out] rar    [h] Topocentric right ascension of object of interest, in hours, referred
 *                    to true equator and equinox of date, affected by refraction if 'ref_option'
 *                    is non-zero. (It may be NULL if not required)
 * @param[out] decr   [deg] Topocentric declination of object of interest, in degrees, referred
 *                    to true equator and equinox of date. (It may be NULL if not required)
 * @return            0 if successful, or -1 if one of the 'zd' or 'az' output pointers are NULL.
 *
 * @sa itrs_to_hor()
 * @sa tod_to_itrs()
 * @sa NOVAS_TOD
 *
 */
int equ2hor(double jd_ut1, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp,
        const on_surface *restrict location, double ra, double dec, enum novas_refraction_model ref_option,
        double *restrict zd, double *restrict az, double *restrict rar, double *restrict decr) {

  double lon, lat, sinlat, coslat, sinlon, coslon, cosdec;
  double uze[3], une[3], uwe[3], uz[3], un[3], uw[3], p[3];
  double pz, pn, pw, proj, pr[3];

  // Default output values in case of error return;
  if(az)
    *az = NAN;
  if(zd)
    *zd = NAN;
  if(rar)
    *rar = ra;
  if(decr)
    *decr = dec;

  if(!location || !zd || !az)
    return novas_error(-1, EINVAL, "equ2hor", "NULL input observer location=%p, output az=%p, or zd=%p pointer", location, az, zd);

  lon = location->longitude * DEGREE;
  lat = location->latitude * DEGREE;

  sinlat = sin(lat);
  coslat = cos(lat);
  sinlon = sin(lon);
  coslon = cos(lon);

  ra *= HOURANGLE;
  dec *= DEGREE;

  cosdec = cos(dec);

  // Define unit vector 'p' toward object in celestial system
  // (wrt equator and equinox of date).
  p[0] = cosdec * cos(ra);
  p[1] = cosdec * sin(ra);
  p[2] = sin(dec);

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
  ter2cel(jd_ut1, 0.0, ut1_to_tt, EROT_GST, accuracy, NOVAS_DYNAMICAL_CLASS, xp, yp, uze, uz);
  ter2cel(jd_ut1, 0.0, ut1_to_tt, EROT_GST, accuracy, NOVAS_DYNAMICAL_CLASS, xp, yp, une, un);
  ter2cel(jd_ut1, 0.0, ut1_to_tt, EROT_GST, accuracy, NOVAS_DYNAMICAL_CLASS, xp, yp, uwe, uw);

  // Compute coordinates of object w.r.t orthonormal basis.

  // Compute components of 'p' - projections of 'p' onto rotated
  // Earth-fixed basis vectors.
  pz = novas_vdot(p, uz);
  pn = novas_vdot(p, un);
  pw = novas_vdot(p, uw);

  // Compute azimuth and zenith distance.
  proj = sqrt(pn * pn + pw * pw);
  *az = proj ? -atan2(pw, pn) / DEGREE : 0.0;
  if(*az < 0.0)
    *az += DEG360;

  *zd = atan2(proj, pz) / DEGREE;

  // Apply atmospheric refraction if requested.
  if(ref_option) {
    // Get refraction in zenith distance.
    const double zd0 = *zd;
    const double refr = refract_astro(location, ref_option, zd0);

    // Apply refraction to celestial coordinates of object.
    if(refr > 0.0) {
      // Shift position vector of object in celestial system to account
      // for refraction (see USNO/AA Technical Note 1998-09).
      const double sinzd = sin(*zd * DEGREE);
      const double coszd = cos(*zd * DEGREE);
      const double sinzd0 = sin(zd0 * DEGREE);
      const double coszd0 = cos(zd0 * DEGREE);

      int j;

      *zd -= refr;

      // Compute refracted position vector.
      for(j = 3; --j >= 0;)
        pr[j] = ((p[j] - coszd0 * uz[j]) / sinzd0) * sinzd + uz[j] * coszd;

      // Compute refracted right ascension and declination.
      proj = sqrt(pr[0] * pr[0] + pr[1] * pr[1]);

      if(rar) {
        *rar = proj ? atan2(pr[1], pr[0]) / HOURANGLE : 0.0;
        if(*rar < 0.0)
          *rar += DAY_HOURS;
      }

      if(decr)
        *decr = atan2(pr[2], proj) / DEGREE;
    }
  }
  return 0;
}

/**
 * Converts GCRS right ascension and declination to coordinates with respect to the equator
 * of date (mean or true). For coordinates with respect to the true equator of date, the
 * origin of right ascension can be either the true equinox or the celestial intermediate
 * origin (CIO). This function only supports the CIO-based method.
 *
 * @param jd_tt       [day] Terrestrial Time (TT) based Julian date. (Unused if 'coord_sys'
 *                    is NOVAS_ICRS_EQUATOR)
 * @param sys         Dynamical equatorial system type
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1) (unused if
 *                    'coord_sys' is not NOVAS_ICRS [3])
 * @param rag         [h] GCRS right ascension in hours.
 * @param decg        [deg] GCRS declination in degrees.
 * @param[out] ra     [h] Right ascension in hours, referred to specified equator and right
 *                    ascension origin of date.
 * @param[out] dec    [deg] Declination in degrees, referred to specified equator of date.
 * @return            0 if successful, or -1 with errno set to EINVAL if the output pointers
 *                    are NULL or the coord_sys is invalid, otherwise &lt;0 if an error from
 *                    vector2radec(), 10--20 error is  10 + error cio_location(); or else
 *                    20 + error from cio_basis()
 */
short gcrs2equ(double jd_tt, enum novas_dynamical_type sys, enum novas_accuracy accuracy, double rag, double decg,
        double *restrict ra, double *restrict dec) {
  static const char *fn = "gcrs2equ";
  double jd_tdb, r, d, pos1[3], pos2[3];

  if(!ra || !dec)
    return novas_error(-1, EINVAL, fn, "NULL output pointer: ra=%p, dec=%p", ra, dec);

  // 'jd_tdb' is the TDB Julian date.
  jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY;

  // Form position vector in equatorial system from input coordinates.
  r = rag * 15.0 * DEG2RAD;
  d = decg * DEG2RAD;

  pos1[0] = cos(d) * cos(r);
  pos1[1] = cos(d) * sin(r);
  pos1[2] = sin(d);

  // Transform the position vector based on the value of 'coord_sys'.
  switch(sys) {
    // Transform the position vector from GCRS to mean equator and equinox
    // of date.

    // If requested, transform further to true equator and equinox of date.
    case NOVAS_DYNAMICAL_TOD:
      gcrs_to_tod(jd_tdb, accuracy, pos1, pos2);
      break;

    case NOVAS_DYNAMICAL_MOD: {
      gcrs_to_mod(jd_tdb, pos1, pos2);
      break;
    }

    case NOVAS_DYNAMICAL_CIRS:
      prop_error(fn, gcrs_to_cirs(jd_tdb, accuracy, pos1, pos2), 10);
      break;

    default:
      return novas_error(-1, EINVAL, fn, "invalid dynamical system type: %d", sys);
  }

  // Convert the position vector to equatorial spherical coordinates.
  prop_error(fn, -vector2radec(pos2, ra, dec), 0);

  return 0;
}

/**
 * Rotates a vector from the terrestrial to the celestial system. Specifically, it transforms
 * a vector in the ITRS (rotating earth-fixed system) to the True of Date (TOD), CIRS, or GCRS
 * (a local space-fixed system) by applying rotations for polar motion, Earth rotation (for
 * TOD); and nutation, precession, and the dynamical-to-GCRS frame tie (for GCRS).
 *
 * If 'system' is NOVAS_CIRS then method EROT_ERA must be used. Similarly, if 'system' is
 * NOVAS_TOD then method must be EROT_ERA. Otherwise an error 3 is returned.
 *
 * If both 'xp' and 'yp' are set to 0 no polar motion is included in the transformation.
 *
 * @deprecated This function can be confusing to use due to the output coordinate system
 *             being specified by a combination of two options. Use itrs_to_cirs() or
 *             itrs_to_tod() instead. You can then follow these with other conversions to
 *             GCRS (or whatever else) as appropriate.
 *
 * REFERENCES:
 *  <ol>
 *   <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *   <li>Kaplan, G. H. (2003), 'Another Look at Non-Rotating Origins', Proceedings of IAU XXV
 *   Joint Discussion 16.</li>
 *  </ol>
 *
 * @param jd_ut1_high   [day] High-order part of UT1 Julian date.
 * @param jd_ut1_low    [day] Low-order part of UT1 Julian date.
 * @param ut1_to_tt     [s] TT - UT1 Time difference in seconds
 * @param erot          EROT_ERA (0) or EROT_GST (1), depending on whether to use GST relative
 *                      to equinox of date (pre IAU 2006) or ERA relative to the CIO (IAU 2006
 *                      standard) as the Earth rotation measure. The main effect of this option
 *                      is that it selects the output coordinate system as CIRS or TOD if
 *                      the output coordinate class is NOVAS_DYNAMICAL_CLASS.
 * @param accuracy      NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param class         Output coordinate class NOVAS_REFERENCE_CLASS (0, or any value other than 1)
 *                      or NOVAS_DYNAMICAL_CLASS (1). Use the former if the output coordinates are
 *                      to be in the GCRS, and the latter if they are to be in CIRS or TOD (the 'erot'
 *                      parameter selects which dynamical system to use for the output.)
 * @param xp            [arcsec] Conventionally-defined X coordinate of celestial intermediate
 *                      pole with respect to ITRS pole, in arcseconds.
 * @param yp            [arcsec] Conventionally-defined Y coordinate of celestial intermediate
 *                      pole with respect to ITRS pole, in arcseconds.
 * @param in            Position vector, geocentric equatorial rectangular coordinates,
 *                      referred to ITRS axes (terrestrial system) in the normal case
 *                      where 'option' is NOVAS_GCRS (0).
 * @param[out] out      Position vector, equatorial rectangular coordinates in the specified
 *                      output system (GCRS if 'class' is NOVAS_REFERENCE_CLASS;
 *                      or else either CIRS if 'erot' is EROT_ERA, or TOD if 'erot' is EROT_GST).
 *                      It may be the same vector as the input.
 * @return              0 if successful, -1 if either of the vector arguments is NULL, 1 if
 *                      'accuracy' is invalid, 2 if 'method' is invalid 10--20, or else 10 + the
 *                      error from cio_location(), or 20 + error from cio_basis().
 *
 * @sa itrs_to_cirs()
 * @sa cirs_to_gcrs()
 * @sa itrs_to_tod()
 * @sa tod_to_j2000()
 * @sa frame_tie()
 * @sa cel2ter()
 */
short ter2cel(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_earth_rotation_measure erot, enum novas_accuracy accuracy,
        enum novas_equatorial_class class, double xp, double yp, const double *in, double *out) {
  static const char *fn = "ter2cel";
  double jd_ut1, jd_tt, jd_tdb;

  if(!in || !out)
    return novas_error(-1, EINVAL, fn, "NULL input or output 3-vector: in=%p, out=%p", in, out);

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  // Compute the TT Julian date corresponding to the input UT1 Julian
  // date.
  jd_ut1 = jd_ut1_high + jd_ut1_low;
  jd_tt = jd_ut1 + (ut1_to_tt / DAY);

  // Compute the TDB Julian date corresponding to the input UT1 Julian
  // date.
  jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY;

  // Apply polar motion
  if(xp || yp)
    wobble(jd_tt, WOBBLE_ITRS_TO_PEF, xp, yp, in, out);
  else if(out != in)
    memcpy(out, in, XYZ_VECTOR_SIZE);

  switch(erot) {
    case (EROT_ERA): {
      // 'CIO-TIO-THETA' method. See second reference, eq. (3) and (4).

      // Compute and apply the Earth rotation angle, 'theta', transforming the
      // vector to the celestial intermediate system.
      spin(-era(jd_ut1_high, jd_ut1_low), out, out);

      if(class != NOVAS_DYNAMICAL_CLASS)
        prop_error(fn, cirs_to_gcrs(jd_tdb, accuracy, out, out), 10);

      break;
    }
    case (EROT_GST): {
      double gast;

      sidereal_time(jd_ut1_high, jd_ut1_low, ut1_to_tt, NOVAS_TRUE_EQUINOX, EROT_GST, accuracy, &gast);
      spin(-15.0 * gast, out, out);

      if(class != NOVAS_DYNAMICAL_CLASS) {
        tod_to_gcrs(jd_tdb, accuracy, out, out);
      }
      break;
    }

    default:
      return novas_error(2, EINVAL, fn, "invalid Earth rotation measure type: %d", erot);
  }

  return 0;
}

/**
 * Rotates a vector from the celestial to the terrestrial system.  Specifically, it transforms
 * a vector in the GCRS, or the dynamcal CIRS or TOD frames to the ITRS (a rotating earth-fixed
 * system) by applying rotations for the GCRS-to-dynamical frame tie, precession, nutation, Earth
 * rotation, and polar motion.
 *
 * If 'system' is NOVAS_CIRS then method EROT_ERA must be used. Similarly, if 'system'
 * is NOVAS_TOD then method must be EROT_ERA. Otherwise an error 3 is returned.
 *
 * If both 'xp' and 'yp' are set to 0 no polar motion is included in the transformation.
 *
 * @deprecated This function can be confusing to use due to the input coordinate system
 *             being specified by a combination of two options. Use itrs_to_cirs() or
 *             itrs_to_tod() instead. You can then follow these with other conversions to
 *             GCRS (or whatever else) as appropriate.
 *
 * REFERENCES:
 *  <ol>
 *   <li>Kaplan, G. H. et. al. (1989). Astron. Journ. 97, 1197-1210.</li>
 *   <li>Kaplan, G. H. (2003), 'Another Look at Non-Rotating Origins', Proceedings of IAU XXV J
 *   oint Discussion 16.</li>
 *  </ol>
 *
 * @param jd_ut1_high   [day] High-order part of UT1 Julian date.
 * @param jd_ut1_low    [day] Low-order part of UT1 Julian date.
 * @param ut1_to_tt     [s] TT - UT1 Time difference in seconds
 * @param erot          EROT_ERA (0) or EROT_GST (1), depending on whether to use GST relative to
 *                      equinox of date (pre IAU 2006) or ERA relative to the CIO (IAU 2006 standard)
 *                      as the Earth rotation measure. The main effect of this option
 *                      is that it specifies the input coordinate system as CIRS or TOD when
 *                      the input coordinate class is NOVAS_DYNAMICAL_CLASS.
 * @param accuracy      NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param class         Input coordinate class, NOVAS_REFERENCE_CLASS (0) or NOVAS_DYNAMICAL_CLASS (1).
 *                      Use the former if the input coordinates are in the GCRS, and the latter if they
 *                      are CIRS or TOD (the 'erot' parameter selects which dynamical system the input is
 *                      specified in.)
 * @param xp            [arcsec] Conventionally-defined X coordinate of celestial intermediate
 *                      pole with respect to ITRS pole, in arcseconds.
 * @param yp            [arcsec] Conventionally-defined Y coordinate of celestial intermediate
 *                      pole with respect to ITRS pole, in arcseconds.
 * @param in            Input position vector, geocentric equatorial rectangular coordinates in the
 *                      specified input coordinate system (GCRS if 'class' is NOVAS_REFERENCE_CLASS;
 *                      or else either CIRS if 'erot' is EROT_ERA, or TOD if 'erot' is EROT_GST).
 * @param[out] out      ITRS position vector, geocentric equatorial rectangular coordinates
 *                      (terrestrial system). It can be the same vector as the input.
 * @return              0 if successful, -1 if either of the vector arguments is NULL, 1 if 'accuracy'
 *                      is invalid, 2 if 'method' is invalid, or else 10 + the error from
 *                      cio_location(), or 20 + error from cio_basis().
 *
 * @sa gcrs_to_cirs()
 * @sa cirs_to_itrs()
 * @sa frame_tie()
 * @sa j2000_to_tod()
 * @sa tod_to_itrs()
 * @sa ter2cel()
 */
short cel2ter(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_earth_rotation_measure erot, enum novas_accuracy accuracy,
        enum novas_equatorial_class class, double xp, double yp, const double *in, double *out) {
  static const char *fn = "cel2ter";
  double jd_ut1, jd_tt, jd_tdb;

  if(!in || !out)
    return novas_error(-1, EINVAL, fn, "NULL input or output 3-vector: in=%p, out=%p", in, out);

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  // Compute the TT Julian date corresponding to the input UT1 Julian date.
  jd_ut1 = jd_ut1_high + jd_ut1_low;
  jd_tt = jd_ut1 + (ut1_to_tt / DAY);

  // Compute the TDB Julian date corresponding to the input UT1 Julian date
  jd_tdb = jd_tt + tt2tdb(jd_tt) / DAY;

  switch(erot) {
    case EROT_ERA:
      // IAU 2006 standard method
      if(class != NOVAS_DYNAMICAL_CLASS) {
        // See second reference, eq. (3) and (4).
        prop_error(fn, gcrs_to_cirs(jd_tt, accuracy, in, out), 10);
      }
      else if (out != in) {
        memcpy(out, in, XYZ_VECTOR_SIZE);
      }

      // Compute and apply the Earth rotation angle, 'theta', transforming the
      // vector to the terrestrial intermediate system.
      spin(era(jd_ut1_high, jd_ut1_low), out, out);
      break;

    case EROT_GST: {
      double gast;

      // Pre IAU 2006 method
      if(class != NOVAS_DYNAMICAL_CLASS) {
        gcrs_to_tod(jd_tdb, accuracy, in, out);
      }
      else if (out != in) {
        memcpy(out, in, XYZ_VECTOR_SIZE);
      }

      // Apply Earth rotation.
      sidereal_time(jd_ut1_high, jd_ut1_low, ut1_to_tt, NOVAS_TRUE_EQUINOX, EROT_GST, accuracy, &gast);
      spin(15.0 * gast, out, out);
      break;
    }

    default:
      return novas_error(2, EINVAL, fn, "invalid Earth rotation measure type: %d", erot);
  }

  // Apply polar motion, transforming the vector to the ITRS.
  if(xp || yp)
    wobble(jd_tdb, WOBBLE_PEF_TO_ITRS, xp, yp, out, out);

  return 0;
}
