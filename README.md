![Build Status](https://github.com/Smithsonian/SuperNOVAS/actions/workflows/build.yml/badge.svg)
![Test](https://github.com/Smithsonian/SuperNOVAS/actions/workflows/test.yml/badge.svg)
![Static Analysis](https://github.com/Smithsonian/SuperNOVAS/actions/workflows/check.yml/badge.svg)
![API documentation](https://github.com/Smithsonian/SuperNOVAS/actions/workflows/dox.yml/badge.svg)

<picture>
  <source srcset="resources/CfA-logo-dark.png" alt="CfA logo" media="(prefers-color-scheme: dark)"/>
  <source srcset="resources/CfA-logo.png" alt="CfA logo" media="(prefers-color-scheme: light)"/>
  <img src="resources/CfA-logo.png" alt="CfA logo" width="400" height="66" align="right"/>
</picture>
<br clear="all">

# SuperNOVAS 
The Naval Observatory NOVAS C library, made better.

SuperNOVAS is a fork of the Naval Observatory Vector Astrometry Software 
([NOVAS](https://aa.usno.navy.mil/software/novas_info)) package, with the overall aim of making it more user-friendly 
and easier to use. It is entirely free to use without any licensing restrictions. 


# Table of Contents

 - [Introduction](#introduction)
 - [Related links](#related-links)
 - [Compatibility with NOVAS C 3.1](#compatibility)
 - [Fixed NOVAS C 3.1 issues](#fixed-issues)
 - [Building and installation](#installation)
 - [Building your application with SuperNOVAS](#building-your-application)
 - [Basic usage examples](#examples)
 - [SuperNOVAS specific features](#supernovas-features)
 - [Notes on precision](#precision)
 - [External Solar-system ephemeris data or services](#solarsystem)
 - [Release schedule](#release-schedule)


-----------------------------------------------------------------------------

<a name="introduction"></a>
## Introduction

SuperNOVAS is a fork of the The Naval Observatory Vector Astrometry Software 
([NOVAS](https://aa.usno.navy.mil/software/novas_info)). NOVAS is a light-weight but capable C library for precise 
positional astronomy calculations, such as one might need for an observatory, where apparent coordinates must be 
calculated to high precision. 

The primary goals of SuperNOVAS is to improve on the stock NOVAS C library by:

 - Fixing [outstanding issues](#fixed-issues)
 - Improving the ease of use by using `enum`s instead of integer constants, which also allows for some checking
   of use during compilations (such as using the incorrect `enum` type).
 - Improving [API documentation](https://smithsonian.github.io/SuperNOVAS.home/apidoc/html/) with 
   [Doxygen](https://www.doxygen.nl/) to provide browseable cross-referenced API docs. 
 - Streamlining calculations where possible to improve performance.
 - Adding `const` modifier to prototype arguments where appropriate
 - Checking arguments and setting `errno` as appropriate (and returning -1 unless another appropriate error code was 
   defined already) 
 - Adding [new features](#supernovas-features) to facilitate more accessible use
 - Providing a GNU `Makefile` to build static and shared libraries from sources easily on POSIX platforms
   (including for MacOS X, Cygwin, or WSL). (At this point we do not provide a similar native build setup for Windows, 
   but speak up if you would like to add it yourself!)
 
At the same time, SuperNOVAS aims to be fully backward compatible with the upstream NOVAS C library, such that it can 
be used as a drop-in, _link-time_ replacement for NOVAS in your application without having to change existing 
code you may have written for NOVAS C.
 
SuperNOVAS is currently based on NOVAS C version 3.1. We plan to rebase SuperNOVAS to the latest upstream release of 
the NOVAS C library, if new releases become available.
 
SuperNOVAS is maintained by Attila Kovacs at the Center for Astrophysics \| Harvard and Smithsonian, and it is 
available through the [Smithsonian/SuperNOVAS](https://github.com/Smithsonian/SuperNOVAS) repo on GitHub.

Outside contributions are very welcome. See how you can contribute 
[here](https://github.com/Smithsonian/SuperNOVAS/CONTRIBUTING.md).


-----------------------------------------------------------------------------

<a name="related-links"></a>
## Related links

Here are some links to SuperNOVAS related content online:

 - [API Documentation](https://smithsonian.github.io/SuperNOVAS.home/apidoc/html/)
 - [Project site](https://github.com/Smithsonian/SuperNOVAS/) on GitHUB. 
 - [SuperNOVAS home page](https://smithsonian.github.io/SuperNOVAS.home) page on github.io. 
 - [How to Contribute](https://github.com/Smithsonian/SuperNOVAS/CONTRIBUTING.md) guide
 - [NOVAS](https://aa.usno.navy.mil/software/novas_info) home page at the US Naval Observatory.
 - [NOVAS C](https://aa.usno.navy.mil/software/novasc_intro) library page. 
 - [SPICE toolkit](https://naif.jpl.nasa.gov/naif/toolkit.html) for integrating Solar-system ephemeris
   via JPL HORIZONS.
 - [IAU Minor Planet Center](https://www.minorplanetcenter.net/iau/mpc.html) provides another source
   of ephemeris data.

-----------------------------------------------------------------------------
https://github.com/attipaci/attipaci.gitgub.io
<a name="compatibility"></a>
## Compatibility with NOVAS C 3.1

SuperNOVAS strives to maintain API compatibility with the upstream NOVAS C 3.1 library, but not binary 
compatilibility. In practical terms it means that you cannot simply drop-in replace your static (e.g. `novas.a`) or 
shared (e.g. `novas.so`) library, from NOVAS C 3.1 with that from SuperNOVAS. Instead, you will need to (re)compile 
and or (re)link your application with the SuperNOVAS versions of these. 

This is because some function signatures have changed, e.g. to use an `enum` argument instead of the nondescript 
`short int` argument of NOVAS C 3.1, or because we added a return value to a functions that was declared `void` 
in NOVAS C 3.1. We also changed the `object` structure to contain a `long` ID number instead of  `short` to 
accommodate JPL NAIF values, which require a 32-bit width. 

-----------------------------------------------------------------------------

<a name="fixed-issues"></a>
## Fixed NOVAS C 3.1 issues

The SuperNOVAS library fixes a number of outstanding issues with NOVAS C 3.1. Here is a list of issues and fixes 
provided by SuperNOVAS over the upstream NOVAS C 3.1 code:

 - Fixes the [sidereal_time bug](https://aa.usno.navy.mil/software/novas_faq), whereby the `sidereal_time()` 
   function had an incorrect unit cast. This is a known issue of NOVAS C 3.1.
   
 - Fixes the [ephem_close bug](https://aa.usno.navy.mil/software/novas_faq), whereby `ephem_close()` in 
   `ephem_manager.c` did not reset the `EPHFILE` pointer to NULL. This is a known issue of NOVAS C 3.1.
   
 - Fixes antedating velocities and distances for light travel time in `ephemeris()`. When getting positions and 
   velocities for Solar-system sources, it is important to use the values from the time light originated from the 
   observed body rather than at the time that light arrives to the observer. This correction was done properly for 
   positions, but not for velocities or distances, resulting in incorrect observed radial velocities or apparent 
   distances being reported for spectroscopic observations or for angular-physical size conversions. 
   
 - Fixes bug in `ira_equinox()` which may return the result for the wrong type of equinox (mean vs. true) if the the 
   `equinox` argument was changing from 1 to 0, and back to 1 again with the date being held the same. This affected 
   routines downstream also, such as `sidereal_time()`.
   
 - Fixes accuracy bug in `ecl2equ_vec()`, `equ2ecl_vec()`, `geo_posvel()`, `place()`, and `sidereal_time()`, which 
   returned a cached value for the other accuracy if the other input parameters are the same as a prior call, 
   except the accuracy. 
   
 - The use of `fmod()` in NOVAS C 3.1 led to the wrong results when the numerator was negative and the result was
   not turned into a proper remainder. This affected the calculation of the mean anomaly in `solsys3.c` (line 261) 
   for dates prior to J2000.
   

-----------------------------------------------------------------------------

<a name="installation"></a>
## Building and installation


The SuperNOVAS distibution contains a `Makefile` for GNU make, which is suitable for compiling the library (as well as 
local documentation, and tests, etc.) on POSIX systems such as Linux, BSD, MacOS X, or Cygwin or WSL on Windows.

Before compiling the library take a look a `config.mk` and edit it as necessary for your needs, such as:

 - Choose which planet calculator function routines are built into the library (for example to provide 
   `earth_sun_calc()` set `BUILTIN_SOLSYS3 = 1`  and/or for `planet_ephem_reader()` set `BUILTIN_SOLSYS_EPHEM = 1`. 
   You can then specify these functions as the default planet calculator for `ephemeris()` in your application 
   dynamically via `set_planet_calc()`.
   
 - Choose which stock planetary calculator module (if any) should provide a default `solarsystem()` implementation for 
   `ephemeris()` calls by setting `DEFAULT_SOLSYS` to 1 -- 3 for `solsys1.c` trough `solsys3.c`, respectively. If you 
   want to link your own `solarsystem()` implementation(s) against the library, you should not set `DEFAULT_SOLSYS` 
   (i.e. delete or comment out the corresponding line or else set `DEFAULT_SOLSYS` to 0).
   
 - If you are going to be using the functions of `solsys1.c` you may also want to specify the source file that will 
   provide the `readeph()` implementation for it by setting `DEFAULT_READEPH` appropriately. (The default setting uses 
   the dummy `readeph0.c` which simply returns an error if one tries to use the functions from `solsys1.c`.

 - If you want ot use the CIO locator binary file for `cio_location()`, you can specify the path to the binary file 
   (e.g. `/usr/local/share/novas/cio_ra.bin`) where the file will be located at on your system. (The CIO locator file 
   is not at all necessary for the functioning of the library, unless you specifically require CIO poistions relative 
   to GCRS.)

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


<a name="building-your-application"></a>
## Building your application with SuperNOVAS

Provided you have installed the SuperNOVAS headers into a standard location (such as `/usr/include` or 
`/usr/local/include`) and the static or shared library into `usr/lib` (or `/usr/local/lib` or similar), you
can build your application against it very easily. For example, to build `myastroapp.c` against SuperNOVAS, 
you might have a `Makefile` with contents like:

```make
  myastroapp: myastroapp.c 
  	cc -o $@ $(CFLAGS) $^ -lm -lnovas
```

If you have a legacy NOVAS C 3.1 application, it is possible that the compilation will give you errors due
to missing includes for `stdio.h`, `stdlib.h`, `ctype.h` or `string.h`. This is because these were explicitly
included in `novas.h` in NOVAS C 3.1, but not in SuperNOVAS (at least not by default), as a matter of best
practice. If this is a problem for you can 'fix' it in one of two ways: (1) Add the missing `#include` 
directives to your application source explicitly, or if that's not an option for you, then (2) set the 
`-DCOMPAT=1` compiler flag when compiling your application:

```make
  myastroapp: myastroapp.c 
  	cc -o $@ $(CFLAGS) -DCOMPAT=1 $^ -lm -lnovas
```

To use your own `solarsystem()` implemetation for `ephemeris()`, you will want to build the library with
`DEFAULT_SOLSYS` not set (or else set to 0) in `config.mk` (see section above), and your applications 
`Makefile` may contain something like:

```make
  myastroapp: myastroapp.c my_solsys.c 
  	cc -o $@ $(CFLAGS) $^ -lm -lnovas
```

The same principle applies to using your specific `readeph()` implementation (only with `DEFAULT_READEPH` 
being unset in `config.mk`).

-----------------------------------------------------------------------------


<a name="examples"></a>
## Basic usage examples

 - [Calculating positions for a sidereal source](#sidereal-example)
 - [Calculating positions for a Solar-system source](#solsys-example)
 - [Reduced accuracy shortcuts](#accuracy-notes)
 - [Performance considerations](#performance-note)
 - [Note on alternative methodologies](#methodologies)
 

<a name="sidereal-example"></a>
### Calculating positions for a sidereal source

Sidereal sources may be anything beyond the solar-system with 'fixed' catalog coordinates. It may be a star, or a 
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
 // First change the catalog coordinates (in place) to the J2000 (FK5)... 
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

We also need to set the time of observation. Out clocks usually measure UTC, but for NOVAS we usually need time 
measured based on Terrestrial Time (TT) or Barycentric Time (TDB) or UT1. So 

```c
 // The current value for the leap seconds (UTC - TAI)
 int leap_seconds = 37;

 // Set the DUT1 = UT1 - UTC time difference in seconds (e.g. from IERS Bulletins)
 int dut1 = ...

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
 double dx = ... 
 double dy = ...
  
 cel_pole(jd_tt, POLE_OFFSETS_X_Y, dx, dy);
```

Now we can calculate the precise apparent position (CIRS or TOD) of the source, such as it's right ascension (R.A.) 
and declination, and the equatorial _x,y,z_ unit vector pointing in the direction of the source (in the requested 
coordinate system and for the specified observing location). We also get radial velocity (for spectroscopy), and 
distance (e.g. for apparent-to-physical size conversion):

```c
 sky_pos pos;	// We'll return the observable positions in this structure
  
 // Calculate the apparent (CIRS) topocentric positions for the above configuration
 int status = place(jd_tt, &source, &obs, ut1_to_tt, NOVAS_CIRS, NOVAS_FULL_ACCURACY, &pos);
  
 // You should always check that the calculation was successful...
 if(status) {
   // Ooops, something went wrong...
   return -1;
 }
```

Finally, we may want to calculate the astrometric zenith distance (= 90&deg; - azimuth) and elevation angles of the source
at the specified observing location (without refraction correction):

```c

 // Zenith distance and azimuth (in degrees) to populate...
 double zd, az;
  
 // Convert CIRS to horizontal using the pole offsets.
 cirs_to_hor(jd_tt - ut1_to_tt, ut1_to_tt, NOVAS_FULL_ACCURACY, dx, dy, &obs.on_surf, 
             pos.ra, pos.dec, &zd, &az);
``` 

In the example above we first calculated the apparent coordinates in the Celestial Intermediate Reference System 
(CIRS). Then we used `cirs_to_hor()` function then convert first it to the Earth-fixed International Terrestrial 
Reference system (ITRS) using the small (arcsec-level) measured variation of the pole (dx, dy) provided explicitly 
since `cirs_to_hor()` does not use the values previously set via `cel_pole()`. Finally, `cirs_to_hor()` converts the 
ITRS coordinates to the horizontal system at the observer location.

You can additionally apply an optical refraction correction for the astrometric (unrefracted) zenith angle, if you 
want, e.g.:

```c
   zd -= refract_astro(&obs.on_surf, NOVAS_STANDARD_ATMOSPHERE, zd);
```


<a name="solsys-example"></a>
### Calculating positions for a Solar-system source

Solar-system sources work similarly to the above with a few important differences.

First, You will have to provide one or more functions to obtain the barycentric ICRS positions for your Solar-system 
source of interest for the specific Barycentric Dynamical Time (TDB) of observation. See Section on integrating 
[External Solar-system ephemeris data or services](#solarsystem) with SuperNOVAS. You can specify the functions
that will handle the respective ephemeris data at runtime before making the NOVAS calls that need them, e.g.:

```c
 // Set the function to use for regular precision planet position calculations
 set_planet_calc(my_planet_calcular_function);
  
 // Set the function for high-precision planet position calculations
 set_planet_calc(my_very_precise_planet_calculator_function);
  
 // Set the function to use for calculating all sorts of solar-system bodies
 set_ephem_reader(my_ephemeris_provider_function);
```

You can use `tt2tdb()` to help convert Terrestrial Time (TT) to Barycentric Dynamic Time (TDB) for your ephemeris 
provider functions (they only differ when you really need extreme precision -- for most applications you can used TT 
and TDB interchangeably in the present era):

```c
 double jd_tdb = jd_tt + tt2tdb(jd_tt) / 86400.0;
```

Instead of `make_cat_entry` you define your source as an `object` with an ID number that is used by the ephemeris 
service you provided. For major planets you might want to use type `NOVAS_MAJOR_PLANET` if they use a 
`novas_planet_calculator` function to access ephemeris data with their NOVAS IDs, or else `NOVAS_MINOR_PLANET` for 
more generic ephemeris handling via a user-provided `novas_ephem_reader_func`. E.g.:

```c
 object mars, ceres; // Hold data on solar-system bodies.
  
 // Mars will be handled by hte planet calculator function
 make_object(NOVAS_MAJOR_PLANET_MARS, NOVAS_MARS, "Mars", NULL, &mars);
  
 // Ceres will be handled by the generic ephemeris reader, which say uses the 
 // NAIF ID of 2000001
 make_object(NOVAS_MINOR_PLANET, 2000001, "Ceres", NULL, &ceres);
```

Other than that, it's the same spiel as before. E.g.:

```c
 int status = place(jd_tt, &mars, &obs, ut1_to_tt, NOVAS_CIRS, NOVAS_FULL_ACCURACY, &pos);
 if(status) {
   // Ooops, something went wrong...
   ...
 }
```


<a name="accuracy-notes"></a>
#### Reduced accuracy shortcuts

When one does not need positions at the microarcsecond level, some shortcuts can be made to the recipe above:

 - You can use `NOVAS_REDUCED_ACCURACY` instead of `NOVAS_FULL_ACCURACY` for the calculations. This typically has an 
   effect at the milliarcsecond level only, but may be much faster to calculate.
 - You can skip the J2000 to ICRS conversion and use J2000 coordinates directly as a fair approximation (at the 
   &lt;~ 22 mas level).
 - You might skip the pole offsets dx, dy. These are tenths of arcsec, typically.
 
<a name="performance-note"></a>
#### Performance considerations

Some of the calculations involved can be expensive from a computational perspective. For the most typical use case
however, NOVAS (and SuperNOVAS) has a trick up its sleve: it caches the last result of intensive calculations so they 
may be re-used if the call is made with the same environmental parameters again (such as JD time and accuracy). 
Therefore, when calculating positions for a large number of sources at different times:

 - It is best to iterate over the sources while keeping the time fixed in the inner loop. 
 - You probably want to stick to one accuracy morde (`NOVAS_FULL_ACCURACY` or `NOVAS_REDUCED_ACCURACY`) to prevent
   re-calculating the same quantities repeatedly to alternating precision.
 - If super-high accuracy is not required `NOVAS_REDUCED_ACCURACY` mode offers much faster calculations, in general.
 
 
 
<a name="methodologies"></a>
### Note on alternative methodologies

The IAU 2000 and 2006 resolutions have completely overhauled the system of astronomical coordinate transformations
to enable higher precision astrometry. (Super)NOVAS supports coordinate calculations both in the old (pre IAU 2000) 
ways, and in the new IAU standard method.

 | Concept                    | Old standard                  | New IAU standard                                  |
 | -------------------------- | ----------------------------- | ------------------------------------------------- |
 | Catalog coordinate system  | J2000 or B1950                | International Celestial Reference System (ICRS)   |
 | Dynamical system	      | True of Date (TOD)            | Celestial Intermediate Reference System (CIRS)    |
 | Dynamical R.A. origin      | true equinox of date          | Celestial Intermediate Origin (CIO)               |
 | Precession, nutation, bias | separate, no tidal terms      | IAU 2006 precession/nutation model                |
 | Celestial Pole offsets     | d&psi;, d&epsilon;            | _dx_, _dy_                                        |
 | Earth rotation measure     | Greenwich Sidereal Time (GST) | Earth Rotation Angle (ERA)                        |
 | Fixed Earth System         | WGS84                         | International Terrestrial Reference System (ITRS) |
 
See the various enums and constands defined in `novas.h`, as well as the descriptions on the various NOVAS routines
on how they are appropriate for the old and new methodologies respectively.

-----------------------------------------------------------------------------


<a name="supernovas-features"></a>
## SuperNOVAS specific features

 - SuperNOVAS functions take `enum`s as their option arguments instead of raw integers. These enums are defined in 
   `novas.h`. The same header also defines a number of useful constants. The enums allow for some compiler checking, 
   and make for more readable code that is easier to debug. They also make it easy to see what choices are available
   for each function argument, without having to consult the documentation each and every time.

 - All SuperNOVAS functions check for the basic validity of the supplied arguments (Such as NULL pointers or illegal 
   duplicate arguments) and will return -1 (with `errno` set, usually to `EINVAL`) if the arguments supplied are
   invalid (unless the NOVAS C API already defined a different return value for specific cases. If so, the NOVAS C
   error code is returned for compatibility).

 - Many SuperNOVAS functions allow `NULL` arguments, both for optional input values as well as outputs that are not 
   required. See the [API Documentation](https://smithsonian.github.io/SuperNOVAS.home/apidoc/html/) for specifics).
   This eliminates the need to declare dummy variables in your application code.

 - All SuperNOVAS functions that take an input vector to produce an output vector allow the output vector argument
   be the same as the input vector argument. For example, `frame_time(pos, TIE_J2000_TO_ICRS, pos)` using the same 
   `pos` vector both as the input and the output. In this case the `pos` vector is modified in place by the call. 
   This can greatly simplify usage, and eliminate extraneous declarations, when intermediates are not required.

 - SuperNOVAS prototypes declare function pointer arguments as `const` whenever the function does not modify the
   data content being pointed at. This supports better programming practices that generally aim to avoid unintended 
   data modifications.
   
 - Source names and catalog names can both be up to 64 bytes (including termination), up from 51 and 4 respectively
   NOVAS C, while keeping `struct` layouts the same thanks to alignment.
   
 - Runtime configurability:

   * The planet position calculator function used by `ephemeris` can be set at runtime via `set_planet_calc()`, and
     `set_planet_calc_hp` (for high precision calculations). Similarly, if `planet_ephem_reader()` or 
     `planet_ephem_reader_hp()` (defined in `solsys-ephem.c`) are set as the planet calculator functions, then 
     `set_ephem_reader()` can set the user-specified function to use with these to actually read ephemeris data
     (e.g. from a JPL ephemeris file).
 
   * If CIO locations vs GSRS are important to the user, the user may call `cio_set_locator_file()` at runtime to
     specify the location of the binary CIO interpolation table (e.g. `cio_ra.bin`) to use, even if the library was
     compiled with the different default CIO locator path. 
 
   * The default low-precision nutation calculator `nu2000k()` can be replaced by another suitable IAU 2006 nutation
     approximation via `nutation_set_lp_calc()`. For example, the user may want to use the `iau2000b()` model instead 
     or some custom algorithm instead.
 
 - New intutitive XYZ coordinate coversion functions:
   * for GCRS - CIRS - ITRS (IAU 2000 standard): `gcrs_to_cirs()`, `cirs_to_itrs()`, and `itrs_to_cirs()`, 
     `cirs_to_gcrs()`.
   * for J2000 - TOD - ITRS (old methodology): `j2000_to_tod()`, `tod_to_itrs()`, and `itrs_to_tod()`, 
     `tod_to_j2000()`.
   
 - New `cirs_to_hor()` and `tod_to_hor()` functions similar to the existing `equ2hor()`. Whereas `tod_to_hor()` is
   effectively a just better named bersion of `equ2hor()` for converting from TOD to to local horizontal (old 
   methodology), the `cirs_to_hor()` does the same from CIRS (new IAU standard methodology), and had no equivalent
   in NOVAS C 3.1. As such, `cirs_to_hor()` may be used after `place()` is called with `NOVAS_CIRS` as the coordinate 
   system. 
   
 - New `refract_astro()` function that complements the existing `refract()` but takes an unrefracted zenith angle
   as its argument.

 - New convenience functions to wrap `place()` for simpler specific use: `place_star()`, `place_icrs()`, 
   `place_gcrs()`, `place_cirs()`, `place_tod()`.
 
 - New time conversion utilities `tt2tdb()` and `get_ut1_to_tt()` make it simpler to convert between UT1, TT, and TDB
   time scales, and to supply `ut1_to_tt` arguments to `place()` or topocentric calculations.
 
 - Co-existing `solarsystem()` variants. It is possible to use the different `solarsystem()` implementations 
   provided by `solsys1.c`, `solsys2.c`, `solsys3.c` and/or `solsys-ephem.c` side-by-side, as they define their
   functionalities with distinct, non-conflicting names, e.g. `earth_sun_calc()` vs `planet_jplint()` vs
   `planet_ephem_manager()` vs `planet_ephem_reader()`. See the section on [Building and installation](#installation)
   further above on including a selection of these in your library build.)



-----------------------------------------------------------------------------

<a name="precision"></a>
## Notes on precision

The SuperNOVAS library is in principle capable of calculating positions to sub-microarcsecond, and velocities to mm/s 
precision for all types of celestial sources. However, there are certain pre-requisites and practical considerations 
before that level of accuracy is reached.


 1. __IAU 2000/2006 conventions__: High precision calculations will generally require that you use SuperNOVAS with the
    new IAU standard quantities and methods. The old ways were simply not suited for precisions much below the 
    milliarcsecond level.

 2. __Earth's polar motion__: Calculating precise positions for any Earth-based observations requires precise 
    knowledge of Earth orientation at the time of observation. The pole is subject to predictable precession and 
    nutation, but  also small irregular variations in the orientation of the rotational axis and the rotation period 
    (a.k.a polar wobble). The [IERS Bulletins](https://www.iers.org/IERS/EN/Publications/Bulletins/bulletins.html) 
    provide up-to-date measurements, historical data, and near-term projections for the polar offsets and the UT1-UTC 
    (DUT1) time difference and leap-seconds (UTC-TAI). In SuperNOVAS you can use `cel_pole()` and `get_ut1_to_tt()` 
    functions to apply / use the published values from these to improve the astrometic precision of Earth-orientation
    based coordinate calculations. Without setting and using the actual polar offset values for the time of 
    observation, positions for Earth-based observations will be accurate at the arcsecond level only.
 
 3. __Solar-system sources__: Precise calculations for Solar-system sources requires precise ephemeris data for both
    the target object as well as for Earth, and the Sun vs the Solar-system barycenter. For the highest precision 
    calculations you also need positions for all major planets to calculate gravitational deflection precisely. By 
    default SuperNOVAS can only provide approximate positions for the Earth and Sun (see `earth_sun_calc()` in 
    `solsys3.c`), but certainly not at the sub-microarcsecond level, and not for other solar-system sources. You will 
    need to provide a way to interface SuperNOVAS with a suitable ephemeris source (such as the CSPICE toolkit from 
    JPL) if you want to use it to obtain precise positions for Solar-system bodies. See the 
    [section below](#solarsystem)for more information how you can do that.
    
  4. __Refraction__: Ground based observations are also subject to atmospheric refraction. SuperNOVAS offers the 
    option to include _optical_ refraction corrections in `equ2hor()` either for a standard atmosphere or more 
    precisely using the weather parameters defined in the `on_surface` data structure that specifies the observer 
    locations. Note, that refraction at radio wavelengths is notably different from the included optical model. In 
    either case you may want to skip the refraction corrections offered in this library, and instead implement your 
    own as appropriate (or not at all).
 

-----------------------------------------------------------------------------

<a name="solarsystem"></a>
## External Solar-system ephemeris data or services

Coming soon...

-----------------------------------------------------------------------------


<a name="release-schedule"></a>
## Release schedule

A predictable release schedule and process can help manage expectations and reduce stress on adopters and developers 
alike.

Releases of the library shall follow a quarterly release schedule. You may expect upcoming releases 
to be published around __March 1__, __June 1__, __September 1__, and/or __December 1__ each year, on an as needed
basis. That means that if there are outstanding bugs, or new pull requests (PRs), you may expect a release that 
addresses these in the upcoming quarter. The dates are placeholders only, with no guarantee that a release will 
actually be available every quarter. If nothing of note comes up, a potential release date may pass without a release 
being published.

_Feature releases_ (e.g. __1.x.0__ version bumps) are provided at least 6 months apart, to reduce stress on adopters 
who may need/want to tweak their code to integrate these. Between feature releases, _bug fix releases_ (without 
significant API changes) may be provided as needed to address issues. New features are generally reserved for the 
feature releases, although they may also be rolled out in bug-fix releases as long as they do not affect the existing 
API -- in line with the desire to keep bug-fix releases fully backwards compatible with their parent versions.

In the month(s) preceding releases one or more _release candidates_ (e.g. `1.0.1-rc3`) will be available on github
briefly, under [Releases](https://github.com/Smithsonian/SuperNOVAS/releases), so that changes can be tested by 
adopters before the releases are finalized. Please use due diligence to test such release candidates with your code 
when they become available to avoid unexpected suprises when the finalized release is published. Release candidates 
are typically available for one week only before they are superseded either by another, or by the finalized release.


