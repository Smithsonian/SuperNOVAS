# Changelog

All notable changes to the SuperNOVAS library will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project adheres to 
[Semantic Versioning](https://semver.org/spec/v2.0.0.html).



## [Unreleased]

Changes coming to the next quarterly release, expected around 1 September 2024. Some or all of these may be readily 
available on the `main` branch.


### Fixed

 - #41: `grav_def()` gravitating body position antedated somewhat incorrectly (in v1.0) when observed source is a 
   Solar-system object between the observer and the gravitating body. The resulting positional error is typically 
   small at below 10 uas.

 - #39: `tod_to_itrs()` used wrong Earth rotation measure (`NOVAS_ERA` instead of `NOVAS_GST`).

 - #38: `gcrs_to_j2000` transformed in the wrong direction.

 - #37: `gcrs_to_cirs()` did not handle well if input and output vectors were the same.

 - #36: `tt2tdb()` Had a wrong scaling in sinusoidal period, resulting in an error of up to +/- 1.7 ms.
   
 - #34: Radial velocity calculation to precede aberration and gravitational bending in `place()`, since the radial 
   velocity that is observed is in the geometric direction towards the source (unaffected by aberration). A precise 
   accounting of the gravitational effects would require figuring out the direction in which the observed light was 
   emitted from the source before it was bent by gravitating bodies along the way. In practice, this may be difficult 
   to generalize. For a single gravitationg body the geometric direction of the source is between the direction in 
   which the light is emitted, and the observed deflected direction. Therefore, for the time being, the radial 
   velocity calculated via the geometric direction is closer to the actual value.
   
 - #29: Fix portability to non-Intel x86 platforms (see Issue #29). Previously, SuperNOVAS used `char` for storing 
   integer coefficients, assuming `char` was signed. However, on some platforms like ARM and PowerPC `char` is 
   unsigned, which broke many calculations badly for such platforms. As of now, we use the explicit platform
   independent `int8_t` storage type for these coefficients.

 - #28: Division by zero bug in `d_light()` (since NOVAS C 3.1) if the first position argument is the ephemeris 
   reference position (e.g. the Sun for `solsys3.c`). The bug affects for example `grav_def()`, where it effectively 
   results in the gravitational deflection due to the Sun being skipped. See Issue #28.

 - `PSI_COR` and `EPS_COR` made globally visible again, thus improving NOVAS C 3.1 compatibility.

 - Adjusted regression testing to treat `nan` and `-nan` effectively the same. They both represent an equally invalid 
   result regardless of the sign.

 - #24: Bungled definition of `SUPERNOVAS_VERSION_STRING` in `novas.h`. 
 
 - Bungled definition of `NOVAS_OBSERVER_PLACES` in `novas.h`. 


### Added
     
 - #33: New observing-frame based approach for calculations (`frames.c`). A `novas_frame` object uniquely defines both 
   the place and time of observation, with a set of pre-calculated transformations and constants. Once the frame is 
   defined it can be used very efficiently to calculate positions for multiple celestial objects with minimal 
   additional computational cost. The frames API is also more elegant and simpler than the low-level NOVAS C approach 
   for performing the same kind of calculations. And, frames are inherently thread-safe since post-creation their 
   internal state is never modified during the calculations. The following new functions were added: 
   `novas_make_frame()`, `novas_change_observer()`, `novas_geom_posvel()`, `novas_geom_to_app()`, `novas_sky_pos()`, 
   `novas_app_to_hor()`, `novas_app_to_geom()`, `novas_hor_to_app()`, `novas_make_transform()`, 
   `novas_invert_transform()`, `novas_transform_vector()`, and `novas_transform_sky_pos()`.
   
 - #33: New `novas_timespec` structure for the self-contained definition of precise astronomical time (`timescale.c`). 
   You can set the time via `novas_set_time()` or `novas_set_split_time()` to a JD date in the timescale of choice 
   (UTC, UT1, GPS, TAI, TT, TCG, TDB, or TCB), or to a UNIX time with `novas_set_unix_time()`. Once set, you can obtain 
   an expression of that time in any timescale of choice via `novas_get_time()`, `novas_get_split_time()` or 
   `novas_get_unix_time()`. And, you can create a new time specification by incrementing an existing one, using 
   `novas_increment_time()`, or measure time differences via `novas_diff_time()`. 
   
 - #32: Added `grav_undef()` to undo gravitational bending of the observed light to obtain geometric positions from
   observed ones.
   
 - Added `obs_posvel()` to calculate the observer position and velocity relative to the Solar System Barycenter (SSB).
 
 - Added `obs_planets()` to calculate planet positions (relative to observer) and velocities (w.r.t. SSB).
 
 - Added new observer locations `NOVAS_AIRBORNE_OBSERVER` for an observer moving relative to the surface of Earth e.g.
   in an aircraft or balloon based telescope platform, and `NOVAS_SOLAR_SYSTEM_OBSERVER` for spacecraft orbiting the 
   Sun. Both of these use the `observer.near_earth` strcture to define (positions and) velocities as appropriate. 
   Hence the `'near_earth` name is a bit misleading, but sticks for back compatibility.
   
 - Added coordinate reference systems `NOVAS_MOD` (Mean of Date) which includes precession by not nutation and
   `NOVAS_J2000` for the J2000 dynamical reference system.

 - New observer locations `NOVAS_AIRBORNE_OBSERVER` and `NOVAS_SOLAR_SYSTEM_OBSERVER`, and corresponding
   `make_airborne_observer()` and `make_solar_system_observer()` functions. Airborne observers have an Earth-fixed
   momentary location, defined by longitude, latitude, and altitude, the same way as for a stationary observer on
   Earth, but are moving relative to the surface, such as in an aircraft or balloon based observatory. Solar-system
   observers are similar to observers in Earth-orbit but their momentary position and velocity is defined relative
   to the Solar System Barycenter (SSB), instead of the geocenter.

 - Added humidity field to `on_surface` structure, e.g. for refraction calculations at radio wavelengths. The
   `make_on_surface()` function will set humidity to 0.0, but the user can set the field appropriately afterwards.

 - New set of built-in refraction models to use with the frame-based `novas_app_to_hor()` function. The models
   `novas_standard_refraction()` and `novas_optical_refraction()` implement the same refraction model as `refract()` 
   in NOVAS C 3.1, with `NOVAS_STANDARD_ATMOSPHERE` and `NOVAS_WEATHER_AT_LOCATION` respectively, including the 
   reversed direction provided by `refract_astro()`. The user may supply their own refraction models to
   `novas_app_to_hor()` also, and may make used of the generic reversal function `novas_inv_refract` to calculate
   refraction in the reverse direction (observer vs astrometric elevations) as needed.

 - Added radio refraction model `novas_radio_refraction()` based on the formulae by Berman &amp; Rockwell 1976.
 
 - #42: `cio_array()` can now parse the original ASCII CIO locator data file (`data/CIO_RA.TXT`) efficiently also, 
   thus no longer requiring a platform-specific binary translation via the `cio_file` tool.
 
 - `make help` to provide a brief list and explanation of the available build targets. (Thanks to `@teuben` for 
   suggesting this.)

 - Added GitHub CI regression testing for non-x86 platforms: `armv7`, `aarch64`, `riscv64`, `ppc64le`. Thus, we
   should avoid misphaps, like the platform specific bug Issue #29, in the future. 
   

### Changed

 - `cio_file` tool parses interval from header rather than the less precise differencing of the first two record
   timestamps. This leads to `cio_array()` being more accurately centered on matching date entries, e.g. J2000.

 - `grav_def()` estimation of light time to where light passes nearest to gravitating body is improved by starting 
   with the body position already antedated for light-time for the gravitating mass. The change typically improves
   the grativational deflection calculations at the few uas level.

 - `grav_def()` is simplified. It no longer uses the location type argument. Instead it will skip deflections
   due to any body, if the observer is within ~1500 km of its center.

 - `place()` now returns an error 3 if and only if the observer is at (or very close, to within ~1.5m) of the 
   observed Solar-system object.

 - Improved precision of some calculations, like `era()`, `fund_args()`, and `planet_lon()` by being more careful
   about the order in which terms are accumulated and combined, resulting in a small improvement on the few uas 
   (micro-arcsecond) level.
   
 - The `ra` or `dec` arguments passed to `vector2radec()` may now be NULL if not required.

 - `tt2tdb()` Now uses the same more precise series as the original NOVAS C `tdb2tt()`.

 - The default make target is now `distro`. It's similar to the deprecated `api` target from before except that it 
   skips building `static` libraries.
   
 - `make` now generates `.so` shared libraries with `SONAME` set to `lib<name>.so.1` where the `.1` indicates that it
   is major version 1 of the `ABI`. All 1.x.x releases are expected to be ABI compatible with earlier releases.
   
 - `lib/*.so` files are now just symlinks to the actual versioned libraries `lib/*.so.1`. This conforms more closely
   to what Linux distros expect.
 
 - Default `make` skips `local-dox` target unless `doxygen` is available (either in the default `PATH` or else 
   specified via the `DOXYGEN` variable, e.g. in `config.mk`). This way the default build does not have unexpected 
   dependencies. (see Issue #22, thanks to `@teuben`).
  
 - `make` can be configured without editing `config.mk` simply by setting the appropriate shell variables (the same
   ones as in `config.mk`) prior to invoking `make`. Standard `CC`, `CPPFLAGS`, `CFLAGS` and `LDFLAGS` will also be 
   used if defined externally.
  
 - `make shared` now also builds `lib/libsolsys1.so.1` and `lib/libsolsys2.so.1` shared libraries that can be used by 
   programs that need solsys1 (via `eph_manager`) or solsys2 (via `jplint`) functionality.
   
 - `make solsys` now generates only the `solarsystem()` implementation objects that are external (not built in).

 - Eliminate unchecked return value compiler warnings from `cio_file` (used typically at build time only to generate
   `cio_ra.bin`).
   
 - `jplint.f` is moved to `examples/` since it provides only a default implementation that typically needs to be
   tweaked for the particualr JPL PLEPH library one intends to use.

 - Doxygen tag file renamed to `supernovas.tag` for consistency.
   
 - Initialize test variable for reproducibility
   


## [1.0.1] -- 2024-05-13

Bug fix release with minor changes.

### Fixed

 - `cirs_to_itrs()`, `itrs_to_cirs()`, `tod_to_itrs()`, and `itrs_to_tod()` all had a unit conversion bug in using the 
  `ut1_to_tt` argument [s] when converting TT-based Julian date to UT1-based JD [day] internally.

 - Fixed errors in `example.c` [by hannorein].

### Added

 - Added `cirs_to_app_ra()` and `app_to_cirs_ra()` for convenience to convert between right ascensions measured from
   the CIO (for CIRS) vs measured from the true equinox of date, on the same true equator of date.

### Changed

 - Changed definition of `NOVAS_AU` to the IAU definition of exactly 1.495978707e+11 m. The old definition is also
   available as `DE405_AU`.
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
   
 - `precession()` can now take arbitrary input and output epochs. Unlike NOVAS C 3.1, it is not required that either 
   of those epochs be J2000.
 
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
   


