![Build Status](https://github.com/Smithsonian/SuperNOVAS/actions/workflows/build.yml/badge.svg)
![Test](https://github.com/Smithsonian/SuperNOVAS/actions/workflows/test.yml/badge.svg)
![Static Analysis](https://github.com/Smithsonian/SuperNOVAS/actions/workflows/analyze.yml/badge.svg)
<a href="https://smithsonian.github.io/SuperNOVAS/apidoc/html/files.html">
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

[![DOI](https://zenodo.org/badge/748170057.svg)](https://doi.org/10.5281/zenodo.14584983)

The NOVAS C astrometry library, made better.

 - [API documentation](https://smithsonian.github.io/SuperNOVAS/apidoc/html/files.html).
 - [SuperNOVAS pages](https://smithsonian.github.io/SuperNOVAS) on github.io 

[SuperNOVAS](https://github.com/Smithsonian/SuperNOVAS/) is a C/C++ astronomy software library, providing 
high-precision astrometry such as one might need for running an observatory or a precise planetarium program. It is a 
fork of the Naval Observatory Vector Astrometry Software ([NOVAS](https://aa.usno.navy.mil/software/novas_info)) 
C version 3.1, providing bug fixes, tons of extra features, while making it easier (and safer) to use also.

SuperNOVAS is entirely free to use without licensing restrictions.  Its source code is compatible with the C99 
standard, and hence should be suitable for old and new platforms alike. It is light-weight and easy to use, with full 
support for the IAU 2000/2006 standards for sub-microarcsecond position calculations.

This document has been updated for the `v1.2` and later releases.


## Table of Contents

 - [Introduction](#introduction)
 - [Fixed NOVAS C 3.1 issues](#fixed-issues)
 - [Compatibility with NOVAS C 3.1](#compatibility)
 - [Building and installation](#installation)
 - [Building your application with SuperNOVAS](#integration)
 - [Example usage](#examples)
 - [Tips and tricks](#tips)
 - [Notes on precision](#precision)
 - [SuperNOVAS specific features](#supernovas-features)
 - [Incorporating Solar-system ephemeris data or services](#solarsystem)
 - [Runtime debug support](#debug-support)
 - [Release schedule](#release-schedule)

-----------------------------------------------------------------------------

<a name="introduction"></a>
## Introduction

SuperNOVAS is a fork of the The Naval Observatory Vector Astrometry Software 
([NOVAS](https://aa.usno.navy.mil/software/novas_info)).

The primary goal of SuperNOVAS is to improve on the stock NOVAS C library via:

 - Fixing [outstanding issues](#fixed-issues).
 - Improved [API documentation](https://smithsonian.github.io/SuperNOVAS/apidoc/html/files.html).
 - [New features](#added-functionality).
 - [Refining the API](#api-changes) to promote best programming practices.
 - [Thread-safe calculations](#multi-threading).
 - [Debug mode](#debug-mode) with informative error tracing.
 - [Regression testing](https://codecov.io/gh/Smithsonian/SuperNOVAS) and continuous integration on GitHub.

At the same time, SuperNOVAS aims to be fully backward compatible with the intended functionality of the upstream 
NOVAS C library, such that it can be used as a _build-time_ replacement for NOVAS in your application without having 
to change existing (functional) code you may have written for NOVAS C.
 
SuperNOVAS is currently based on NOVAS C version 3.1. We plan to rebase SuperNOVAS to the latest upstream release of 
the NOVAS C library, if new releases become available.
 
SuperNOVAS is maintained by [Attila Kovács](https://github.com/attipaci) at the Center for Astrophysics \| Harvard 
&amp; Smithsonian, and it is available through the [Smithsonian/SuperNOVAS](https://github.com/Smithsonian/SuperNOVAS) 
repository on GitHub.

Outside contributions are very welcome. See
[how you can contribute](https://github.com/Smithsonian/SuperNOVAS/CONTRIBUTING.md) to make SuperNOVAS even better.

### Related links

 - [NOVAS](https://aa.usno.navy.mil/software/novas_info) home page at the US Naval Observatory.
 - [CALCEPH C library](https://calceph.imcce.fr/docs/4.0.0/html/c/index.html) for integrating Solar-system ephemerides
   from JPL and/or in INPOP 2.0/3.0 format.
 - [NAIF SPICE toolkit](https://naif.jpl.nasa.gov/naif/toolkit.html) for integrating Solar-system ephemerides
   from JPL.
 - [Smithsonian/cspice-sharedlib](https://github.com/Smithsonian/cspice-sharedlib) for building CSPICE as a shared
   library for dynamic linking.
 - [IAU Minor Planet Center](https://www.minorplanetcenter.net/iau/mpc.html) provides up-to-date orbital elements
   for asteroids, comets, and near-Earth objects (NEOs), including newly discovered objects.


-----------------------------------------------------------------------------

<a name="fixed-issues"></a>
## Fixed NOVAS C 3.1 issues

The SuperNOVAS library fixes a number of outstanding issues with NOVAS C 3.1. Here is a list of issues and fixes 
provided by SuperNOVAS over the upstream NOVAS C 3.1 code:

 - Fixes the [sidereal_time bug](https://aa.usno.navy.mil/software/novas_faq), whereby the `sidereal_time()` function 
   had an incorrect unit cast. This was a documented issue of NOVAS C 3.1.
   
 - Fixes the [ephem_close bug](https://aa.usno.navy.mil/software/novas_faq), whereby `ephem_close()` in 
   `eph_manager.c` did not reset the `EPHFILE` pointer to NULL. This was a documented issue of NOVAS C 3.1.
     
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
   `coord_sys = 0` and the same date as before would return the results in GCRS coordinates instead of the requested 
   mean equinox of date coordinates.
 
 - Some remainder calculations in NOVAS C 3.1 used the result from `fmod()` unchecked, which resulted in angles outside
   of the expected [0:2&pi;] range and was also the reason why `cal_date()` did not work for negative JD values.
 
 - Fixes `aberration()` returning NaN vectors if the `ve` argument is 0. It now returns the unmodified input vector 
   appropriately instead.
   
 - Fixes unpopulated `az` output value in `equ2hor()` at zenith. While any azimuth is acceptable really, it results in 
   unpredictable behavior. Hence, we set `az` to 0.0 for zenith to be consistent.
   
 - Fixes potential string overflows and eliminates associated compiler warnings.
 
 - [__v1.1__] Fixes division by zero bug in `d_light()` if the first position argument is the ephemeris reference
   position (e.g. the Sun for `solsys3.c`). The bug affects for example `grav_def()`, where it effectively results in
   the gravitational deflection due to the Sun being skipped.
    
 - [__v1.1__] The NOVAS C 3.1 implementation of `rad_vel()` has a number of issues that produce inaccurate results. 
   The errors are typically at or below the tens of m/s level for objects not moving at relativistic speeds.
   

-----------------------------------------------------------------------------

<a name="compatibility"></a>
## Compatibility with NOVAS C 3.1

SuperNOVAS strives to maintain API compatibility with the upstream NOVAS C 3.1 library, but not binary (ABI) 
compatibility. 

If you have code that was written for NOVAS C 3.1, it should work with SuperNOVAS as is, without modifications. Simply 
(re)build your application against SuperNOVAS, and you are good to go. 

The lack of binary compatibility just means that you cannot drop-in replace your compiled objects (e.g. `novas.o`, or 
the static `libnovas.a`, or the shared `libnovas.so`) libraries, from NOVAS C 3.1 with those from SuperNOVAS. Instead, 
you will have to build (compile) your application referencing the SuperNOVAS headers and/or libraries from the start.

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

Before compiling the library take a look a `config.mk` and edit it as necessary for your needs, or else define
the necessary variables in the shell prior to invoking `make`. For example:

 - Choose which planet calculator function routines are built into the library (for example to provide 
   `earth_sun_calc()` set `BUILTIN_SOLSYS3 = 1`  and/or for `planet_ephem_provider()` set `BUILTIN_SOLSYS_EPHEM = 1`. 
   You can then specify these functions (or others) as your planet calculator of choice for `ephemeris()` in your 
   application dynamically via `set_planet_provider()`.
   
 - [CALCEPH](https://www.imcce.fr/recherche/equipes/asd/calceph/) C library integration is automatic if `ldconfig` can 
   locate the `libcalceph` shared library. You can also control CALCEPH integration manually, e.g. by setting 
   `CALCEPH_SUPPORT = 1` in `config.mk` or in the shell prior to the build. CALCEPH integration will require an 
   accessible installation of the CALCEPH development files (C headers and unversioned static or shared libraries 
   depending on the needs of the build).
   
 - [NAIF CSPICE Toolkit](https://naif.jpl.nasa.gov/naif/toolkit.html) integration automatic, if `ldconfig` can locate 
   the `libcspice` shared library. You can also control CSPICE integration manually, e.g. by setting 
   `CSPICE_SUPPORT = 1` in `config.mk` or in the shell prior to the build. CSPICE integration will require an 
   accessible installation of the CSPICE development files (C headers, under a `cspice/` sub-folder in the header 
   search path, and unversioned static or shared libraries depending on the needs of the build). You might want to 
   check out the [Smithsonian/cspice-sharedlib](https://github.com/Smithsonian/cspice-sharedlib) repository for 
   building CSPICE as a shared library.
   
 - Choose which stock planetary calculator module (if any) should provide a default `solarsystem()` implementation for 
   `ephemeris()` calls by setting `DEFAULT_SOLSYS` to 1 -- 3 for `solsys1.c` trough `solsys3.c`, respectively. If you 
   want to link your own `solarsystem()` implementation(s) against the library, you should not set `DEFAULT_SOLSYS` 
   (i.e. delete or comment out the corresponding line or else set `DEFAULT_SOLSYS` to 0).
   
 - You may also specify the source file that will provide a `readeph()` implementation, by setting `DEFAULT_READEPH`. 
   (The default setting uses the dummy `readeph0.c` which simply returns an error). Note, that a `readeph()` 
   implementation is a relic of NOVAS C and not generally needed. You can provide a superior ephemeris reader 
   implementation at runtime via the `set_ephem_provider()` call or equivalent (e.g. `novas_use_calceph()` or 
   `novas_use_cspice()`, if they are available).

 - If you want to use a CIO locator file for `cio_location()`, you can specify the path to the CIO locator file (e.g. 
   `/usr/local/share/supernovas/CIO_RA.TXT`) on your system e.g. by setting the `CIO_LOCATOR_FILE` shell variable 
   prior to calling `make`. (The CIO locator file is not necessary for the functioning of the library, unless you 
   specifically require CIO positions relative to GCRS.)
   
 - If your compiler does not support the C11 standard and it is not GCC &gt;=3.3, but provides some non-standard
   support for declaring thread-local variables, you may want to pass the keyword to use to declare variables as
   thread local via `-DTHREAD_LOCAL=...` added to `CFLAGS`. (Don't forget to enclose the string value in escaped
   quotes in `config.mk`, or unescaped if defining the `THREAD_LOCAL` shell variable prior to invoking `make`.)


Additionally, you may set number of environment variables to futher customize the build, such as:

 - `CC`: The C compiler to use (default: `gcc`).

 - `CPPFLAGS`: C preprocessor flags, such as externally defined compiler constants.
 
 - `CFLAGS`: Flags to pass onto the C compiler (default: `-g -Os -Wall`). Note, `-Iinclude` will be added 
   automatically.
   
 - `CSTANDARD`: Optionally, specify the C standard to compile for, e.g. `c99` to compile for the C99 standard. If
   defined then `-std=$(CSTANDARD)` is added to `CFLAGS` automatically.
   
 - `WEXTRA`: If set to 1, `-Wextra` is added to `CFLAGS` automatically.
   
 - `LDFLAGS`: Extra linker flags (default is _not set_). Note, `-lm -lxchange` will be added automatically.

 - `CHECKEXTRA`: Extra options to pass to `cppcheck` for the `make check` target

Now you are ready to build the library:

```bash
  $ make
```

will compile the shared (e.g. `lib/libsupernovas.so`) libraries, produce a CIO locator data file (e.g. 
`tools/data/cio_ra.bin`), and compile the API documentation (into `apidoc/`) using `doxygen` (if available). 
Alternatively, you can build select components of the above with the `make` targets `shared`, and `local-dox` 
respectively. And, if unsure, you can always call `make help` to see what build targets are available.

After building the library you can install the above components to the desired locations on your system. For a 
system-wide install you may simply run:

```bash
  $ sudo make install
```

Or, to install in some other locations, you may set a prefix and/or `DESTDIR`. For example, to install under `/opt` 
instead, you can:

```bash
  $ sudo make prefix="/opt" install
```

Or, to stage the installation (to `/usr`) under a 'build root':

```bash
  $ make DESTDIR="/tmp/stage" install
```


-----------------------------------------------------------------------------


<a name="integration"></a>
## Building your application with SuperNOVAS

Provided you have installed the SuperNOVAS headers into a standard location, you can build your application against it 
easily. For example, to build `myastroapp.c` against SuperNOVAS, you might have a `Makefile` with contents like:

```make
  myastroapp: myastroapp.c 
  	$(CC) -o $@ $(CFLAGS) $^ -lm -lsupernovas
```

If you have a legacy NOVAS C 3.1 application, it is possible that the compilation will give you errors due to missing 
includes for `stdio.h`, `stdlib.h`, `ctype.h` or `string.h`. This is because these headers were explicitly included by 
`novas.h` in NOVAS C 3.1, but not in SuperNOVAS (at least not by default), as a matter of best practice. If this is a 
problem for you can 'fix' it in one of two ways: (1) Add the missing `#include` directives to your application source 
explicitly, or if that's not an option for you, then (2) set the `-DCOMPAT=1` compiler flag when compiling your 
application:

```make
  myastroapp: myastroapp.c 
  	$(CC) -o $@ $(CFLAGS) -DCOMPAT=1 $^ -lm -lsupernovas
```

If your application uses optional planet or ephemeris calculator modules, you may need to specify the appropriate 
optional shared library also:

```make
  myastroapp: myastroapp.c 
  	$(CC) -o $@ $(CFLAGS) $^ -lm -lsupernovas -lsolsys-calceph
```

### Legacy linking `solarsystem()` and `readeph()` modules

The NOVAS C way to handle planet or other ephemeris functions was to link particular modules to provide the
`solarsystem()` / `solarsystem_hp()` and `readeph()` functions. This approach is discouraged in SuperNOVAS, since we 
now allow selecting different implementations at runtime, but the old way is supported for legacy applications,
nevertheless.

To use your own existing default `solarsystem()` implementation in this way, you must build the library with 
`DEFAULT_SOLSYS` unset (or else set to 0) in `config.mk` (see section above), and your applications `Makefile` may 
contain something like:

```make
  myastroapp: myastroapp.c my_solsys.c 
  	$(CC) -o $@ $(CFLAGS) $^ -lm -lsupernovas
```

The same principle applies to using your specific `readeph()` implementation (only with `DEFAULT_READEPH` being unset 
in `config.mk`).

### Legacy modules: a better way...

Note, a better way to recycle your old planet and ephemeris calculator modules may be to rename `solarsystem()` / 
`solarsystem_hp()` functions therein to e.g. `my_planet_calculator()` / `my_planet_calculator_hp()` and then in your 
application can specify these functions as the provider at runtime:

```c
  set_planet_calculator(my_planet_calculator);
  set_planet_calculator(my_planet_calculator_hp);
```

For `readeph()` implementations, it is recommended that you change both the name and the footprint to e.g.:

```c
  int my_ephem_provider(const char *name, long id, double jd_tdb_high, double jd_tdb_low, 
                        enum novas_origin *origin, double *pos, double *vel);
```

and then then apply it in your application as:

```c
  set_ephem_provider(my_ephem_provider);
```

While it requires some minimal changes to the old code, the advantage of this preferred approach is (a) that you do 
not need to re-build the library with the `DEFAULT_SOLSYS` and `DEFAULT_READEPH` options disabled, and (b) you can 
switch between different planet and ephemeris calculator functions at will, during runtime.


-----------------------------------------------------------------------------


<a name="examples"></a>
## Example usage

 - [Note on alternative methodologies](#methodologies)
 - [Calculating positions for a sidereal source](#sidereal-example)
 - [Calculating positions for a Solar-system source](#solsys-example)


SuperNOVAS __v1.1__ has introduced a new, more intuitive, more elegant, and more efficient approach for calculating
astrometric positions of celestial objects. The guide below is geared towards this new method. However, the original
NOVAS C approach remains viable also (albeit often less efficient). You may find an equivalent example usage 
showcasing the original NOVAS method in [LEGACY.md](LEGACY.html).

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
on how they are appropriate for the old and new methodologies respectively. Figure 1 also shows the relation of the
various old and new coordinate systems and the (Super)NOVAS functions for converting position / velocity vectors among 
them.

In NOVAS, the barycentric BCRS and the geocentric GCRS systems are effectively synonymous to ICRS, since the origin 
for positions and for velocities, in any reference system, is determined by the `observer` location, while aberration
and gravitational deflection is included for apparent places only (as seen from the observer location). 

Older catalogs, such as B1950 (FK4) or B1900 are just special cases of MOD (mean-of-date) coordinates for the B1950
and B1900 epochs, respectively.

TIRS (Terrestrial Intermediate Reference System) and its older equivalent PEF (Pseudo-Earth-Fixed) are not explicitly 
referenced in SuperNOVAS. But they can be thought of as a special case of ITRS (International Terrestrial Reference 
System) with zero polar offsets (_dx_, _dy_).

WGS84 has been superseded by ITRS for higher accuracy definitions of Earth-based locations. WGS84 matches ITRS to the 
10m level globally, but it does not account for continental drifts and crustal motion. In (Super)NOVAS all Earth-fixed 
coordinates are effectively assumed as ITRS, whether explicitly or implicitly. There is nothing WGS84-specific in the 
implementation.

| ![SuperNOVAS coordinate systems and conversions](resources/SuperNOVAS-systems.png) |
|:--:| 
| __Figure 1.__ *SuperNOVAS Coordinate Systems and Conversions*. Functions indicated in bold face are available in NOVAS C also. All other functions are available in SuperNOVAS only. Dotted arrows indicate possible loss of precision due to the inadequacy of the old precession-nutation (Lieske et al. 1977) model. |


<a name="sidereal-example"></a>
### Calculating positions for a sidereal source

A sidereal source may be anything beyond the solar-system with 'fixed' catalog coordinates. It may be a star, or a 
galactic molecular cloud, or a distant quasar. 

#### Specify the object of interest

First, you must provide the coordinates (which may include proper motion and parallax). Let's assume we pick a star 
for which we have B1950 (i.e. FK4) coordinates:

```c
 cat_entry star; // Structure to contain information on sidereal source 

 // Let's assume we have B1950 (FK4) coordinates...
 // 16h26m20.1918s, -26d19m23.138s (B1950), proper motion -12.11, -23.30 mas/year, 
 // parallax 5.89 mas, radial velocity -3.4 km/s.
 make_cat_entry("Antares", "FK4", 1, 16.43894213, -26.323094, -12.11, -23.30, 5.89, -3.4, &star);
```

We must convert these coordinates to the now standard ICRS system for calculations in SuperNOVAS, first by calculating 
equivalent J2000 coordinates, by applying the proper motion and the appropriate precession. Then, we apply a small 
adjustment to convert from J2000 to ICRS coordinates.

```c
 // First change the catalog coordinates (in place) to the J2000 (FK5) system... 
 transform_cat(CHANGE_EPOCH, NOVAS_JD_B1950, &star, NOVAS_JD_J2000, "FK5", &star);
  
 // Then convert J2000 coordinates to ICRS (also in place). Here the dates don't matter...
 transform_cat(CHANGE_J2000_TO_ICRS, 0.0, &star, 0.0, "ICRS", &star);
```

(Naturally, you can skip the transformation steps above if you have defined your source in ICRS coordinates from the 
start.) Once the catalog entry is defined in ICRS, you can proceed wrapping it in a generic source structure (which
handles both catalog and Solar-system sources).

```c
 object source;   // Common structure for a sidereal or an Solar-system source
  
 // Wrap it in a generic source data structure
 make_cat_object(&star, &source);
```

Alternatively, for high-_z_ sources you might use `make_redshifted_cat_entry()` or `make_redshifted_object()` instead,
e.g.:

```c
  object quasar;
  
  // 12h29m6.6997s +2d3m8.598s (ICRS) z=0.158339
  make_redshifted_object("3c273", 12.4851944, 2.0523883, 0.158339, &quasar);
```


#### Specify the observer location

Next, we define the location where we observe from. Here we can (but don't have to) specify local weather parameters
(temperature and pressure) also for refraction correction later (in this example, we'll skip the weather):

```c
 observer obs;    // Structure to contain observer location 

 // Specify the location we are observing from
 // 50.7374 deg N, 7.0982 deg E, 60m elevation
 make_observer_on_surface(50.7374, 7.0982, 60.0, 0.0, 0.0, &obs);
```

Similarly, you can also specify observers in Earth orbit, in Sun orbit, at the geocenter, or at the Solar-system 
barycenter.


#### Specify the time of observation

Next, we set the time of observation. For a ground-based observer, you will need to provide SuperNOVAS with the
UT1 - UTC time difference (a.k.a. DUT1), and the current leap seconds. Let's assume 37 leap seconds, and DUT1 = 0.114,
then we can set the time of observation, for example, using the current UNIX time:

```c
 novas_timespec obs_time;       // Structure that will define astrometric time
 struct timespec unix_time;     // Standard precision UNIX time structure

 // Get the current system time, with up to nanosecond resolution...
 clock_gettime(CLOCK_REALTIME, &unix_time);
 
 // Set the time of observation to the precise UTC-based UNIX time
 novas_set_unix_time(unix_time.tv_sec, unix_time.tv_nsec, 37, 0.114, &obs_time);
```

Alternatively, you may set the time as a Julian date in the time measure of choice (UTC, UT1, TT, TDB, GPS, TAI, TCG, 
or TCB):

```c
 double jd_tai = ...     // TAI-based Julian Date 

 novas_set_time(NOVAS_TAI, jd_tai, leap_seconds, dut1, &obs_time);
```

or, for the best precision we may do the same with an integer / fractional split:

```c
 long ijd_tai = ...     // Integer part of the TAI-based Julian Date
 double fjd_tai = ...   // Fractional part of the TAI-based Julian Date 
  
 novas_set_split_time(NOVAS_TAI, ijd_tai, fjd_tai, 37, 0.114, &obs_time);
```


#### Set up the observing frame

Next, we set up an observing frame, which is defined for a unique combination of the observer location and the time of
observation:

```c
 novas_frame obs_frame;  // Structure that will define the observing frame
 double dx = ...         // [mas] Earth polar offset x, e.g. from IERS Bulletin A.
 double dy = ...         // [mas] Earth polar offset y, from same source as above.
  
 // Initialize the observing frame with the given observing parameters
 novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &obs_time, dx, dy, &obs_frame);
```

Here `dx` and `dy` are small diurnal (sub-arcsec level) corrections to Earth orientation, which are published
in the [IERS Bulletins](https://www.iers.org/IERS/EN/Publications/Bulletins/bulletins.html). They are needed when 
converting positions from the celestial CIRS frame to the Earth-fixed ITRS frame. You may ignore these and set zeroes 
if sub-arcsecond precision is not required.

The advantage of using the observing frame, is that it enables very fast position calculations for multiple objects
in that frame. So, if you need to calculate positions for thousands of sources for the same observer and time, it 
will be significantly faster than using the low-level NOVAS C routines instead. You can create derivative frames
for different observer locations, if need be, via `novas_change_observer()`.

Note that without a proper ephemeris provider for the major planets, you are invariably restricted to working with 
`NOVAS_REDUCED_ACCURACY` frames, providing milliarcsecond precision only. To create `NOVAS_FULL_ACCURACY` frames, with 
sub-&mu;as precision, you will you will need a high-precision ephemeris provider for the major planets (beyond the 
low-precision Earth and Sun calculator included by default), to account for gravitational bending around massive 
planets. Without it, &mu;as accuracy cannot be ensured, in general. Therefore, attempting to construct high-accuracy 
frames without an appropriate high-precision ephemeris provider will result in an error from the requisite 
`ephemeris()` call. 


#### Calculate an apparent place on sky

Now we can calculate the apparent R.A. and declination for our source, which includes proper motion (for sidereal
sources) or light-time correction (for Solar-system bodies), and also aberration corrections for the moving observer 
and gravitational deflection around the major Solar System bodies. You can calculate an apparent location in the 
coordinate system of choice (ICRS/GCRS, CIRS, J2000, MOD, or TOD):

```c
  sky_pos apparent;    // Structure containing the precise observed position
  
  novas_sky_pos(&source, &obs_frame, NOVAS_CIRS, &apparent);
```

Apart from providing precise apparent R.A. and declination coordinates, the `sky_pos` structure also provides the 
_x,y,z_ unit vector pointing in the observed direction of the source (in the designated coordinate system). We also 
get radial velocity (for spectroscopy), and apparent distance for Solar-system bodies (e.g. for apparent-to-physical 
size conversion).

Note, that if you want geometric positions (and/or velocities) instead, without aberration and gravitational 
deflection, you might use `novas_geom_posvel()` instead. And regardless, which function you use you can always easily 
and efficiently change the coordinate system in which your results are expressed by creating an appropriate transform 
via `novas_make_transform()` and then using `novas_transform_vector()` or `novas_transform_skypos()`.


#### Calculate azimuth and elevation angles at the observing location

If your ultimate goal is to calculate the azimuth and elevation angles of the source at the specified observing 
location, you can proceed from the `sky_pos` data you obtained above (in whichever coordinate system!) as:

```c
 double az, el;   // [deg] local azimuth and elevation angles to populate
  
 // Convert the apparent position in CIRS on sky to horizontal coordinates
 novas_app_to_hor(&obs_frame, NOVAS_CIRS, apparent.ra, apparent.dec, novas_standard_refraction, &az, &el);
```

Above we converted the apparent coordinates, assuming they were calculated in CIRS, to refracted azimuth and 
elevation coordinates at the observing location, using the `novas_standard_refraction()` function to provide a 
suitable refraction correction. We could have used `novas_optical_refraction()` instead to use the weather data 
embedded in the frame's `observer` structure, or some user-defined refraction model, or else `NULL` to calculate 
unrefracted elevation angles.


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
 set_planet_provider_hp(my_very_precise_planet_function);
  
 // Set the function to use for calculating all other solar-system bodies
 set_ephem_provider(my_ephemeris_provider_function);
```

Or, if you have the CALCEPH library installed on your system, and you have built SuperNOVAS with `CALCEPH_SUPPORT = 1`, 
then you might call:

```c
  #include <novas-calceph.h>
  
  // Use calceph to open se set of ephemeris files...
  t_calcephbin *ephem_data = calceph_open_array(...);
  
  // Use CALCEPH with the specified data for all Solar-system objects.
  novas_use_calceph(ephem_data);
```

Next, instead of `make_cat_object()` you define your source as an `object` with an name or ID number that is used by 
the ephemeris service you provided. For major planets you might want to use `make_planet()`, if they use a 
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

As of version 1.2 you can also define solar system sources with orbital elements (such as the most up-to-date ones 
provided by the [Minor Planet Center](https://minorplanetcenter.net/data) for asteroids, comets, etc.):

```c
  object NEA;		// e.g. a Near-Earth Asteroid
  
  // Fill in the orbital parameters (pay attention to units!)
  novas_orbital orbit = NOVAS_ORBIT_INIT;
  orbit.a = ...;	// Major axis in AU...
  ...			// ... and the rest of the orbital elements
  
  // Create an object for that orbit
  make_orbital_object("NEAxxx", -1, &orbit, &NEA);
```

Note, that even with orbital elements, you will, in general, require a planet calculator, to provide precise
positions for the Sun or planet, around which the orbit is defined.

Other than that, it's the same spiel as before, e.g.:

```c
 int status = novas_sky_pos(&mars, &obs_frame, NOVAS_TOD, &apparent);
 if(status) {
   // Oops, something went wrong...
   ...
 }
```

------------------------------------------------------------------------------

<a name="tips"></a>
## Tips and tricks

 - [Reduced accuracy shortcuts](#accuracy-notes)
 - [Multi-threaded calculations](#multi-threading)
 - [Physical units](#physical-units)


<a name="accuracy-notes"></a>
### Reduced accuracy shortcuts

When one does not need positions at the microarcsecond level, some shortcuts can be made to the recipe above:

 - You can use `NOVAS_REDUCED_ACCURACY` instead of `NOVAS_FULL_ACCURACY` for the calculations. This typically has an 
   effect at or below the milliarcsecond level only, but may be much faster to calculate.
 - You might skip the pole offsets _dx_, _dy_. These are tenths of arcsec, typically.
 
 
<a name="multi-threading"></a>
### Multi-threaded calculations

Some of the calculations involved can be expensive from a computational perspective. For the most typical use case
however, NOVAS (and SuperNOVAS) has a trick up its sleeve: it caches the last result of intensive calculations so they 
may be re-used if the call is made with the same environmental parameters again (such as JD time and accuracy).
 
A direct consequence of the caching of results in NOVAS is that calculations are generally not thread-safe as 
implemented by the original NOVAS C 3.1 library. One thread may be in the process of returning cached values for one 
set of input parameters while, at the same time, another thread is saving cached values for a different set of 
parameters. Thus, when running calculations in more than one thread, the results returned may at times be incorrect, 
or more precisely they may not correspond to the requested input parameters.
 
While you should never call NOVAS C from  multiple threads simultaneously, SuperNOVAS caches the results in thread
local variables (provided your compiler supports it), and is therefore generally safe to use in multi-threaded 
applications. Just make sure that you:

 - use a compiler which supports the C11 language standard;
 - or, compile with GCC &gt;= 3.3;
 - or else, set the appropriate non-standard keyword to use for declaring thread-local variables for your compiler in 
   `config.mk` or in your equivalent build setup.
 
 
<a name="physical-units"></a>
### Physical units

The NOVAS API has been using conventional units (e.g. AU, km, day, deg, h) typically for its parameters and return 
values alike. Hence, SuperNOVAS follows the same conventions for its added functions and data structures also. 
However, when interfacing SuperNOVAS with other programs, libraries, or data files, it is often necessary to use
quantities that are expressed in different units, such as SI or CGS. To facilitate such conversions, `novas.h` 
provides a set of unit constants, which can be used for converting to/from SI units (and radians). For example, 
`novas.h` contains the following definitions:

```c
  /// [s] The length of a synodic day, that is 24 hours exactly. @since 1.2
  #define NOVAS_DAY                 86400.0

  /// [rad] A degree expressed in radians. @since 1.2
  #define NOVAS_DEGREE              (M_PI / 180.0)

  /// [rad] An hour of angle expressed in radians. @since 1.2
  #define NOVAS_HOURANGLE           (M_PI / 12.0)
```

You can use these, for example, to convert quantities expressed in conventional units for NOVAS to standard (SI) 
values, by multiplying NOVAS quantities with the corresponding unit definition. E.g.:

```c
  // A difference in Julian Dates [day] in seconds.
  double delta_t = (tjd - tjd0) * NOVAS_DAY;
  
  // R.A. [h] / declination [deg] converted radians (e.g. for trigonometric functions).
  double ra_rad = ra_h * NOVAS_HOURANGLE;
  double dec_rad = dec_d * NOVAS_DEGREE; 
```

And vice-versa: to convert values expressed in standard (SI) units, you can divide by the appropriate constant to
'cast' an SI value into the particular physical unit, e.g.:

```c
  // Increment a Julian Date [day] with some time differential [s].
  double tjd = tjd0 + delta_t / NOVAS_DAY;
  
  // convert R.A. / declination in radians to hours and degrees
  double ra_h = ra_rad / NOVAS_HOURANGLE;
  double dec_d = dec_rad / NOVAS_DEGREE;
```

Finally, you can combine them to convert between two different conventional units, e.g.:

```c
  // Convert angle from [h] -> [rad] -> [deg]
  double lst_d = lst_h * HOURANGLE / DEGREE; 
  
  // Convert [AU/day] -> [m/s] (SI) -> [km/s]
  double v_kms = v_auday * (NOVAS_AU / NOVAS_DAY) / NOVAS_KM
```

 
-----------------------------------------------------------------------------

<a name="precision"></a>
## Notes on precision

Many of the (Super)NOVAS functions take an accuracy argument, which determines to what precision quantities are 
calculated. The argument can have one of two values, which correspond to typical precisions around:

 | `enum novas_accuracy` value  | Typical precision                |
 | ---------------------------- | -------------------------------- |
 | `NOVAS_REDUCED_ACCURACY`     | ~ 1 milli-arcsecond (mas)        |
 | `NOVAS_FULL_ACCURACY`        | below 1 micro-arcsecond (&mu;as) |

Note, that some functions will not support full accuracy calculations, unless you have provided a high-precision
ephemeris provider for the major planets (and any Solar-system bodies of interest), which does not come with 
SuperNOVAS out of the box. In the absense of a suitable high-precision ephemeris provider, some functions might return 
an error if called with `NOVAS_FULL_ACCURACY`.

### Prerequisites to precise results

The SuperNOVAS library is in principle capable of calculating positions to sub-microarcsecond, and velocities to mm/s, 
precision for all types of celestial sources. However, there are certain prerequisites and practical considerations 
before that level of accuracy is reached.

    
 1. __IAU 2000/2006 conventions__: High precision calculations will generally require that you use SuperNOVAS with the
    new IAU standard quantities and methods. The old ways were simply not suited for precision much below the 
    milliarcsecond level.
    
 2. __Gravitational bending__: Calculations much below the milliarcsecond level will require to account for 
    gravitational bending around massive Solar-system bodies, and hence will require you to provide a high-precision 
    ephemeris provider for the major planets. Without it, there is no guarantee of achieving the desired &mu;as-level 
    precision in general, especially when observing near massive planets (e.g. observing Jupiter's or Saturn's moons, 
    near transit). Therefore some functions will return with an error, if used with `NOVAS_FULL_ACCURACY` in the 
    absense of a suitable high-precision planetary ephemeris provider.

 3. __Solar-system sources__: Precise calculations for Solar-system sources requires precise ephemeris data for both
    the target object as well as for Earth, and the Sun. For the highest precision calculations you also need 
    positions for all major planets to calculate gravitational deflection precisely. By default SuperNOVAS can only 
    provide approximate positions for the Earth and Sun (see `earth_sun_calc()` in `solsys3.c`), but certainly not at 
    the sub-microarcsecond level, and not for other solar-system sources. You will need to provide a way to interface 
    SuperNOVAS with a suitable ephemeris source (such as the CSPICE toolkit from JPL or CALCEPH) if you want to use it 
    to obtain precise positions for Solar-system bodies. See the [section further below](#solarsystem) for more 
    information how you can do that.

 4. __Earth's polar motion__: Calculating precise positions for any Earth-based observations requires precise 
    knowledge of Earth orientation at the time of observation. The pole is subject to predictable precession and 
    nutation, but also small irregular variations in the orientation of the rotational axis and the rotation period 
    (a.k.a polar wobble). The [IERS Bulletins](https://www.iers.org/IERS/EN/Publications/Bulletins/bulletins.html) 
    provide up-to-date measurements, historical data, and near-term projections for the polar offsets and the UT1-UTC 
    (DUT1) time difference and leap-seconds (UTC-TAI). In SuperNOVAS you can use `cel_pole()` and `get_ut1_to_tt()` 
    functions to apply / use the published values from these to improve the astrometric precision of Earth-orientation
    based coordinate calculations. Without setting and using the actual polar offset values for the time of 
    observation, positions for Earth-based observations will be accurate at the tenths of arcsecond level only.
   
  5. __Refraction__: Ground based observations are also subject to atmospheric refraction. SuperNOVAS offers the 
    option to include approximate _optical_ refraction corrections either for a standard atmosphere or more precisely 
    using the weather parameters defined in the `on_surface` data structure that specifies the observer locations.
    Note, that refraction at radio wavelengths is notably different from the included optical model, and a standard
    radio refraction model is included as of version 1.1. In any case you may want to skip the refraction corrections 
    offered in this library, and instead implement your own as appropriate (or not at all).
  


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
 
   * If CIO locations vs GCRS are important to the user, the user may call `set_cio_locator_file()` at runtime to
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
   `planet_eph_manager()` vs `planet_ephem_provider()`. See the section on 
   [Building and installation](#installation) further above on including a selection of these in your library 
   build.)

 - New `novas_case_sensitive(int)` to enable (or disable) case-sensitive processing of object names. (By default NOVAS 
   `object` names are converted to upper-case, making them effectively case-insensitive.)

 - New `make_planet()` and `make_ephem_object()` to make it simpler to configure Solar-system objects.


#### Added in v1.1

 - New observing-frame based approach for calculations (`frames.c`). A `novas_frame` object uniquely defines both the 
   place and time of observation, with a set of pre-calculated transformations and constants. Once the frame is 
   defined it can be used very efficiently to calculate positions for multiple celestial objects with minimum 
   additional computational cost. The frames API is also more elegant and more versatile than the low-level NOVAS C 
   approach for performing the same kind of calculations. And, frames are inherently thread-safe since post-creation 
   their internal state is never modified during the calculations. The following new functions were added: 
   `novas_make_frame()`, `novas_change_observer()`, `novas_geom_posvel()`, `novas_geom_to_app()`, `novas_sky_pos()`, 
   `novas_app_to_hor()`, `novas_app_to_geom()`, `novas_hor_to_app()`, `novas_make_transform()`, 
   `novas_invert_transform()`, `novas_transform_vector()`, and `novas_transform_sky_pos()`.
   
 - New `novas_timespec` structure for the self-contained definition of precise astronomical time (`timescale.c`). You 
   can set the time via `novas_set_time()` or `novas_set_split_time()` to a JD date in the timescale of choice (UTC, 
   UT1, GPS, TAI, TT, TCG, TDB, or TCB), or to a UNIX time with `novas_set_unix_time()`. Once set, you can obtain an 
   expression of that time in any timescale of choice via `novas_get_time()`, `novas_get_split_time()` or 
   `novas_get_unix_time()`. And, you can create a new time specification by incrementing an existing one, using 
   `novas_increment_time()`, or measure time differences via `novas_diff_time()`, `novas_diff_tcg()`, or 
   `novas_diff_tcb()`.
 
 - Added `novas_planet_bundle` structure to handle planet positions and velocities more elegantly (e.g. for 
   gravitational deflection calculations).
   
 - `obs_posvel()` to calculate the observer position and velocity relative to the Solar System Barycenter (SSB).
 
 - `obs_planets()` to calculate planet positions (relative to observer) and velocities (w.r.t. SSB).
   
 - `grav_undef()` to undo gravitational bending of the observed light to obtain geometric positions from
   observed ones.
   
 - `grav_planets()` and `grav_undo_planets()` functions to apply/ or undo gravitational deflection using a specific
   set of gravitating bodies.
   
 - New coordinate reference systems `NOVAS_MOD` (Mean of Date) which includes precession by not nutation and
   `NOVAS_J2000` for the J2000 dynamical reference system.

 - New observer locations `NOVAS_AIRBORNE_OBSERVER` and `NOVAS_SOLAR_SYSTEM_OBSERVER`, and corresponding
   `make_airborne_observer()` and `make_solar_system_observer()` functions. Airborne observers have an Earth-fixed
   momentary location, defined by longitude, latitude, and altitude, the same way as for a stationary observer on
   Earth, but are moving relative to the surface, such as in an aircraft or balloon based observatory. Solar-system
   observers are similar to observers in Earth-orbit but their momentary position and velocity is defined relative
   to the Solar System Barycenter (SSB), instead of the geocenter.

 - Added humidity field to `on_surface` structure, e.g. for refraction calculations at radio wavelengths. The
   `make_on_surface()` function will set humidity to 0.0, but the user can set the field appropriately afterwards.

 - New set of built-in refraction models to use with the frame-based `novas_app_to_hor()` / `novas_hor_to_app()` 
   functions. The models `novas_standard_refraction()` and `novas_optical_refraction()` implement the same refraction 
   model as `refract()` in NOVAS C 3.1, with `NOVAS_STANDARD_ATMOSPHERE` and `NOVAS_WEATHER_AT_LOCATION` 
   respectively, including the reversed direction provided by `refract_astro()`. The user may supply their own custom 
   refraction model also, and may make use of the generic reversal function `novas_inv_refract()` to calculate 
   refraction in the reverse direction (observer vs astrometric elevations) as needed.

 - Added radio refraction model `novas_radio_refraction()` based on the formulae by Berman &amp; Rockwell 1976.
 
 - Added `cirs_to_tod()` and `tod_to_cirs()` functions for efficient transformation between True of Date (TOD) and
   Celestial Intermediate Reference System (CIRS), and vice versa.

 - Added `make_cat_object()` function to create a NOVAS celestial `object` structure from existing `cat_entry` data.


#### Added in v1.2

 - New `make_redshifted_cat_entry()` and `make_redshifted_object()` to simplify the creation of distant catalog 
   sources that are characterized with a redshift measure rather than a radial velocity value.

 - New generic redshift-handling functions `novas_v2z()`, `novas_z2v()`, 
 
 - New functions to calculate and apply additional gravitational redshift corrections for light that originates
   near massive gravitating bodies (other than major planets, or Sun or Moon), or for observers located near massive
   gravitating bodies (other than the Sun and Earth). The added functions are `grav_redshift()`, `redhift_vrad()`,
   `unredshift_vrad()`, `novas_z_add()`, and `novas_z_inv()`.

 - CALCEPH integration: `novas_use_calceph()` and/or `novas_use_calceph_planets()` to specify and use ephemeris data 
   via CALCEPH for Solar-system sources in general, and for major planets specifically; and `novas_calceph_use_ids()` 
   to specify whether `object.number` in `NOVAS_EPHEM_OBJECT` type objects is a NAIF ID (default) or else a CALCEPH ID 
   number of the Solar-system body.
   
 - NAIF CSPICE integration: `novas_use_cspice()`, `novas_use_cspice_planets()`, `novas_use_cspice_ephem()` to use the 
   NAIF CSPICE library for all Solar-system sources, major planets only, or for other bodies only. 
   `NOVAS_EPHEM_OBJECTS` should use NAIF IDs with CSPICE (or else -1 for name-based lookup). Also provides
   `cspice_add_kernel()` and `cspice_remove_kernel()`.
   
 - NAIF/NOVAS ID conversions for major planets (and Sun, Moon, SSB...): `novas_to_naif_planet()`, 
   `novas_to_dexxx_planet()`, and `naif_to_novas_planet()`.
   
 - Access to custom ephemeris provider functions: `get_planet_provider()` and `get_planet_provider_hp()`.

 - Added `novas_planet_for_name()` function to return the NOVAS planet ID for a given (case insensitive) name.

 - Added support for using orbital elements. `object.type` can now be set to `NOVAS_ORBITAL_OBJECT`, whose orbit can 
   be defined by the set of `novas_orbital`, relative to a `novas_orbital_system`. You can initialize an `object` with 
   a set of orbital elements using `make_orbital_object()`, and for planetary satellite orbits you might use 
   `novas_set_orbsys_pole()`. For orbital objects, `ephemeris()` will call on the new `novas_orbit_posvel()` to 
   calculate positions. While orbital elements do not always yield precise positions, they can for shorter periods, 
   provided that the orbital elements are up-to-date. For example, the Minor Planer Center (MPC) publishes accurate 
   orbital elements for all known asteroids and comets regularly. For newly discovered objects, this may be the only 
   and/or most accurate information available anywhere.

 - Added `NOVAS_EMB` (Earth-Moon Barycenter) and `NOVAS_PLUTO_BARYCENTER` to `enum novas_planets` to distinguish
   from the corresponding planet centers in calculations.
 
 - Added `gcrs_to_tod()` / `tod_to_gcrs()` and `gcrs_to_mod()` / `mod_to_gcrs()` vector conversion functions for
   convenience.
   
 - Added various `object` initializer macros in `novas.h` for the major planets, Sun, Moon, and barycenters, e.g. 
   `NOVAS_EARTH_INIT` or `NOVAS_SSB_INIT`. These wrap the parametric `NOVAS_PLANET_INIT(num, name)` macro, and can be
   used to simplify the initialization of NOVAS `object`s.

 - Added more physical unit constants to `novas.h`.


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
   required (see the [API Documentation](https://smithsonian.github.io/SuperNOVAS/apidoc/html/) for specifics).
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
   
 - `cio_location()` will always return a valid value as long as neither output pointer argument is NULL. (NOVAS C
   3.1 would return an error if a CIO locator file was previously opened but cannot provide the data for whatever
   reason). 

 - `cel2ter()` and `ter2cel()` can now process 'option'/'class' = 1 (`NOVAS_REFERENCE_CLASS`) regardless of the
   methodology (`EROT_ERA` or `EROT_GST`) used to input or output coordinates in GCRS.
   
 - More efficient paging (cache management) for `cio_array()`, including I/O error checking.
 
 - IAU 2000A nutation model uses higher-order Delaunay arguments provided by `fund_args()`, instead of the linear
   model in NOVAS C 3.1.
   
 - IAU 2000 nutation made a bit faster, reducing the the number of floating-point multiplications necessary by 
   skipping terms that do not contribute. Its coefficients are also packed more frugally in memory, resulting in a
   smaller footprint.
   
 - Changed the standard atmospheric model for (optical) refraction calculation to include a simple model for the 
   annual average temperature at the site (based on latitude and elevation). This results is a slightly more educated 
   guess of the actual refraction than the global fixed temperature of 10 &deg;C assumed by NOVAC C 3.1 regardless of 
   observing location.
   
 - [__v1.1__] Improved precision of some calculations, like `era()`, `fund_args()`, and `planet_lon()` by being more 
   careful about the order in which terms are accumulated and combined, resulting in a small improvement on the few 
   uas (micro-arcsecond) level.
   
 - [__v1.1__] `place()` now returns an error 3 if and only if the observer is at (or very close, within ~1.5m) of the 
   observed Solar-system object.

 - [__v1.1__] `grav_def()` is simplified. It no longer uses the location type argument. Instead it will skip 
   deflections due to a body if the observer is within ~1500 km of its center (which is below the surface for all
   major Solar system bodies).

 - [__v1.1.1__] For major planets (and Sun and Moon) `rad_vel()` and `place()` will include gravitational corrections 
   to radial velocity for light originating at the surface, and observed near Earth or at a large distance away from 
   the source.

-----------------------------------------------------------------------------

<a name="solarsystem"></a>
## Incorporating Solar-system ephemeris data or services

If you want to use SuperNOVAS to calculate positions for a range of Solar-system objects, and/or to do it with 
precision, you will have to interface it to a suitable provider of ephemeris data. Given the NOVAS C heritage, and 
some added SuperNOVAS flexibility in this area, you have several options for going about it. These are listed from the 
most practical (and preferred) to the least so (the old ways). 

NASA/JPL provides [generic ephemerides](https://naif.jpl.nasa.gov/pub/naif/generic_kernels/spk/) for the major 
planets, satellites thereof, the 300 largest asteroids, the Lagrange points, and some Earth orbiting stations. For 
example, [DE440](https://naif.jpl.nasa.gov/pub/naif/generic_kernels/spk/planets/de440.bsp) covers the major planets, 
and the Sun, Moon, and barycenters for times between 1550 AD and 2650 AD. Or, you can use the 
[JPL HORIZONS](https://ssd.jpl.nasa.gov/horizons/) system (via the commnad-line / telnet or API interfaces) to 
generate custom ephemerides (SPK/BSP) for just about all known solar systems bodies, down to the tiniest rocks.

 - [Optional CALCEPH integration](#calceph-integration)
 - [Optional NAIF CSPICE toolkit integration](#cspice-integration)
 - [Universal ephemeris data / service integration](#universal-ephemerides)
 - [Legacy support for (older) JPL major planet ephemerides](#builtin-ephem-readers)
 - [Legacy linking of custom ephemeris functions](#explicit-ephem-linking)


<a name="calceph-integration"></a>
### Optional CALCEPH integration

The [CALCEPH](https://www.imcce.fr/recherche/equipes/asd/calceph/) library provides easy-to-use access to JPL and 
INPOP ephemeris files from C/C++. As of version 1.2, we provide optional support for interfacing SuperNOVAS with the 
the CALCEPH C library for handling Solar-system objects.

Prior to building SuperNOVAS simply set `CALCEPH_SUPPORT` to 1 in `config.mk` or in your environment. Depending on the 
build target, it will build `libsolsys-calceph.so[.1]` (target `shared`) or `libsolsys-calceph.a` (target `static`) 
libraries or `solsys-calceph.o` (target `solsys`), which provide the `novas_use_calceph()` and 
`novas_use_calceph_planets()`, and `novas_calceph_use_ids()` functions.

Of course, you will need access to the CALCEPH C development files (C headers and unversioned `libcalceph.so` or `.a` 
library) for the build to succeed. Here is an example on how you'd use CALCEPH with SuperNOVAS in your application 
code:

```c
  #include <novas.h>
  #include <novas-calceph.h>
  
  // You can open a set of JPL/INPOP ephemeris files with CALCEPH...
  t_calcephbin *eph = calceph_open_array(...);
  
  // Then use them as your generic SuperNOVAS ephemeris provider
  int status = novas_use_calceph(eph);
  if(status < 0) {
    // Ooops something went wrong...
  }
  
  // -----------------------------------------------------------------------
  // Optionally you may use a separate ephemeris dataset for major planets
  // (or if planet ephemeris was included in 'eph' above, you don't have to) 
  t_calcephbin *pleph = calceph_open(...);
  status = novas_use_calceph_planets(pleph);
  if(status < 0) {
    // Ooops something went wrong...
  }
```

All modern JPL (SPK) ephemeris files should work with the `solsys-calceph` plugin. When linking your application, 
add `-lsolsys-calceph` to your link flags (or else link with `solsys-calceph.o`). That's all there is to it.


<a name="cspice-integration"></a>
### Optional NAIF CSPICE toolkit integration

The [NAIF CSPICE Toolkit](https://naif.jpl.nasa.gov/naif/toolkit.html) is the canonical standard library for JPL 
ephemeris files from C/C++. As of version 1.2, we provide optional support for interfacing SuperNOVAS with CSPICE for 
handling Solar-system objects.

Prior to building SuperNOVAS simply set `CSPICE_SUPPORT` to 1 in `config.mk` or in your environment. Depending on the 
build target, it will build `libsolsys-cspice.so[.1]` (target `shared`) or `libsolsys-cspice.a` (target `static`) 
libraries or `solsys-cspice.o` (target `solsys`), which provide the `novas_use_cspice()`, 
`novas_use_cspice_planets()`, and `novas_use_cspice_ephem()` functions to enable CSPICE for providing data for all 
Solar-system sources, or for major planets only, or for other bodies only, respectively. You can also manage the 
active kernels with the `cspice_add_kernel()` and `cspice_remove_kernel()` functions.

Of course, you will need access to the CSPICE development files (C headers, installed under a `cspice/` directory 
of an header search location, and the unversioned `libcspice.so` or `.a` library) for the build to succeed. You may 
want to check out the [Smithsonian/cspice-sharedlib](https://github.com/Smithsonian/cspice-sharedlib) GitHub 
repository to help you build CSPICE with shared libraries and dynamically linked tools.

Here is an example on how you might use CSPICE with SuperNOVAS in your application code:

```c
  #include <novas.h>
  #include <novas-cspice.h>

  // You can load the desired kernels for CSPICE
  // E.g. load DE440s and the Mars satellites from /data/ephem:
  int status;
  
  status = cspice_add_kernel("/data/ephem/de440s.bsp");
  if(status < 0) {
    // oops, the kernels must not have loaded...
    ...
  }
  
  // Load additional kernels as needed...
  status = cspice_add_kernel("/data/ephem/mar097.bsp");
  ...
  
  // Then use CSPICE as your SuperNOVAS ephemeris provider
  novas_use_cspice();
```

All JPL ephemeris data will work with the `solsys-cspice` plugin. When linking your application, add `-lsolsys-cspice` 
to your link flags (or else link with `solsys-cspice.o`). That's all there is to it.


<a name="universal-ephemerides"></a>
### Universal ephemeris data / service integration 

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
and returns the type of origin with corresponding ICRS position and velocity vectors in the supplied pointer 
locations. The function can use either the ID number or the name to identify the object or file (whatever is the most 
appropriate for the implementation and for the supplied parameters). The positions and velocities may be returned 
either relative to the SSB or relative to the  heliocenter, and accordingly, your function should set the value 
pointed at by origin to `NOVAS_BARYCENTER` or `NOVAS_HELIOCENTER` accordingly. Positions and velocities are 
rectangular ICRS _x,y,z_ vectors in units of AU and AU/day respectively. 

This way you can easily integrate current ephemeris data, e.g. for the Minor Planet Center (MPC), or whatever other 
ephemeris service you prefer.

Once you have your adapter function, you can set it as your ephemeris service via `set_ephem_provider()`:

```c
 set_ephem_provider(my_ephem_reader);
```

By default, your custom `my_ephem_reader` function will be used for 'minor planets' only (i.e. anything other than the 
major planets, the Sun, Moon, Solar-system Barycenter...). But, you can use the same function for the mentioned 
'major planets' also via:

```c
 set_planet_provider(planet_ephem_provider);
 set_planet_provider_hp(planet_ephem_provider_hp);
```

provided you compiled SuperNOVAS with `BUILTIN_SOLSYS_EPHEM = 1` (in `config.mk`), or else you link your code against
`solsys-ephem.c` explicitly. Easy-peasy.



<a name="builtin-ephem-readers"></a>
### Legacy support for (older) JPL major planet ephemerides

If you only need support for major planets, you may be able to use one of the modules included with the distribution. 
The legacy NOVAS modules `solsys1.c` and `solsys2.c` provide built-in support to older JPL ephemerides (DE200 to DE421), 
either via the `eph_manager` interface of `solsys1.c` or via the FORTRAN `pleph` interface with `solsys2.c`.

#### Planets via `eph_manager`

To use the `eph_manager` interface for planet 1997 JPL planet ephemeris (DE200 through DE421), you must either build 
SuperNOVAS with `BUILTIN_SOLSYS1 = 1` in `config.mk`, or else link your application with `solsys1.c` and 
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
retrieve data from multiple ephemeris input files at the same time. (But you can with CALCEPH or the CSPICE toolkit, 
either of which you can interface as discussed further above!)

That's all, except the warning that this method will not work with newer JPL ephemeris data, beyond DE421.


#### Planets via JPL's `pleph` FORTRAN interface

To interface eith the JPL PLEPH library (FORTRAN) for planet ephemerides, you must either build SuperNOVAS with 
`BUILTIN_SOLSYS2 = 1` in `config.mk`, or else link your application with `solsys2.c` and your appropriately modified 
`jplint.f` (from the `examples` sub-directory) explicitly, together with the JPL PLEPH library. If you want this to 
be your default ephemeris provider (the old way) you might also want to set `DEFAULT_SOLSYS = 2` in `config.mk`. 
Otherwise, your application should set your planetary ephemeris provider at runtime via:

```c
 set_planet_provider(planet_jplint);
 set_planet_provider_hp(planet_jplint_hp);
```

Integrating JPL ephemeris data this way can be arduous. You will need to compile and link FORTRAN with C (not the end
of the world), but you may also have to modify `jplint.f` (providing the intermediate FORTRAN `jplint_()` / 
`jplihp_()` interfaces to `pleph_()`) to work with the version of `pleph.f` that you will be using. Unless you already 
have code that relies on this method, you are probably better off choosing one of the other ways for integrating 
planetary ephemeris data with SuperNOVAS.


<a name="explicit-ephem-linking"></a>
### Legacy linking of custom ephemeris functions

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

<a name="debug-support"></a>
## Runtime debug support

You can enable or disable debugging output to `stderr` with `novas_debug(enum novas_debug_mode)`, where the argument 
is one of the defined constants from `novas.h`:

 | `novas_debug_mode` value   | Description                                        |
 | -------------------------- | -------------------------------------------------- |
 | `NOVAS_DEBUG_OFF`          | No debugging output (_default_)                    |
 | `NOVAS_DEBUG_ON`           | Prints error messages and traces to `stderr`       |
 | `NOVAS_DEBUG_EXTRA`        | Same as above but with stricter error checking     |
 
The main difference between `NOVAS_DEBUG_ON` and `NOVAS_DEBUG_EXTRA` is that the latter will treat minor issues as 
errors also, while the former may ignore them. For example, `place()` will return normally by default if it cannot 
calculate gravitational bending around massive planets in full accuracy mode. It is unlikely that this omission would 
significantly alter the result in most cases, except for some very specific ones when observing in a direction close 
to a major planet. Thus, with `NOVAS_DEBUG_ON`, `place()` go about as usual even if the Jupiter's position is not 
known. However, `NOVAS_DEBUG_EXTRA` will not give it a free pass, and will make `place()` return an error (and print 
the trace) if it cannot properly account for gravitational bending around the major planets as it is expected to.


-----------------------------------------------------------------------------


<a name="release-schedule"></a>
## Release schedule

A predictable release schedule and process can help manage expectations and reduce stress on adopters and developers 
alike.

Releases of the library shall follow a quarterly release schedule. You may expect upcoming releases 
to be published around __February 1__, __May 1__, __August 1__, and/or __November 1__ each year, on an as-needed
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
temporarily on GitHub, under [Releases](https://github.com/Smithsonian/SuperNOVAS/releases), so that changes can be 
tested by adopters before the releases are finalized. Please use due diligence to test such release candidates with 
your code when they become available to avoid unexpected surprises when the finalized release is published. Release 
candidates are typically available for one week only before they are superseded either by another, or by the finalized 
release.

-----------------------------------------------------------------------------
Copyright (C) 2024 Attila Kovács
