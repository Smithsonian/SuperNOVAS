/**
 * @file
 *
 * @date Created  on Jun 24, 2024
 * @author Attila Kovacs
 * @since 1.1
 *
 *   A set of routines to make handling of astronomical timescale and conversions between them
 *   easier.
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
#define DTA         (32.184 / DAY)        ///< [day] TT - UTC time difference
#define GPS2TAI     (19.0 / DAY)          ///< [day] GPS to TAI time difference

#define IDAY        86400                 ///< [s] 1 day

#define UNIX_SECONDS_0UTC_1JAN2000  946684800    ///< [s] UNIX time at J2000.0
#define UNIX_J2000                  (UNIX_SECONDS_0UTC_1JAN2000 + (IDAY / 2))
/// \endcond


/**
 * Sets an astronomical time to the fractional Julian Date value, defined in the specified
 * timescale
 *
 * @param timescale     The astronomical time scale in which the Julian Date is given
 * @param jd            [day] Julian day value in the specified timescale
 * @param leap          [s] Leap seconds
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
  return novas_set_split_time(timescale, 0, jd, leap, dut1, time);
}

/**
 * Sets an astronomical time to the split Julian Date value, defined in the specified timescale.
 * The split into the integer and fractional parts can be done in any convenient way. The highest
 * precision is reached if the fractional part is on the order of &le;= 1 day.
 *
 * @param timescale     The astronomical time scale in which the Julian Date is given
 * @param ijd           [day] integer part of the Julian day in the specified timescale
 * @param fjd           [day] fractional part Julian day value in the specified timescale
 * @param leap          [s] Leap seconds
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
  time->ut1_to_tt = leap - dut1 + DTA;    // TODO check!

  switch(timescale) {
    case NOVAS_TT:
      break;
    case NOVAS_TDB: {
      tdb2tt(ijd + fjd, NULL, &time->tt2tdb);
      time->tt2tdb = -time->tt2tdb;
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

  time->ijd_tt = ijd + (int) floor(fjd);
  time->fjd_tt = remainder(fjd, 1.0);

  if(time->fjd_tt < 0.0)
    time->fjd_tt += 1.0;

  if(isnan(time->tt2tdb))
    time->tt2tdb = tt2tdb(time->ijd_tt + time->fjd_tt) / DAY;

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
int novas_increment_time(const novas_timespec *time, double seconds, novas_timespec *out) {
  int dd;

  if(!time || !out)
    return novas_error(-1, EINVAL, "novas_increment_time", "NULL parameter: time=%p, out=%p", time, out);

  if(out != time)
    *out = *time;

  out->fjd_tt += seconds / DAY;
  dd = (int) floor(out->fjd_tt);
  if(dd) {
    out->fjd_tt -= dd * DAY;
    out->ijd_tt += dd;
  }

  return 0;
}

/**
 * Returns the fractional Julian date of an astronomical time in the specified timescale.
 *
 * @param time        Pointer to the astronimical time specification data structure.
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
  return ijd + fjd;
}

/**
 * Returns the fractional Julian date of an astronomical time in the specified timescale, as an
 * integer and fractional part.
 *
 * @param time        Pointer to the astronimical time specification data structure.
 * @param timescale   The astronomical time scale in which the returned Julian Date is to be
 *                    provided
 * @param[out] ijd    [day] The integer part of the Julian date in the requested timescale. It may
 *                    be NULL if not required.
 * @return            [day] The fractional part of the Julian date in the requested timescale.
 *
 * @sa novas_set_split_time()
 * @sa novas_get_time()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
double novas_get_split_time(const novas_timespec *time, enum novas_timescale timescale, long *ijd) {
  static const char *fn = "novas_get_time";
  double f;

  if(!time) {
    novas_error(-1, EINVAL, fn, "NULL input time specification");
    return NAN;
  }
  if(timescale < 0 || timescale > NOVAS_TIMESCALES)
    if(ijd)
      *ijd = time->ijd_tt;

  f = time->fjd_tt;

  switch(timescale) {
    case NOVAS_TT:
      break;
    case NOVAS_TDB:
      f += time->tt2tdb / DAY;
      break;
    case NOVAS_TAI:
      f -= DTA;
      break;
    case NOVAS_GPS:
      f -= (DTA + GPS2TAI);
      break;
    case NOVAS_UTC:
      f -= (time->ut1_to_tt + time->dut1);
      break;
    case NOVAS_UT1:
      f -= time->ut1_to_tt;
      break;
    default:
      novas_error(-1, EINVAL, fn, "Invalid timescale: %d", timescale);
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
 * Returns the time difference (t1 - t2) in days between two astronomical time specifications.
 *
 * @param t1    First time
 * @param t2    Second time
 * @return      [day] Precise time difference (t1-t2), or NAN if one of the inputs was NULL (errno
 *              will be set to EINVAL)
 *
 * @sa novas_set_time()
 * @sa novas_increment_time()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
double novas_diff_time(const novas_timespec *t1, const novas_timespec *t2) {
  if(!t1 || !t2) {
    novas_error(-1, EINVAL, "novas_diff_time", "NULL parameter: t1=%p, t2=%p", t1, t2);
    return NAN;
  }

  return (t1->ijd_tt - t2->ijd_tt) + (t1->fjd_tt - t2->fjd_tt);
}


/**
 * Sets an astronomical time to the split Julian Date value, defined in the specified timescale.
 * The split into the integer and fractional parts can be done in any convenient way. The highest
 * precision is reached if the fractional part is on the order of &le;= 1 day.
 *
 * @param unix_time   [s] UNIX time (UTC) seconds
 * @param nanos       [ns] UTC sub-second component
 * @param leap        [s] Leap seconds
 * @param dut1        [s] UT1-UTC time difference, e.g. as published in IERS Bulletin A.
 * @param[out] time   Pointer to the data structure that uniquely defines the astronomical time
 *                    for all applications.
 * @return            0 if successful, or else -1 if there was an error (errno will be set to
 *                    indicate the type of error).
 *
 * @sa novas_set_time()
 * @sa novas_get_unix_time()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_set_unix_time(time_t unix_time, long nanos, int leap, double dut1, novas_timespec *time) {
  long jd, sojd;

  // UTC based integer JD
  unix_time -= UNIX_J2000;
  jd = NOVAS_JD_J2000 + unix_time / IDAY;

  // seconds of JD date
  sojd = unix_time % IDAY;
  if(sojd < 0)
    sojd += IDAY;

  return novas_set_split_time(NOVAS_UTC, jd, (sojd + 1e-9 * nanos + time->dut1 + time->ut1_to_tt) / DAY, leap, dut1,
          time);
}

/**
 * Returns the UNIX time for an astronomical time instant.
 *
 * @param time      The astronomical time scale in which the returned Julian Date is to be provided
 * @param nanos     [ns] UTC sub-second component.
 * @return          [s] The integer UNIX time
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
  isod = floor(sod);
  seconds = UNIX_J2000 + (ijd - NOVAS_JD_J2000) * DAY + isod;

  if(nanos)
    *nanos = floor(1e9 * (sod - isod));
  return seconds;
}
