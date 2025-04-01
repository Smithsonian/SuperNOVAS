/**
 * @file
 *
 * @date Created  on Jun 24, 2024
 * @author Attila Kovacs
 * @since 1.1
 *
 *   A set of SuperNOVAS routines to make handling of astronomical timescales and conversions
 *   among them easier.
 *
 * @sa frames.c
 */

#define _GNU_SOURCE                 ///< for strcasecmp()

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>              // strcasecmp() / strncasecmp()
#include <errno.h>
#include <math.h>
#include <ctype.h>                // isspace()
#include "novas.h"

/// \cond PRIVATE
#define DTA         (32.184 / DAY)        ///< [day] TT - TAI time difference
#define GPS2TAI     (19.0 / DAY)          ///< [day] TAI - GPS time difference

#define IDAY        86400                 ///< [s] 1 day

#define IJD_J2000   2451545

#define UNIX_SECONDS_0UTC_1JAN2000  946684800    ///< [s] UNIX time at J2000.0
#define UNIX_J2000                  (UNIX_SECONDS_0UTC_1JAN2000 + (IDAY / 2))

// IAU 2006 Resolution B3
#define TC_T0      2443144.5003725       ///< 1977 January 1, 0h 0m 0s TAI
#define TC_LB      1.550519768e-8        ///< Relative rate at which Barycentric coordinate time progresses fastern than time on Earth.
#define TC_LG      6.969291e-10          ///< Relative rate at which Geocentric coordinate time progresses fastern than time on Earth.
#define TC_TDB0    (6.55e-5 / DAY)       ///< TDB time offset at TC_T0

#define E9          1000000000           ///< 10<sup>9</sup> as integer

#define DATE_SEP_CHARS  "-_./ \t\r\n\f"             ///< characters that may separate date components
#define DATE_SEP        "%*[" DATE_SEP_CHARS "]"    ///< Parse pattern for ignored date separators

/// Parse pattern for month specification, either as a 1-2 digit integer or as a month name or abbreviation.
#define MONTH_SPEC      "%9[^" DATE_SEP_CHARS "]"

#define DAY_MILLIS    86400000L         ///< milliseconds in a day
#define HOUR_MILLIS   3600000L          ///< milliseconds in an hour
#define MIN_MILLIS    60000L            ///< milliseconds in a minute

/// \endcond

#if __Lynx__ && __powerpc__
// strcasecmp() / strncasecmp() are not defined on PowerPC / LynxOS 3.1
int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);
#endif


/**
 * Computes the Terrestrial Time (TT) or Terrestrial Dynamical Time (TDT) Julian date
 * corresponding to a Barycentric Dynamical Time (TDB) Julian date.
 *
 * Expression used in this function is a truncated form of a longer and more precise
 * series given in the first reference.  The result is good to about 10 microseconds.
 *
 * @deprecated Use the less computationally intensive an more accurate tt2tdb()
 *            routine instead.
 *
 * REFERENCES:
 * <ol>
 * <li>Fairhead, L. & Bretagnon, P. (1990) Astron. & Astrophys. 229, 240.</li>
 * <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 * <li><a href="https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/FORTRAN/req/time.html#The%20Relationship%20between%20TT%20and%20TDB">
 * https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/FORTRAN/req/time.html</a></li>
 * <li><a href="https://gssc.esa.int/navipedia/index.php/Transformations_between_Time_Systems">
 * https://gssc.esa.int/navipedia/index.php/Transformations_between_Time_Systems</a></li>
 * </ol>
 *
 * @param jd_tdb         [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param[out] jd_tt     [day] Terrestrial Time (TT) based Julian date. (It may be NULL
 *                       if not required)
 * @param[out] secdiff   [s] Difference 'tdb_jd'-'tt_jd', in seconds. (It may be NULL if
 *                       not required)
 * @return               0 if successful, or -1 if the tt_jd pointer argument is NULL.
 *
 * @sa tt2tdb()
 */
int tdb2tt(double jd_tdb, double *restrict jd_tt, double *restrict secdiff) {
  const double t = (jd_tdb - JD_J2000) / JULIAN_CENTURY_DAYS;

  // Expression given in USNO Circular 179, eq. 2.6.
  const double d = 0.001657 * sin(628.3076 * t + 6.2401) + 0.000022 * sin(575.3385 * t + 4.2970) + 0.000014 * sin(1256.6152 * t + 6.1969)
  + 0.000005 * sin(606.9777 * t + 4.0212) + 0.000005 * sin(52.9691 * t + 0.4444) + 0.000002 * sin(21.3299 * t + 5.5431)
  + 0.000010 * t * sin(628.3076 * t + 4.2490);

  // The simpler formula with a precision of ~30 us.
  //  const double t = (jd_tt - JD_J2000) / JULIAN_CENTURY_DAYS;
  //  const double g = 6.239996 + 630.0221385924 * t;
  //  const double d = 0.001657 * sin(g + 0.01671 * sin(g));

  if(jd_tt)
    *jd_tt = jd_tdb - d / DAY;
  if(secdiff)
    *secdiff = d;

  return 0;
}

/**
 * Returns the TDB - TT time difference in seconds for a given TT date.
 *
 * Note, as of version 1.1, it uses the same calculation as the more precise original tdb2tt(). It thus has an acuracy of
 * about 10 &mu;s vs around 30 &mu;s with the simpler formula from the references below.
 *
 *
 * REFERENCES
 * <ol>
 * <li>Fairhead, L. & Bretagnon, P. (1990) Astron. & Astrophys. 229, 240.</li>
 * <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 * <li><a href="https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/FORTRAN/req/time.html#The%20Relationship%20between%20TT%20and%20TDB">
 * https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/FORTRAN/req/time.html</a></li>
 * <li><a href="https://gssc.esa.int/navipedia/index.php/Transformations_between_Time_Systems">
 * https://gssc.esa.int/navipedia/index.php/Transformations_between_Time_Systems</a></li>
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
  double dt;

  tdb2tt(jd_tt, NULL, &dt);
  return dt;
}

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
 * Sets an astronomical time to the fractional Julian Date value, defined in the specified
 * timescale. The time set this way is accurate to a few &mu;s (microseconds) due to the inherent
 * precision of the double-precision argument. For higher precision applications you may use
 * `novas_set_split_time()` instead, which has an inherent accuracy at the picosecond level.
 *
 * @param timescale     The astronomical time scale in which the Julian Date is given
 * @param jd            [day] Julian day value in the specified timescale
 * @param leap          [s] Leap seconds, e.g. as published by IERS Bulletin C.
 * @param dut1          [s] UT1-UTC time difference, e.g. as published in IERS Bulletin A.
 * @param[out] time     Pointer to the data structure that uniquely defines the astronomical time
 *                      for all applications.
 * @return              0 if successful, or else -1 if there was an error (errno will be set to
 *                      indicate the type of error).
 *
 * @sa novas_set_split_time()
 * @sa novas_set_unix_time()
 * @sa novas_get_time()
 * @sa novas_timescale_for_string()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_set_time(enum novas_timescale timescale, double jd, int leap, double dut1, novas_timespec *restrict time) {
  prop_error("novas_set_time", novas_set_split_time(timescale, 0, jd, leap, dut1, time), 0);
  return 0;
}

/**
 * Sets an astronomical time to the split Julian Date value, defined in the specified timescale.
 * The split into the integer and fractional parts can be done in any convenient way. The highest
 * precision is reached if the fractional part is &le; 1 day. In that case, the time may be
 * specified to picosecond accuracy, if needed.
 *
 * The accuracy of Barycentric Time measures (TDB and TCB) relative to other time measures is
 * limited by the precision of `tbd2tt()` implementation, to around 10 &mu;s.
 *
 *
 * REFERENCES:
 * <ol>
 * <li>IAU 1991, RECOMMENDATION III. XXIst General Assembly of the
 * International Astronomical Union. Retrieved 6 June 2019.</li>
 * <li>IAU 2006 resolution 3, see Recommendation and footnotes, note 3.</li>
 * <li>Fairhead, L. & Bretagnon, P. (1990) Astron. & Astrophys. 229, 240.</li>
 * <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 * <li><a href="https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/FORTRAN/req/time.html#The%20Relationship%20between%20TT%20and%20TDB">
 * https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/FORTRAN/req/time.html</a></li>
 * <li><a href="https://gssc.esa.int/navipedia/index.php/Transformations_between_Time_Systems">
 * https://gssc.esa.int/navipedia/index.php/Transformations_between_Time_Systems</a></li>
 * </ol>
 *
 * @param timescale     The astronomical time scale in which the Julian Date is given
 * @param ijd           [day] integer part of the Julian day in the specified timescale
 * @param fjd           [day] fractional part Julian day value in the specified timescale
 * @param leap          [s] Leap seconds, e.g. as published by IERS Bulletin C.
 * @param dut1          [s] UT1-UTC time difference, e.g. as published in IERS Bulletin A.
 * @param[out] time     Pointer to the data structure that uniquely defines the astronomical time
 *                      for all applications.
 * @return              0 if successful, or else -1 if there was an error (errno will be set to
 *                      indicate the type of error).
 *
 * @sa novas_set_time()
 * @sa novas_set_unix_time()
 * @sa novas_get_split_time()
 * @sa novas_timescale_for_string()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_set_split_time(enum novas_timescale timescale, long ijd, double fjd, int leap, double dut1,
        novas_timespec *restrict time) {
  static const char *fn = "novas_set_split_time";

  long ifjd;

  if(!time)
    return novas_error(-1, EINVAL, "novas_set_time", "NULL output time structure");

  time->tt2tdb = NAN;
  time->dut1 = dut1;
  time->ut1_to_tt = leap - dut1 + DTA * DAY;    // TODO check!

  switch(timescale) {
    case NOVAS_TT:
      break;
    case NOVAS_TCB:
      time->tt2tdb = tt2tdb(ijd + fjd);
      fjd -= time->tt2tdb / DAY - TC_TDB0;
      fjd -= TC_LB * ((ijd - TC_T0) + fjd);
      break;
    case NOVAS_TCG:
      fjd -= TC_LG * ((ijd - TC_T0) + fjd);
      break;
    case NOVAS_TDB: {
      time->tt2tdb = tt2tdb(ijd + fjd);
      fjd -= time->tt2tdb / DAY;
      break;
    }
    case NOVAS_TAI:
      fjd += DTA;
      break;
    case NOVAS_GPS:
      fjd += (DTA + GPS2TAI);
      break;
    case NOVAS_UTC:
      fjd += (time->ut1_to_tt + time->dut1) / DAY;
      break;
    case NOVAS_UT1:
      fjd += time->ut1_to_tt / DAY;
      break;
    default:
      return novas_error(-1, EINVAL, fn, "Invalid timescale: %d", timescale);
  }

  ifjd = (long) floor(fjd);

  time->ijd_tt = ijd + ifjd;
  time->fjd_tt = fjd - ifjd;

  if(isnan(time->tt2tdb))
    time->tt2tdb = tt2tdb(time->ijd_tt + time->fjd_tt);

  return 0;
}

/**
 * Increments the astrometric time by a given amount.
 *
 * @param time        Original time specification
 * @param seconds     [s] Seconds to add to the original
 * @param[out] out    New incremented time specification. It may be the same as the input.
 * @return            0 if successful, or else -1 if either the input or the output is NULL
 *                    (errno will be set to EINVAL).
 *
 * @sa novas_set_time()
 * @sa novas_diff_time()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_offset_time(const novas_timespec *time, double seconds, novas_timespec *out) {
  long dd;

  if(!time || !out)
    return novas_error(-1, EINVAL, "novas_offset_time", "NULL parameter: time=%p, out=%p", time, out);

  if(out != time)
    *out = *time;

  out->fjd_tt += seconds / DAY;
  dd = (long) floor(out->fjd_tt);
  if(dd) {
    out->fjd_tt -= dd;
    out->ijd_tt += dd;
  }

  return 0;
}

/**
 * Returns the fractional Julian date of an astronomical time in the specified timescale. The
 * returned time is accurate to a few &mu;s (microsecond) due to the inherent precision of the
 * double-precision result. For higher precision applications you may use `novas_get_split_time()`
 * instead, which has an inherent accuracy at the picosecond level.
 *
 * @param time        Pointer to the astronomical time specification data structure.
 * @param timescale   The astronomical time scale in which the returned Julian Date is to be
 *                    provided
 * @return            [day] The Julian date in the requested timescale.
 *
 * @sa novas_set_time()
 * @sa novas_get_split_time()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
double novas_get_time(const novas_timespec *restrict time, enum novas_timescale timescale) {
  long ijd;
  double fjd = novas_get_split_time(time, timescale, &ijd);
  if(isnan(fjd))
    return novas_trace_nan("novas_get_time");
  return ijd + fjd;
}

/**
 * Returns the fractional Julian date of an astronomical time in the specified timescale, as an
 * integer and fractional part. The two-component split of the time allows for absolute precisions
 * at the picosecond level, as opposed to `novas_set_time()`, whose precision is limited to a
 * few microseconds typically.
 *
 * The accuracy of Barycentric Time measures (TDB and TCB) relative to other time measures is
 * limited by the precision of the `tbd2tt()` implemenation, to around 10 &mu;s.
 *
 * REFERENCES:
 * <ol>
 * <li>IAU 1991, RECOMMENDATION III. XXIst General Assembly of the
 * International Astronomical Union. Retrieved 6 June 2019.</li>
 * <li>IAU 2006 resolution 3, see Recommendation and footnotes, note 3.</li>
 * <li>Fairhead, L. & Bretagnon, P. (1990) Astron. & Astrophys. 229, 240.</li>
 * <li>Kaplan, G. (2005), US Naval Observatory Circular 179.</li>
 * <li><a href="https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/FORTRAN/req/time.html#The%20Relationship%20between%20TT%20and%20TDB">
 * https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/FORTRAN/req/time.html</a></li>
 * <li><a href="https://gssc.esa.int/navipedia/index.php/Transformations_between_Time_Systems">
 * https://gssc.esa.int/navipedia/index.php/Transformations_between_Time_Systems</a></li>
 * </ol>
 *
 * @param time        Pointer to the astronomical time specification data structure.
 * @param timescale   The astronomical time scale in which the returned Julian Date is to be
 *                    provided
 * @param[out] ijd    [day] The integer part of the Julian date in the requested timescale. It may
 *                    be NULL if not required.
 * @return            [day] The fractional part of the Julian date in the requested timescale or
 *                    NAN is the time argument is NULL (ijd will be set to -1 also).
 *
 * @sa novas_set_split_time()
 * @sa novas_get_time()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
double novas_get_split_time(const novas_timespec *restrict time, enum novas_timescale timescale, long *restrict ijd) {
  static const char *fn = "novas_get_split_time";
  double f;

  if(ijd) *ijd = -1;

  if(!time) {
    novas_set_errno(EINVAL, fn, "NULL input time specification");
    return NAN;
  }

  if(ijd)
    *ijd = time->ijd_tt;

  f = time->fjd_tt;

  switch(timescale) {
    case NOVAS_TT:
      break;
    case NOVAS_TDB:
      f += time->tt2tdb / DAY;
      break;
    case NOVAS_TCB:
      f += time->tt2tdb / DAY - TC_TDB0;
      f += TC_LB * ((time->ijd_tt - TC_T0) + f);
      break;
    case NOVAS_TCG:
      f += TC_LG * ((time->ijd_tt - TC_T0) + f);
      break;
    case NOVAS_TAI:
      f -= DTA;
      break;
    case NOVAS_GPS:
      f -= (DTA + GPS2TAI);
      break;
    case NOVAS_UTC:
      f -= (time->ut1_to_tt + time->dut1) / DAY;
      break;
    case NOVAS_UT1:
      f -= time->ut1_to_tt / DAY;
      break;
    default:
      novas_set_errno(EINVAL, fn, "Invalid timescale: %d", timescale);
      return NAN;
  }

  if(f < 0.0) {
    f += 1.0;
    if(ijd)
      (*ijd)--;
  }
  else if(f > 1.0) {
    f -= 1.0;
    if(ijd)
      (*ijd)++;
  }

  return f;
}

/**
 * Returns the Terrestrial Time (TT) based time difference (t1 - t2) in days between two
 * astronomical time specifications.
 *
 * @param t1    First time
 * @param t2    Second time
 * @return      [s] Precise time difference (t1-t2), or NAN if one of the inputs was NULL (errno
 *              will be set to EINVAL)
 *
 * @sa novas_set_time()
 * @sa novas_offset_time()
 * @sa novas_diff_tcb()
 * @sa novas_diff_tcg()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
double novas_diff_time(const novas_timespec *t1, const novas_timespec *t2) {
  if(!t1 || !t2) {
    novas_set_errno(EINVAL, "novas_diff_time", "NULL parameter: t1=%p, t2=%p", t1, t2);
    return NAN;
  }

  return ((t1->ijd_tt - t2->ijd_tt) + (t1->fjd_tt - t2->fjd_tt)) * DAY;
}

/**
 * Returns the Barycentric Coordinate Time (TCB) based time difference (t1 - t2) in days between
 * two astronomical time specifications. TCB progresses slightly faster than time on Earth, at a
 * rate about 1.6&times10<sup>-8</sup> higher, due to the lack of gravitational time dilation by
 * the Earth or Sun.
 *
 * @param t1    First time
 * @param t2    Second time
 * @return      [s] Precise TCB time difference (t1-t2), or NAN if one of the inputs was
 *              NULL (errno will be set to EINVAL)
 *
 * @sa novas_diff_tcg()
 * @sa novas_diff_time()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
double novas_diff_tcb(const novas_timespec *t1, const novas_timespec *t2) {
  double dt = novas_diff_time(t1, t2) * (1.0 + TC_LB);
  if(isnan(dt))
    return novas_trace_nan("novas_diff_tcb");
  return dt;
}

/**
 * Returns the Geocentric Coordinate Time (TCG) based time difference (t1 - t2) in days between
 * two astronomical time specifications. TCG progresses slightly faster than time on Earth, at a
 * rate about 7&times10<sup>-10</sup> higher, due to the lack of gravitational time dilation by
 * Earth. TCG is an appropriate time measure for a spacecraft that is in the proximity of the
 * orbit of Earth, but far enough from Earth such that the relativistic effects of Earth's gravity
 * can be ignored.
 *
 * @param t1    First time
 * @param t2    Second time
 * @return      [s] Precise TCG time difference (t1-t2), or NAN if one of the inputs was
 *              NULL (errno will be set to EINVAL)
 *
 * @sa novas_diff_tcb()
 * @sa novas_diff_time()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
double novas_diff_tcg(const novas_timespec *t1, const novas_timespec *t2) {
  double dt = novas_diff_time(t1, t2) * (1.0 + TC_LG);
  if(isnan(dt))
    return novas_trace_nan("novas_diff_tcg");
  return dt;
}

/**
 * Sets an astronomical time to a UNIX time value. UNIX time is defined as UTC seconds measured
 * since 0 UTC, 1 Jan 1970 (the start of the UNIX era). Specifying time this way supports
 * precisions to the nanoseconds level by construct. Specifying UNIX time in split seconds and
 * nanoseconds is a common way CLIB handles precision time, e.g. with `struct timespec` and
 * functions like `clock_gettime()` (see `time.h`).
 *
 * @param unix_time   [s] UNIX time (UTC) seconds
 * @param nanos       [ns] UTC sub-second component
 * @param leap        [s] Leap seconds, e.g. as published by IERS Bulletin C.
 * @param dut1        [s] UT1-UTC time difference, e.g. as published in IERS Bulletin A.
 * @param[out] time   Pointer to the data structure that uniquely defines the astronomical time
 *                    for all applications.
 * @return            0 if successful, or else -1 if there was an error (errno will be set to
 *                    indicate the type of error).
 *
 * @sa novas_set_time()
 * @sa novas_get_unix_time()
 * @sa clock_gettime()
 * @sa struct timespec
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_set_unix_time(time_t unix_time, long nanos, int leap, double dut1, novas_timespec *restrict time) {
  long jd, sojd;

  // UTC based integer JD
  unix_time -= UNIX_J2000;
  jd = IJD_J2000 + unix_time / IDAY;

  // seconds of JD date
  sojd = unix_time % IDAY;
  if(sojd < 0) {
    sojd += IDAY;
    jd--;
  }

  prop_error("novas_set_unix_time", novas_set_split_time(NOVAS_UTC, jd, (sojd + 1e-9 * nanos) / DAY, leap, dut1, time), 0);
  return 0;
}

/**
 * Returns the UNIX time for an astronomical time instant.
 *
 * @param time        Pointer to the astronomical time specification data structure.
 * @param[out] nanos  [ns] UTC sub-second component. It may be NULL if not required.
 * @return            [s] The integer UNIX time, or -1 if the input time is NULL.
 *
 * @sa novas_set_unix_time()
 * @sa novas_get_time()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
time_t novas_get_unix_time(const novas_timespec *restrict time, long *restrict nanos) {
  long ijd, isod;
  double sod;
  time_t seconds;

  sod = novas_get_split_time(time, NOVAS_UTC, &ijd) * DAY;
  if(isnan(sod)) {
    static const char *fn = "novas_get_unix_time";
    if(nanos) *nanos = novas_trace_nan(fn);
    return novas_trace(fn, -1, 0);
  }

  isod = (long) floor(sod);
  seconds = UNIX_J2000 + (ijd - IJD_J2000) * IDAY + isod;

  if(nanos) {
    *nanos = (long) floor(1e9 * (sod - isod) + 0.5);
    if(*nanos == E9) {
      seconds++;
      *nanos = 0;
    }
  }

  return seconds;
}

static int skip_white(const char *str, char **tail) {
  char *next = (char *) str;

  // Consume trailing 'white' spaces / punctuation
  for(; *next; next++)
    if(!isspace(*next) && *next != '_')
      break;

  *tail = next;
  return 0;
}

static int parse_zone(const char *str, char **tail) {
  char *next = (char *) str;

  *tail = next;

  if(*str == '+' || *str == '-') {
    static const char *fn = "parse_zone";

    // zone in {+|-}HH[:[MM]] format...
    int H = 0, M = 0;
    int sign = *(next++) == '-' ? -1 : 1;
    int colon = 0;

    if(isdigit(next[0]) && isdigit(next[1])) {
      H = 10 * (next[0] - '0') + (next[1] - '0');
      if(H >= 24)
        return novas_error(-1, EINVAL, fn, "invalid zone hours: %d, expected [0-23]", H);
      next += 2;
    }
    else
      return novas_error(-1, EINVAL, fn, "invalid time zone specification");

    if(*next == ':') {
      next++;
      colon = 1;
    }

    if(isdigit(next[0])) {
      if(!isdigit(next[1]))
        return novas_error(-1, EINVAL, fn, "invalid time zone specification");

      M = 10 * (next[0] - '0') + (next[1] - '0');
      if(M >= 60)
        return novas_error(-1, EINVAL, fn, "invalid zone minutes: %d, expected [0-60]", M);
      next += 2;
    }
    else if(colon)
      next--;

    *tail = next;
    return sign * (H * 3600 + M * 60); // zone time to UTC...
  }

  if(*str == 'Z' || *str == 'z')
    *tail = (char *) str + 1;

  return 0;
}

/**
 * Parses a calndar date/time string, expressed in the specified type of calendar, into a Julian
 * day (JD). The date must be composed of a full year (e.g. 2025), a month (numerical or name or
 * 3-letter abbreviation, e.g. "01", "1", "January", or "Jan"), and a day (e.g. "08" or "8"). The
 * components may be separated by dash `-`, underscore `_`, dot `.`,  slash '/', or spaces/tabs,
 * or any combination thereof. The components will be parsed in the specified order.
 *
 * The date may be followed by a time specification in HMS format, separated from the date by the
 * letter `T` or `t`, or spaces, comma `,`, or semicolon `;` or underscore '_', or a combination
 * thereof. Finally, the time may be followed by the letter `Z`, or `z` (for UTC) or else by a
 * {+/-}HH[:[MM]] time zone specification.
 *
 * For example, for `format` NOVAS_YMD, all of the following strings may specify the date:
 *
 * <pre>
 *  2025-01-26
 *  2025 January 26
 *  2025_Jan_26
 *  2025-01-26T19:33:08Z
 *  2025.01.26T19:33:08
 *  2025 1 26 19h33m28.113
 *  2025/1/26 19:33:28+02
 *  2025-01-26T19:33:28-0600
 *  2025 Jan 26 19:33:28+05:30
 * </pre>
 *
 * are all valid dates that can be parsed.
 *
 * If your date format cannot be parsed with this function, you may parse it with your own
 * function into year, month, day, and decimal hour-of-day components, and use julian_date() with
 * those.
 *
 * NOTES:
 * <ol>
 *  <li>B.C. dates are indicated with years &lt;=0 according to the astronomical
 * and ISO 8601 convention, i.e., X B.C. as (1-X), so 45 B.C. as -44.</li>
 * </oL>
 *
 * @param calendar    The type of calendar to use: NOVAS_ASTRONOMICAL_CALENDAR,
 *                    NOVAS_GREGORIAN_CALENDAR, or NOVAS_ROMAN_CALENDAR.
 * @param format      Expected order of date components: NOVAS_YMD, NOVAS_DMY, or NOVAS_MDY.
 * @param date        The date specification, possibly including time and timezone, in the
 *                    specified standard format.
 * @param[out] tail   (optional) If not NULL it will be set to the next character in the string
 *                    after the parsed time.
 *
 * @return            [day] The Julian Day corresponding to the string date/time specification or
 *                    NAN if the string is NULL or if it does not specify a date/time in the
 *                    expected format.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_parse_date()
 * @sa novas_timescale_for_string()
 * @sa novas_iso_timestamp()
 * @sa julian_date()
 */
double novas_parse_date_format(enum novas_calendar_type calendar, enum novas_date_format format, const char *restrict date,
       char **restrict tail) {
  static const char *fn = "novas_parse_date";
  static const char md[13] = { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  int y = 0, m = 0, d = 0, n = 0, N = 0;
  double h = 0.0;
  char month[10] = {'\0'}, *next = (char *) date;

  if(tail)
    *tail = (char *) date;

  if(!date) {
    novas_error(0, EINVAL, fn, "input string is NULL");
    return NAN;
  }
  if(!date[0]) {
    novas_error(0, EINVAL, fn, "input string is empty");
    return NAN;
  }

  switch(format) {
    case NOVAS_YMD:
      N = sscanf(date, "%d" DATE_SEP MONTH_SPEC DATE_SEP "%d%n", &y, month, &d, &n);
      break;
    case NOVAS_DMY:
      N = sscanf(date, "%d" DATE_SEP MONTH_SPEC DATE_SEP "%d%n", &d, month, &y, &n);
      break;
    case NOVAS_MDY:
      N = sscanf(date, MONTH_SPEC DATE_SEP "%d" DATE_SEP "%d%n", month, &d, &y, &n);
      break;
    default:
      novas_error(0, EINVAL, fn, "invalid date format: %d", format);
      return NAN;
  }

  if(N < 3) {
    novas_error(0, EINVAL, fn, "invalid date: '%s'", date);
    return NAN;
  }

  if(sscanf(month, "%d", &m) == 1) {
    // Month as integer, check if in expected range
    if(m < 1 || m > 12) {
      novas_error(0, EINVAL, fn, "invalid month: got %d, expected 1-12", m);
      return NAN;
    }
  }
  else {
    // Perhaps month as string...
    for(m = 1; m <= 12; m++) {
      static const char *monNames[13] = { NULL, "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

      if(strcasecmp(monNames[m], month) == 0)
        break;      // match full month name
      if(strncasecmp(monNames[m], month, 3) == 0)
        break;      // match abbreviated month name
    }
    if(m > 12) {
      // No match to month names...
      novas_error(0, EINVAL, fn, "invalid month: %s", month);
      return NAN;
    }
  }

  // Check that day is valid in principle (for leap years)
  if(d < 1 || d > md[m]) {
    novas_error(0, EINVAL, fn, "invalid day-of-month: got %d, expected 1-%d", d, md[m]);
    return NAN;
  }

  if(tail)
    *tail += n;

  skip_white(&date[n], &next);

  if(*next) {
    char *from = next;
    enum novas_debug_mode saved = novas_get_debug_mode();

    // Check if 'T' is used to separate time component, as in ISO timestamps.
    if(*next == 'T' || *next == 't')
      next++;

    // suppress debug messages while we parse time...
    novas_debug(NOVAS_DEBUG_OFF);

    // Try parse time
    h = novas_parse_hms(next, &next);

    // Restore prior debug state...
    errno = 0;
    novas_debug(saved);

    if(!isnan(h)) {
      int ds = parse_zone(next, &next);
      if(errno)
        return novas_trace_nan(fn);
      h -= ds / 3600.0;
    }
    else if(tail) {
      h = 0.0;
      next = from; // Time parsing unsuccessful, no extra characters consumed.
    }

    if(tail)
      *tail = next;
  }

  return novas_jd_from_date(calendar, y, m, d, h);
}

/**
 * Parses a date/time string into a Julian date specification. Typically the date may be an ISO
 * date specification, but with some added flexibility. The date must be YMD-type with full year,
 * followed the month (numerical or name or 3-letter abbreviation), and the day. The components
 * may be separated by dash `-`, underscore `_`, dot `.`,  slash '/', or spaces/tabs, or any
 * combination thereof. The date may be followed by a time specification in HMS format, separated
 * from the date by the letter `T` or `t`, or spaces, comma `,`, or semicolon `;`, or underscore
 * `_` or a combination thereof. Finally, the time may be followed by the letter `Z`, or `z` (for
 * UTC) or else {+/-}HH[:[MM]] time zone specification.
 *
 * For example:
 *
 * <pre>
 *  2025-01-26
 *  2025 January 26
 *  2025_Jan_26
 *  2025-01-26T19:33:08Z
 *  2025.01.26T19:33:08
 *  2025 1 26 19h33m28.113
 *  2025/1/26 19:33:28+02
 *  2025-01-26T19:33:28-0600
 *  2025 Jan 26 19:33:28+05:30
 * </pre>
 *
 * are all valid dates that can be parsed.
 *
 * NOTES:
 * <ol>
 * <li>This function uses Gregorian dates since their introduction on 1582 October 15, and
 * Julian/Roman datew before that, as was the convention of the time. I.e., the day before of the
 * introduction of the Gregorian calendar reform is 1582 October 4.</li>
 *
 * <li>B.C. dates are indicated with years &lt;=0 according to the astronomical
 * and ISO 8601 convention, i.e., X B.C. as (1-X), so 45 B.C. as -44.</li>
 * </oL>
 *
 * @param date        The date specification, possibly including time and timezone, in a standard
 *                    format.
 * @param[out] tail   (optional) If not NULL it will be set to the next character in the string
 *                    after the parsed time.
 *
 * @return            [day] The Julian Day corresponding to the string date/time specification or
 *                    NAN if the string is NULL or if it does not specify a date/time in the
 *                    expected format.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_date()
 * @sa novas_date_scale()
 * @sa novas_parse_date_format()
 * @sa novas_timescale_for_string()
 * @sa novas_iso_timestamp()
 * @sa novas_timestamp()
 */
double novas_parse_date(const char *restrict date, char **restrict tail) {
  double jd = novas_parse_date_format(NOVAS_ASTRONOMICAL_CALENDAR, NOVAS_YMD, date, tail);
  if(isnan(jd))
    return novas_trace_nan("novas_parse_date");
  return jd;
}

/**
 * Returns a Julian date (in non-specific timescale) corresponding the specified input
 * string date/time. E.g. for "2025-02-28T09:41:12.041+0200", with some flexibility
 * on how the date is represented as long as it's YMD date followed by HMS time. For
 * other date formats (MDY or DMY) you can use `novas_parse_date_format()` instead.
 *
 * @param date  The date specification, possibly including time and timezone, in a standard
 *              format. See novas_parse_date() on more information on acceptable date/time
 *              formats.
 * @return      [day] The Julian Day corresponding to the string date/time specification or
 *              NAN if the string is NULL or if it does not specify a date/time in the
 *              expected format.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_date_scale()
 * @sa novas_parse_date()
 * @sa novas_parse_date_format()
 * @sa novas_iso_timestamp()
 */
double novas_date(const char *restrict date) {
  double jd = novas_parse_date(date, NULL);
  if(isnan(jd))
    return novas_trace_nan("novas_date");
  return jd;
}

/**
 * Returns a Julian date and the timescale corresponding the specified input string date/time
 * and timescale marker. E.g. for "2025-02-28T09:41:12.041+0200 TAI", with some flexibility on
 * how the date is represented as long as it's YMD date followed by HMS time. For other date
 * formats (MDY or DMY) you can use `novas_parse_date_format()` instead.
 *
 * @param date          The date specification, possibly including time and timezone, in a
 *                      standard format. See novas_parse_date() on more information on
 *                      acceptable date/time formats.
 * @param[out] scale    The timescale constant, or else -1 if the string could not be parsed
 *                      into a date and timescale. If the string is a bare timestamp without
 *                      an hint of a timescale marker, then NOVAS_UTC will be assumed.
 * @return      [day] The Julian Day corresponding to the string date/time specification or
 *              NAN if the string is NULL or if it does not specify a date/time in the
 *              expected format.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_date()
 * @sa novas_timestamp()
 */
double novas_date_scale(const char *restrict date, enum novas_timescale *restrict scale) {
  const char *fn = "novas_date_scale";

  char *tail = (char *) date;
  double jd = novas_parse_date(date, &tail);

  if(!scale) {
    novas_error(0, EINVAL, fn, "output scale is NULL");
    return NAN;
  }

  *scale = -1;

  if(isnan(jd))
    return novas_trace_nan(fn);

  *scale = novas_parse_timescale(tail, &tail);

  return jd;
}

static int timestamp(long ijd, double fjd, char *buf) {
  long ms;
  int y, M, d, h, m;

  // Day start 12TT -> 0TT
  fjd += 0.5;

  // fjd -> [0.0:1.0) range
  d = (short) floor(fjd - 0.5);
  ijd += d;
  fjd -= d;

  // Round to nearest ms.
  ms = (long) floor(fjd * DAY_MILLIS + 0.5);
  if(ms >= DAY_MILLIS) {
    ms -= DAY_MILLIS;     // rounding to 0h next day...
    ijd++;
  }

  novas_jd_to_date(ijd, NOVAS_ASTRONOMICAL_CALENDAR, &y, &M, &d, NULL);

  h = (short) (ms / HOUR_MILLIS);
  ms -= HOUR_MILLIS * h;

  m = (short) (ms / MIN_MILLIS);
  ms -= MIN_MILLIS * m;

  return sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03d", y, M, d, h, m, (int) (ms/1000L), (int) (ms%1000L));
}

/**
 * Prints a UTC-based ISO timestamp to millisecond precision to the specified string buffer.
 * E.g.:
 *
 * <pre>
 *  2025-01-26T21:32:49.701Z
 * </pre>
 *
 * NOTES:
 * <ol>
 * <li>The timestamp uses the conventional date of the time. That is Gregorian dates after the
 * Gregorian calendar reform of 15 October 1582, and Julian/Roman dates prior to that.</li>
 * <li>B.C. dates are indicated with years &lt;=0 according to the astronomical
 * and ISO 8601 convention, i.e., X B.C. as (1-X), so 45 B.C. as -44.</li>
 * </ol>
 *
 * @param time      Pointer to the astronomical time specification data structure.
 * @param[out] dst  Output string buffer. At least 25 bytes are required for a complete
 *                  timestamp with termination.
 * @param maxlen    The maximum number of characters that can be printed into the output
 *                  buffer, including the string termination. If the full ISO timestamp
 *                  is longer than `maxlen`, then it will be truncated to fit in the allotted
 *                  space, including a termination character.
 * @return          the number of characters printed into the string buffer, not including
 *                  the termination. As such it is at most `maxlen - 1`.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_timestamp()
 * @sa novas_parse_time()
 */
int novas_iso_timestamp(const novas_timespec *restrict time, char *restrict dst, int maxlen) {
  static const char *fn = "novas_iso_timestamp";

  char buf[40];
  long ijd = 0;
  double fjd;
  int l;

  if(!dst)
    return novas_error(-1, EINVAL, fn, "output buffer is NULL");

  if(maxlen < 1)
    return novas_error(-1, EINVAL, fn, "invalid maxlen: %d", maxlen);

  *dst = '\0';

  if(!time)
    return novas_error(-1, EINVAL, fn, "input time is NULL");

  fjd = novas_get_split_time(time, NOVAS_UTC, &ijd);
  l = timestamp(ijd, fjd, buf);

  // Add 'Z' to indicate UTC time zone.
  buf[l++] = 'Z';
  buf[l] = '\0';

  if(l >= maxlen)
    l = maxlen - 1;

  strncpy(dst, buf, l);
  dst[l] = '\0';

  return l;
}

/**
 * Prints a timestamp to millisecond precision in the specified timescale to the specified
 * string buffer. E.g.:
 *
 * <pre>
 *  2025-01-26T21:32:49.701 TAI
 * </pre>
 *
 * NOTES:
 * <ol>
 * <li>The timestamp uses the astronomical date. That is Gregorian dates after the
 * Gregorian calendar reform of 15 October 1582, and Julian/Roman dates prior to that.</li>
 *
 * <li>B.C. dates are indicated with years &lt;=0 according to the astronomical
 * and ISO 8601 convention, i.e., X B.C. as (1-X), so 45 B.C. as -44.</li>
 * </ol>
 *
 * @param time      Pointer to the astronomical time specification data structure.
 * @param scale     The timescale to use.
 * @param[out] dst  Output string buffer. At least 28 bytes are required for a complete
 *                  timestamp with termination.
 * @param maxlen    The maximum number of characters that can be printed into the output
 *                  buffer, including the string termination. If the full ISO timestamp
 *                  is longer than `maxlen`, then it will be truncated to fit in the allotted
 *                  space, including a termination character.
 * @return          the number of characters printed into the string buffer, not including
 *                  the termination. As such it is at most `maxlen - 1`.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_iso_timestamp()
 */
int novas_timestamp(const novas_timespec *restrict time, enum novas_timescale scale, char *restrict dst, int maxlen) {
  static const char *fn = "novas_timestamp_scale";

  char buf[40];
  long ijd;
  double fjd;
  int n;

  if(!dst)
    return novas_error(-1, EINVAL, fn, "output buffer is NULL");

  if(maxlen < 1)
    return novas_error(-1, EINVAL, fn, "invalid maxlen: %d", maxlen);

  *dst = '\0';

  if(!time)
    return novas_error(-1, EINVAL, fn, "input time is NULL");

  fjd = novas_get_split_time(time, scale, &ijd);
  n = timestamp(ijd, fjd, buf);

  buf[n++] = ' ';

  n += novas_print_timescale(scale, &buf[n]);

  if(n >= maxlen)
    n = maxlen - 1;

  memcpy(dst, buf, n);
  dst[n] = '\0';

  return n;
}

/**
 * Prints the standard string representation of the timescale to the specified buffer. The
 * string is terminated after. E.g. "UTC", or "TAI". It will print dates in the Gregorian
 * calendar, which was introduced in was introduced on 15 October 1582 only. Thus the
 *
 * @param scale   The timescale
 * @param buf     String in which to print. It should have at least 4-bytes of available
 *                storage.
 * @return        the number of characters printed, not including termination, or else -1
 *                if the timescale is invalid or the output buffer is NULL.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_timestamp()
 * @sa novas_timescale_for_string()
 */
int novas_print_timescale(enum novas_timescale scale, char *restrict buf) {
  static const char *fn = "novas_print_timescale";

  if(!buf)
    return novas_error(-1, EINVAL, fn, "output buffer is NULL");

  switch(scale) {
    case NOVAS_UT1:
      return sprintf(buf, "UT1");
    case NOVAS_UTC:
      return sprintf(buf, "UTC");
    case NOVAS_GPS:
      return sprintf(buf, "GPS");
    case NOVAS_TAI:
      return sprintf(buf, "TAI");
    case NOVAS_TT:
      return sprintf(buf, "TT");
    case NOVAS_TCG:
      return sprintf(buf, "TCG");
    case NOVAS_TCB:
      return sprintf(buf, "TCB");
    case NOVAS_TDB:
      return sprintf(buf, "TDB");
  }

  *buf = '\0';

  return novas_error(-1, EINVAL, fn, "invalid timescale: %d", scale);
}

/**
 * Returns the timescale constant for a string that denotes the timescale in
 * with a standard abbreviation (case insensitive). The following values are
 * recognised: "UTC", "UT", "UT0", "UT1", "GMT", "TAI", "GPS", "TT", "ET",
 * "TCG", "TCB", "TDB".
 *
 * @param str     String specifying an astronomical timescale
 * @return        The SuperNOVAS timescale constant (&lt;=0), or else -1 if
 *                the string was NULL, empty, or could not be matched to
 *                a timescale value (errno will be set to EINVAL also).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_parse_timescale()
 * @sa novas_set_time()
 * @sa novas_set_split_time()
 * @sa novas_print_timescale()
 */
enum novas_timescale novas_timescale_for_string(const char *restrict str) {
  static const char *fn = "novas_str_timescale";

  if(!str)
    return novas_error(-1, EINVAL, fn, "input string is NULL");

  if(!str[0])
    return novas_error(-1, EINVAL, fn, "input string is empty");

  if(strcasecmp("UTC", str) == 0 || strcasecmp("UT", str) == 0 || strcasecmp("UT0", str) == 0 || strcasecmp("GMT", str) == 0)
    return NOVAS_UTC;

  if(strcasecmp("UT1", str) == 0)
    return NOVAS_UT1;

  if(strcasecmp("TAI", str) == 0)
    return NOVAS_TAI;

  if(strcasecmp("GPS", str) == 0)
    return NOVAS_GPS;

  if(strcasecmp("TT", str) == 0 || strcasecmp("ET", str) == 0)
    return NOVAS_TT;

  if(strcasecmp("TCG", str) == 0)
    return NOVAS_TCG;

  if(strcasecmp("TCB", str) == 0)
    return NOVAS_TCB;

  if(strcasecmp("TDB", str) == 0)
    return NOVAS_TDB;

  return novas_error(-1, EINVAL, fn, "unknown timescale: %s", str);
}

/**
 * Parses the timescale from a string containing a standard abbreviation (case insensitive), and
 * returns the updated parse position after the timescale specification (if any). The following
 * timescale values are recognised: "UTC", "UT", "UT0", "UT1", "GMT", "TAI", "GPS", "TT", "ET",
 * "TCG", "TCB", "TDB".
 *
 * @param str         String specifying an astronomical timescale. Leading white spaces will be
                      skipped over.
 * @param[out] tail   (optional) If not NULL it will be set to the next character in the string
 *                    after the parsed timescale specification.
 *
 * @return            The SuperNOVAS timescale constant (&lt;=0), or else -1 if the string was
 *                    NULL, empty, or could not be matched to a timescale value (errno will be set
 *                    to EINVAL also).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_timescale_for_string()
 * @sa novas_set_time()
 * @sa novas_set_split_time()
 * @sa novas_print_timescale()
 */
enum novas_timescale novas_parse_timescale(const char *restrict str, char **restrict tail) {
  static const char *fn = "novas_parse_timescale";

  enum novas_timescale scale = NOVAS_UTC;
  char s[4] = {'\0'};
  int n = 0;

  if(tail)
    *tail = (char *) str;

  if(!str)
    return novas_error(-1, EINVAL, fn, "input string is NULL");

  if(sscanf(str, "%3s%n", s, &n) == 1) {
    scale = novas_timescale_for_string(s);
    if(scale < 0)
      return novas_trace(fn, scale, 0);
  }

  if(tail)
    *tail += n;

  return scale;
}


