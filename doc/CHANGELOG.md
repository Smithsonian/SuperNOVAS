# Changelog

All notable changes to the [SuperNOVAS](https://github.com/Smithsonian/SuperNOVAS) library will be documented in this 
file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and this project adheres to 
[Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## [1.2.0] - 2025-01-15

Feature release. New easy to use adapter modules for CALCEPH or the NAIF CSPICE Toolkit to provide precise positions 
for Solar-system sources. Also, added support for Keplerian orbitals, such as the ones published by the IAU Minor 
Planet Center (MPC) for asteroids, comets, and Near-Earth-Objects (NEOs). And, many more fixes and improvements. 


### Fixed

 - Fixes to GCC version checking for macros.

 - Fixed dummy `readeph()` implementation in `readeph0.c`, and the use of `DEFAULT_READEPH` in `config.mk`. `readeph0.c` 
   is not linked by default, and was not linked in prior releases either.

 - Cure LLVM `clang` compiler warnings, in tests and examples also.
 

### Added

 - #57: New generic redshift-handling functions `novas_v2z()`, `novas_z2v()`.

 - #57, #102: New `make_redshifted_cat_entry()` and `make_redshifted_object()` to simplify the creation of distant 
   catalog sources that are characterized with a redshift measure rather than a radial velocity value.
 
 - #58: New functions to calculate and apply additional gravitational redshift corrections for light that originates
   near massive gravitating bodies (other than major planets, or Sun or Moon), or for observers located near massive
   gravitating bodies (other than the Sun and Earth). The added functions are `grav_redshift()`, `redhift_vrad()`,
   `unredshift_vrad()`, `novas_z_add()`, and `novas_z_inv()`.
   
 - #83: CALCEPH integration: `novas_use_calceph()` and/or `novas_use_calceph_planets()` to specify and use ephemeris 
   data via CALCEPH for Solar-system sources in general, and for major planets specifically; and 
   `novas_calceph_use_ids()` to specify whether `object.number` in `NOVAS_EPHEM_OBJECT` type objects is a NAIF ID 
   (default) or else a CALCEPH ID number of the Solar-system body. These functions are provided by the 
   `libsolsys-calceph.so[.1]` and/or `.a` plugin libraries, which are built contingent on the `CALCEPH_SUPPORT` 
   variable being set to 1 prior to the build. The build of the plugin module requires an accessible installation of 
   the CALCEPH development files (C headers and unversioned static or shared libraries depending on the needs of the 
   build).
   
 - #86: NAIF CSPICE integration: `novas_use_cspice()`, `novas_use_cspice_planets()`, `novas_use_cspice_ephem()`
   to use the NAIF CSPICE library for all Solar-system sources, major planets only, or for other bodies only. 
   `NOVAS_EPHEM_OBJECTS` should use NAIF IDs with CSPICE (or else -1 for name-based lookup). Also provides 
   `cspice_add_kernel()` and `cspice_remove_kernel()` functions for convenience to manage the set of active kernels 
   (#89). These functions are provided by the `libsolsys-cspice.so[.1]` and/or `.a` plugin libraries, which are built 
   contingent on the `CSPICE_SUPPORT` variable being set to 1 prior to the build. The build of the plugin module 
   requires an accessible installation of the CSPICE development files (C headers and unversioned static or shared 
   libraries depending on the needs of the build).
   
 - #87: Added `novas_planet_for_name()` function to return the NOVAS planet ID for a given (case insensitive) name.
   
 - NOVAS-NAIF conversions for major planets (and Sun, Moon, SSB): `novas_to_naif_planet()` (planet centers), 
   `novas_to_dexxx_planet()` (mixed planet center/barycenter for DExxx ephemeris files), and the inverse
   `naif_to_novas_planet()`.

 - Added `get_planet_provider()` and `get_planet_provider_hp()` functions for convenience to return the `solarsystem()`
   / `solarsystem_hp()` type custom planet ephemeris provider functions currently configured, so they may be used
   directly, outside of `ephemeris()` calls.
   
 - #93: Now supporting `make install` with `prefix` and `DESTDIR` support (e.g. `make prefix="/opt" install` to 
   install under `/opt`, and/or `make DESTDIR="/tmp/stage" install` to stage install under `/tmp/stage`). You can also 
   set other standard directory variables, as prescribed by the 
   [GNU standard](https://www.gnu.org/prep/standards/html_node/Directory-Variables.html) to further customize the
   install locations.
 
 - #95, #98: Added support for using orbital elements. `object.type` can now be set to `NOVAS_ORBITAL_OBJECT`, whose 
   orbit can be defined by the set of `novas_orbital`, relative to a `novas_orbital_system`. You can initialize an 
   `object` with a set of orbital elements using `make_orbital_object()`, and for planetary satellite orbits you might 
   use `novas_set_orbsys_pole()`. For orbital objects, `ephemeris()` will call on the new `novas_orbit_posvel()` to 
   calculate positions. While orbital elements do not always yield precise positions, they can for shorter periods, 
   provided that the orbital elements are up-to-date. For example, the Minor Planer Center (MPC) publishes accurate 
   orbital elements for all known asteroids and comets regularly. For newly discovered objects, this may be the only 
   and/or most accurate information available anywhere.
   
 - #97: Added `NOVAS_EMB` (Earth-Moon Barycenter) and `NOVAS_PLUTO_BARYCENTER` to `enum novas_planets` to distinguish
   from the planet center in calculations.
   
 - #98: Added `gcrs_to_tod()` / `tod_to_gcrs()` and `gcrs_to_mod()` / `mod_to_gcrs()` vector conversion functions for
   convenience.
   
 - #106: New example files under `examples/` demonstrating the recommended approach for using SuperNOVAS to calculate
   positions for various types of object.
 
 - Added various `object` initializer macros in `novas.h` for the major planets, Sun, Moon, and barycenters, e.g. 
   `NOVAS_EARTH_INIT` or `NOVAS_SSB_INIT`. These wrap the parametric `NOVAS_PLANET_INIT(num, name)` macro, and can be
   used to simplify the initialization of NOVAS `object`s.
 
 - Added more physical unit constants to `novas.h` and a corresponding section in the README on how these may be used
   to convert to/from NOVAS conventional quantities.
 
 - SuperNOVAS headers now include each other as system-headers, not local headers. This is unlikely to affect anything
   really but it is more proper for an installation of the library, and works with our own `Makefile` too.
   
 - Added 'Coordinate Systems and Conversions' flowchart to `README`.
 
 - Added a GNU standard documentation targets to `Makefile`.
 
 - Added `infer` make target for running Facebook's `infer` static analysis tool.
 
 - Added missing error tracing in `nutation()`.
 
 - Added `README.md` Figure 1, to clarify relation of coordinate systems and (Super)NOVAS function to convert vectors
   among them.
 
   
### Changed

 - #96: Changed `object` structure to include `novas_orbital` for `NOVAS_ORBITAL_OBJECT` types. To keep ABI 
   compatibility to earlier SuperNOVAS releases, `make_object()` will not initialize the new `.orbit` field _unless_
   `type` is set to `NOVAS_ORBITAL_OBJECT` (which was not available before).

 - #97: Updated `NOVAS_PLANETS`, `NOVAS_PLANET_NAMES_INIT`, and `NOVAS_RMASS_INIT` macros to include the added planet 
   constants.
   
 - #106: The old (legacy) NOVAS C example has been removed. Instead a new set of examples are provided, which are 
   better suited for SuperNOVAS.
   
 - `make check` now runs both static analysis by cppcheck (new `analysis` target) and regression tests (`test` 
   target), in closer conformance to GNU Makefile standards.
   
 - Added `-g` to default `CFLAGS` as a matter of GNU best practice.
 
 - Static library is now named `ibsupernovas.a`, which is symlinked to `libnovas.a` for back compatibility.
 
 - `readeph0.c` moved to `examples/`. It's a dummy legacy NOVAS C implementation that is not really needed in 
   SuperNOVAS.
 
 - Various small tweaks to Makefiles.
 
 - Updated `README.md` and API documentation.
 
   
   
## [1.1.1] - 2024-10-28

Bug fix release. Nothing too scary, mainly just a collection of smaller fixes and improvements.

### Fixed

 - #55: Relativistic corrections to radial velocity were still not applied correctly. The revised calculation now uses
   the actual relativistic velocity differential between the source and observer to apply the appropriate time 
   dilation factor, and applies gravitational corrections for Sun and Earth consistently at source and observer alike.

 - #64: `NOVAS_TAI_TO_TT` definition had wrong decimal in last place, thus was 3 ms off from what it should have been.
   (thanks to kiranshila)

 - #68: `readeph_dummy()` dummy implementation in `readeph0.c` called non existing error handler function.

### Changed

 - #59: For observing major planets (and Sun and Moon) `rad_vel()`, `rad_vel2()`, `place()`, and `novas_sky_pos()` 
   will include gravitational corrections to radial velocity for light originating at the surface, and observed near 
   Earth or else at a large distance away. These corrections, along with those for the Solar potential at the source, 
   may be skipped for `rad_vel()` / `rad_vel2()` by setting `d_src_sun` negative.

 - #55: Use relativistic formulae to add/difference velocities (i.e. change velocity reference frame).

 - #60: Moved SuperNOVAS-only functions to a separate `super.c` module to alleviate the bloating of `novas.c`, which
   can still be used as a self-contained, standalone, NOVAS C replacement for legacy applications if need be.

 - #62: Improve debug mode error tracing when NAN values are returned, so the trace indicates NAN rather than a bogus
   integer return value before.
   
 - #66: Various tweaks for C/C++ best practices (by kiranshila) 
   
 - #67: Use accuracy argument in `tod_to_cirs()`. (thanks to kiranshila)
 
 - #68: Various improvements to debug error tracing.
 
 - #73: Initializer macros (primarily for internal use), forced 'use' of unused variables after declarations, and no
   order-only-dependencies in Makefiles -- in order to cure warnings and to conform with older compilers and make.
 
 - Slight tweaks to `Makefile`, and `.mk` snippets, with added `make` configurability.
 
 

## [1.1.0] - 2024-08-04

Feature release. Introducing a more efficient and elegant approach to position and velocity calculations using 
observer frames; versatile handling of astronomical timescales; and support for further observer locations, coordinate 
reference systems, and atmospheric refraction models. The release also fixes a number of bugs, of varying severity, 
which affected prior SuperNOVAS releases.


### Fixed

 - #29: Fix portability to non-Intel platforms. Previously, SuperNOVAS used `char` for storing small integer 
   coefficients, assuming `char` was signed. However, on some platforms like ARM and PowerPC `char` is unsigned, which 
   broke calculations badly. As of now, we use the explicit platform independent signed `int8_t` storage type for 
   these coefficients.

 - #38: `gcrs_to_j2000()` transformed in the wrong direction in v1.0.

 - #39: `tod_to_itrs()` used wrong Earth rotation measure (`NOVAS_ERA` instead of `NOVAS_GST`) in v1.0.

 - #45: `cel2ter()` invalid output in v1.0 with CIRS input coordinates (`erot` = `EROT_ERA` and 
   `class` = `NOVAS_DYNAMICAL_CLASS`) if output vector was distinct from input vector. Affects `cirs_to_itrs()` also.

 - #36: `tt2tdb()` Had a wrong scaling in sinusoidal period in v1.0, resulting in an error of up to +/- 1.7 ms.
 
 - #37: `gcrs_to_cirs()` did not handle well if input and output vectors were the same in v1.0.

 - #28: Division by zero bug in `d_light()` (since NOVAS C 3.1) if the first position argument is the ephemeris 
   reference position (e.g. the Sun for `solsys3.c`). The bug affects for example `grav_def()`, where it effectively 
   results in the gravitational deflection due to the Sun being skipped. See Issue #28.
 
 - #41: `grav_def()` gravitating body position antedated somewhat incorrectly (in v1.0) when observed source is a 
   Solar-system object between the observer and the gravitating body. The resulting positional error is typically 
   small at below 10 uas.

 - #50: The NOVAS C 3.1 implementation of `rad_vel()` applied relativistic corrections for a moving observer 
   conditional on applying relativistic gravitational corrections (for Sun and/or Earth potential) for the observer. 
   However, it seems more reasonable that the corrections for a moving observer should be applied always and 
   independently of the (optional) gravitational corrections.

 - #34: `place()` radial velocities were not quite correct in prior SuperNOVAS releases. The radial velocity 
   calculation now precedes aberration, since the radial velocity that is observed is in the geometric direction 
   towards the source (unaffected by aberration). As for gravitational deflection, the geometric direction is the 
   correct direction in which light was emitted from the source for sidereal objects. For Solar system sources we now 
   reverse trace the deflected light to calculate the direction in which it was emitted from the source. As such, the 
   radial velocities calculated should now be precise under all conditions. The typical errors of the old calculations 
   were up to tens of m/s because of aberration, and a few m/s due to the wrong gravitational deflection calculation.

 - #24: Bungled definition of `SUPERNOVAS_VERSION_STRING` in `novas.h` in v1.0. 
 
 - Bungled definition of `NOVAS_OBSERVER_PLACES` in `novas.h` in v1.0. 


### Added
     
 - #33: New observing-frame based approach for calculations (`frames.c`). A `novas_frame` object uniquely defines both 
   the place and time of observation, with a set of pre-calculated transformations and constants. Once the frame is 
   defined it can be used very efficiently to calculate positions for multiple celestial objects with minimum 
   additional computational cost. The frames API is also more elegant and more versatile than the low-level NOVAS C 
   approach for performing the same kind of calculations. And, frames are inherently thread-safe since post-creation 
   their internal state is never modified during the calculations. The following new functions were added: 
   `novas_make_frame()`, `novas_change_observer()`, `novas_geom_posvel()`, `novas_geom_to_app()`, `novas_sky_pos()`, 
   `novas_app_to_hor()`, `novas_app_to_geom()`, `novas_hor_to_app()`.
   
 - #33: New coordinate system transformations via the `novas_transform` structure. You may use these transforms to 
   express position / velocity vectors, calculated for a given observer and time, in the coordinate reference system of 
   choice.The following new functions were added: `novas_make_transform()`, `novas_invert_transform()`, 
   `novas_transform_vector()`, and `novas_transform_sky_pos()`.
 
 - #33: New `novas_timespec` structure for the self-contained definition of precise astronomical time (`timescale.c`). 
   You can set the time via `novas_set_time()` or `novas_set_split_time()` to a JD date in the timescale of choice 
   (UTC, UT1, GPS, TAI, TT, TCG, TDB, or TCB), or to a UNIX time with `novas_set_unix_time()`. Once set, you can obtain 
   an expression of that time in any timescale of choice via `novas_get_time()`, `novas_get_split_time()` or 
   `novas_get_unix_time()`. And, you can create a new time specification by incrementing an existing one, using 
   `novas_increment_time()`, or measure time differences via `novas_diff_time()`, `novas_diff_tcg()`, or 
   `novas_diff_tcb()`. 
 
 - Added `novas_planet_bundle` structure to handle planet positions and velocities more elegantly (e.g. for 
   gravitational deflection calculations).
 
 - #32: Added `grav_undef()` to undo gravitational bending of the observed light to obtain geometric positions from
   observed ones.
   
 - Added `obs_posvel()` to calculate the observer position and velocity relative to the Solar System Barycenter (SSB).
 
 - Added `obs_planets()` to calculate apparent planet positions (relative to observer) and velocities (w.r.t. SSB).
 
 - Added new observer locations `NOVAS_AIRBORNE_OBSERVER` for an observer moving relative to the surface of Earth e.g.
   in an aircraft or balloon based telescope platform, and `NOVAS_SOLAR_SYSTEM_OBSERVER` for spacecraft orbiting the 
   Sun. Both of these use the `observer.near_earth` strcture to define (positions and) velocities as appropriate. 
   Hence the `'near_earth` name is a bit misleading, but remains for back compatibility.
   
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

 - New set of built-in refraction models to use with the frame-based `novas_app_to_hor()` / `novas_hor_to_app()` 
   functions. The models `novas_standard_refraction()` and `novas_optical_refraction()` implement the same refraction 
   model as `refract()`  in NOVAS C 3.1, with `NOVAS_STANDARD_ATMOSPHERE` and `NOVAS_WEATHER_AT_LOCATION` 
   respectively, including the reversed direction provided by `refract_astro()`. The user may supply their own custom 
   refraction also, and may make use of the generic reversal function `novas_inv_refract()` to calculate refraction in 
   the reverse direction (observer vs astrometric elevations) as needed.

 - Added radio refraction model `novas_radio_refraction()` based on the formulae by Berman &amp; Rockwell 1976.
 
 - Added `cirs_to_tod()` and `tod_to_cirs()` functions for efficient tranformation between True of Date (TOD) and
   Celestial Intermediate Reference System (CIRS), and vice versa.
   
 - Added `make_cat_object()` function to create a NOVAS celestial `object` structure from existing `cat_entry` data.
 
 - Added `rad_vel2()` to calculate radial velocities precisely in the presense of gravitational deflection i.e., 
   when the direction in which light was emitted is different from the direction it is detected by the observer.
   This new function is now used by both `place()` and `novas_sky_pos()`.
 
 - `make help` to provide a brief list and explanation of the available build targets. (Thanks to `@teuben` for 
   suggesting this.)

 - Added GitHub CI regression testing for non-x86 platforms: `armv7`, `aarch64`, `riscv64`, `ppc64le`. Thus, we
   should avoid misphaps, like the platform specific bug Issue #29, in the future. 
   

### Changed

 - #42: `cio_array()` can now parse the original ASCII CIO locator data file (`data/CIO_RA.TXT`) efficiently also, 
   thus no longer requiring a platform-specific binary translation via the `cio_file` tool.

 - #51: The NOVAS C implementation of `rad_vel()` has ignored this redshifting when the Sun was being observed. From 
   now on, we shall gravitationally reference radial velocities when observing the Sun to its photosphere.

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
   
 - `vector2radec()`: `ra` or `dec` arguments may now be NULL if not required.

 - `tt2tdb()` Now uses the same, slightly more precise series as the original NOVAS C `tdb2tt()`.

 - `rad_vel()` You can use negative values for the distances to skip particular gravitational corrections to the 
   radial velocity measure. The value 0.0 also continues to have the same effect as before, except if the observed
   source is the Sun. Then `d_src_sun` being 0.0 takes on a different meaning than before: rather than skipping 
   gravitational redshift corrections for the Solar potential (as before) we will apply gravitational corrections for 
   light originating at the Sun's photosphere.

 - `PSI_COR` and `EPS_COR` made globally visible again, thus improving NOVAS C 3.1 compatibility.
 
 - Convergent inverse calculations now use the `novas_inv_max_iter` variable declared in `novas.c` to specify the
   maximum number of iterations before inverse functions return with an error (with errno set to `ECANCELED`). Users
   may adjust this limit, if they prefer some other maximum value.

 - Adjusted regression testing to treat `nan` and `-nan` effectively the same. They both represent an equally invalid 
   result regardless of the sign.

 - The default make target is now `distro`. It's similar to the deprecated `api` target from before except that it 
   skips building `static` libraries and `cio_ra.bin`.
   
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
   
 - Initialize test variables for reproducibility
   


## [1.0.1] - 2024-05-13

Bug fix release with minor changes.

### Fixed

 - `cirs_to_itrs()`, `itrs_to_cirs()`, `tod_to_itrs()`, and `itrs_to_tod()` all had a unit conversion bug in using the 
  `ut1_to_tt` argument [s] when converting TT-based Julian date to UT1-based JD [day] internally. 
  (thanks to hannorein)

 - Fixed errors in `example.c` [by hannorein].

### Added

 - Added `cirs_to_app_ra()` and `app_to_cirs_ra()` for convenience to convert between right ascensions measured from
   the CIO (for CIRS) vs measured from the true equinox of date, on the same true equator of date.

### Changed

 - Changed definition of `NOVAS_AU` to the IAU definition of exactly 1.495978707e+11 m. The old definition is also
   available as `DE405_AU`. (thanks to hannorein)
 - Various corrections and changes to documentation.



## [1.0.0] - 2024-03-01

This is the initial release of the SuperNOVAS library. Changes are indicated w.r.t. the upstream NOVAS C 3.1 library 
from which SuperNOVAS is forked from.

### Fixed

 - Fixes the NOVAS C 3.1 [sidereal_time bug](https://aa.usno.navy.mil/software/novas_faq), whereby the 
   `sidereal_time()` function had an incorrect unit cast.
   
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
 
 - Some remainder calculations in NOVAS C 3.1 used the result from `fmod()` unchecked, which resulted in angles outside
   of the expected [0:2&pi;] range and was also the reason why `cal_date()` did not work for negative JD values.
 
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
   `planet_eph_manager()` vs `planet_ephem_provider()`.

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
   in user code. As a result, the constants have been moved to `novas.h` with more unique names (such as `NOVAS_C` and 
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
   


