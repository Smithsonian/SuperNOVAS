# Changelog

All notable changes to the SuperNOVAS library will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project adheres to 
[Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## [Unreleased]

Changes for the upcoming release of SuperNOVAS, possibly around 1 June 2024. Stay tuned...

### Added

 - Added `cirs_to_app_ra()` and `app_to_cirs_ra()` for convenience to convert between right ascensions measured from
   the CIO (for CIRS) vs measured from the true equinox of date, on the same true equator of date.
   
### Changes

 - Various corrections and changes to documentation.



## [1.0.0] - 2024-03-01

This is the initial release of the SuperNOVAS library. Changes are indicated w.r.t. the upstream NOVAS C 3.1 library 
from which SuperNOVAS is forked from.

### Fixed

 - Fixes the NOVAS C 3.1 [sidereal_time bug](https://aa.usno.navy.mil/software/novas_faq), whereby the 
   `sidereal_time()` function had an incorrect unit cast.
   
 - Some remainder calculations in NOVAS C 3.1 used the result from `fmod()` unchecked, which led to the wrong results 
   when the numerator was negative. This affected the calculation of the mean anomaly in `solsys3.c` (line 261) and 
   the fundamental arguments calculated in `fund_args()` and `ee_ct()` for dates prior to J2000. Less critically, it 
   also was the reason `cal_date()` did not work for negative JD values.
   
 - Fixes antedating velocities and distances for light travel time in NOVAS C 3.1 `ephemeris()`. When getting 
   positions and velocities for Solar-system sources, it is important to use the values from the time light originated 
   from the observed body rather than at the time that light arrives to the observer. This correction was done 
   properly for positions, but not for velocities or distances, resulting in incorrect observed radial velocities or 
   apparent distances being reported for spectroscopic observations or for angular-physical size conversions. 
   
 - Fixes NOVAS C 3.1 bug in `ira_equinox()` which may return the result for the wrong type of equinox (mean vs. true) 
   if the `equinox` argument was changing from 1 to 0, and back to 1 again with the date being held the same. This 
   affected NOVAS C 3.1 routines downstream also, such as `sidereal_time()`.
   
 - Fixes NOVAS C 3.1 accuracy switching bug in `cio_basis()`, `cio_location()`, `ecl2equ()`, `equ2ecl_vec()`, 
   `ecl2equ_vec()`, `geo_posvel()`, `place()`, and `sidereal_time()`. All these functions returned a cached value for 
   the other accuracy if the other input parameters are the same as a prior call, except the accuracy. 
   
 - Fixes multiple NOVAS C 3.1 bugs related to using cached values in `cio_basis()` with alternating CIO location 
   reference systems. This affected many CIRS-based position calculations downstream.
   
 - Fixes NOVAS C 3.1 bug in `equ2ecl_vec()` and `ecl2equ_vec()` whereby a query with `coord_sys = 2` (GCRS) has 
   overwritten the cached mean obliquity value for `coord_sys = 0` (mean equinox of date). As a result, a subsequent 
   call with `coord_sys = 0` and the same date as before would return the results GCRS coordinates instead of the 
   requested mean equinox of date coordinates.
 
 - Fixes NOVAS C 3.1 `aberration()` returning NaN vectors if the `ve` argument is 0. It now returns the unmodified 
   input vector appropriately instead.
   
 - Fixes unpopulated `az` output value in `equ2hor()` at zenith in NOVAS C 3.1. While any azimuth is acceptable 
   really, it results in unpredictable behavior. Hence, we set `az` to 0.0 for zenith to be consistent.
   
 - Fixes potential NOVAS C 3.1 string overflows and eliminates associated compiler warnings.
 
 - Fixes the NOVAS C 3.1 [ephem_close bug](https://aa.usno.navy.mil/software/novas_faq), whereby `ephem_close()` in 
   `eph_manager.c` did not reset the `EPHFILE` pointer to NULL.
   
 - Supports calculations in parallel threads by making cached results thread-local.
 

### Added

 - New debug mode and error traces. Simply call `novas_debug(NOVAS_DEBUG_ON)` or `novas_debug(NOVAS_DEBUG_EXTRA)`
   to enable. When enabled, any error conditions (such as NULL pointer arguments, or invalid input values etc.) will
   be reported to the standard error, complete with call tracing within the SuperNOVAS library, s.t. users can have
   a better idea of what exactly did not go to plan (and where). The debug messages can be disabled by passing
   `NOVAS_DEBUF_OFF` (0) as the argument to the same call.

 - Added Doxygen markup of source code and header.
  
 - Added Makefile for GNU make.
 
 - Added continuous integration on GitHub, including regression testing, static analysis, and doxygen validation.

 - Added an number of precompiler constants and enums in `novas.h` to promote consistent usage and easier to read
   code.

 - New runtime configurability:

   * The planet position calculator function used by `ephemeris()` can be set at runtime via `set_planet_provider()`, 
     and `set_planet_provider_hp()` (for high precision calculations). Similarly, if `planet_ephem_provider()` or 
     `planet_ephem_provider_hp()` (defined in `solsys-ephem.c`) are set as the planet calculator functions, then 
     `set_ephem_provider()` can set the user-specified function to use with these to actually read ephemeris data
     (e.g. from a JPL ephemeris file).
 
   * If CIO locations vs GCRS are important to the user, the user may call `set_cio_locator_file()` at runtime to
     specify the location of the binary CIO interpolation table (e.g. `cio_ra.bin`) to use, even if the library was
     compiled with the different default CIO locator path. 
 
   * The default low-precision nutation calculator `nu2000k()` can be replaced by another suitable IAU 2006 nutation
     approximation via `set_nutation_lp_provider()`. For example, the user may want to use the `iau2000b()` model 
     instead or some custom algorithm instead.
 
 - New intutitive XYZ coordinate conversion functions:
   * for GCRS - CIRS - ITRS (IAU 2000 standard): `gcrs_to_cirs()`, `cirs_to_itrs()`, and `itrs_to_cirs()`, 
     `cirs_to_gcrs()`.
   * for GCRS - J2000 - TOD - ITRS (old methodology): `gcrs_to_j2000()`, `j2000_to_tod()`, `tod_to_itrs()`, and 
     `itrs_to_tod()`, `tod_to_j2000()`, `j2000_to_gcrs()`.

 - New `itrs_to_hor()` and `hor_to_itrs()` functions to convert Earth-fixed ITRS coordinates to astrometric azimuth 
   and elevation or back. Whereas `tod_to_itrs()` followed by `itrs_to_hor()` is effectively a just a more explicit 
   2-step version of the existing `equ2hor()` for converting from TOD to to local horizontal (old methodology), the 
   `cirs_to_itrs()`  followed by `itrs_to_hor()` does the same from CIRS (new IAU standard methodology), and had no 
   prior equivalent in NOVAS C 3.1.

 - New `ecl2equ()` for converting ecliptic coordinates to equatorial, complementing existing `equ2ecl()`.
   
 - New `gal2equ()` for converting galactic coordinates to ICRS equatorial, complementing existing `equ2gal()`.
   
 - New `refract_astro()` complements the existing `refract()` but takes an unrefracted (astrometric) zenith angle as 
   its argument.

 - New convenience functions to wrap `place()` for simpler specific use: `place_star()`, `place_icrs()`, 
   `place_gcrs()`, `place_cirs()`, and `place_tod()`.
 
 - New `radec_star()` and `radec_planet()` as the common point for existing functions such as `astro_star()`
   `local_star()`, `virtual_planet()`, `topo_planet()` etc.
   
 - New time conversion utilities `tt2tdb()`, `get_utc_to_tt()`, and `get_ut1_to_tt()` make it simpler to convert 
   between UTC, UT1, TT, and TDB time scales, and to supply `ut1_to_tt` arguments to `place()` or topocentric 
   calculations.
 
 - Co-existing `solarsystem()` variants. It is possible to use the different `solarsystem()` implementations 
   provided by `solsys1.c`, `solsys2.c`, `solsys3.c` and/or `solsys-ephem.c` side-by-side, as they define their
   functionalities with distinct, non-conflicting names, e.g. `earth_sun_calc()` vs `planet_jplint()` vs
   `planet_eph_manager` vs `planet_ephem_provider()`.

 - New `novas_case_sensitive(int)` to enable (or disable) case-sensitive processing of object names. (By default NOVAS 
   `object` names are converted to upper-case, making them effectively case-insensitive.)

 - New `make_planet()` and `make_ephem_object()` to make it simpler to configure Solar-system objects.


### Changed

 - Changed to support for calculations in parallel threads by making cached results thread-local (as opposed to the 
   globally cached values in NOVAS C 3.1). This works using the C11 standard `_Thread_local` or else the earlier GNU C 
   &gt;= 3.3 standard `__thread` modifier. You can also set the preferred thread-local keyword for your compiler by 
   passing it via `-DTHREAD_LOCAL=...` in `config.mk` to ensure that your build is thread-safe. And, if your compiler 
   has no support whatsoever for thread_local variables, then SuperNOVAS will not be thread-safe, just as NOVAS C 
   isn't.

 - SuperNOVAS functions take `enum`s as their option arguments instead of the raw integers in NOVAS C 3.1. These enums 
   are defined in `novas.h`. The same header also defines a number of useful constants. The enums allow for some 
   compiler checking, and make for more readable code that is easier to debug. They also make it easy to see what 
   choices are available for each function argument, without having to consult the documentation each and every time.

 - All SuperNOVAS functions check for the basic validity of the supplied arguments (Such as NULL pointers or illegal 
   duplicate arguments) and will return -1 (with `errno` set, usually to `EINVAL`) if the arguments supplied are
   invalid (unless the NOVAS C API already defined a different return value for specific cases. If so, the NOVAS C
   error code is returned for compatibility). There were no such checks performed in NOVAS C 3.1.
   
 - All erroneous returns now set `errno` so that users can track the source of the error in the standard C way and use 
   functions such as `perror()` and `strerror()` to print human-readable error messages. (NOVAS C 3.1 did not set 
   `errno`).
   
 - Many output values supplied via pointers are set to clearly invalid values in case of erroneous returns, such as
   `NAN` so that even if the caller forgets to check the error code, it becomes obvious that the values returned 
   should not be used as if they were valid. (No more sneaky silent errors, which were common in NOVAS C 3.1.)

 - Many SuperNOVAS functions allow `NULL` arguments (unlike NOVAS C 3.1), both for optional input values as well as 
   outputs that are not required (see the [API Documentation](https://smithsonian.github.io/SuperNOVAS/apidoc/html/) 
   for specifics). This eliminates the need to declare dummy variables in your application code for quantities you do 
   not require.

 - All SuperNOVAS functions that take an input vector to produce an output vector allow the output vector argument
   be the same as the input vector argument (unlike in NOVAS C 3.1 where this was not consistently implented). For 
   example, `frame_tie(pos, J2000_TO_ICRS, pos)` using the same `pos` vector both as the input and the output. In this 
   case the `pos` vector is modified in place by the call. This can greatly simplify usage, and can eliminate 
   extraneous declarations, when intermediates are not required.
   
 - SuperNOVAS declares function pointer arguments as `const` whenever the function does not modify the data content 
   being referenced. This supports better programming practices that generally aim to avoid unintended data 
   modifications. (The passing of `const` arguments to NOVAS C 3.1 calls would result in compiler warnings.)
 
 - Catalog names can be up to 6 bytes (including termination), up from 4 in NOVAS C 3.1, while keeping `struct` 
   layouts the same as NOVAS C thanks to alignment, thus allowing cross-compatible binary exchange of `cat_entry` 
   records with NOVAS C 3.1.
   
 - Object ID numbers are `long` instead of `short` (in NOVAS C 3.1) to accommodate NAIF IDs, which require minimum 
   32-bit integers.
 
 - `cel2ter()` and `ter2cel()` can now process 'option'/'class' = 1 (`NOVAS_REFERENCE_CLASS`) regardless of the
   methodology (`EROT_ERA` or `EROT_GST`) used to input or output coordinates in GCRS (unlike in NOVAS C 3.1).
 
 - Changed `make_object()` to retain the specified number argument (which can be different from the `starnumber` value
   in the supplied `cat_entry` structure), in contrast to NOVAS C 3.1, which set `object->number` to 0 for `cat_entry`
   arguments.
   
 - `cio_location()` will always return a valid value as long as neither output pointer argument is NULL. (NOVAS C
   3.1 would return an error if a CIO locator file was previously opened but cannot provide the data for whatever
   reason). 
   
 - `sun_eph()` in `solsysl3.c` evaluates the series in reverse order compared to NOVAS C 3.1, accumulating the least 
   significant terms first, and thus resulting in higher precision result in the end.
   
 - Changed `vector2radec()` to return NAN values if the input is a null-vector (i.e. all components are zero), as
   opposed to NOVAS C 3.1, which left the input vector argument unchanged.
   
 - IAU 2000A nutation model uses higher-order Delaunay arguments provided by `fund_args()`, instead of the linear
   model in NOVAS C 3.1.
   
 - IAU 2000 nutation made a bit faster vs NOVAS C 3.1, via reducing the the number of floating-point multiplications 
   necessary by skipping terms that do not contribute. Its coefficients are also packed more frugally in memory, 
   resulting in a smaller footprint than in NOVAS C 3.1.
   
 - More efficient paging (cache management) for `cio_array()` vs NOVAS C 3.1, including I/O error checking.
 
 - Changed the standard atmospheric model for (optical) refraction calculation to include a simple model for the 
   annual average temperature at the site (based on latitude and elevation). This results is a slightly more educated 
   guess of the actual refraction than the global fixed temperature of 10 &deg;C assumed by NOVAC C 3.1 regardless of 
   observing location.
   

### Deprecated

 - `novascon.h` / `novascon.c`: These definitions of constants in NOVAS C 3.1 was troublesome for two reasons: (1) 
   They were primarily meant for use internally within the library itself. As the library clearly defines in what 
   units input and output quantities are expressed, the user code can apply its own appropriate conversions that need 
   not match the internal system used by the library. Hence exposing these constants to users was half baked. (2) The 
   naming of constants was too simplistic (with names such as `C` or `F`) that it was rather prone to naming conflicts 
   in user code. As a result, the constants have been moved to novas.h with more unique names (such as `NOVAS_C` and 
   `NOVAS_EARTH_FLATTENING`. New code should rely on these definitions instead of the troubled constants of 
   `novascon.c` / `.h` if at all necessary.

 - `equ2hor()`: It's name does not make it clear that this function is suitable only for converting TOD (old 
   methodology) to horizontal but not CIRS to horizontal (IAU 2000 standard). You should use the equivalent but more
   specific `tod_to_itrs()` or the newly added `cirs_to_itrs()`, followed by `itrs_to_hor()` instead.
   
 - `cel2ter()` / `ter2cel()`: These NOVAS C 3.1 function can be somewhat confusing to use. You are likely better off 
   with `tod_to_itrs()` and `cirs_to_itrs()` instead, and possibly followed by further conversions if desired.
   
 - `app_star()`, `app_planet()`, `topo_star()` and `topo_planet()`: These NOVAS C 3.1 function use the old (pre IAU 
   2000) methodology, which isn't clear from their naming. Use `place()` or `place_star()` with `NOVAS_TOD` or 
   `NOVAS_CIRS` as the system instead, as appropriate.
   
 - `readeph()`: This NOVAS C 3.1 function is prone to memory leaks, and not flexible with its origin (necessarily at 
   the barycenter). Instead, use a similar `novas_ephem_provider` implementation and `set_ephem_provider()` for a more 
   flexible and less troublesome equivalent, which also does not need to be baked into the library and can be 
   configured at runtime.
   
 - `tdb2tt()`. Use `tt2tdb()` instead. It's both more intuitive to use (returning the time difference as a double) and 
   faster to calculate than the NOVAS C function, not to mention that it implements the more standard approach.
   


