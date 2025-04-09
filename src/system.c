/**
 * @file
 *
 * @date Created  on Mar 6, 2025
 * @author G. Kaplan and Attila Kovacs
 *
 *  Various functions to transform between different equatorial coordinate systems.
 */

#include <string.h>
#include <errno.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"
/// \endcond

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
 * @param coordType     Output coordinate class NOVAS_REFERENCE_CLASS (0, or any value other than 1)
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
        enum novas_equatorial_class coordType, double xp, double yp, const double *in, double *out) {
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

      if(coordType != NOVAS_DYNAMICAL_CLASS)
        prop_error(fn, cirs_to_gcrs(jd_tdb, accuracy, out, out), 10);

      break;
    }
    case (EROT_GST): {
      double gast;

      sidereal_time(jd_ut1_high, jd_ut1_low, ut1_to_tt, NOVAS_TRUE_EQUINOX, EROT_GST, accuracy, &gast);
      spin(-15.0 * gast, out, out);

      if(coordType != NOVAS_DYNAMICAL_CLASS) {
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
 * @param coordType     Input coordinate class, NOVAS_REFERENCE_CLASS (0) or NOVAS_DYNAMICAL_CLASS (1).
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
        enum novas_equatorial_class coordType, double xp, double yp, const double *in, double *out) {
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
      if(coordType != NOVAS_DYNAMICAL_CLASS) {
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
      if(coordType != NOVAS_DYNAMICAL_CLASS) {
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


/**
 * Transforms a vector from the dynamical reference system to the International Celestial
 * Reference System (ICRS), or vice versa. The dynamical reference system is based on the
 * dynamical mean equator and equinox of J2000.0.  The ICRS is based on the space-fixed ICRS
 * axes defined by the radio catalog positions of several hundred extragalactic objects.
 *
 * For geocentric coordinates, the same transformation is used between the dynamical reference
 * system and the GCRS.
 *
 * NOTES:
 * <ol>
 * <li>More efficient 3D rotation implementation for small angles by A. Kovacs</li>
 * </ol>
 *
 * REFERENCES:
 *  <ol>
 *   <li>Hilton, J. and Hohenkerk, C. (2004), Astronomy and Astrophysics 413, 765-770, eq. (6)
 *   and (8).</li>
 *   <li>IERS (2003) Conventions, Chapter 5.</li>
 *  </ol>
 *
 * @param in          Position vector, equatorial rectangular coordinates.
 * @param direction   <0 for for dynamical to ICRS transformation, or else &gt;=0 for ICRS to
 *                    dynamical transformation. Alternatively you may use the constants
 *                    J2000_TO_ICRS (-1; or negative) or ICRS_TO_J2000 (0; or positive).
 * @param[out] out    Position vector, equatorial rectangular coordinates. It can be the same
 *                    vector as the input.
 * @return            0 if successfor or -1 if either of the vector arguments is NULL.
 *
 * @sa j2000_to_gcrs()
 * @sa gcrs_to_j2000()
 * @sa tod_to_j2000()
 * @sa j2000_to_tod()
 * @sa j2000_to_gcrs()
 */
int frame_tie(const double *in, enum novas_frametie_direction direction, double *out) {

  // 'xi0', 'eta0', and 'da0' are ICRS frame biases in arcseconds taken
  // from IERS (2003) Conventions, Chapter 5.
  static const double xi0 = -0.0166170 * ARCSEC;
  static const double eta0 = -0.0068192 * ARCSEC;
  static const double da0 = -0.01460 * ARCSEC;

  if(!in || !out)
    return novas_error(-1, EINVAL, "frame_tie", "NULL input or output 3-vector: in=%p, out=%p", in, out);

  if(direction < 0)
    novas_tiny_rotate(in, -eta0, xi0, da0, out);
  else
    novas_tiny_rotate(in, eta0, -xi0, -da0, out);

  return 0;
}

/**
 * Transforms a rectangular equatorial (x, y, z) vector from J2000 coordinates to the
 * True of Date (TOD) reference frame at the given epoch
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TDB) based Julian date that defines
 *                  the output epoch. Typically it does not require much precision, and
 *                  Julian dates in other time measures will be unlikely to affect the
 *                  result
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in        Input (x, y, z) position or velocity vector in rectangular equatorial
 *                  coordinates at J2000
 * @param[out] out  Output position or velocity 3-vector in the True equinox of Date
 *                  coordinate frame. It can be the same vector as the input.
 * @return          0 if successful, or -1 if either of the vector arguments is NULL or the
 *                  accuracy is invalid.
 *
 * @sa j2000_to_gcrs()
 * @sa tod_to_j2000()
 * @sa gcrs_to_j2000()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int j2000_to_tod(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out) {
  static const char *fn = "j2000_to_tod";

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  prop_error(fn, precession(JD_J2000, in, jd_tdb, out), 0);
  prop_error(fn, nutation(jd_tdb, NUTATE_MEAN_TO_TRUE, accuracy, out, out), 0);

  return 0;
}

/**
 * Transforms a rectangular equatorial (x, y, z) vector from True of Date (TOD) reference frame
 * at the given epoch to the J2000 coordinates.
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TDB) based Julian date that defines the
 *                  input epoch. Typically it does not require much precision, and Julian dates
 *                  in other time measures will be unlikely to affect the result
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in        Input (x, y, z)  position or velocity 3-vector in the True equinox of Date
 *                  coordinate frame.
 * @param[out] out  Output position or velocity vector in rectangular equatorial coordinates at
 *                  J2000. It can be the same vector as the input.
 * @return          0 if successful, or -1 if either of the vector arguments is NULL or the
 *                  'accuracy' is invalid.
 *
 * @sa j2000_to_tod()
 * @sa j2000_to_gcrs()
 * @sa tod_to_gcrs()
 * @sa tod_to_cirs()
 * @sa tod_to_itrs()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int tod_to_j2000(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out) {
  static const char *fn = "tod_to_j2000";

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  prop_error(fn, nutation(jd_tdb, NUTATE_TRUE_TO_MEAN, accuracy, in, out), 0);
  prop_error(fn, precession(jd_tdb, out, JD_J2000, out), 0);

  return 0;
}

/**
 * Change GCRS coordinates to J2000 coordinates. Same as frame_tie() called with ICRS_TO_J2000
 *
 * @param in        GCRS input 3-vector
 * @param[out] out  J2000 output 3-vector
 * @return          0 if successful, or else an error from frame_tie()
 *
 * @sa j2000_to_gcrs()
 * @sa tod_to_j2000()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int gcrs_to_j2000(const double *in, double *out) {
  prop_error("gcrs_to_j2000", frame_tie(in, ICRS_TO_J2000, out), 0);
  return 0;
}

/**
 * Transforms a rectangular equatorial (x, y, z) vector from the Geocentric Celestial
 * Reference System (GCRS) to the Mean of Date (MOD) reference frame at the given epoch
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TT) based Julian date that defines the
 *                  output epoch. Typically it does not require much precision, and Julian
 *                  dates in other time measures will be unlikely to affect the result
 * @param in        GCRS Input (x, y, z) position or velocity vector
 * @param[out] out  Output position or velocity 3-vector in the Mean wquinox of Date coordinate
 *                  frame. It can be the same vector as the input.
 * @return          0 if successful, or -1 if either of the vector arguments is NULL.
 *
 * @sa mod_to_gcrs()
 * @sa gcrs_to_tod()
 *
 * @since 1.2
 * @author Attila Kovacs
 */
int gcrs_to_mod(double jd_tdb, const double *in, double *out) {
  static const char *fn = "gcrs_to_tod [internal]";
  prop_error(fn, frame_tie(in, ICRS_TO_J2000, out), 0);
  prop_error(fn, precession(NOVAS_JD_J2000, out, jd_tdb,  out), 0);
  return 0;
}

/**
 * Transforms a rectangular equatorial (x, y, z) vector from Mean of Date (MOD) reference
 * frame at the given epoch to the Geocentric Celestial Reference System(GCRS)
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TDB) based Julian date that defines the
 *                  input epoch. Typically it does not require much precision, and Julian dates
 *                  in other time measures will be unlikely to affect the result
 * @param in        Input (x, y, z)  position or velocity 3-vector in the Mean equinox of Date
 *                  coordinate frame.
 * @param[out] out  Output GCRS position or velocity vector. It can be the same vector as the
 *                  input.
 * @return          0 if successful, or -1 if either of the vector arguments is NULL.
 *
 * @sa gcrs_to_mod()
 * @sa tod_to_gcrs()
 *
 * @since 1.2
 * @author Attila Kovacs
 */
int mod_to_gcrs(double jd_tdb, const double *in, double *out) {
  static const char *fn = "tod_to_gcrs [internal]";
  prop_error(fn, precession(jd_tdb, in, NOVAS_JD_J2000, out), 0);
  prop_error(fn, frame_tie(out, J2000_TO_ICRS, out), 0);
  return 0;
}

/**
 * Transforms a rectangular equatorial (x, y, z) vector from the Geocentric Celestial
 * Reference System (GCRS) to the True of Date (TOD) reference frame at the given epoch
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TT) based Julian date that defines the
 *                  output epoch. Typically it does not require much precision, and Julian
 *                  dates in other time measures will be unlikely to affect the result
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in        GCRS Input (x, y, z) position or velocity vector
 * @param[out] out  Output position or velocity 3-vector in the True equinox of Date coordinate
 *                  frame. It can be the same vector as the input.
 * @return          0 if successful, or -1 if either of the vector arguments is NULL.
 *
 * @sa gcrs_to_cirs()
 * @sa tod_to_gcrs()
 * @sa j2000_to_tod()
 *
 * @since 1.2
 * @author Attila Kovacs
 */
int gcrs_to_tod(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out) {
  static const char *fn = "gcrs_to_tod [internal]";
  prop_error(fn, frame_tie(in, ICRS_TO_J2000, out), 0);
  prop_error(fn, j2000_to_tod(jd_tdb, accuracy, out, out), 0);
  return 0;
}

/**
 * Transforms a rectangular equatorial (x, y, z) vector from True of Date (TOD) reference
 * frame at the given epoch to the Geocentric Celestial Reference System(GCRS)
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TDB) based Julian date that defines the
 *                  input epoch. Typically it does not require much precision, and Julian dates
 *                  in other time measures will be unlikely to affect the result
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in        Input (x, y, z)  position or velocity 3-vector in the True equinox of Date
 *                  coordinate frame.
 * @param[out] out  Output GCRS position or velocity vector. It can be the same vector as the
 *                  input.
 * @return          0 if successful, or -1 if either of the vector arguments is NULL.
 *
 * @sa j2000_to_tod()
 * @sa tod_to_cirs()
 * @sa tod_to_j2000()
 * @sa tod_to_itrs()
 *
 * @since 1.2
 * @author Attila Kovacs
 */
int tod_to_gcrs(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out) {
  static const char *fn = "tod_to_gcrs [internal]";
  prop_error(fn, tod_to_j2000(jd_tdb, accuracy, in, out), 0);
  prop_error(fn, frame_tie(out, J2000_TO_ICRS, out), 0);
  return 0;
}

/**
 * Transforms a rectangular equatorial (x, y, z) vector from the Geocentric Celestial Reference
 * System (GCRS) to the Celestial Intermediate Reference System (CIRS) frame at the given epoch
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TDB) based Julian date that defines the
 *                  output epoch. Typically it does not require much precision, and Julian dates
 *                  in other time measures will be unlikely to affect the result
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in        GCRS Input (x, y, z) position or velocity vector
 * @param[out] out  Output position or velocity 3-vector in the True equinox of Date coordinate
 *                  frame. It can be the same vector as the input.
 * @return          0 if successful, or -1 if either of the vector arguments is NULL or the
 *                  accuracy is invalid, or an error from cio_location(), or
 *                  else 10 + the error from cio_basis().
 *
 * @sa gcrs_to_j2000()
 * @sa cirs_to_gcrs()
 *
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int gcrs_to_cirs(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out) {
  static const char *fn = "gcrs_to_cirs";
  double r_cio, v[3], x[3], y[3], z[3];
  short sys;

  if(!in || !out)
    return novas_error(-1, EINVAL, fn, "NULL input or output 3-vector: in=%p, out=%p", in, out);

  memcpy(v, in, sizeof(v));

  // Obtain the basis vectors, in the GCRS, of the celestial intermediate
  // system.
  prop_error(fn, cio_location(jd_tdb, accuracy, &r_cio, &sys), 0);
  prop_error(fn, cio_basis(jd_tdb, r_cio, sys, accuracy, x, y, z), 10);

  // Transform position vector to celestial intermediate system.
  out[0] = novas_vdot(x, v);
  out[1] = novas_vdot(y, v);
  out[2] = novas_vdot(z, v);

  return 0;
}

/**
 * Transforms a rectangular equatorial (x, y, z) vector from the Celestial Intermediate
 * Reference System (CIRS) frame at the given epoch to the Geocentric Celestial Reference
 * System (GCRS).
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TDB) based Julian date that defines
 *                  the output epoch. Typically it does not require much precision, and
 *                  Julian dates in other time measures will be unlikely to affect the
 *                  result
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in        CIRS Input (x, y, z) position or velocity vector
 * @param[out] out  Output position or velocity 3-vector in the GCRS coordinate frame.
 *                  It can be the same vector as the input.
 * @return          0 if successful, or -1 if either of the vector arguments is NULL
 *                  or the accuracy is invalid, or an error from cio_location(), or else
 *                  10 + the error from cio_basis().
 *
 * @sa tod_to_gcrs()
 * @sa gcrs_to_cirs()
 * @sa cirs_to_itrs()
 * @sa cirs_to_tod()
 *
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int cirs_to_gcrs(double jd_tdb, enum novas_accuracy accuracy, const double *in, double *out) {
  static const char *fn = "cirs_to_gcrs";
  double r_cio, vx[3], vy[3], vz[4], x, y, z;
  short sys;
  int i;

  if(!in || !out)
    return novas_error(-1, EINVAL, fn, "NULL input or output 3-vector: in=%p, out=%p", in, out);

  // Obtain the basis vectors, in the GCRS, of the celestial intermediate
  // system.
  prop_error(fn, cio_location(jd_tdb, accuracy, &r_cio, &sys), 0);
  prop_error(fn, cio_basis(jd_tdb, r_cio, sys, accuracy, vx, vy, vz), 10);

  x = in[0];
  y = in[1];
  z = in[2];

  // Transform position vector to GCRS system.
  for(i = 3; --i >= 0;) {
    out[i] = x * vx[i] + y * vy[i] + z * vz[i];
  }

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
 * Precesses equatorial rectangular coordinates from one epoch to another. Unlike the original
 * NOVAS routine, this routine works for any pairing of the time arguments.
 *
 * This function calculates precession for the old (pre IAU 2000) methodology. Its main use
 * for NOVAS users is to allow converting older catalog coordinates e.g. to J2000 coordinates,
 * which then can be converted to the now standard ICRS system via frame_tie().
 *
 * NOTE:
 * <ol>
 * <li>Unlike the original NOVAS C 3.1 version, this one does not require that one
 *     of the time arguments must be J2000. You can precess from any date to
 *     any other date, and the intermediate epoch of J2000 will be handled internally
 *     as needed.</li>
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Explanatory Supplement To The Astronomical Almanac, pp. 103-104.</li>
 * <li>Capitaine, N. et al. (2003), Astronomy And Astrophysics 412, pp. 567-586.</li>
 * <li>Hilton, J. L. et al. (2006), IAU WG report, Celest. Mech., 94, pp. 351-367.</li>
 * </ol>
 *
 * @param jd_tdb_in   [day] Barycentric Dynamic Time (TDB) based Julian date of the input
 *                    epoch
 * @param in          Position 3-vector, geocentric equatorial rectangular coordinates,
 *                    referred to mean dynamical equator and equinox of the initial epoch.
 * @param jd_tdb_out  [day] Barycentric Dynamic Time (TDB) based Julian date of the output
 *                    epoch
 * @param[out] out    Position 3-vector, geocentric equatorial rectangular coordinates,
 *                    referred to mean dynamical equator and equinox of the final epoch.
 *                    It can be the same vector as the input.
 * @return            0 if successful, or -1 if either of the position vectors is NULL.
 *
 * @sa nutation()
 * @sa frame_tie()
 * @sa novas_epoch()
 * @sa tt2tdb()
 * @sa cio_basis()
 * @sa NOVAS_TOD
 * @sa NOVAS_JD_J2000
 * @sa NOVAS_JD_B1950
 * @sa NOVAS_JD_B1900
 */
short precession(double jd_tdb_in, const double *in, double jd_tdb_out, double *out) {
  static THREAD_LOCAL double t_last = NAN;
  static THREAD_LOCAL double xx, yx, zx, xy, yy, zy, xz, yz, zz;
  double t;

  if(!in || !out)
    return novas_error(-1, EINVAL, "precession", "NULL input or output 3-vector: in=%p, out=%p", in, out);

  if(jd_tdb_in == jd_tdb_out) {
    if(out != in)
      memcpy(out, in, XYZ_VECTOR_SIZE);
    return 0;
  }

  // Check to be sure that either 'jd_tdb1' or 'jd_tdb2' is equal to JD_J2000.
  if(!novas_time_equals(jd_tdb_in, JD_J2000) && !novas_time_equals(jd_tdb_out, JD_J2000)) {
    // Do the precession in two steps...
    precession(jd_tdb_in, in, JD_J2000, out);
    precession(JD_J2000, out, jd_tdb_out, out);
    return 0;
  }

  // 't' is time in TDB centuries between the two epochs.
  t = (jd_tdb_out - jd_tdb_in) / JULIAN_CENTURY_DAYS;
  if(jd_tdb_out == JD_J2000)
    t = -t;

  if(!novas_time_equals(t, t_last)) {
    double psia, omegaa, chia, sa, ca, sb, cb, sc, cc, sd, cd, t1, t2;
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
    t1 = cd * sb + sd * cc * cb;
    t2 = sd * sc;
    xx = cd * cb - sb * sd * cc;
    yx = ca * t1 - sa * t2;
    zx = sa * t1 + ca * t2;

    t1 = cd * cc * cb - sd * sb;
    t2 = cd * sc;
    xy = -sd * cb - sb * cd * cc;
    yy = ca * t1 - sa * t2;
    zy = sa * t1 + ca * t2;

    xz = sb * sc;
    yz = -sc * cb * ca - sa * cc;
    zz = -sc * cb * sa + cc * ca;

    t_last = t;
  }

  if(jd_tdb_out == JD_J2000) {
    const double x = in[0], y = in[1], z = in[2];
    // Perform rotation from epoch to J2000.0.
    out[0] = xx * x + xy * y + xz * z;
    out[1] = yx * x + yy * y + yz * z;
    out[2] = zx * x + zy * y + zz * z;
  }
  else {
    const double x = in[0], y = in[1], z = in[2];
    // Perform rotation from J2000.0 to epoch.
    out[0] = xx * x + yx * y + zx * z;
    out[1] = xy * x + yy * y + zy * z;
    out[2] = xz * x + yz * y + zz * z;
  }

  return 0;
}

/**
 * Nutates equatorial rectangular coordinates from mean equator and equinox of epoch to true
 * equator and equinox of epoch. Inverse transformation may be applied by setting flag
 * 'direction'.
 *
 * This is the old (pre IAU 2006) method of nutation calculation. If you follow the now
 * standard IAU 2000/2006 methodology you will want to use nutation_angles() instead.
 *
 * REFERENCES:
 * <ol>
 * <li>Explanatory Supplement To The Astronomical Almanac, pp. 114-115.</li>
 * </ol>
 *
 *
 * @param jd_tdb      [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param direction   NUTATE_MEAN_TO_TRUE (0) or NUTATE_TRUE_TO_MEAN (-1; or non-zero)
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param in          Position 3-vector, geocentric equatorial rectangular coordinates,
 *                    referred to mean equator and equinox of epoch.
 * @param[out] out    Position vector, geocentric equatorial rectangular coordinates,
 *                    referred to true equator and equinox of epoch. It can be the same
 *                    as the input position.
 *
 * @return            0 if successful, or -1 if one of the vector arguments is NULL.
 *
 * @sa nutation_angles()
 * @sa tt2tdb()
 * @sa NOVAS_TOD
 */
int nutation(double jd_tdb, enum novas_nutation_direction direction, enum novas_accuracy accuracy, const double *in, double *out) {
  double oblm, oblt, psi;
  double cm, sm, ct, st, cp, sp;
  double xx, yx, zx, xy, yy, zy, xz, yz, zz;

  if(!in || !out)
    return novas_error(-1, EINVAL, "nutation", "NULL input or output 3-vector: in=%p, out=%p", in, out);

  // Call 'e_tilt' to get the obliquity and nutation angles.
  prop_error("nutation", e_tilt(jd_tdb, accuracy, &oblm, &oblt, NULL, &psi, NULL), 0);

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
    const double x = in[0], y = in[1], z = in[2];
    // Perform rotation.
    out[0] = xx * x + yx * y + zx * z;
    out[1] = xy * x + yy * y + zy * z;
    out[2] = xz * x + yz * y + zz * z;
  }
  else {
    const double x = in[0], y = in[1], z = in[2];
    // Perform inverse rotation.
    out[0] = xx * x + xy * y + xz * z;
    out[1] = yx * x + yy * y + yz * z;
    out[2] = zx * x + zy * y + zz * z;
  }

  return 0;
}

/**
 * Returns the values for nutation in longitude and nutation in obliquity for a given TDB
 * Julian date.  The nutation model selected depends upon the input value of 'accuracy'. See
 * notes below for important details.
 *
 * This function selects the nutation model depending first upon the input value of 'accuracy'.
 * If 'accuracy' is NOVAS_FULL_ACCURACY (0), the IAU 2000A nutation model is used. Otherwise
 * the model set by set_nutation_lp_provider() is used, or else the default nu2000k().
 *
 * See the prologs of the nutation functions in file 'nutation.c' for details concerning the
 * models.
 *
 * REFERENCES:
 * <ol>
 * <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 * </ol>
 *
 * @param t           [cy] TDB time in Julian centuries since J2000.0
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param[out] dpsi   [arcsec] Nutation in longitude in arcseconds.
 * @param[out] deps   [arcsec] Nutation in obliquity in arcseconds.
 *
 * @return            0 if successful, or -1 if the output pointer arguments are NULL
 *
 * @sa set_nutation_lp_provider()
 * @sa nutation()
 * @sa iau2000b()
 * @sa nu2000k()
 * @sa cio_basis()
 * @sa NOVAS_CIRS
 * @sa NOVAS_JD_J2000
 */
int nutation_angles(double t, enum novas_accuracy accuracy, double *restrict dpsi, double *restrict deps) {
  if(!dpsi || !deps) {
    if(dpsi)
      *dpsi = NAN;
    if(deps)
      *deps = NAN;

    return novas_error(-1, EINVAL, "nutation_angles", "NULL output pointer: dspi=%p, deps=%p", dpsi, deps);
  }

  if(accuracy == NOVAS_FULL_ACCURACY) {
    // High accuracy mode -- use IAU 2000A.
    iau2000a(JD_J2000, t * JULIAN_CENTURY_DAYS, dpsi, deps);
  }
  else {
    // Low accuracy mode -- model depends upon value of 'low_acc_choice'.
    novas_nutation_provider nutate_call = get_nutation_lp_provider();
    nutate_call(JD_J2000, t * JULIAN_CENTURY_DAYS, dpsi, deps);
  }

  // Convert output to arcseconds.
  *dpsi /= ARCSEC;
  *deps /= ARCSEC;

  return 0;
}
