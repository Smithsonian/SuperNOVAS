/**
 * @file
 *
 * @author G. Kaplan and A. Kovacs
 *
 *  SuperNOVAS prototypes for the IAU2000 nutation series calculations, with varying trade-offs
 *  between computational cost and precision. It provides support for both the IAU 2000A and
 *  IAU 2000B series as well as a NOVAS-specific truncated low-precision version we call NU2000K.
 *
 *  Based on the NOVAS C Edition, Version 3.1,  U. S. Naval Observatory
 *  Astronomical Applications Dept.
 *  Washington, DC
 *  <a href="http://www.usno.navy.mil/USNO/astronomical-applications">http://www.usno.navy.mil/USNO/astronomical-applications</a>
 */

#ifndef _NUTATION_
#define _NUTATION_

/**
 * Function type definition for the IAU 2000 nutation series calculation.
 *
 * @param jd_tt_high  [day] High-order part of the Terrestrial Time (TT) based Julian date. Typically
 *                    it may be the integer part of a split date for the highest precision, or the
 *                    full date for normal (reduced) precision.
 * @param jd_tt_low   [day] Low-order part of the Terrestrial Time (TT) based Julian date. Typically
 *                    it may be the fractional part of a split date for the highest precision, or 0.0
 *                    for normal (reduced) precision.
 * @param[out] dpsi   [rad] &delta;&psi; Nutation (luni-solar + planetary) in longitude, in radians.
 * @param[out] deps   [rad] &delta;&epsilon; Nutation (luni-solar + planetary) in obliquity, in radians.
 * @return            0 if successful, or else -1 (errno should be set to indicate the type of error).
 *
 * @sa nutation_angles()
 * @sa iau2000a()
 * @sa iau2000b()
 * @sa iau2000k()
 */
typedef int (*novas_nutation_provider)(double jd_tt_high, double jd_tt_low, double *dpsi, double *deps);


int iau2000a(double jd_tt_high, double jd_tt_low, double *dpsi, double *deps);


int iau2000b(double jd_tt_high, double jd_tt_low, double *dpsi, double *deps);


int nu2000k(double jd_tt_high, double jd_tt_low, double *dpsi, double *deps);



#endif
