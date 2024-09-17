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
/// \endcond


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
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_set_time(enum novas_timescale timescale, double jd, int leap, double dut1, novas_timespec *time) {
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
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_set_split_time(enum novas_timescale timescale, long ijd, double fjd, int leap, double dut1,
        novas_timespec *time) {
  static const char *fn = "novas_set_split_time";

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

  time->ijd_tt = ijd + (long) floor(fjd);
  time->fjd_tt = remainder(fjd, 1.0);

  if(time->fjd_tt < 0.0)
    time->fjd_tt += 1.0;

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
double novas_get_time(const novas_timespec *time, enum novas_timescale timescale) {
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
double novas_get_split_time(const novas_timespec *time, enum novas_timescale timescale, long *ijd) {
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
 * @return      [day] Precise time difference (t1-t2), or NAN if one of the inputs was NULL (errno
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
 * @return      [day] Precise TCB time difference (t1-t2), or NAN if one of the inputs was
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
 * @return      [day] Precise TCG time difference (t1-t2), or NAN if one of the inputs was
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
int novas_set_unix_time(time_t unix_time, long nanos, int leap, double dut1, novas_timespec *time) {
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
time_t novas_get_unix_time(const novas_timespec *time, long *nanos) {
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
