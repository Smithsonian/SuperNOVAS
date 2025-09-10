# Legacy example usage

As of version 1.1, the SuperNOVAS library offers a new, more versatile, more intuitive, and more efficient way to 
calculate the astrometric positions (and velocities) of celestial sources, via observing frames (see the 
_Example Usage_ section of the main [`README.md`](https://smithsonian.github.io/SuperNOVAS/doc/README.html)). 
However the old approach of the NOVAS C library remain viable also. This document demonstrates calculating the 
astrometric places of sources the old way, without using the observing frames approach that is now preferred in 
SuperNOVAS.

 - [Calculating positions for a sidereal source](#old-sidereal-example)
 - [Calculating positions for a Solar-system source](#old-solsys-example)


<a name="old-sidereal-example"></a>
## Calculating positions for a sidereal source

A sidereal source may be anything beyond the solar-system with 'fixed' catalog coordinates. It may be a star, or a 
galactic molecular cloud, or a distant quasar. 

### Specify the object of interest

First, you must provide the coordinates (which may include proper motion and parallax). Let's assume we pick a star 
for which we have B1950 (i.e. FK4) coordinates:

```c
 cat_entry source; // Structure to contain information on sidereal source 

 // Let's assume we have B1950 (FK4) coordinates...
 // 16h26m20.1918s, -26d19m23.138s (B1950), proper motion -12.11, -23.30 mas/year, 
 // parallax 5.89 mas, radial velocity -3.4 km/s.
 make_cat_entry("Antares", "FK4", 1, 16.43894213, -26.323094, -12.11, -23.30, 5.89, -3.4, &source);
```

We must convert these coordinates to the now standard ICRS system for calculations in SuperNOVAS, first by calculating 
equivalent J2000 coordinates, by applying the proper motion and the appropriate precession. Then, we apply a small 
adjustment to convert from J2000 to ICRS coordinates.

```c
 // First change the catalog coordinates (in place) to the J2000 (FK5) system... 
 transform_cat(CHANGE_EPOCH, NOVAS_B1950, &source, NOVAS_J2000, "FK5", &source);
  
 // Then convert J2000 coordinates to ICRS (also in place). Here the dates don't matter...
 transform_cat(CHANGE_J2000_TO_ICRS, 0.0, &source, 0.0, "ICRS", &source);
```

(Naturally, you can skip the transformation steps above if you have defined your source in ICRS coordinates from the 
start.)

### Specify the observer location

Next, we define the location where we observe from. Here we can (but don't have to) specify local weather parameters
(temperature and pressure) also for refraction correction later (in this example, we'll skip the weather):

```c
 observer obs;	 // Structure to contain observer location 

 // Specify the location we are observing from
 // 50.7374 deg N, 7.0982 deg E, 60m elevation
 make_observer_on_surface(50.7374, 7.0982, 60.0, 0.0, 0.0, &obs);
```

### Specify the time of observation

We also need to set the time of observation. Our clocks usually measure UTC, but for astrometry we usually need time 
measured based on Terrestrial Time (TT) or Barycentric Time (TDB) or UT1. For a ground-based observer, you will often
have to provide NOVAS with the TT - UT1 time difference, which can be calculated from the current leap seconds and the 
UT1 - UTC time difference (a.k.a. DUT1): 

```c
 // The current value for the leap seconds (UTC - TAI)
 int leap_seconds = 37;

 // Set the DUT1 = UT1 - UTC time difference in seconds (e.g. from IERS Bulletins)
 int dut1 = ...;

 // Calculate the Terrestrial Time (TT) based Julian date of observation (in days)
 // Let's say on 2024-02-06 at 14:53:06 UTC.
 double jd_tt = julian_date(2024, 2, 6, 14.885) + get_utc_to_tt(leap_seconds) / 86400.0; 
  
 // We'll also need the TT - UT1 difference, which we can obtain from what we already
 // defined above
 double ut1_to_tt = get_ut1_to_tt(leap_seconds, dut1);
```

### Specify Earth orientation parameters

Next, you may want to set the small diurnal (sub-arcsec level) corrections to Earth orientation, which are published
in the [IERS Bulletins](https://www.iers.org/IERS/EN/Publications/Bulletins/bulletins.html). The obvious utility of 
these values comes later, when converting positions from the celestial CIRS frame to the Earth-fixed ITRS frame. Less 
obviously, however, it is also needed for calculating the CIO location for CIRS coordinates when a CIO locator file 
is not available, or for calculations sidereal time measures etc. Therefore, it's best to set the pole offsets 
early on:

```c
 // Current polar offsets provided by the IERS Bulletins (in arcsec)
 double dx = ...;
 double dy = ...;
  
 cel_pole(jd_tt, POLE_OFFSETS_X_Y, dx, dy);
```

### Calculate apparent positions on sky

Now we can calculate the precise apparent position (CIRS or TOD) of the source, such as it's right ascension (R.A.) 
and declination, and the equatorial _x,y,z_ unit vector pointing in the direction of the source (in the requested 
coordinate system and for the specified observing location). We also get radial velocity (for spectroscopy), and 
distance (e.g. for apparent-to-physical size conversion):

```c
 sky_pos pos;	// We'll return the observable positions (in CIRS) in this structure
  
 // Calculate the apparent (CIRS) topocentric positions for the above configuration
 int status = place_star(jd_tt, &source, &obs, ut1_to_tt, NOVAS_CIRS, NOVAS_FULL_ACCURACY, &pos);
  
 // You should always check that the calculation was successful...
 if(status) {
   // Oops, something went wrong...
   return -1;
 }
```

The _placement_ of the celestial target in the observer's frame includes appropriate aberration corrections for the
observer's motion, as well as appropriate gravitational deflection corrections due to the Sun and Earth, and for other 
major gravitating solar system bodies (in full precision mode and if a suitable planet provider function is available).

The calculated `sky_pos` structure contains all the information needed about the apparent position of the source at 
the given date/time of observation. We may use it to get true apparent R.A. and declination from it, or to calculate 
azimuth and elevation at the observing location. We'll consider these two cases separately below.


#### A. True apparent R.A. and declination

If you want to know the apparent R.A. and declination coordinates from the `sky_pos` structure you obtained, then you 
can follow with:

```c
  double ra, dec; // [h, deg] We'll return the apparent R.A. [h] and declination [deg] in these
 
  // Convert the rectangular equatorial unit vector to R.A. [h] and declination [deg]
  vector2radec(pos.r_hat, &ra, &dec);
```

Alternatively, you can simply call `radec_star()` instead of `place_star()` to get apparent R.A. and declination in a 
single step if you do not need the `sky_pos` data otherwise. If you followed the less-precise old methodology (Lieske 
et. al. 1977) thus far, calculating TOD coordinates, you are done here. 

If, however, you calculated the position in CIRS with the more precise IAU 2006 methodology (as we did in the example 
above), you have one more step to go still. The CIRS equator is the true equator of date, however its origin (CIO) is 
not the true equinox of date. Thus, we must correct for the difference of the origins to get the true apparent R.A.:

```c
  ra = cirs_to_app_ra(jd_tt, NOVAS_FULL_ACCURACY, ra);
```


#### B. Azimuth and elevation angles at the observing location

If your goal is to calculate the astrometric azimuth and zenith distance (= 90&deg; - elevation) angles of the source 
at the specified observing location (without refraction correction), you can proceed from the `sky_pos` data you 
obtained from `place_star()` as:

```c
 double itrs[3];  // ITRS position vector of source to populate
 double az, zd;   // [deg] local azimuth and zenith distance angles to populate
  
 // Convert CIRS to Earth-fixed ITRS using the pole offsets.
 cirs_to_itrs(jd_tt, 0.0, ut1_to_tt, NOVAS_FULL_ACCURACY, dx, dy, pos.r_hat, itrs);
 
 // Finally convert ITRS to local horizontal coordinates at the observing site
 itrs_to_hor(itrs, &obs.on_surface, &az, &zd);
``` 

Above we used `cirs_to_itrs()` function, and then converted the `sky_pos` rectangular equatorial unit vector 
calculated in CIRS to the Earth-fixed International Terrestrial Reference system (ITRS) using the small (arcsec-level) 
measured variation of the pole (dx, dy) provided explicitly since `cirs_to_itrs()` does not use the values previously 
set via `cel_pole()`. Finally, `itrs_to_hor()` converts the ITRS coordinates to the horizontal system at the observer 
location.

If you followed the old (Lieske et al. 1977) method instead to calculate `sky_pos` in the less precise TOD coordinate
system, then you'd  simply replace the `cirs_to_itrs()` call above with `tod_to_itrs()` accordingly. 

You can additionally apply an approximate optical refraction correction for the astrometric (unrefracted) zenith angle, 
if you want, e.g.:

```c
   zd -= refract_astro(&obs.on_surf, NOVAS_STANDARD_ATMOSPHERE, zd);
```

<a name="old-solsys-example"></a>
## Calculating positions for a Solar-system source

Solar-system sources work similarly to the above with a few important differences.

First, You will have to provide one or more functions to obtain the barycentric ICRS positions for your Solar-system 
source(s) of interest for the specific Barycentric Dynamical Time (TDB) of observation. See section on integrating 
[External Solar-system ephemeris data or services](#solarsystem) with SuperNOVAS. You can specify the functions that 
will handle the respective ephemeris data at runtime before making the NOVAS calls that need them, e.g.:

```c
 // Set the function to use for regular precision planet position calculations
 set_planet_provider(my_planet_function);
  
 // Set the function for high-precision planet position calculations
 set_planet_provider_hp(my_very_precise_planet_function);
  
 // Set the function to use for calculating all sorts of solar-system bodies
 set_ephem_provider(my_ephemeris_provider_function);
```

You can use `tt2tdb()` to convert Terrestrial Time (TT) to Barycentric Dynamic Time (TDB) for your ephemeris provider 
functions (they only differ when you really need extreme precision -- for most applications you can used TT and TDB 
interchangeably in the present era):

```c
 double jd_tdb = jd_tt + tt2tdb(jd_tt) / 86400.0;
```

Instead of `make_cat_entry()` you define your source as an `object` with an name or ID number that is used by the 
ephemeris service you provided. For major planets you might want to use `make_planet()`, if they use a 
`novas_planet_provider` function to access ephemeris data with their NOVAS IDs, or else `make_ephem_object()` for 
more generic ephemeris handling via a user-provided `novas_ephem_provider`. E.g.:

```c
 object mars, ceres; // Hold data on solar-system bodies.
  
 // Mars will be handled by the planet provider function
 make_planet(NOVAS_MARS, &mars);
  
 // Ceres will be handled by the generic ephemeris provider function, which let's say 
 // uses the NAIF ID of 2000001
 make_ephem_object("Ceres", 2000001, &ceres);
```

Other than that, it's the same spiel as before, except using the appropriate `place()` for generic celestial
targets instead of `place_star()` for the sidereal sources (or else `radec_planet()` instead of `radec_star()`). 
E.g.:

```c
 int status = place(jd_tt, &mars, &obs, ut1_to_tt, NOVAS_CIRS, NOVAS_FULL_ACCURACY, &pos);
 if(status) {
   // Oops, something went wrong...
   ...
 }
```

