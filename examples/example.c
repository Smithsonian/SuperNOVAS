/*
 Naval Observatory Vector Astrometry Software (NOVAS)
 C Edition, Version 3.1
 
 example.c: Examples of NOVAS calculations
 
 U. S. Naval Observatory
 Astronomical Applications Dept.
 Washington, DC
 http://www.usno.navy.mil/USNO/astronomical-applications
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "novas.h"
#include "novascon.h"
#include "eph_manager.h"  // For solsys1.c

int main(void) {

  /*
   NOVAS 3.1 Example Calculations
   
   See Chapter 3 of User's Guide for explanation.
   
   Written for use with solsys version 1.
   
   To adapt for use with solsys version 2, see comments throughout file.
   Assumes JPL ephemeris file "JPLEPH" located in same directory as
   application.
   */

  const double latitude = 42.0;       ///< [deg] geodetic latitude of observer
  const double longitude = -70;       ///< [deg] geodetic longitude of observer
  const double height = 0.0;          ///< [m] observer's altitude above sea level
  const double temperature = 10.0;    ///< [C] Ambient temperature at observing location (for refraction correction)
  const double pressure = 1010.0;     ///< [mbar] Atmospheric pressure at observing location (for refraction correction)

  const int leap_secs = 33;           ///< [s] Leap seconds (UTC - TAI; see IERS Bulletins)
  const double x_pole = -0.002;       ///< [arcsec] Celestial pole offset in x (see IERS Bulletins)
  const double y_pole = +0.529;       ///< [arcsec] Celestial pole offset in y (see IERS Bulletins)
  const double ut1_utc = -0.387845;   ///< [s] UT1 - UTC time difference (see IERS Bulletins)

  // Various variables we will calculate / populate....
  double jd_beg, jd_end, jd_utc, jd_tt, jd_ut1, jd_tdb, delta_t, ra, dec, dis, rat, dect, dist, zd, az, rar, decr, gast, last, theta, jd[2],
          pos[3], vel[3], pose[3], elon, elat, r, lon_rad, lat_rad, sin_lon, cos_lon, sin_lat, cos_lat, vter[3], vcel[3];


  observer obs_loc;
  on_surface *geo_loc;
  cat_entry star;
  object moon, mars;
  sky_pos t_place;

  int error = 0;
  short de_num = 0;

  // Make structures of type 'on_surface' and 'observer-on-surface' containing
  // the observer's position and weather (latitude, longitude, height,
  //temperature, and atmospheric pressure)
  make_observer_on_surface(latitude, longitude, height, temperature, pressure, &obs_loc);
  geo_loc = &obs_loc.on_surf;

  // Make a structure of type 'cat_entry' containing the ICRS position
  // and motion of star FK6 1307.
  make_cat_entry("GMB 1830", "FK6", 1307, 11.88299133, 37.71867646, 4003.27, -5815.07, 109.21, -98.8, &star);

  if((error = make_object(NOVAS_PLANET, NOVAS_MOON, "Moon", NULL, &moon)) != 0) {
    printf("Error %d from make_object (Moon)\n", error);
    return (error);
  }

  if((error = make_object(NOVAS_PLANET, NOVAS_MARS, "Mars", NULL, &mars)) != 0) {
    printf("Error %d from make_object (Mars)\n", error);
    return (error);
  }

  // Open the JPL binary ephemeris file, here named "JPLEPH".
  error = ephem_open("JPLEPH", &jd_beg, &jd_end, &de_num);
  if(error != 0) {
    if(error == 1) printf("JPL ephemeris file not found.\n");
    else printf("Error reading JPL ephemeris file header.\n");
    return (error);
  }
  else {
    printf("JPL ephemeris DE%d open. Start JD = %10.2f  End JD = %10.2f\n", de_num, jd_beg, jd_end);
    printf("\n");
  }

  // Set the planet calculator to the one using eph_manager
  set_planet_provider(planet_eph_manager);
  set_planet_provider_hp(planet_eph_manager_hp);

  // Write banner.
  printf("NOVAS Sample Calculations\n");
  printf("-------------------------\n");
  printf("\n");

  // Write assumed longitude, latitude, height (ITRS = WGS-84).
  printf("Geodetic location:\n");
  printf("%15.10f        %15.10f        %15.10f\n\n", geo_loc->longitude, geo_loc->latitude, geo_loc->height);

  // Establish time arguments.
  jd_utc = julian_date(2008, 4, 24, 10.605);
  jd_ut1 = jd_utc + ut1_utc / 86400.0;
  delta_t = get_ut1_to_tt(leap_secs, ut1_utc);

  jd_tdb = jd_tt; /* Approximation good to 0.0017 seconds. */

  printf("TT and UT1 Julian Dates and Delta-T:\n");
  printf("%15.6f        %15.6f        %16.11f\n", jd_tt, jd_ut1, delta_t);
  printf("\n");

  // Apparent and topocentric place of star FK6 1307 = GMB 1830.
  error = app_star(jd_tt, &star, NOVAS_FULL_ACCURACY, &ra, &dec);
  if(error != 0) {
    printf("Error %d from app_star.\n", error);
    return (error);
  }

  error = topo_star(jd_tt, delta_t, &star, &obs_loc.on_surf, NOVAS_FULL_ACCURACY, &rat, &dect);
  if(error != 0) {
    printf("Error %d from topo_star.\n", error);
    return (error);
  }

  printf("FK6 1307 geocentric and topocentric positions:\n");
  printf("%15.10f        %15.10f\n", ra, dec);
  printf("%15.10f        %15.10f\n", rat, dect);
  printf("\n");

  // Apparent and topocentric place of the Moon.
  error = app_planet(jd_tt, &moon, NOVAS_FULL_ACCURACY, &ra, &dec, &dis);
  if(error != 0) {
    printf("Error %d from app_planet.", error);
    return (error);
  }

  error = topo_planet(jd_tt, &moon, delta_t, &obs_loc.on_surf, NOVAS_FULL_ACCURACY, &rat, &dect, &dist);
  if(error != 0) {
    printf("Error %d from topo_planet.", error);
    return (error);
  }

  printf("Moon geocentric and topocentric positions:\n");
  printf("%15.10f        %15.10f        %15.12f\n", ra, dec, dis);
  printf("%15.10f        %15.10f        %15.12f\n", rat, dect, dist);

  // Topocentric (True of Date) place of the Moon using function 'place'-- should be
  // same as above.
  error = place(jd_tt, &moon, &obs_loc, delta_t, NOVAS_TOD, NOVAS_FULL_ACCURACY, &t_place);
  if(error != 0) {
    printf("Error %d from place.", error);
    return (error);
  }

  printf("%15.10f        %15.10f        %15.12f\n", t_place.ra, t_place.dec, t_place.dis);
  printf("\n");

  // Position of the Moon in local horizon coordinates.  (Polar motion ..ignored here.)
  equ2hor(jd_ut1, delta_t, NOVAS_FULL_ACCURACY, 0.0, 0.0, &obs_loc.on_surf, rat, dect, NOVAS_STANDARD_ATMOSPHERE, //
          &zd, &az, &rar, &decr);

  printf("Moon zenith distance and azimuth:\n");
  printf("%15.10f        %15.10f\n", zd, az);
  printf("\n");

  // Greenwich and local apparent sidereal time and Earth Rotation Angle.
  error = sidereal_time(jd_ut1, 0.0, delta_t, NOVAS_TRUE_EQUINOX, EROT_ERA, NOVAS_FULL_ACCURACY, &gast);
  if(error != 0) {
    printf("Error %d from sidereal_time.", error);
    return (error);
  }

  last = remainder(gast + geo_loc->longitude / 15.0, 24.0);
  if(last < 0.0) last += 24.0;

  theta = era(jd_ut1, 0.0);

  printf("Greenwich and local sidereal time and Earth Rotation Angle:\n");
  printf("%16.11f        %16.11f        %15.10f\n", gast, last, theta);
  printf("\n");

  // Heliocentric position of Mars in BCRS.

  // TDB ~ TT approximation could lead to error of ~50 m in position of Mars.
  jd[0] = jd_tdb;
  jd[1] = 0.0;

  error = ephemeris(jd, &mars, NOVAS_HELIOCENTER, NOVAS_FULL_ACCURACY, pos, vel);
  if(error != 0) {
    printf("Error %d from ephemeris (Mars).", error);
    return (error);
  }

  error = equ2ecl_vec(T0, NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, pos, pose);
  if(error != 0) {
    printf("Error %d from equ2ecl_vec.", error);
    return (error);
  }

  if((error = vector2radec(pose, &elon, &elat)) != 0) {
    printf("Error %d from vector2radec.", error);
    return (error);
  }
  elon *= 15.0;

  r = sqrt(pose[0] * pose[0] + pose[1] * pose[1] + pose[2] * pose[2]);

  printf("Mars heliocentric ecliptic longitude and latitude and radius vector:\n");
  printf("%15.10f        %15.10f        %15.12f\n", elon, elat, r);
  printf("\n");

  // Terrestrial to celestial transformation.
  lon_rad = geo_loc->longitude * DEG2RAD;
  lat_rad = geo_loc->latitude * DEG2RAD;
  sin_lon = sin(lon_rad);
  cos_lon = cos(lon_rad);
  sin_lat = sin(lat_rad);
  cos_lat = cos(lat_rad);

  // Form vector toward local zenith (orthogonal to ellipsoid) in ITRS.
  vter[0] = cos_lat * cos_lon;
  vter[1] = cos_lat * sin_lon;
  vter[2] = sin_lat;

  // Transform vector to GCRS.
  error = ter2cel(jd_ut1, 0.0, delta_t, EROT_ERA, NOVAS_FULL_ACCURACY, NOVAS_REFERENCE_CLASS, x_pole, y_pole, vter, vcel);
  if(error != 0) {
    printf("Error %d from ter2cel.", error);
    return (error);
  }

  error = vector2radec(vcel, &ra, &dec);
  if(error != 0) {
    printf("Error %d from vector2radec.", error);
    return (error);
  }

  printf("Direction of zenith vector (RA & Dec) in GCRS:\n");
  printf("%15.10f        %15.10f\n", ra, dec);
  printf("\n");

  ephem_close(); /* remove this line for use with solsys version 2 */

  return (0);
}
