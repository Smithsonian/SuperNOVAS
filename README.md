![Build Status](https://github.com/Smithsonian/SuperNOVAS/actions/workflows/build.yml/badge.svg)
![Test](https://github.com/Smithsonian/SuperNOVAS/actions/workflows/test.yml/badge.svg)
![Static Analysis](https://github.com/Smithsonian/SuperNOVAS/actions/workflows/check.yml/badge.svg)
<a href="https://smithsonian.github.io/SuperNOVAS.home/apidoc/html/files.html">
 ![API documentation](https://github.com/Smithsonian/SuperNOVAS/actions/workflows/dox.yml/badge.svg)
</a>
<a href="https://codecov.io/gh/Smithsonian/SuperNOVAS">
 ![Coverage Status](https://codecov.io/gh/Smithsonian/SuperNOVAS/graph/badge.svg?token=E11OFOISMW)
</a>

<picture>
  <source srcset="resources/CfA-logo-dark.png" alt="CfA logo" media="(prefers-color-scheme: dark)"/>
  <source srcset="resources/CfA-logo.png" alt="CfA logo" media="(prefers-color-scheme: light)"/>
  <img src="resources/CfA-logo.png" alt="CfA logo" width="400" height="67" align="right"/>
</picture>
<br clear="all">

# SuperNOVAS 
The NOVAS C software library, made better.

[SuperNOVAS](https://github.com/Smithsonian/SuperNOVAS/) is an astronomy software library for the C programming 
language, providing high-precision astrometry such as one might need for running an observatory or a precise 
planetarium program. It is a fork of the Naval Observatory Vector Astrometry Software 
([NOVAS](https://aa.usno.navy.mil/software/novas_info)) version 3.1, providing bug fixes and making it easier to use 
overall.

SuperNOVAS is entirely free to use without any licensing restrictions.  Its source code is compatible with the C90 
standard, and hence should be suitable for many older platforms also. It is light-weight and easy to use, with full 
support for the IAU 2000/2006 standards for sub-microarcsecond position calculations.



# Table of Contents

 - [Introduction](#introduction)
 - [Fixed NOVAS C 3.1 issues](#fixed-issues)
 - [Compatibility with NOVAS C 3.1](#compatibility)
 - [Building and installation](#installation)
 - [Example usage](#examples)
 - [Notes on precision](#precision)
 - [SuperNOVAS specific features](#supernovas-features)
 - [External Solar-system ephemeris data or services](#solarsystem)
 - [Release schedule](#release-schedule)


-----------------------------------------------------------------------------

<a name="introduction"></a>
## Introduction

SuperNOVAS is a fork of the The Naval Observatory Vector Astrometry Software 
([NOVAS](https://aa.usno.navy.mil/software/novas_info)).

The primary goal of SuperNOVAS is to improve on the stock NOVAS C library via:

 - Fixing [outstanding issues](#fixed-issues)
 - Improved [API documentation](https://smithsonian.github.io/SuperNOVAS.home/apidoc/html/files.html).
 - [New features](#added-functionality)
 - [Refining the API](#api-changes) to promote best programing practices.
 - [Thread-safe calculations](#multi-threading).
 - [Debug mode](#debug-mode) with informative error tracing.
 - [Regression testing](https://codecov.io/gh/Smithsonian/SuperNOVAS) and continuous integration on GitHub.

At the same time, SuperNOVAS aims to be fully backward compatible with the intended functionality of the upstream 
NOVAS C library, such that it can be used as a drop-in, _link-time_ replacement for NOVAS in your application without 
having to change existing (functional) code you may have written for NOVAS C.
 
SuperNOVAS is currently based on NOVAS C version 3.1. We plan to rebase SuperNOVAS to the latest upstream release of 
the NOVAS C library, if new releases become available.
 
SuperNOVAS is maintained by Attila Kovacs at the Center for Astrophysics \| Harvard & Smithsonian, and it is 
available through the [Smithsonian/SuperNOVAS](https://github.com/Smithsonian/SuperNOVAS) repository on GitHub.

Outside contributions are very welcome. See
[how you can contribute](https://github.com/Smithsonian/SuperNOVAS/CONTRIBUTING.md) to make SuperNOVAS even better.

Here are some links to other SuperNOVAS related content online:

 - [SuperNOVAS.home](https://smithsonian.github.io/SuperNOVAS.home) page on github.io.
 - [NOVAS](https://aa.usno.navy.mil/software/novas_info) home page at the US Naval Observatory.
 - [SPICE toolkit](https://naif.jpl.nasa.gov/naif/toolkit.html) for integrating Solar-system ephemeris
   via JPL HORIZONS.
 - [IAU Minor Planet Center](https://www.minorplanetcenter.net/iau/mpc.html) provides another source
   of ephemeris data.


-----------------------------------------------------------------------------

<a name="fixed-issues"></a>
## Fixed NOVAS C 3.1 issues

The SuperNOVAS library fixes a number of outstanding issues with NOVAS C 3.1. Here is a list of issues and fixes 
provided by SuperNOVAS over the upstream NOVAS C 3.1 code:

 - Fixes the [sidereal_time bug](https://aa.usno.navy.mil/software/novas_faq), whereby the `sidereal_time()` function 
   had an incorrect unit cast. This was a documented issue of NOVAS C 3.1.
   
 - Some remainder calculations in NOVAS C 3.1 used the result from `fmod()` unchecked, which led to the wrong results 
   when the numerator was negative. This affected the calculation of the mean anomaly in `solsys3.c` (line 261) and 
   the fundamental arguments calculated in `fund_args()` and `ee_ct()` for dates prior to J2000. Less critically, it 
   also was the reason `cal_date()` did not work for negative JD values.
   
 - Fixes antedating velocities and distances for light travel time in `ephemeris()`. When getting positions and 
   velocities for Solar-system sources, it is important to use the values from the time light originated from the 
   observed body rather than at the time that light arrives to the observer. This correction was done properly for 
   positions, but not for velocities or distances, resulting in incorrect observed radial velocities or apparent 
   distances being reported for spectroscopic observations or for angular-physical size conversions. 
   
 - Fixes bug in `ira_equinox()` which may return the result for the wrong type of equinox (mean vs. true) if the 
   `equinox` argument was changing from 1 to 0, and back to 1 again with the date being held the same. This affected 
   routines downstream also, such as `sidereal_time()`.
   
 - Fixes accuracy switching bug in `cio_basis()`, `cio_location()`, `ecl2equ()`, `equ2ecl_vec()`, `ecl2equ_vec()`, 
   `geo_posvel()`, `place()`, and `sidereal_time()`. All these functions returned a cached value for the other 
   accuracy if the other input parameters are the same as a prior call, except the accuracy. 
   
 - Fixes multiple bugs related to using cached values in `cio_basis()` with alternating CIO location reference 
   systems. This affected many CIRS-based position calculations downstream.
   
 - Fixes bug in `equ2ecl_vec()` and `ecl2equ_vec()` whereby a query with `coord_sys = 2` (GCRS) has overwritten the
   cached mean obliquity value for `coord_sys = 0` (mean equinox of date). As a result, a subsequent call with
   `coord_sys = 0` and the same date as before would return the results GCRS coordinates instead of the requested mean 
   equinox of date coordinates.
 
 - Fixes `aberration()` returning NaN vectors if the `ve` argument is 0. It now returns the unmodified input vector 
   appropriately instead.
   
 - Fixes unpopulated `az` output value in `equ2hor()` at zenith. While any azimuth is acceptable really, it results in 
   unpredictable behavior. Hence, we set `az` to 0.0 for zenith to be consistent.
   
 - Fixes potential string overflows and eliminates associated compiler warnings.
 
 - Fixes the [ephem_close bug](https://aa.usno.navy.mil/software/novas_faq), whereby `ephem_close()` in 
   `eph_manager.c` did not reset the `EPHFILE` pointer to NULL. This was a documented issue of NOVAS C 3.1.

-----------------------------------------------------------------------------

<a name="compatibility"></a>
## Compatibility with NOVAS C 3.1

SuperNOVAS strives to maintain API compatibility with the upstream NOVAS C 3.1 library, but not binary compatibility. 
In practical terms it means that you cannot simply drop-in replace your compiled objects (e.g. `novas.o`), or the 
static (e.g. `novas.a`) or shared (e.g. `novas.so`) libraries, from NOVAS C 3.1 with that from SuperNOVAS. Instead, 
you will need to (re)compile and or (re)link your application with the SuperNOVAS versions of these. 

This is because some function signatures have changed, e.g. to use an `enum` argument instead of the nondescript 
`short int` argument of NOVAS C 3.1, or because we added a return value to a function that was declared `void` in 
NOVAS C 3.1. We also changed the `object` structure to contain a `long` ID number instead of `short` to accommodate 
JPL NAIF codes, for which 16-bit storage is insufficient. 


-----------------------------------------------------------------------------


<a name="installation"></a>
## Building and installation


The SuperNOVAS distribution contains a GNU `Makefile`, which is suitable for compiling the library (as well as local 
documentation, and tests, etc.) on POSIX systems such as Linux, BSD, MacOS X, or Cygwin or WSL. (At this point we do 
not provide a similar native build setup for Windows, but speak up if you would like to add it yourself!)

Before compiling the library take a look a `config.mk` and edit it as necessary for your needs, such as:

 - Choose which planet calculator function routines are built into the library (for example to provide 
   `earth_sun_calc()` set `BUILTIN_SOLSYS3 = 1`  and/or for `planet_ephem_provider()` set `BUILTIN_SOLSYS_EPHEM = 1`. 
   You can then specify these functions as the default planet calculator for `ephemeris()` in your application 
   dynamically via `set_planet_provider()`.
   
 - Choose which stock planetary calculator module (if any) should provide a default `solarsystem()` implementation for 
   `ephemeris()` calls by setting `DEFAULT_SOLSYS` to 1 -- 3 for `solsys1.c` trough `solsys3.c`, respectively. If you 
   want to link your own `solarsystem()` implementation(s) against the library, you should not set `DEFAULT_SOLSYS` 
   (i.e. delete or comment out the corresponding line or else set `DEFAULT_SOLSYS` to 0).
   
 - You may also want to specify the source file that will provide the `readeph()` implementation for it by setting 
   `DEFAULT_READEPH` appropriately. (The default setting uses the dummy `readeph0.c` which simply returns an error if 
   one tries to use the functions from `solsys1.c`). Note, that a `readeph()` implementation is not always necessary 
   and you can provide a superior ephemeris reader implementation at runtime via the `set_ephem_provider()` call.

 - If you want to use the CIO locator binary file for `cio_location()`, you can specify the path to the binary file 
   (e.g. `/usr/local/share/novas/cio_ra.bin`) on your system. (The CIO locator file is not at all necessary for the 
   functioning of the library, unless you specifically require CIO positions relative to GCRS.)
   
 - If your compiler does not support the C11 standard and it is not GCC &gt;=3.3, but provides some non-standard
   support for declaring thread-local variables, you may want to pass the keyword to use to declare variables as
   thread local via `-DTHREAD_LOCAL=...` added to `CFLAGS`. (Don't forget to enclose the string value in escaped
   quotes.)

Now you are ready to build the library:

```bash
  $ make
```

will compile the static (`lib/novas.a`) and shared (`lib/novas.so`) libraries, produce a CIO locator data file 
(`tools/data/cio_ra.bin`), and compile the API documentation (into `apidoc/`) using `doxygen`. Alternatively, you can 
build select components of the above with the `make` targets `static`, `shared`, `cio_file`, and `dox` respectively.

After building the library you can install the above components to the desired locations on your system. For a 
system-wide install you may place the static or shared library into `/usr/loval/lib/`, copy the CIO locator file to 
the place you specified in `config.mk` etc. You may also want to copy the header files in `include/` to e.g. 
`/usr/local/include` so you can compile your application against SuperNOVAS easily on your system.


### Building your application with SuperNOVAS

Provided you have installed the SuperNOVAS headers into a standard location (such as `/usr/include` or 
`/usr/local/include`) and the static or shared library into `usr/lib` (or `/usr/local/lib` or similar), you
can build your application against it very easily. For example, to build `myastroapp.c` against SuperNOVAS, 
you might have a `Makefile` with contents like:

```make
  myastroapp: myastroapp.c 
  	$(CC) -o $@ $(CFLAGS) $^ -lm -lnovas
```

If you have a legacy NOVAS C 3.1 application, it is possible that the compilation will give you errors due
to missing includes for `stdio.h`, `stdlib.h`, `ctype.h` or `string.h`. This is because these were explicitly
included in `novas.h` in NOVAS C 3.1, but not in SuperNOVAS (at least not by default), as a matter of best
practice. If this is a problem for you can 'fix' it in one of two ways: (1) Add the missing `#include` 
directives to your application source explicitly, or if that's not an option for you, then (2) set the 
`-DCOMPAT=1` compiler flag when compiling your application:

```make
  myastroapp: myastroapp.c 
  	$(CC) -o $@ $(CFLAGS) -DCOMPAT=1 $^ -lm -lnovas
```

To use your own `solarsystem()` implemetation for `ephemeris()`, you will want to build the library with
`DEFAULT_SOLSYS` not set (or else set to 0) in `config.mk` (see section above), and your applications 
`Makefile` may contain something like:

```make
  myastroapp: myastroapp.c my_solsys.c 
  	$(CC) -o $@ $(CFLAGS) $^ -lm -lnovas
```

The same principle applies to using your specific `readeph()` implementation (only with `DEFAULT_READEPH` 
being unset in `config.mk`).

-----------------------------------------------------------------------------


<a name="examples"></a>
## Example usage

 - [Note on alternative methodologies](#methodologies)
 - [Calculating positions for a sidereal source](#sidereal-example)
 - [Calculating positions for a Solar-system source](#solsys-example)
 - [Reduced accuracy shortcuts](#accuracy-notes)
 - [Performance considerations](#performance-note)


<a name="methodologies"></a>
### Note on alternative methodologies

The IAU 2000 and 2006 resolutions have completely overhauled the system of astronomical coordinate transformations
to enable higher precision astrometry. (Super)NOVAS supports coordinate calculations both in the old (pre IAU 2000) 
ways, and in the new IAU standard method. Here is an overview of how the old and new methods define some of the
terms differently:

 | Concept                    | Old standard                  | New IAU standard                                  |
 | -------------------------- | ----------------------------- | ------------------------------------------------- |
 | Catalog coordinate system  | FK4, FK5, HIP...              | International Celestial Reference System (ICRS)   |
 | Dynamical system           | True of Date (TOD)            | Celestial Intermediate Reference System (CIRS)    |
 | Dynamical R.A. origin      | equinox of date               | Celestial Intermediate Origin (CIO)               |
 | Precession, nutation, bias | separate, no tidal terms      | IAU 2006 precession/nutation model                |
 | Celestial Pole offsets     | d&psi;, d&epsilon;            | _dx_, _dy_                                        |
 | Earth rotation measure     | Greenwich Sidereal Time (GST) | Earth Rotation Angle (ERA)                        |
 | Fixed Earth System         | WGS84                         | International Terrestrial Reference System (ITRS) |
 
See the various enums and constants defined in `novas.h`, as well as the descriptions on the various NOVAS routines
on how they are appropriate for the old and new methodologies respectively.

In NOVAS, the barycentric BCRS and the geocentric GCRS systems are effectively synonymous to ICRS. The origin for
positions and for velocities, in any reference system, is determined by the `observer` location in the vicinity of 
Earth (at the geocenter, on the surface, or in Earth orbit).


<a name="sidereal-example"></a>
### Calculating positions for a sidereal source

A sidereal source may be anything beyond the solar-system with 'fixed' catalog coordinates. It may be a star, or a 
galactic molecular cloud, or a distant quasar. First, you must provide the coordinates (which may include proper 
motion and parallax). Let's assume we pick a star for which we have B1950 (i.e. FK4) coordinates:

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

Next, we define the location where we observe from. Here we can (but don't have to) specify local weather parameters
(temperature and pressure) also for refraction correction later (in this example, we'll skip the weather):

```c
 observer obs;	 // Structure to contain observer location 

 // Specify the location we are observing from
 // 50.7374 deg N, 7.0982 deg E, 60m elevation
 make_observer_on_surface(50.7374, 7.0982, 60.0, 0.0, 0.0, &obs);
```

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
 double ut1_to_tt = get_ut1_to_tt(jd_tt, dut1);
```

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
  double ra_cio;  // [h] R.A. of the CIO (from the true equinox) we'll calculate

  // Obtain the R.A. [h] of the CIO at the given date
  cio_ra(jd_tt, NOVAS_FULL_ACCURACY, &ra_cio);
  
  // Convert CIRS R.A. to true apparent R.A., keeping the result in the [0:24] h range
  ra = remainder(ra + ra_cio, 24.0);
  if(ra < 0.0) ra += 24.0;
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

Above we used `cirs_to_itrs()` function then convert the `sky_pos` rectangular equatorial unit vector calculated 
in CIRS to the Earth-fixed International Terrestrial Reference system (ITRS) using the small (arcsec-level) measured 
variation of the pole (dx, dy) provided explicitly since `cirs_to_itrs()` does not use the values previously set via 
`cel_pole()`. Finally, `itrs_to_hor()` converts the ITRS coordinates to the horizontal system at the observer 
location.

If you followed the old (Lieske et al. 1977) method instead to calculate `sky_pos` in the less precise TOD coordinate
system, then you'd  simply replace the `cirs_to_itrs()` call above with `tod_to_itrs()` accordingly. 

You can additionally apply an approximate optical refraction correction for the astrometric (unrefracted) zenith angle, 
if you want, e.g.:

```c
   zd -= refract_astro(&obs.on_surf, NOVAS_STANDARD_ATMOSPHERE, zd);
```


<a name="solsys-example"></a>
### Calculating positions for a Solar-system source

Solar-system sources work similarly to the above with a few important differences.

First, You will have to provide one or more functions to obtain the barycentric ICRS positions for your Solar-system 
source(s) of interest for the specific Barycentric Dynamical Time (TDB) of observation. See section on integrating 
[External Solar-system ephemeris data or services](#solarsystem) with SuperNOVAS. You can specify the functions that 
will handle the respective ephemeris data at runtime before making the NOVAS calls that need them, e.g.:

```c
 // Set the function to use for regular precision planet position calculations
 set_planet_provider(my_planet_function);
  
 // Set the function for high-precision planet position calculations
 set_planet_provider(my_very_precise_planet_function);
  
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
ephemeris service you provided. For major planets you might want to use `make_planet(), if they use a 
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


<a name="accuracy-notes"></a>
### Reduced accuracy shortcuts

When one does not need positions at the microarcsecond level, some shortcuts can be made to the recipe above:

 - You can use TT and TDB timescales interchangably in the present era unless you require the utmost precision.
 - You can use `NOVAS_REDUCED_ACCURACY` instead of `NOVAS_FULL_ACCURACY` for the calculations. This typically has an 
   effect at the milliarcsecond level only, but may be much faster to calculate.
 - You can skip the J2000 to ICRS conversion and use J2000 coordinates directly as a fair approximation (at the 
   &lt;= 22 mas level).
 - You might skip the pole offsets dx, dy. These are tenths of arcsec, typically.
 
<a name="performance-note"></a>
### Performance considerations

Some of the calculations involved can be expensive from a computational perspective. For the most typical use case
however, NOVAS (and SuperNOVAS) has a trick up its sleeve: it caches the last result of intensive calculations so they 
may be re-used if the call is made with the same environmental parameters again (such as JD time and accuracy). 
Therefore, when calculating positions for a large number of sources at different times:

 - It is best to iterate over the sources in the inner loop while keeping the time fixed. 
 - You probably want to stick to one accuracy mode (`NOVAS_FULL_ACCURACY` or `NOVAS_REDUCED_ACCURACY`) to prevent
   re-calculating the same quantities repeatedly to alternating precision.
 - If super-high accuracy is not required `NOVAS_REDUCED_ACCURACY` mode offers much faster calculations, in general.
 
<a name="multi-threading"></a>
### Multi-threaded calculations
 
A direct consequence of the caching of results in NOVAS is that calculations are generally not thread-safe as 
implemented by the original NOVAS C 3.1 library. One thread may be in the process of returning cached values for one 
set of input parameters while, at the same time, another thread is saving cached values for a different set of 
parameters. Thus, when running calculations in more than one thread, the results returned may at times be incorrect, 
or more precisely they may not correspond to the requested input parameters.
 
While you should never call NOVAS C from  multiple threads simultaneously, SuperNOVAS caches the results in thread
local variables (provided your compiler supports it), and is therefore safe to use in multi-threaded applications.
Just make sure that you:

 - use a compiler which supports the C11 language standard;
 - or, compile with GCC &gt;= 3.3;
 - or else, set the appropriate non-standard keyword to use for declaring thread-local variables for your compiler in 
   `config.mk` or in your equivalent build setup.
 
 
-----------------------------------------------------------------------------

## Usage 

<a name="precision"></a>
## Notes on precision

The SuperNOVAS library is in principle capable of calculating positions to sub-microarcsecond, and velocities to mm/s 
precision for all types of celestial sources. However, there are certain prerequisites and practical considerations 
before that level of accuracy is reached.


 1. __IAU 2000/2006 conventions__: High precision calculations will generally require that you use SuperNOVAS with the
    new IAU standard quantities and methods. The old ways were simply not suited for precision much below the 
    milliarcsecond level.

 2. __Earth's polar motion__: Calculating precise positions for any Earth-based observations requires precise 
    knowledge of Earth orientation at the time of observation. The pole is subject to predictable precession and 
    nutation, but also small irregular variations in the orientation of the rotational axis and the rotation period 
    (a.k.a polar wobble). The [IERS Bulletins](https://www.iers.org/IERS/EN/Publications/Bulletins/bulletins.html) 
    provide up-to-date measurements, historical data, and near-term projections for the polar offsets and the UT1-UTC 
    (DUT1) time difference and leap-seconds (UTC-TAI). In SuperNOVAS you can use `cel_pole()` and `get_ut1_to_tt()` 
    functions to apply / use the published values from these to improve the astrometric precision of Earth-orientation
    based coordinate calculations. Without setting and using the actual polar offset values for the time of 
    observation, positions for Earth-based observations will be accurate at the tenths of arcsecond level only.
 
 3. __Solar-system sources__: Precise calculations for Solar-system sources requires precise ephemeris data for both
    the target object as well as for Earth, and the Sun vs the Solar-system barycenter. For the highest precision 
    calculations you also need positions for all major planets to calculate gravitational deflection precisely. By 
    default SuperNOVAS can only provide approximate positions for the Earth and Sun (see `earth_sun_calc()` in 
    `solsys3.c`), but certainly not at the sub-microarcsecond level, and not for other solar-system sources. You will 
    need to provide a way to interface SuperNOVAS with a suitable ephemeris source (such as the CSPICE toolkit from 
    JPL) if you want to use it to obtain precise positions for Solar-system bodies. See the 
    [section further below](#solarsystem) for more information how you can do that.
    
  4. __Refraction__: Ground based observations are also subject to atmospheric refraction. SuperNOVAS offers the 
    option to include approximate _optical_ refraction corrections either for a standard atmosphere or more precisely 
    using the weather parameters defined in the `on_surface` data structure that specifies the observer locations. 
    Note, that refraction at radio wavelengths is notably different from the included optical model. In any case you 
    may want to skip the refraction corrections offered in this library, and instead implement your own as appropriate 
    (or not at all).
  


-----------------------------------------------------------------------------


<a name="supernovas-features"></a>
## SuperNOVAS specific features

- [Newly added functionality](#added-functionality)
- [Refinements to the NOVAS C API](#api-changes)


<a name="added-functionality"></a>
### Newly added functionality

 <a name="debug-mode"></a>
 - Changed to [support for calculations in parallel threads](#multi-threading) by making cached results thread-local.
   This works using the C11 standard `_Thread_local` or else the earlier GNU C &gt;= 3.3 standard `__thread` modifier.
   You can also set the preferred thread-local keyword for your compiler by passing it via `-DTHREAD_LOCAL=...` in 
   `config.mk` to ensure that your build is thread-safe. And, if your compiler has no support whatsoever for
   thread_local variables, then SuperNOVAS will not be thread-safe, just as NOVAS C isn't.
 
 - New debug mode and error traces. Simply call `novas_debug(NOVAS_DEBUG_ON)` or `novas_debug(NOVAS_DEBUG_EXTRA)`
   to enable. When enabled, any error condition (such as NULL pointer arguments, or invalid input values etc.) will
   be reported to the standard error, complete with call tracing within the SuperNOVAS library, s.t. users can have
   a better idea of what exactly did not go to plan (and where). The debug messages can be disabled by passing
   `NOVAS_DEBUF_OFF` (0) as the argument to the same call. Here is an example error trace when your application
   calls `grav_def()` with `NOVAS_FULL_ACCURACY` while `solsys3` provides Earth and Sun positions only and when debug 
   mode is `NOVAS_DEBUG_EXTRA` (otherwise we'll ignore that we skipped the almost always negligible deflection due to 
   planets):
   ```
    ERROR! earth_sun_calc: invalid or unsupported planet number: 5 [=> 2]
         @ earth_sun_calc_hp [=> 2]
         @ solarsystem_hp [=> 2]
         @ ephemeris:planet [=> 12]
         @ grav_def:Jupiter [=> 12]
   ```
   
 - New runtime configuration:

   * The planet position calculator function used by `ephemeris()` can be set at runtime via `set_planet_provider()`, 
     and `set_planet_provider_hp()` (for high precision calculations). Similarly, if `planet_ephem_provider()` or 
     `planet_ephem_provider_hp()` (defined in `solsys-ephem.c`) are set as the planet calculator functions, then 
     `set_ephem_provider()` can set the user-specified function to use with these to actually read ephemeris data
     (e.g. from a JPL `.bsp` file).
 
   * If CIO locations vs GSRS are important to the user, the user may call `set_cio_locator_file()` at runtime to
     specify the location of the binary CIO interpolation table (e.g. `cio_ra.bin`) to use, even if the library was
     compiled with the different default CIO locator path. 
 
   * The default low-precision nutation calculator `nu2000k()` can be replaced by another suitable IAU 2006 nutation
     approximation via `set_nutation_lp_provider()`. For example, the user may want to use the `iau2000b()` model 
     or some custom algorithm instead.
 
 - New intuitive XYZ coordinate conversion functions:
   * for GCRS - CIRS - ITRS (IAU 2000 standard): `gcrs_to_cirs()`, `cirs_to_itrs()`, and `itrs_to_cirs()`, 
     `cirs_to_gcrs()`.
   * for GCRS - J2000 - TOD - ITRS (old methodology): `gcrs_to_j2000()`, `j2000_to_tod()`, `tod_to_itrs()`, and 
     `itrs_to_tod()`, `tod_to_j2000()`, `j2000_to_gcrs()`.

 - New `itrs_to_hor()` and `hor_to_itrs()` to convert Earth-fixed ITRS coordinates to astrometric azimuth and 
   elevation or back. Whereas `tod_to_itrs()` followed by `itrs_to_hor()` is effectively a just a more explicit 2-step 
   version of the existing `equ2hor()` for converting from TOD to to local horizontal (old methodology), the 
   `cirs_to_itrs()`  followed by `itrs_to_hor()` does the same from CIRS (new IAU standard methodology), and had no 
   prior equivalent in NOVAS C 3.1.
   
 - New `ecl2equ()` for converting ecliptic coordinates to equatorial, complementing existing `equ2ecl()`.
   
 - New `gal2equ()` for converting galactic coordinates to ICRS equatorial, complementing existing `equ2gal()`.
   
 - New `refract_astro()` complements the existing `refract()` but takes an unrefracted (astrometric) zenith angle as 
   its argument.

 - New convenience functions to wrap `place()` for simpler specific use: `place_star()`, `place_icrs()`, 
   `place_gcrs()`, `place_cirs()`, and `place_tod()`.
   
 - New `radec_star()` and `radec_planet()` as the common point for existing functions such as `astro_star()`, 
   `local_star()`, `virtual_planet()`, `topo_planet()` etc.
 
 - New time conversion utilities `tt2tdb()`, `get_utc_to_tt()`, and `get_ut1_to_tt()` make it simpler to convert 
   between UTC, UT1, TT, and TDB time scales, and to supply `ut1_to_tt` arguments to `place()` or topocentric 
   calculations.
 
 - Co-existing `solarsystem()` variants. It is possible to use the different `solarsystem()` implementations 
   provided by `solsys1.c`, `solsys2.c`, `solsys3.c` and/or `solsys-ephem.c` side-by-side, as they define their
   functionalities with distinct, non-conflicting names, e.g. `earth_sun_calc()` vs `planet_jplint()` vs
   `planet_eph_manager` vs `planet_ephem_provider()`. See the section on 
   [Building and installation](#installation) further above on including a selection of these in your library 
   build.)

 - New `novas_case_sensitive(int)` to enable (or disable) case-sensitive processing of object names. (By default NOVAS 
   `object` names were converted to upper-case, making them effectively case-insensitive.)

 - New `make_planet()` and `make_ephem_object()` to make it simpler to configure Solar-system objects.


<a name="api-changes"></a>
### Refinements to the NOVAS C API

 - SuperNOVAS functions take `enum`s as their option arguments instead of raw integers. These enums are defined in 
   `novas.h`. The same header also defines a number of useful constants. The enums allow for some compiler checking, 
   and make for more readable code that is easier to debug. They also make it easy to see what choices are available
   for each function argument, without having to consult the documentation each and every time.

 - All SuperNOVAS functions check for the basic validity of the supplied arguments (Such as NULL pointers or illegal 
   duplicate arguments) and will return -1 (with `errno` set, usually to `EINVAL`) if the arguments supplied are
   invalid (unless the NOVAS C API already defined a different return value for specific cases. If so, the NOVAS C
   error code is returned for compatibility).
   
 - All erroneous returns now set `errno` so that users can track the source of the error in the standard C way and
   use functions such as `perror()` and `strerror()` to print human-readable error messages.

 - SuperNOVAS prototypes declare function pointer arguments as `const` whenever the function does not modify the
   data content being pointed at. This supports better programming practices that generally aim to avoid unintended 
   data modifications.

 - Many SuperNOVAS functions allow `NULL` arguments, both for optional input values as well as outputs that are not 
   required (see the [API Documentation](https://smithsonian.github.io/SuperNOVAS.home/apidoc/html/) for specifics).
   This eliminates the need to declare dummy variables in your application code.
  
 - Many output values supplied via pointers are set to clearly invalid values in case of erroneous returns, such as
   `NAN` so that even if the caller forgets to check the error code, it becomes obvious that the values returned
   should not be used as if they were valid. (No more sneaky silent failures.)

 - All SuperNOVAS functions that take an input vector to produce an output vector allow the output vector argument
   be the same as the input vector argument. For example, `frame_tie(pos, J2000_TO_ICRS, pos)` using the same 
   `pos` vector both as the input and the output. In this case the `pos` vector is modified in place by the call. 
   This can greatly simplify usage, and eliminate extraneous declarations, when intermediates are not required.

 - Catalog names can be up to 6 bytes (including termination), up from 4 in NOVAS C, while keeping `struct` layouts 
   the same as NOVAS C thanks to alignment, thus allowing cross-compatible binary exchange of `cat_entry` records
   with NOVAS C 3.1.

 - Changed `make_object()` to retain the specified number argument (which can be different from the `starnumber` value
   in the supplied `cat_entry` structure).
   
 - `cio_location()` will always return a valid value as long as neither output pointer argument is NULL.

 - `cel2ter()` and `ter2cel()` can now process 'option'/'class' = 1 (`NOVAS_REFERENCE_CLASS`) regardless of the
   methodology (`EROT_ERA` or `EROT_GST`) used to input or output coordinates in GCRS.
   
 - More efficient paging (cache management) for `cio_array()`, including I/O error checking.
 
 - IAU 2000A nutation model uses higher-order Delaunay arguments provided by `fund_args()`, instead of the linear
   model in NOVAS C 3.1.
   
 - IAU 2000 nutation made a bit faster, reducing the the number of floating-point multiplications necessary by 
   skipping terms that do not contribute. Its coefficients are also packed more frugally in memory, resulting in a
   smaller foortprint.
   
 - Changed the standard atmospheric model for (optical) refraction calculation to include a simple model for the 
   annual average temperature at the site (based on latitude and elevation). This results is a slightly more educated 
   guess of the actual refraction than the global fixed temperature of 10 &deg;C assumed by NOVAC C 3.1 regardless of 
   observing location.



-----------------------------------------------------------------------------

<a name="solarsystem"></a>
## External Solar-system ephemeris data or services


If you want to use SuperNOVAS to calculate positions for a range of Solar-system objects, and/or to do it with 
sufficient precision, you will have to integrate it with a suitable provider of ephemeris data, such as 
[JPL Horizons](https://ssd.jpl.nasa.gov/horizons/app.html#/) or the 
[Minor Planet Center](https://www.minorplanetcenter.net/iau/mpc.html). Given the NOVAS C heritage, and some added 
SuperNOVAS flexibility in this area, you have several options on doing that. These are listed from the most flexible 
(and preferred) to the least flexible (old ways).

 1. [Universal ephemeris data / service integration](#universal-ephemerides)
 2. [Built-in support for (old) JPL major planet ephemerides](#builtin-ephem-readers)
 3. [Explicit linking of custom ephemeris functions](#explicit-ephem-linking)


<a name="universal-ephemerides"></a>
### 1. Universal ephemeris data / service integration 

Possibly the most universal way to integrate ephemeris data with SuperNOVAS is to write your own 
`novas_ephem_provider`, e.g.:

```c
 int my_ephem_reader(const char *name, long id, double jd_tdb_high, double jd_tdb_low, 
                     enum novas_origin *origin, double *pos, double *vel) {
   // Your custom ephemeris reader implementation here
   ...
 }
```

which takes an object ID number (such as a NAIF) an object name, and a split TDB date (for precision) as it inputs, 
and returns the type of origin with corresponding ICRS position and velocity vectors in the supplied pointer locations. 
The function can use either the ID number or the name to identify the object or file (whatever is the most appropriate 
for the implementation). The positions and velocities may be returned either relative to the SSB or relative to the 
heliocenter, and accordingly, your function should set the value pointed at by origin to `NOVAS_BARYCENTER` or 
`NOVAS_HELIOCENTER` accordingly. Positions and velocities are rectangular ICRS _x,y,z_ vectors in units of AU and 
AU/day respectively. 

This way you can easily integrate current ephemeris data for JPL Horizons, e.g. using the CSPICE toolkit, or for the 
Minor Planet Center (MPC), or whatever other ephemeris service you prefer.

Once you have your adapter function, you can set it as your ephemeris service via `set_ephem_provider()`:

```c
 set_ephem_provider(my_ephem_reader);
```

By default, your custom `my_ephem_reader` function will be used for 'minor planets' only (i.e. anything other than the 
major planets, the Sun, Moon, and the Solar System Barycenter). And, you can use the same function for the mentioned 
'major planets' also via:

```c
 set_planet_provider(planet_ephem_provider);
 set_planet_provider_hp(planet_ephem_provider_hp);
```

provided you compiled SuperNOVAS with `BUILTIN_SOLSYS_EPHEM = 1` (in `config.mk`), or else you link your code against
`solsys-ephem.c` explicitly. Easy-peasy.


<a name="builtin-ephem-readers"></a>
### 2. Built-in support for (old) JPL major planet ephemerides

If you only need support for major planets, you may be able to use one of the modules included in the SuperNOVAS
distribution. The modules `solsys1.c` and `solsys2.c` provide built-in support to older JPL ephemerides (DE200 to DE421), 
either via the `eph_manager` interface of `solsys1.c` or via the FORTRAN `pleph` interface with `solsys2.c`.

#### 2.1. Planets via `eph_manager`

To use the `eph_manager` interface for planet 1997 JPL planet ephemeris (DE200 through DE421), you must either build 
superNOVAS with `BUILTIN_SOLSYS1 = 1` in `config.mk`, or else link your application with `solsys1.c` and 
`eph_manager.c` from SuperNOVAS explicitly. If you want `eph_manager` to be your default ephemeris provider (the old 
way) you might also want to set `DEFAULT_SOLSYS = 1` in `config.mk`. Otherwise, your application should set 
`eph_manager` as your planetary ephemeris provider at runtime via:

```c
 set_planet_provider(planet_eph_manager);
 set_planet_provider_hp(planet_eph_manager_hp);
```

Either way, before you can use the ephemeris, you must also open the relevant ephemeris data file with `ephem_open()`:

```c
 int de_number;	         // The DE number, e.g. 405 for DE405
 double from_jd, to_jd;  // [day] Julian date range of the ephemeris data
  
 ephem_open("path-to/de405.bsp", &from_jd, &to_jd, &de_number);
```

And, when you are done using the ephemeris file, you should close it with

```c
 ephem_close();
```
 
Note, that at any given time `eph_manager` can have only one ephemeris data file opened. You cannot use it to 
retrieve data from multiple ephemeris input files at the same time. (But you can with the CSPICE toolkit, which you 
can integrate as discussed further above!)
That's all, except the warning that this method will not work with newer JPL ephemeris data, beyond DE421.


#### 2.b. Planets via JPL's `pleph` FORTRAN interface

To use the FORTRAN `pleph` interface for planet ephemerides, you must either build SuperNOVAS with 
`BUILTIN_SOLSYS2 = 1` in `config.mk`, or else link your application with `solsys2.c` and `jplint.f` from SuperNOVAS 
explicitly (as well as `pleph.f` etc. from the JPL library). If you want the JPL `pleph`-based interface  to be your 
default ephemeris provider (the old way) you might also want to set `DEFAULT_SOLSYS = 2` in `config.mk`. Otherwise, 
your application should set your planetary ephemeris provider at runtime via:

```c
 set_planet_provider(planet_jplint);
 set_planet_provider_hp(planet_jplint_hp);
```

Integrating JPL ephemeris data this way can be arduous. You will need to compile and link FORTRAN with C (not the end
of the world), but you may also have to modify `jplint.f` (providing the intermediate FORTRAN `jplint_` / `jplihp_()`
interfaces to `pleph`) to work with the version of `pleph.f` that you will be using. Unless you already have code that 
relies on this method, you are probably better off choosing one of the other ways for integrating planetary ephemeris 
data with SuperNOVAS.

<a name="explicit-ephem-linking"></a>
### 3. Explicit linking of custom ephemeris functions

Finally, if none of the above is appealing, and you are fond of the old ways, you may compile SuperNOVAS with the 
`DEFAULT_SOLSYS` option disabled (commented, removed, or else set to 0), and then link your own implementation of
`solarsystem()` and `solarsystem_hp()` calls with your application. 

For Solar-system objects other than the major planets, you may also provide your own `readeph()` implementation. (In
this case you will want to set `DEFAULT_READEPH` in `config.mk` to specify your source code for that function before
building the SuperNOVAS library, or else disable that option entirely (e.g. by commenting or removing it), and link
your application explicitly with your `readeph()` implementation.

The downside of this approach is that your SuperNOVAS library will not be usable without invariably providing a
`solarsystem()` / `solarsystem_hp()` and/or `readeph()` implementations for _every_ application that you will want
to use SuperNOVAS with. This is why the runtime configuration of the ephemeris provider functions is the best and
most generic way to add your preferred implementations while also providing some minimum default implementations for
_other users_ of the library, who may not need _your_ ephemeris service, or have no need for planet data beyond the 
approximate positions for the Earth and Sun.



-----------------------------------------------------------------------------


<a name="release-schedule"></a>
## Release schedule

A predictable release schedule and process can help manage expectations and reduce stress on adopters and developers 
alike.

Releases of the library shall follow a quarterly release schedule. You may expect upcoming releases 
to be published around __March 1__, __June 1__, __September 1__, and/or __December 1__ each year, on an as-needed
basis. That means that if there are outstanding bugs, or new pull requests (PRs), you may expect a release that 
addresses these in the upcoming quarter. The dates are placeholders only, with no guarantee that a new release will 
actually be available every quarter. If nothing of note comes up, a potential release date may pass without a release 
being published.

_Feature releases_ (e.g. __1.x.0__ version bumps) are provided at least 6 months apart, to reduce stress on adopters 
who may need/want to tweak their code to integrate these. Between feature releases, _bug fix releases_ (without 
significant API changes) may be provided as needed to address issues. New features are generally reserved for the 
feature releases, although they may also be rolled out in bug-fix releases as long as they do not affect the existing 
API -- in line with the desire to keep bug-fix releases fully backwards compatible with their parent versions.

In the weeks and month(s) preceding releases one or more _release candidates_ (e.g. `1.0.1-rc3`) will be published 
temporarily on github, under [Releases](https://github.com/Smithsonian/SuperNOVAS/releases), so that changes can be 
tested by adopters before the releases are finalized. Please use due diligence to test such release candidates with 
your code when they become available to avoid unexpected surprises when the finalized release is published. Release 
candidates are typically available for one week only before they are superseded either by another, or by the finalized 
release.


