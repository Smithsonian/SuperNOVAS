/**
 * @file
 *
 * @date Created  on Jan 9, 2025
 * @author Attila Kovacs
 *
 *  Example file for using the SuperNOVAS C/C++ library for checking rise, set, or transit times
 *  for sources observed from Earth's surface or airspace.
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
#define  DUT1             0.114     ///< [s] current UT1 - UTC time difference from IERS Bulletin A
#define  POLAR_DX         230.0     ///< [mas] Earth polar offset x, e.g. from IERS Bulletin A.
#define  POLAR_DY         -62.0     ///< [mas] Earth polar offset y, e.g. from IERS Bulletin A.

/*
 * example-riseset [elevation]
 *
 * Arguments:
 *
 *   elevation    [deg] elevation angle (default 0.0).
 *
 */
int main(int argc, const char *argv[]) {
  // Input parameters
  double el = 0.0;                  // [deg] elevation angle (set via command-line argument)

  // SuperNOVAS variables used for the calculations ------------------------->
  cat_entry star = CAT_ENTRY_INIT;  // catalog information about a sidereal source
  object source;                    // a celestial object: sidereal, planet, ephemeris or orbital source
  observer obs;                     // observer location
  novas_timespec obs_time;          // astrometric time of observation
  novas_frame obs_frame;            // observing frame defined for observing time and location
  enum novas_accuracy accuracy;     // NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY

  // Calculated quantities ------------------------------------------------->
  double jd_utc;                    // [day] UTC-based Julian day of rise/set/transit
  char timestamp[40];               // String timestamp.

  // Intermediate variables we'll use -------------------------------------->
  struct timespec unix_time;        // Standard precision UNIX time structure
  novas_timespec ts;                // astronomical time of rise/set/transit

  // Check if called with argument.
  if(argc > 1)
    el = strtod(argv[1], NULL);

  // We'll print debugging messages and error traces...
  novas_debug(NOVAS_DEBUG_ON);


  // -------------------------------------------------------------------------
  // Define a sidereal source

  // Let's assume we have B1950 (FK4) coordinates...
  // 16h26m20.1918s, -26d19m23.138s (B1950), proper motion -12.11, -23.30 mas/year,
  // parallax 5.89 mas, radial velocity -3.4 km/s.
  //
  // NOTE, that make_cat_entry() expects radial velocities defined relative to the
  // Solar-System Barycenter (SSB). But you can convert LSR-basec velocities to
  // the required SSB-based radial velocities using novas_lsr_to_ssb_vel() if needed.
  if(make_cat_entry("Antares", "FK4", 1, 16.43894213, -26.323094, -12.11, -23.30, 5.89, -3.4, &star) != 0) {
    fprintf(stderr, "ERROR! defining cat_entry.\n");
    return 1;
  }

  // -------------------------------------------------------------------------
  // Wrap the sidereal source into an object structure...
  if(make_cat_object_sys(&star, "B1950", &source) != 0) {
    fprintf(stderr, "ERROR! configuring observed object\n");
    return 1;
  }


  // -------------------------------------------------------------------------
  // Define observer somewhere on Earth (we can also define observers in Earth
  // or Sun orbit, at the geocenter or at the Solary-system barycenter...)

  // Specify the location we are observing from
  // 50.7374 deg N, 7.0982 deg E, 60m elevation
  // (We'll ignore the local weather parameters here, but you can set those too.)
  if(make_observer_on_surface(50.7374, 7.0982, 60.0, 0.0, 0.0, &obs) != 0) {
    fprintf(stderr, "ERROR! defining Earth-based observer location.\n");
    return 1;
  }


  // -------------------------------------------------------------------------
  // Set the astrometric time of observation...

  // Get the current system time, with up to nanosecond resolution...
  clock_gettime(CLOCK_REALTIME, &unix_time);

  // Set the time of observation to the precise UTC-based UNIX time
  if(novas_set_unix_time(unix_time.tv_sec, unix_time.tv_nsec, LEAP_SECONDS, DUT1, &obs_time) != 0) {
    fprintf(stderr, "ERROR! failed to set time of observation.\n");
    return 1;
  }

  // ... Or you could set a time explicily in any known timescale.
  /*
  // Let's set a TDB-based time for the start of the J2000 epoch exactly...
  if(novas_set_time(NOVAS_TDB, NOVAS_JD_J2000, 32, 0.0, &obs_time) != 0) {
    fprintf(stderr, "ERROR! failed to set time of observation.\n");
    return 1;
  }
  */


  // -------------------------------------------------------------------------
  // You might want to set a provider for precise planet positions so we might
  // calculate Earth, Sun and major planet positions accurately. If an planet
  // provider is configured, we can unlock the ultimate (sub-uas) accuracy of
  // SuperNOVAS.
  //
  // There are many ways to set a provider of planet positions. For example,
  // you may use the CALCEPH library:
  //
  // t_calcephbin *planets = calceph_open("path/to/de440s.bsp");
  // novas_use_calceph(planets);
  //
  // accuracy = NOVAS_FULL_ACCURACY;      // sub-uas precision

  // Without a planet provider, we are stuck with reduced (mas) precisions
  // only...
  accuracy = NOVAS_REDUCED_ACCURACY;      // mas-level precision, typically


  // -------------------------------------------------------------------------
  // Initialize the observing frame with the given observing and Earth
  // orientation patameters.
  //
  if(novas_make_frame(accuracy, &obs, &obs_time, POLAR_DX, POLAR_DY, &obs_frame) != 0) {
    fprintf(stderr, "ERROR! failed to define observing frame.\n");
    return 1;
  }


  // -------------------------------------------------------------------------
  // Print source name to output
  printf("'%s' observed from lon = %.3f, lat = %.3f:\n", source.name, obs.on_surf.longitude, obs.on_surf.latitude);


  // -------------------------------------------------------------------------
  // Calculate next UTC-based date/time source rises above 20 degrees elevation
  // (as corrected for optical refraction under a standard atmosphere)
  jd_utc = novas_rises_above(el, &source, &obs_frame, novas_standard_refraction);

  if(isnan(jd_utc)) {
    printf(" will not rise above %5.1f degrees\n", el);
  }
  else {
    novas_set_time(NOVAS_UTC, jd_utc, LEAP_SECONDS, DUT1, &ts);
    novas_iso_timestamp(&ts, timestamp, sizeof(timestamp));
    printf(" will rise above %5.1f degrees at  : %s\n", el, timestamp);
  }

  // -------------------------------------------------------------------------
  // Calculate next UTC-based date/time source transits at observer location
  jd_utc = novas_transit_time(&source, &obs_frame);

  novas_set_time(NOVAS_UTC, jd_utc, LEAP_SECONDS, DUT1, &ts);
  novas_iso_timestamp(&ts, timestamp, sizeof(timestamp));
  printf(" will transit at                   : %s\n", timestamp);


  // -------------------------------------------------------------------------
  // Calculate next UTC-based date/time source sets below 20 degrees elevation
  // (as corrected for optical refraction under a standard atmosphere)
  jd_utc = novas_sets_below(el, &source, &obs_frame, novas_standard_refraction);

  if(isnan(jd_utc)) {
    printf(" will not set below %5.1f degrees\n", el);
  }
  else {
    novas_set_time(NOVAS_UTC, jd_utc, LEAP_SECONDS, DUT1, &ts);
    novas_iso_timestamp(&ts, timestamp, sizeof(timestamp));
    printf(" will set below %5.1f degrees at   : %s\n", el, timestamp);
  }

  return 0;
}

