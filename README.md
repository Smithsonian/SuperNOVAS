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

 - Many SuperNOVAS functions allow NULL arguments, both for optional input values as well as outputs that are not 
   required. See the [API Documentation](https://smithsonian.github.io/SuperNOVAS.home/apidoc/html/) for specifics).
   This eliminates the need to declare dummy variables in your application code.

 - All SuperNOVAS functions that take an input vector to produce an output vector allow the output vector argument
   be the same as the input vector argument. For example, `frame_time(pos, TIE_J2000_TO_ICRS, pos)` using the same 
   `pos` vector both as the input and the output. In this case the `pos` vector is modified in place by the call. 
   This can greatly simplify, and eliminate extraneous declarations, usage when intermediates are not required.

 - SuperNOVAS prototypes declare function pointer arguments as `const` whenever the function does not modify the
   data content being pointed at. This supports better programming practices that generally aim to avoid unintended 
   data modifications.

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
     approximation via `nutation_set_lp()`. For example, the user may want to use the `iau2000b()` model instead or
     some custom algorithm instead.
 
 - Added several new convenience functions to wrap `place()` for simpler specific use: `place_star()`, `place_icrs()`, 
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

The SuperNOVAS library is in principle capable of calculating positions and velocities to sub-microarcsecond precision 
for all types of celestial sources. However, there are certain pre-requisites and practical considerations before that 
level of accuracy is reached.


 1. __Earth's polar motion__: Calculating precise positions for any Earth-based observations requires precise 
    knowledge of Earth orientation at the time of observation. The pole is subject to predictable precession and 
    nutation, but  also small irregular variations in the orientation of the rotational axis and the rotation period 
    (a.k.a polar wobble). The [IERS Bulletins](https://www.iers.org/IERS/EN/Publications/Bulletins/bulletins.html) 
    provide up-to-date measurements, historical data, and near-term projections for the polar offsets and the UT1-UTC 
    (DUT1) time difference and leap-seconds (UTC-TAI). In SuperNOVAS you can use `cel_pole()` and `get_ut1_to_tt()` 
    functions to apply / use the published values from these to improve the astrometic precision of calls such as 
    `calc_pos()`, or `topo_star()`. Without setting and using the actual polar offset values for the time of 
    observation, positions for Earth-based observations will be accurate at the arcsecond level only.
 
 2. __Solar-system sources__: Precise calculations for Solar-system sources requires precise ephemeris data for both
    the target object as well as for Earth, and the Sun vs the Solar-system barycenter. By default SuperNOVAS can only 
    provide  approximate positions for the Earth and Sun (see `earth_sun_calc()` in `solsys3.c`), but certainly not at 
    the sub-microarcsecond level, and not for other solar-system sources. You will need to provide a way to interface
    SuperNOVAS with a suitable ephemeris source (such as the CSPICE toolkit from JPL) if you want to use it to 
    obtain precise positions for Solar-system bodies. See the [section below](#solarsystem)for more information how 
    you can do that.
    
  3. __Refraction__: Ground based observations are also subject to atmospheric refraction. SuperNOVAS offers the 
    option to include _optical_ refraction corrections in `equ2hor()` either for a standard atmosphere or more 
    precisely using the weather parameters defined in the `on_surface` data structure that specifies the observer 
    locations. Note, that refraction at radio wavelengths is notably different from the included optical value. In 
    either case you may want to skip the refraction corrections offered in this library, and instead implement your 
    own as appropriate.
 

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


