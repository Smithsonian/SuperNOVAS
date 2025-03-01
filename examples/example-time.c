/**
 * @file
 *
 * @date Created  on Jan 29, 2025
 * @author Attila Kovacs
 *
 *  Example file for using the time-related functions of the SuperNOVAS C/C++ library
 *
 *  Link with
 *
 *  ```
 *   -lsupernovas
 *  ```
 */

#define _POSIX_C_SOURCE 199309L   ///< for clock_gettime()

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <novas.h>      ///< SuperNOVAS functions and definitions


// Below are some Earth orientation values. Here we define them as constants, but they may
// of course be variables. They should be set to the appropriate values for the time
// of observation based on the IERS Bulletins...

#define  LEAP_SECONDS     37        ///< [s] current leap seconds from IERS Bulletin C
#define  DUT1             0.048     ///< [s] current UT1 - UTC time difference from IERS Bulletin A
#define  POLAR_DX         230.0     ///< [mas] Earth polar offset x, e.g. from IERS Bulletin A.
#define  POLAR_DY         -62.0     ///< [mas] Earth polar offset y, e.g. from IERS Bulletin A.


int main() {
  // SuperNOVAS variables -------------------------------------------------->
  novas_timespec time1 = {}, time2 = {}, time3 = {};     // Astronomical time data for SuperNOVAS

  // Calculated quantities ------------------------------------------------->
  enum novas_timescale scale;   // astronomical timescale
  double jd;                    // [day] Julian days
  long ijd;                     // [day] integer part of Julian Day
  double fjd;                   // [day] fractional part of Julian Day.
  double dt;                    // [s] time difference
  int year, month, day;         // broken-down date components
  double hours;                 // broken-down time-of-day component
  char timestamp[40];           // String timestamp, at leat 29 characters.

  // Intermediate variables we'll use -------------------------------------->
  struct timespec ts;           // precision UNIX time

  // -------------------------------------------------------------------------
  // 1.a. Dates from strings...

  // - Parse a date/time string in YMD format -- there is a fait bit of
  //   flexibility on how the components are separated, but they must be year,
  //   month, day, then optionally time and possibly time zone also. For
  //   example:
  jd = novas_date("2025-01-29T18:09:29.333+0200");

  // Alternatively, you could use a string time that also contains a timescale
  // specification:
  jd = novas_date_scale("2025-01-29 18:09:29.333+0200 TAI", &scale);

  // - Next, convert that date to an astronomical time of a specific time
  //   scale. Let's say the above date was on the TAI... (It could be UTC, or
  //   GPS, or TDB...)
  novas_set_time(NOVAS_TAI, jd, LEAP_SECONDS, DUT1, &time1);


  // -------------------------------------------------------------------------
  // 1.b. Dates from broken-down time

  // - Use year, month, day, and time-of-day, e.g. 1974-01-31 23:30
  //   The date is in the astronomical calendar of date (as opposed to a fixed
  jd = novas_jd_from_date(NOVAS_ASTRONOMICAL_CALENDAR, 1974, 1, 31, 23.5);

  // - Next, convert that date to an astronomical time of a specific time
  //   scale. Let's say the above date was on the TAI... (It could be UTC, or
  //   GPS, or TDB...)
  novas_set_time(NOVAS_TAI, jd, LEAP_SECONDS, DUT1, &time2);


  // -------------------------------------------------------------------------
  // 1.c. Current time...

  // - Use the system clock to get a precise system time
  clock_gettime(CLOCK_REALTIME, &ts);

  // - Convert UNIX time to astronomical time
  novas_set_unix_time(ts.tv_sec, ts.tv_nsec, LEAP_SECONDS, DUT1, &time3);


  // -------------------------------------------------------------------------
  // 2. Extract specific timescales from the astronomical time

  // - Get a TDB-based JD date for time1
  jd = novas_get_time(&time1, NOVAS_TDB);

  // - Or for higher precision, get a split JD...
  fjd = novas_get_split_time(&time1, NOVAS_TDB, &ijd);
  printf("Split date is %ld / %.9f", ijd, fjd);


  // -------------------------------------------------------------------------
  // 3. Measure time difference

  // - Difference in Earth-based time measures (UTC, GPS, TAI, TT)
  dt = novas_diff_time(&time1, &time2);
  printf(" UTC Time difference is %.6f days\n", dt / NOVAS_DAY);

  // - Difference in TCG -- time progresses differently outside of Earth gravity
  dt = novas_diff_tcg(&time1, &time2);
  printf(" TCG Time difference is %.6f days\n", dt / NOVAS_DAY);

  // - Difference in TCB -- time progresses differently outside of Earth gravity
  dt = novas_diff_tcb(&time1, &time2);
  printf(" TDB Time difference is %.6f days\n", dt / NOVAS_DAY);

  // -------------------------------------------------------------------------
  // 4. Offset time

  // - Add 5.31 seconds to time1. We can put the result in a different time data
  //   or we can overwrite the input (as we'll do here).
  novas_offset_time(&time1, 5.31, &time1);


  // -------------------------------------------------------------------------
  // 5. Print time

  // - Print an UTC-based ISO timestamp to a string
  novas_iso_timestamp(&time1, timestamp, sizeof(timestamp));
  printf(" Offset time is: %s\n", timestamp);


  // - Print a timestamp in a specific time measure, e.g. GPS time
  novas_timestamp(&time1, NOVAS_GPS, timestamp, sizeof(timestamp));
  printf(" in TDB        : %s\n", timestamp);


  // -------------------------------------------------------------------------
  // 6. Get Calendar date in specific calendar

  // - Get a Broken down date, say in the Julian/Roman calendar
  novas_jd_to_date(jd, NOVAS_ROMAN_CALENDAR, &year, &month, &day, &hours);


  return 0;
}

