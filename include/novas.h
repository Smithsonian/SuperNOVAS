/**
 * @file
 *
 * @author G. Kaplan and A. Kovacs
 * @version 1.2.0
 *
 *  SuperNOVAS astrometry software based on the Naval Observatory Vector Astrometry Software (NOVAS).
 *  It has been modified to fix outstanding issues and to make it easier to use.
 *
 *  Based on the NOVAS C Edition, Version 3.1:
 *
 *  U. S. Naval Observatory<br>
 *  Astronomical Applications Dept.<br>
 *  Washington, DC<br>
 *  <a href="http://www.usno.navy.mil/USNO/astronomical-applications">
 *  http://www.usno.navy.mil/USNO/astronomical-applications</a>
 */

#ifndef _NOVAS_
#define _NOVAS_

#include <math.h>   // for sin, cos
#include <stdlib.h> // NULL
#include <stdint.h>
#include <time.h>

/// Definition of &pi; in case it's not defined in math.h
#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif

// The upstream NOVAS library had a set of include statements that really were not necessary
// First, including standard libraries here meant that those libraries were included in the
// source code of any application that included 'novas.h', even if that source code did not
// contain any reference to those libraries. This is just unnecessary bloating, and simply a bad
// practice
//
// It also included "novascon.h", which defined various constants -- mainly for internal
// use by the library itself. This was problematic for two reasons. First, because the constants
// were declared as 'const' variables, rather than precompiler constants, which meant (a) that they
// needed to be defined in an actual source code "novascon.c"; a minor inconvenience, and (b)
// they could not be undefined in case the application needed something else locally with
// the same name. The rather non unique naming of constants such as 'C', 'F', 'T0', or 'AU' made
// exacerbated the chance of namespace conflicts when compiled or linked against the user's
// application code, which may have defined their own constants or variables by the same name.
//
// For these reasons, we removed the unneeded '#include' directives by default. If you need them
// for compiling your code, which may have relied on these, you can add '-DCOMPAT=1' to the
// compiler options
//
//
#if COMPAT
#  include <stdio.h>
#  include <ctype.h>
#  include <string.h>

#  include "novascon.h"
#endif

#include "nutation.h"

/// API major version
#define SUPERNOVAS_MAJOR_VERSION  1

/// API minor version
#define SUPERNOVAS_MINOR_VERSION  2

/// Integer sub version of the release
#define SUPERNOVAS_PATCHLEVEL     0

/// Additional release information in version, e.g. "-1", or "-rc1".
#define SUPERNOVAS_RELEASE_STRING "-devel"

/// \cond PRIVATE

#ifdef str_2
#  undef str_2
#endif

/// Stringify level 2 macro
#define str_2(s) str_1(s)

#ifdef str_1
#  undef str_1
#endif

/// Stringify level 1 macro
#define str_1(s) #s

/// \endcond

/// The version string for this library
/// \hideinitializer
#define SUPERNOVAS_VERSION_STRING str_2(SUPERNOVAS_MAJOR_VERSION) "." str_2(SUPERNOVAS_MINOR_VERSION) \
                                  "." str_2(SUPERNOVAS_PATCHLEVEL) SUPERNOVAS_RELEASE_STRING

#define NOVAS_MAJOR_VERSION       3       ///< Major version of NOVAS on which this library is based
#define NOVAS_MINOR_VERSION       1       ///< Minor version of NOVAS on which this library is based

/// The version string of the upstream NOVAS library on which this library is based.
#define NOVAS_VERSION_STRING      #NOVAS_MAJOR_VERSION "." NOVAS_MINOR_VERSION

/// [pts] cache size for GCRS CIO locator data (16 bytes per point).
#define NOVAS_CIO_CACHE_SIZE      1024

/// [day] Julian date at J2000
#define NOVAS_JD_J2000            2451545.0

/// [day] Julian date at which the Modified Julian Date (MJD) is zero
#define NOVAS_JD_MJD0             2400000.5

/// [day] Julian date at B1950
#define NOVAS_JD_B1950            2433282.42345905

/// [day] Julian date at B1900
#define NOVAS_JD_B1900            15019.81352

/// [day] Julian date for J1991.25, which the Hipparcos catalog is
/// referred to
#define NOVAS_JD_HIP              2448349.0625

/// [m/s] Speed of light in meters/second is a defining physical constant.
#define NOVAS_C                   299792458.0

/// [m] Astronomical unit (AU). IAU definition.
/// See <a href="https://www.iau.org/static/resolutions/IAU2012_English.pdf">IAU 2012 Resolution B2</a>.
/// @sa DE405_AU
#define NOVAS_AU                  1.495978707e+11

/// [m] Astronomical unit (AU). based on DE-405. (old definition)
/// @sa NOVAS_AU
#define DE405_AU                  1.4959787069098932e+11

/// [s] Light-time for one astronomical unit (AU) in seconds.
#define NOVAS_AU_SEC              ( NOVAS_AU / NOVAS_C )

/// [AU/day] Speed of light in AU/day.  Value is 86400 / AU_SEC.
#define NOVAS_C_AU_PER_DAY        ( 86400.0 / AU_SEC )

/// [km] Astronomical Unit in kilometers.
#define NOVAS_AU_KM               ( 1e-3 * NOVAS_AU )

/// [m<sup>3</sup>/s<sup>2</sup>] Heliocentric gravitational constant in
/// meters^3 / second^2, from DE-405.
#define NOVAS_G_SUN               1.32712440017987e+20

/// [m<sup>3</sup>/s<sup>2</sup>] Geocentric gravitational constant in
/// meters^3 / second^2, from DE-405.
#define NOVAS_G_EARTH             3.98600433e+14

/// [m] Solar radius (photosphere)
/// @since 1.1
#define NOVAS_SOLAR_RADIUS        696340000.0

/// [m] Radius of Earth in meters from IERS Conventions (2003).
#define NOVAS_EARTH_RADIUS        6378136.6

/// Earth ellipsoid flattening from IERS Conventions (2003). Value is
/// 1 / 298.25642.
#define NOVAS_EARTH_FLATTENING    (1.0 / 298.25642)

/// [rad/s] Rotational angular velocity of Earth in radians/sec from IERS
/// Conventions (2003).
#define NOVAS_EARTH_ANGVEL        7.2921150e-5

/// [s] TAI - GPS time offset
#define NOVAS_GPS_TO_TAI          19.0

/// [s] TT - TAI time offset
#define NOVAS_TAI_TO_TT           32.184


/// Reciprocal masses of solar system bodies, from DE-405 (Sun mass / body mass).
/// [0]: Earth/Moon barycenter, MASS[1] = Mercury, ...,
/// [9]: Pluto, [10]: Sun, [11]: Moon.
#define NOVAS_RMASS_INIT  { \
      328900.561400, 6023600.0, 408523.71, 332946.050895, 3098708.0, 1047.3486, 3497.898, \
      22902.98, 19412.24, 135200000.0, 1.0, 27068700.387534 }

/// Gravitational redshifts for major planets (and Moon and Sun) for light emitted at surface
/// and detected at a large distance away.
/// @since 1.1.1
#define NOVAS_PLANET_GRAV_Z_INIT { \
  0.0, 1.0047e-10, 5.9724e-10, 7.3050e-10, 1.4058e-10, 2.0166e-8, 7.2491e-9, 2.5420e-9, \
  3.0893e-9, 9.1338e-12, 2.120483e-6, 3.1397e-11 }

#if !COMPAT
// If we are not in the strict compatibility mode, where constants are defined
// as variables in novascon.h (with implementation in novascon.c), then define
// them here

#  ifndef TWOPI
#    define TWOPI                 (2.0 * M_PI)    ///< 2&pi;
#  endif

#  ifndef ASEC360
/// [arcsec] Number of arcseconds in 360 degrees.
#    define ASEC360               (360 * 60 * 60)
#  endif

#  ifndef DEG2RAD
/// [rad/deg] 1 degree in radians
#    define DEG2RAD               (M_PI / 180.0)
#  endif

#  ifndef RAD2DEG
/// [deg/rad] 1 radian in degrees
#    define RAD2DEG               (1.0 / DEG2RAD)
#  endif

#  ifndef ASEC2RAD
/// [rad/arcsec] 1 arcsecond in radians
#    define ASEC2RAD              (DEG2RAD / 3600.0)
#  endif

#endif

/**
 * Settings for 'novas_debug()'
 *
 * @sa novas_debug
 */
enum novas_debug_mode {
 NOVAS_DEBUG_OFF = 0,     ///< Do not print errors and traces to the standard error (default).
 NOVAS_DEBUG_ON,          ///< Print errors and traces to the standard error.
 NOVAS_DEBUG_EXTRA        ///< Print all errors and traces to the standard error, even if they may be acceptable behavior.
};

/**
 * The type of astronomical objects distinguied by the NOVAS library.
 *
 */
enum novas_object_type {
  /// A major planet, or else the Sun, the Moon, or the Solar-System Barycenter (SSB).
  /// @sa enum novas_planet
  /// @sa novas_planet_provider
  /// @sa novas_planet_provider_hp
  NOVAS_PLANET = 0,

  /// A Solar-system body that does not fit the major planet type, and requires specific
  /// user-provided novas_ephem_provider implementation.
  /// @sa novas_ephem_provider
  NOVAS_EPHEM_OBJECT,

  /// Any non-solar system object that may be handled via 'catalog' coordinates, such as a star
  /// or a quasar.
  NOVAS_CATALOG_OBJECT
};

/// The number of object types distinguished by NOVAS.
#define NOVAS_OBJECT_TYPES      (NOVAS_CATALOG_OBJECT + 1)

/**
 * Enumeration for the 'major planet' numbers in NOVAS to use as the solar-system body number whenever
 * the object type is NOVAS_PLANET.
 *
 * @sa NOVAS_PLANET
 * @sa NOVAS_PLANET_NAMES_INIT
 */
enum novas_planet {
  NOVAS_SSB = 0,        ///< Solar-system barycenter position ID
  NOVAS_MERCURY,        ///< Major planet number for the Mercury in NOVAS.
  NOVAS_VENUS,          ///< Major planet number for the Venus in NOVAS.
  NOVAS_EARTH,          ///< Major planet number for the Earth in NOVAS.
  NOVAS_MARS,           ///< Major planet number for the Mars in NOVAS.
  NOVAS_JUPITER,        ///< Major planet number for the Jupiter in NOVAS.
  NOVAS_SATURN,         ///< Major planet number for the Saturn in NOVAS.
  NOVAS_URANUS,         ///< Major planet number for the Uranus in NOVAS.
  NOVAS_NEPTUNE,        ///< Major planet number for the Neptune in NOVAS.
  NOVAS_PLUTO,          ///< Major planet number for the Pluto in NOVAS.
  NOVAS_SUN,            ///< Numerical ID for the Sun in NOVAS.
  NOVAS_MOON            ///< Numerical ID for the Moon in NOVAS.
};

/// The number of major planets defined in NOVAS.
#define NOVAS_PLANETS             (NOVAS_MOON + 1)

/**
 * String array initializer for Major planet names, matching the enum novas_planet. E.g.
 *
 * \code
 * char *planet_names[] = NOVAS_PLANET_NAMES_INIT;
 * \endcode
 *
 * @sa novas_majot_planet
 */
#define NOVAS_PLANET_NAMES_INIT { "SSB", "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune", "Pluto", "Sun", "Moon" }

/**
 * Types of places on and around Earth that may serve a a reference position for the observation.
 *
 */
enum novas_observer_place {
  /// Calculate coordinates as if observing from the geocenter for location and Earth rotation
  /// independent coordinates.
  NOVAS_OBSERVER_AT_GEOCENTER = 0,

  /// Stationary observer in the corotating frame of Earth.
  NOVAS_OBSERVER_ON_EARTH,

  /// Observer is on Earth orbit, with a position and velocity vector relative to geocenter.
  /// This may also be appropriate for observatories at the L2 or other Earth-based Langrange
  /// points.
  NOVAS_OBSERVER_IN_EARTH_ORBIT,

  /// Observer airborne, moving relative to the surface of Earth.
  /// @since 1.1
  NOVAS_AIRBORNE_OBSERVER,

  /// Observer is orbiting the Sun.
  /// @since 1.1
  NOVAS_SOLAR_SYSTEM_OBSERVER
};

/// The number of observer place types supported
#define NOVAS_OBSERVER_PLACES     (NOVAS_SOLAR_SYSTEM_OBSERVER + 1)

/**
 * The basic types of positional coordinate reference systems supported by NOVAS. These
 * determine only how the celestial pole is to be located, but not how velocities are to be
 * referenced. specific pos-vel coordinates are referenced to an 'astro_frame', which must
 * specify one of the values defined here.
 *
 * @sa novas_frame
 */
enum novas_reference_system {
  /// Geocentric Celestial Reference system. Essentially the same as ICRS but includes
  /// aberration and gravitational deflection for an observer around Earth.
  NOVAS_GCRS = 0,

  /// True equinox Of Date: dynamical system of the 'true' equator, with its origin at the
  /// 'true' equinox (pre IAU 2006 system). It is inherently less precise than the new standard
  /// CIRS because mainly because it is based on separate, and less-precise, precession
  /// and nutation models (Lieske et. al. 1977).
  NOVAS_TOD,

  /// Celestial Intermediate Reference System: dynamical system of the true equator, with its
  /// origin at the CIO (preferred since IAU 2006)
  NOVAS_CIRS,

  /// International Celestial Reference system. The equatorial system fixed to the frame of
  /// distant quasars.
  NOVAS_ICRS,

  /// The J2000 dynamical reference system
  /// @since 1.1
  NOVAS_J2000,

  /// Mean equinox of date:  dynamical system of the 'mean' equator, with its origin at the
  /// 'mean' equinox (pre IAU 2006 system). It includes precession (Lieske et. al. 1977),
  /// but no nutation.
  /// @since 1.1
  NOVAS_MOD,
};

/// The number of basic coordinate reference systems in NOVAS.
#define NOVAS_REFERENCE_SYSTEMS   (NOVAS_MOD + 1)

/**
 * Constants that determine the type of equator to be used for the coordinate system.
 */
enum novas_equator_type {
  NOVAS_MEAN_EQUATOR = 0, ///< Mean equator without nutation (pre IAU 2006 system).
  NOVAS_TRUE_EQUATOR,     ///< True equator (pre IAU 2006 system).
  NOVAS_GCRS_EQUATOR      ///< Geocentric Celestial Reference System (GCRS).
};

/**
 * Constants that determine the type of dynamical system type for gcrs2equ()
 */
enum novas_dynamical_type {
  /// Mean equinox Of Date (TOD): dynamical system not including nutation (pre IAU 2006 system).
  NOVAS_DYNAMICAL_MOD = 0,

  /// True equinox Of Date (TOD): dynamical system of the 'true' equator, with its origin at the
  /// true equinox (pre IAU 2006 system; Lieske et. al. 1977).
  NOVAS_DYNAMICAL_TOD,

  /// Celestial Intermediate Reference System (CIRS): dynamical system of the true equator,
  /// with its origin at the CIO (preferred since IAU 2006)
  NOVAS_DYNAMICAL_CIRS
};

/**
 * Constants to control the precision of NOVAS nutation calculations.
 *
 */
enum novas_accuracy {
  /// Use full precision calculations to micro-arcsecond accuracy. It can be computationally
  /// intensive when using the dynamical equator.
  NOVAS_FULL_ACCURACY = 0,

  /// Calculate with truncated terms. It can be significantly faster if a few milliarcsecond
  /// accuracy is sufficient.
  NOVAS_REDUCED_ACCURACY
};

/**
 * Constants that determine whether refraction calculations should use a standard atmospheric
 * model, or whatever weather parameters have been been specified for the observing location.
 *
 * @sa on_surface
 */
enum novas_refraction_model {
  /// Do not apply atmospheric refraction correction
  NOVAS_NO_ATMOSPHERE = 0,

  /// Uses a standard atmospheric model, ignoring all weather values defined for the specific
  /// observing location
  NOVAS_STANDARD_ATMOSPHERE,

  /// Uses the weather parameters that are specified together with the observing location.
  NOVAS_WEATHER_AT_LOCATION
};

/**
 * Constants that determine the type of rotation measure to use.
 */
enum novas_earth_rotation_measure {
  /// Use Earth Rotation Angle (ERA) as the rotation measure, relative to the CIO (new IAU
  /// 2006 standard)
  EROT_ERA = 0,

  /// Use GST as the rotation measure, relative to the true equinox (pre IAU 20006 standard)
  EROT_GST
};

/**
 * Constants for ter2cel() and cel2ter()
 */
enum novas_equatorial_class {
  NOVAS_REFERENCE_CLASS = 0,        ///< Celestial coordinates are in GCRS
  NOVAS_DYNAMICAL_CLASS             ///< Celestial coordinates are apparent values (CIRS or TOD)
};

/**
 * The convention in which the celestial pole offsets are defined for polar wobble.
 *
 */
enum novas_pole_offset_type {
  /// Offsets are &Delta;d&psi;, &Delta;d&epsilon; pairs (pre IAU 2006 precession-nutation
  /// model).
  POLE_OFFSETS_DPSI_DEPS = 1,

  /// Offsets are dx, dy pairs (IAU 2006 precession-nutation model)
  POLE_OFFSETS_X_Y
};

/**
 * The type of equinox (old methodology)
 *
 */
enum novas_equinox_type {
  NOVAS_MEAN_EQUINOX = 0,         /// Mean equinox: includes precession but not nutation
  NOVAS_TRUE_EQUINOX              /// True apparent equinox: includes both precession and nutation
};

/**
 * The origin of the ICRS system for referencing positions and velocities for solar-system bodies.
 */
enum novas_origin {
  NOVAS_BARYCENTER = 0,           ///< Origin at the Solar-system baricenter (i.e. BCRS)
  NOVAS_HELIOCENTER               ///< Origin at the center of the Sun.
};

/// the number of different ICSR origins available in NOVAS.
#define NOVAS_ORIGIN_TYPES        (NOVAS_HELIOCENTER + 1)

/// @deprecated Old definition of the Barycenter origin. NOVAS_BARYCENTER is preferred.
#define BARYC                     NOVAS_BARYCENTER

/// @deprecated Old definition of the Center of Sun as the origin. NOVAS_HELIOCENTER is preferred.
#define HELIOC                    NOVAS_HELIOCENTER

/**
 * The types of coordinate transformations available for tranform_cat().
 *
 * @sa transform_cat()
 */
enum novas_transform_type {
  /// Updates the star's data to account for the star's space motion between
  /// the first and second dates, within a fixed reference frame.
  PROPER_MOTION = 1,

  /// applies a rotation of the reference frame
  /// corresponding to precession between the first and second dates,
  /// but leaves the star fixed in space.
  PRECESSION,

  /// The combined equivalent of PROPER_MOTION and PRECESSION together.
  CHANGE_EPOCH,

  /// A fixed rotation about very small angles (<0.1 arcsecond) to take data from the
  /// dynamical system of J2000.0 to the ICRS.
  CHANGE_J2000_TO_ICRS,

  /// The inverse transformation of J2000_TO_ICRS
  CHANGE_ICRS_TO_J2000
};

/// The number of coordinate transfor types in NOVAS.
#define NOVAS_TRANSFORM_TYPES     (ICRS_TO_J2000 + 1)

/**
 * System in which CIO location is calculated.
 */
enum novas_cio_location_type {
  /// The location of the CIO relative to the GCRS frame.
  CIO_VS_GCRS = 1,

  /// The location of the CIO relative to the true equinox in the dynamical frame
  CIO_VS_EQUINOX
};

#ifndef DEFAULT_CIO_LOCATOR_FILE
#  if COMPAT
     /// Path / name of file to use for interpolating the CIO location relative to GCRS
     /// This file can be generated with the <code>cio_file.c</code> tool using the
     /// <code>CIO_RA.TXT</code> data (both are included in the distribution)
#    define DEFAULT_CIO_LOCATOR_FILE      "cio_ra.bin"
#  else
/// Path / name of file to use for interpolating the CIO location relative to GCRS
/// This file can be generated with the <code>cio_file.c</code> tool using the
/// <code>CIO_RA.TXT</code> data (both are included in the distribution)
#    define DEFAULT_CIO_LOCATOR_FILE      "/usr/share/novas/cio_ra.bin"
#  endif
#endif

/**
 * Direction constants for polar wobble corrections via the wobble() function.
 *
 * @sa wobble()
 * @sa WOBBLE_ITRS_TO_TIRS
 */
enum novas_wobble_direction {
  /// use for wobble() to change from ITRS (actual rotating Earth) to Pseudo Earth Fixed (PEF).
  WOBBLE_ITRS_TO_PEF = 0,

  /// use for wobble() to change from Pseudo Earth Fixed (PEF) to ITRS (actual rotating Earth).
  /// (You can use any non-zero value as well.)
  WOBBLE_PEF_TO_ITRS
};

/**
 * Direction constant to use for frame_tie(), to determine the direction of transformation
 * between J2000 and ICRS coordinates.
 *
 * @sa frame_tie()
 * @sa J2000_TO_ICRS
 */
enum novas_frametie_direction {
  /// Change coordinates from ICRS to the J2000 (dynamical) frame. (You can also use any
  /// negative value for the same effect).
  J2000_TO_ICRS = -1,

  /// Change coordinates from J2000 (dynamical) frame to the ICRS. (You can use any value
  /// &gt;=0 for the same effect).
  ICRS_TO_J2000
};

/**
 * Direction constant for nutation(), between mean and true equatorial coordinates.
 *
 */
enum novas_nutation_direction {
  /// Change from true equator to mean equator (i.e. undo wobble corrections). You may use
  /// any non-zero value as well.
  NUTATE_TRUE_TO_MEAN = -1,

  /// Change from mean equator to true equator (i.e. apply wobble corrections)
  NUTATE_MEAN_TO_TRUE
};

/**
 * Fundamental Delaunay arguments of the Sun and Moon, from Simon section 3.4(b.3),
 * precession = 5028.8200 arcsec/cy)
 *
 * @since 1.0
 * @author Attila Kovacs
 */
typedef struct {
  double l;           ///< [rad] mean anomaly of the Moon
  double l1;          ///< [rad] mean anomaly of the Sun
  double F;           ///< [rad] mean argument of the latitude of the Moon
  double D;           ///< [rad] mean elongation of the Moon from the Sun
  double Omega;       ///< [rad] mean longitude of the Moon's ascending node.
} novas_delaunay_args;


#define SIZE_OF_OBJ_NAME 50     ///< Maximum bytes in object names including string termination.
#define SIZE_OF_CAT_NAME 6      ///< Maximum bytes in catalog IDs including string termination.

/**
 * Basic astrometric data for any sidereal object located outside the solar system.
 *
 * Note, that despite the slightly expanded catalog name, this has the same memory footprint
 * as the original NOVAS C version, allowing for cross-compatible binary exchange (I/O) of
 * these structures.
 *
 */
typedef struct {
  char starname[SIZE_OF_OBJ_NAME];  ///< name of celestial object
  char catalog[SIZE_OF_CAT_NAME];   ///< catalog designator (e.g., HIP)
  long starnumber;                  ///< integer identifier assigned to object
  double ra;                        ///< [h] ICRS right ascension
  double dec;                       ///< [deg] ICRS declination
  double promora;                   ///< [mas/yr] ICRS proper motion in right ascension
  double promodec;                  ///< [mas/yr] ICRS proper motion in declination
  double parallax;                  ///< [mas] parallax
  double radialvelocity;            ///< [km/s] catalog radial velocity
} cat_entry;

/**
 * Initializer for a NOVAS cat_entry structure.
 * @since 1.1.1
 */
#define CAT_ENTRY_INIT { {'\0'}, {'\0'}, 0L, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }

/**
 * Celestial object of interest.
 *
 * Note, the memory footprint is different from NOVAS C due to the use of the enum vs short 'type'
 * and the long vs. short 'number' values -- hence it is not cross-compatible for binary data
 * exchange with NOVAS C 3.1.
 */
typedef struct {
  enum novas_object_type type;    ///< NOVAS object type
  long number;                    ///< enum novas_planet, or minor planet ID (e.g. NAIF), or star catalog ID.
  char name[SIZE_OF_OBJ_NAME];    ///< name of the object (0-terminated)
  cat_entry star;                 ///< basic astrometric data for any 'catalog' object.
} object;

/**
 * data for an observer's location on the surface of
 * the Earth.  The atmospheric parameters are used
 * only by the refraction function called from
 * function 'equ2hor'. Additional parameters can be
 * added to this structure if a more sophisticated
 * refraction model is employed.
 *
 */
typedef struct {
  double latitude;      ///< [deg] geodetic (ITRS) latitude; north positive
  double longitude;     ///< [deg] geodetic (ITRS) longitude; east positive
  double height;        ///< [m] altitude above sea level
  double temperature;   ///< [C] temperature (degrees Celsius); for optical refraction
  double pressure;      ///< [mbar] atmospheric pressure for optical refraction
  double humidity;      ///< [%] Relative humidity. @since 1.1
} on_surface;

/**
 * data for an observer's location on Earth orbit
 *
 */
typedef struct {
  double sc_pos[3];     ///< [km] geocentric (or [AU] ICRS barycentric) position vector (x, y, z)
  double sc_vel[3];     ///< [km/s] geocentric (or [AU/day] ICRS barycentric) velocity vector (x_dot, y_dot, z_dot)
} in_space;

/**
 * Initializer for a NOVAS in_space structure.
 * @since 1.1.1
 */
#define IN_SPACE_INIT   {{0.0}, {0.0}}

/**
 * Observer location (somewhere around Earth).
 *
 */
typedef struct {
  enum novas_observer_place where;    ///< observer location type

  /// structure containing data for an observer's location on the surface of the Earth
  /// (if where = NOVAS_OBSERVER_ON_EARTH)
  on_surface on_surf;

  /// data for an observer's location in orbit (if where = NOVAS_OBSERVER_IN_EARTH_ORBIT)
  /// As of v1.1 the same structure may be used to store heliocentric location and motion
  /// for any Solar-system observer also (if where = NOVAS_SOLAR_SYSTEM_OBSERVER).
  in_space near_earth;
} observer;

/**
 * Celestial object's place on the sky; contains the output from place()
 *
 * @sa place()
 */
typedef struct {
  double r_hat[3];  ///< unit vector toward object (dimensionless)
  double ra;        ///< [h] apparent, topocentric, or astrometric right ascension (hours)
  double dec;       ///< [deg] apparent, topocentric, or astrometric declination (degrees)
  double dis;       ///< [AU] true (geometric, Euclidian) distance to solar system body or 0.0 for star (AU)
  double rv;        ///< [km/s] radial velocity (km/s)
} sky_pos;

/**
 * Initializer for a NOVAS sky_pos structure.
 * @since 1.1.1
 */
#define SKY_POS_INIT { {0.0}, 0.0, 0.0, 0.0, 0.0 }

/**
 * Right ascension of the Celestial Intermediate Origin (CIO) with respect to the GCRS
 *
 */
typedef struct {
  double jd_tdb;    ///< [day] Barycentric Dynamical Time (TDB) based Julian date.
  double ra_cio;    ///< [arcsec] right ascension of the CIO with respect to the GCRS (arcseconds)
} ra_of_cio;


/**
 * Constants to reference various astrnomical timescales used
 *
 * @since 1.1
 */
enum novas_timescale {
  NOVAS_TCB = 0,    ///< Barycentric Coordinate Time (TCB)
  NOVAS_TDB,        ///< Barycentric Dynamical Time (TDB)
  NOVAS_TCG,        ///< Geocentric Coordinate Time (TCG)
  NOVAS_TT,         ///< Terrestrial Time (TT)
  NOVAS_TAI,        ///< Innternational Atomic Time (TAI)
  NOVAS_GPS,        ///< GPS Time
  NOVAS_UTC,        ///< Universal Coordinated Time (UTC)
  NOVAS_UT1,        ///< UT1 earth rotation time, based on the measured Earth orientation parameters published in IERS Bulletin A.
};

/**
 * The number of asronomical time scales supported.
 *
 * @since 1.1
 */
#define NOVAS_TIMESCALES    (NOVAS_UT1 + 1)

/**
 * A structure, which defines a precise instant of time that can be extpressed in any of the
 * astronomical timescales. Precisions to picosecond accuracy are supported, which ought to be
 * plenty accurate for any real astronomical application.
 *
 * @see enum novas_timescale
 *
 * @since 1.1
 */
typedef struct {
  long ijd_tt;        ///< [day] Integer part of the Terrestrial Time (TT) based Julian Date
  double fjd_tt;      ///< [day] Terrestrial time (TT) based fractional Julian day.
  double tt2tdb;      ///< [s] TDB - TT time difference
  double ut1_to_tt;   ///< [s] UT1 - TT time difference
  double dut1;        ///< [s] UT1 - UTC time difference
} novas_timespec;

/**
 * A 3x3 matrix for coordinate transformations
 *
 * @since 1.1
 */
typedef struct {
  double M[3][3];     ///< matrix elements
} novas_matrix;


/**
 * Position and velocity data for a set of major planets (which may include the Sun and the Moon also).
 *
 * @since 1.1
 *
 * @sa enum novas_planet
 */
typedef struct {
  int mask;                      ///< Bitwise mask (1 << planet-number) specifying wich planets have pos/vel data
  double pos[NOVAS_PLANETS][3];  ///< [AU] Apparent positions of planets w.r.t. observer antedated for light-time
  double vel[NOVAS_PLANETS][3];  ///< [AU/day] Apparent velocity of planets w.r.t. barycenter antedated for light-time
} novas_planet_bundle;

/**
 * A set of parameters that uniquely define the place and time of observation. The user may
 * initialize the frame with novas_make_frame(). Once the observer frame is set up, it can be
 * used repeatedly to perform efficient calculations of multiple objects in the coordinate
 * system of choice, much faster than what place() can do. Frames also allow for transforming
 * coordinates calculated for one coordinate syste, into another coordinate system with
 * little effort.
 *
 * You should never set or change fields in this structure manually. Instead the structure
 * should always be initialized by an appropriate call to novas_make_frame(). After that you
 * may change the observer location, if need be, with novas_change_observer().
 *
 * The structure may expand with additional field in the future. Thus neither its size nor its
 * particular layout should be assumed fixed over SuperNOVAS releases.
 *
 * @since 1.1
 *
 * @see novas_make_frame()
 * @see novas_change_observer()
 */
typedef struct {
  uint64_t state;                 ///< An internal state for checking validity.
  enum novas_accuracy accuracy;   ///< NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
  novas_timespec time;            ///< The instant of time for which this observing frame is valid
  observer observer;              ///< The observer location, or NULL for barycentric
  double mobl;                    ///< [rad] Mean obliquity
  double tobl;                    ///< [rad] True obliquity
  double ee;                      ///< [rad] Equation of the equinoxes
  double dpsi0;                   ///< [rad] Modeled Earth orientation &psi; (not including polar wobble)
  double deps0;                   ///< [rad] Modeled Earth orientation &epsilon; (not including polar wobble)
  double dx;                      ///< [mas] Polar wobble parameter dx.
  double dy;                      ///< [mas] Polar wobble parameter dy.
  double era;                     ///< [deg] Earth Rotation Angle (ERA);
  double gst;                     ///< [h] Greenwich (Apparent) Sidereal Time (GST / GAST)
  double obs_pos[3];              ///< [AU] Observer position rel. to barycenter
  double obs_vel[3];              ///< [AU/day] Observer movement rel. to barycenter
  double v_obs;                   ///< [AU/day] Magnitude of observer motion rel. to barycenter
  double beta;                    ///< Observer relativistic &beta; rel SSB
  double gamma;                   ///< Observer Lorentz factor &Gamma; rel SSB
  double sun_pos[3];              ///< [AU] Sun's geometric position, rel SSB.
  double sun_vel[3];              ///< [AU/day] Sun's velocity, rel SSB.
  double earth_pos[3];            ///< [AU] Earth's geometric position, rel SSB.
  double earth_vel[3];            ///< [AU/day] Earth's velocity, rel SSB.
  novas_matrix icrs_to_j2000;     ///< ICRS to J2000 matrix
  novas_matrix precession;        ///< precession matrix
  novas_matrix nutation;          ///< nutation matrix (Lieske 1977 method)
  novas_matrix gcrs_to_cirs;      ///< GCRS to CIRS conversion matrix
  novas_planet_bundle planets;    ///< Planet positions and velocities
} novas_frame;

/**
 * A transformation between two astronomical coordinate systems for the same observer
 * location and time. This allows for more elegant, generic, and efficient coordinate
 * transformations than the low-level NOVAS functions.
 *
 * The transformation can be (should be) initialized via novas_make_trasform(), or via
 * novas_invert_transform().
 *
 * @since 1.1
 *
 * @sa novas_make_transform()
 * @sa novas_invert_transform()
 */
typedef struct {
  enum novas_reference_system from_system;  ///< The original coordinate system
  enum novas_reference_system to_system;    ///< The final coordinate system
  novas_frame frame;                        ///< The observer place and time for which the transform is valid
  novas_matrix matrix;                      ///< Transformation matrix elements
} novas_transform;

/**
 * The type of elevation value for which to calculate a refraction.
 *
 * @sa RefractionModel
 *
 * @since 1.1
 */
enum novas_refraction_type {
  NOVAS_REFRACT_OBSERVED = -1,  ///< Refract observed elevation value
  NOVAS_REFRACT_ASTROMETRIC     ///< Refract astrometric elevation value
};

/**
 * Default set of gravitating bodies to use for deflection calculations in reduced accuracy mode.
 *
 * @sa grav_bodies_reduced_accuracy
 *
 * @since 1.1
 * @author Attila Kovacs
 */
#define DEFAULT_GRAV_BODIES_REDUCED_ACCURACY   ( (1 << NOVAS_SUN) | (1 << NOVAS_EARTH) )

/**
 * Default set of gravitating bodies to use for deflection calculations in full accuracy mode.
 *
 * @sa grav_bodies_full_accuracy
 *
 * @since 1.1
 * @author Attila Kovacs
 */
#define DEFAULT_GRAV_BODIES_FULL_ACCURACY      ( DEFAULT_GRAV_BODIES_REDUCED_ACCURACY | (1 << NOVAS_JUPITER) | (1 << NOVAS_SATURN) )

/**
 * Current set of gravitating bodies to use for deflection calculations in reduced accuracy mode. Each
 * bit signifies whether a given body is to be accounted for as a gravitating body that bends light,
 * such as the bit `(1 << NOVAS_JUPITER)` indicates whether or not Jupiter is considered as a deflecting
 * body. You should also be sure that you provide ephemeris data for bodies that are designated for the
 * deflection calculation.
 *
 * @sa grav_def()
 * @sa grav_planets()
 * @sa DEFAULT_GRAV_BODIES_REDUCED_ACCURACY
 * @sa set_ephem_provider()
 *
 * @since 1.1
 */
extern int grav_bodies_reduced_accuracy;

/**
 * Current set of gravitating bodies to use for deflection calculations in full accuracy mode. Each
 * bit signifies whether a given body is to be accounted for as a gravitating body that bends light,
 * such as the bit `(1 << NOVAS_JUPITER)` indicates whether or not Jupiter is considered as a deflecting
 * body. You should also be sure that you provide ephemeris data for bodies that are designated for the
 * deflection calculation.
 *
 * @sa grav_def()
 * @sa grav_planets()
 * @sa DEFAULT_GRAV_BODIES_FULL_ACCURACY
 * @sa set_ephem_provider_hp()
 *
 * @since 1.1
 */
extern int grav_bodies_full_accuracy;


/**
 * A function that returns a refraction correction for a given date/time of observation at the
 * given site on earth, and for a given astrometric source elevation
 *
 * @param jd_tt     [day] Terrestrial Time (TT) based Julian data of observation
 * @param loc       Pointer to structure defining the observer's location on earth, and local weather
 * @param type      Whether the input elevation is observed or astrometric: REFRACT_OBSERVED (-1) or
 *                  REFRACT_ASTROMETRIC (0).
 * @param el        [deg] Astrometric (unrefracted) source elevation
 * @return          [arcsec] Estimated refraction, or NAN if there was an error (it should
 *                  also set errno to indicate the type of error).
 *
 * @since 1.1
 */
typedef double (*RefractionModel)(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el);


short app_star(double jd_tt, const cat_entry *star, enum novas_accuracy accuracy, double *ra, double *dec);

short virtual_star(double jd_tt, const cat_entry *star, enum novas_accuracy accuracy, double *ra, double *dec);

short astro_star(double jd_tt, const cat_entry *star, enum novas_accuracy accuracy, double *ra, double *dec);

short app_planet(double jd_tt, const object *ss_body, enum novas_accuracy accuracy, double *ra, double *dec,
        double *dis);

short virtual_planet(double jd_tt, const object *ss_body, enum novas_accuracy accuracy, double *ra, double *dec,
        double *dis);

short astro_planet(double jd_tt, const object *ss_body, enum novas_accuracy accuracy, double *ra, double *dec,
        double *dis);

short topo_star(double jd_tt, double ut1_to_tt, const cat_entry *star, const on_surface *position,
        enum novas_accuracy accuracy, double *ra, double *dec);

short local_star(double jd_tt, double ut1_to_tt, const cat_entry *star, const on_surface *position,
        enum novas_accuracy accuracy, double *ra, double *dec);

short topo_planet(double jd_tt, const object *ss_body, double ut1_to_tt, const on_surface *position,
        enum novas_accuracy accuracy, double *ra, double *dec, double *dis);

short local_planet(double jd_tt, const object *ss_body, double ut1_to_tt, const on_surface *position,
        enum novas_accuracy accuracy, double *ra, double *dec, double *dis);

short mean_star(double jd_tt, double tra, double tdec, enum novas_accuracy accuracy, double *ira, double *idec);

short place(double jd_tt, const object *source, const observer *location, double ut1_to_tt,
        enum novas_reference_system coord_sys, enum novas_accuracy accuracy, sky_pos *output);

int equ2gal(double ra, double dec, double *glon, double *glat);

short equ2ecl(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double ra, double dec,
        double *elon, double *elat);

short equ2ecl_vec(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, const double *in,
        double *out);

short ecl2equ_vec(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, const double *in,
        double *out);

int equ2hor(double jd_ut1, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp,
        const on_surface *location, double ra, double dec, enum novas_refraction_model option, double *zd, double *az,
        double *rar, double *decr);

short gcrs2equ(double jd_tt, enum novas_dynamical_type sys, enum novas_accuracy accuracy, double rag, double decg,
        double *ra, double *dec);

short sidereal_time(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_equinox_type gst_type,
        enum novas_earth_rotation_measure method, enum novas_accuracy accuracy, double *gst);

double era(double jd_ut1_high, double jd_ut1_low);

short ter2cel(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_earth_rotation_measure erot,
        enum novas_accuracy accuracy, enum novas_equatorial_class class, double xp, double yp, const double *in,
        double *out);

short cel2ter(double jd_ut1_high, double jd_ut1_low, double ut1_to_tt, enum novas_earth_rotation_measure erot,
        enum novas_accuracy accuracy, enum novas_equatorial_class class, double xp, double yp, const double *in,
        double *out);

int spin(double angle, const double *in, double *out);

int wobble(double jd_tt, enum novas_wobble_direction direction, double xp, double yp, const double *in, double *out);

int terra(const on_surface *location, double lst, double *pos, double *vel);

int e_tilt(double jd_tdb, enum novas_accuracy accuracy, double *mobl, double *tobl, double *ee, double *dpsi,
        double *deps);

short cel_pole(double jd_tt, enum novas_pole_offset_type type, double dpole1, double dpole2);

double ee_ct(double jd_tt_high, double jd_tt_low, enum novas_accuracy accuracy);

int frame_tie(const double *in, enum novas_frametie_direction direction, double *out);

int proper_motion(double jd_tdb_in, const double *pos, const double *vel, double jd_tdb_out, double *out);

int bary2obs(const double *pos, const double *pos_obs, double *out, double *lighttime);

short geo_posvel(double jd_tt, double ut1_to_tt, enum novas_accuracy accuracy, const observer *obs, double *pos,
        double *vel);

short light_time(double jd_tdb, const object *body, const double *pos_obs, double tlight0, enum novas_accuracy accuracy,
        double *pos_src_obs, double *tlight);

double d_light(const double *pos_src, const double *pos_body);

short grav_def(double jd_tdb, enum novas_observer_place unused, enum novas_accuracy accuracy, const double *pos_src,
        const double *pos_obs, double *out);

int grav_vec(const double *pos_src, const double *pos_obs, const double *pos_body, double rmass, double *out);

int aberration(const double *pos, const double *vobs, double lighttime, double *pos2);

int rad_vel(const object *source, const double *pos, const double *vel, const double *vel_obs, double d_obs_geo,
        double d_obs_sun, double d_src_sun, double *rv);

short precession(double jd_tdb_in, const double *in, double jd_tdb_out, double *out);

int nutation(double jd_tdb, enum novas_nutation_direction direction, enum novas_accuracy accuracy, const double *in,
        double *out);

int nutation_angles(double t, enum novas_accuracy accuracy, double *dpsi, double *deps);

int fund_args(double t, novas_delaunay_args *a);

double planet_lon(double t, enum novas_planet planet);

double accum_prec(double t);

double mean_obliq(double jd_tdb);

short vector2radec(const double *pos, double *ra, double *dec);

int radec2vector(double ra, double dec, double dist, double *pos);

int starvectors(const cat_entry *star, double *pos, double *vel);

double get_ut1_to_tt(int leap_seconds, double dut1);

int tdb2tt(double jd_tdb, double *jd_tt, double *secdiff);

short cio_ra(double jd_tt, enum novas_accuracy accuracy, double *ra_cio);

short cio_location(double jd_tdb, enum novas_accuracy accuracy, double *ra_cio, short *loc_type);

short cio_basis(double jd_tdb, double ra_cio, enum novas_cio_location_type loc_type, enum novas_accuracy accuracy,
        double *x, double *y, double *z);

short cio_array(double jd_tdb, long n_pts, ra_of_cio *cio);

double ira_equinox(double jd_tdb, enum novas_equinox_type equinox, enum novas_accuracy accuracy);

short ephemeris(const double *jd_tdb, const object *body, enum novas_origin origin, enum novas_accuracy accuracy,
        double *pos, double *vel);

int transform_hip(const cat_entry *hipparcos, cat_entry *hip_2000);

short transform_cat(enum novas_transform_type, double jd_tt_in, const cat_entry *in, double jd_tt_out, const char *out_id,
        cat_entry *out);

int limb_angle(const double *pos_src, const double *pos_obs, double *limb_ang, double *nadir_ang);

double refract(const on_surface *location, enum novas_refraction_model option, double zd_obs);

double julian_date(short year, short month, short day, double hour);

int cal_date(double tjd, short *year, short *month, short *day, double *hour);

double norm_ang(double angle);

short make_cat_entry(const char *star_name, const char *catalog, long cat_num, double ra, double dec, double pm_ra,
        double pm_dec, double parallax, double rad_vel, cat_entry *star);

short make_object(enum novas_object_type, long number, const char *name, const cat_entry *star_data, object *source);

short make_observer(enum novas_observer_place, const on_surface *loc_surface, const in_space *loc_space, observer *obs);

int make_observer_at_geocenter(observer *obs);

int make_observer_on_surface(double latitude, double longitude, double height, double temperature, double pressure,
        observer *obs);

int make_observer_in_space(const double *sc_pos, const double *sc_vel, observer *obs);

int make_on_surface(double latitude, double longitude, double height, double temperature, double pressure,
        on_surface *loc);

int make_in_space(const double *sc_pos, const double *sc_vel, in_space *loc);



// -------------------------------------------------------------------------------------------------------------------
// SuperNOVAS API:

void novas_debug(enum novas_debug_mode mode);

enum novas_debug_mode novas_get_debug_mode();

void novas_case_sensitive(int value);

int make_planet(enum novas_planet num, object *planet);

int make_ephem_object(const char *name, long num, object *body);

int set_cio_locator_file(const char *filename);

int set_nutation_lp_provider(novas_nutation_provider func);

int place_star(double jd_tt, const cat_entry *star, const observer *obs, double ut1_to_tt,
        enum novas_reference_system system, enum novas_accuracy accuracy, sky_pos *pos);

int place_icrs(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos);

int place_gcrs(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos);

int place_cirs(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos);

int place_tod(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos);

int radec_star(double jd_tt, const cat_entry *star, const observer *obs, double ut1_to_tt,
        enum novas_reference_system sys, enum novas_accuracy accuracy, double *ra, double *dec, double *rv);

int radec_planet(double jd_tt, const object *ss_body, const observer *obs, double ut1_to_tt,
        enum novas_reference_system sys, enum novas_accuracy accuracy, double *ra, double *dec, double *dis, double *rv);

double refract_astro(const on_surface *location, enum novas_refraction_model option, double zd_calc);

int light_time2(double jd_tdb, const object *body, const double *pos_obs, double tlight0, enum novas_accuracy accuracy,
        double *pos_src_obs, double *v_ssb, double *tlight);

double tt2tdb(double jd_tt);

double get_ut1_to_tt(int leap_seconds, double dut1);

double get_utc_to_tt(int leap_seconds);

int ecl2equ(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double elon, double elat,
        double *ra, double *dec);

int gal2equ(double glon, double glat, double *ra, double *dec);

// GCRS - CIRS - ITRS conversions
int gcrs_to_cirs(double jd_tt, enum novas_accuracy accuracy, const double *in, double *out);

int cirs_to_itrs(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const double *in,
        double *out);

int itrs_to_cirs(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const double *in,
        double *out);

int cirs_to_gcrs(double jd_tt, enum novas_accuracy accuracy, const double *in, double *out);

// GCRS - J2000 - TOD - ITRS conversions
int gcrs_to_j2000(const double *in, double *out);

int j2000_to_tod(double jd_tt, enum novas_accuracy accuracy, const double *in, double *out);

int tod_to_itrs(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const double *in,
        double *out);

int itrs_to_tod(double jd_tt_high, double jd_tt_low, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const double *in,
        double *out);

int tod_to_j2000(double jd_tt, enum novas_accuracy accuracy, const double *in, double *out);

int j2000_to_gcrs(const double *in, double *out);

// ITRS - horizontal conversions
int itrs_to_hor(const on_surface *location, const double *itrs, double *az, double *za);

int hor_to_itrs(const on_surface *location, double az, double za, double *itrs);



// ---------------------- Added in 1.0.1 -------------------------
double cirs_to_app_ra(double jd_tt, enum novas_accuracy accuracy, double ra);

double app_to_cirs_ra(double jd_tt, enum novas_accuracy accuracy, double ra);



// ---------------------- Added in 1.1.0 -------------------------
int obs_posvel(double jd_tdb, double ut1_to_tt, enum novas_accuracy accuracy, const observer *obs,
        const double *geo_pos, const double *geo_vel, double *pos, double *vel);

int obs_planets(double jd_tdb, enum novas_accuracy accuracy, const double *pos_obs, int pl_mask, novas_planet_bundle *planets);

int grav_undef(double jd_tdb, enum novas_accuracy accuracy, const double *pos_app, const double *pos_obs, double *out);

int grav_planets(const double *pos_src, const double *pos_obs, const novas_planet_bundle *planets, double *out);

int grav_undo_planets(const double *pos_app, const double *pos_obs, const novas_planet_bundle *planets, double *out);

int make_airborne_observer(const on_surface *location, const double *vel, observer *obs);

int make_solar_system_observer(const double *sc_pos, const double *sc_vel, observer *obs);

int make_cat_object(const cat_entry *star, object *source);

int place_mod(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos);

int place_j2000(double jd_tt, const object *source, enum novas_accuracy accuracy, sky_pos *pos);

int cirs_to_tod(double jd_tt, enum novas_accuracy accuracy, const double *in, double *out);

int tod_to_cirs(double jd_tt, enum novas_accuracy accuracy, const double *in, double *out);

double rad_vel2(const object *source, const double *pos_emit, const double *vel_src, const double *pos_det, const double *vel_obs,
        double d_obs_geo, double d_obs_sun, double d_src_sun);

// in timescale.c
int novas_set_time(enum novas_timescale timescale, double jd, int leap, double dut1, novas_timespec *time);

int novas_set_split_time(enum novas_timescale timescale, long ijd, double fjd, int leap, double dut1, novas_timespec *time);

double novas_get_time(const novas_timespec *time, enum novas_timescale timescale);

double novas_get_split_time(const novas_timespec *time, enum novas_timescale timescale, long *ijd);

int novas_set_unix_time(time_t unix_time, long nanos, int leap, double dut1, novas_timespec *time);

time_t novas_get_unix_time(const novas_timespec *time, long *nanos);

double novas_diff_time(const novas_timespec *t1, const novas_timespec *t2);

double novas_diff_tcb(const novas_timespec *t1, const novas_timespec *t2);

double novas_diff_tcg(const novas_timespec *t1, const novas_timespec *t2);

int novas_offset_time(const novas_timespec *time, double seconds, novas_timespec *out);


// in frames.c
int novas_make_frame(enum novas_accuracy accuracy, const observer *obs, const novas_timespec *time, double dx, double dy,
        novas_frame *frame);

int novas_change_observer(const novas_frame *orig, const observer *obs, novas_frame *out);

int novas_geom_posvel(const object *source, const novas_frame *frame, enum novas_reference_system sys, double *pos, double *vel);

int novas_geom_to_app(const novas_frame *frame, const double *pos, enum novas_reference_system sys, sky_pos *out);

int novas_sky_pos(const object *object, const novas_frame *frame, enum novas_reference_system sys, sky_pos *output);

int novas_app_to_hor(const novas_frame *frame, enum novas_reference_system sys, double ra, double dec, RefractionModel ref_model,
        double *az, double *el);

int novas_app_to_geom(const novas_frame *frame, enum novas_reference_system sys, double ra, double dec, double dist, double *geom_icrs);

int novas_hor_to_app(const novas_frame *frame, double az, double el, RefractionModel ref_model, enum novas_reference_system sys,
        double *ra, double *dec);

int novas_make_transform(const novas_frame *frame, enum novas_reference_system from_system, enum novas_reference_system to_system,
        novas_transform *transform);

int novas_invert_transform(const novas_transform *transform, novas_transform *inverse);

int novas_transform_vector(const double *in, const novas_transform *transform, double *out);

int novas_transform_sky_pos(const sky_pos *in, const novas_transform *transform, sky_pos *out);


// in refract.c
double novas_standard_refraction(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el);

double novas_optical_refraction(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el);

double novas_radio_refraction(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el);

double novas_inv_refract(RefractionModel model, double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el0);


// ---------------------- Added in 1.2.0 -------------------------
int make_redshifted_object(const char *name, double ra, double dec, double z, object *source);

double novas_z2v(double z);

double novas_v2z(double vel);

double grav_redshift(double M_kg, double r_m);

double redshift_vrad(double vrad, double z);

double unredshift_vrad(double vrad, double z);

double novas_z_add(double z1, double z2);

double novas_z_inv(double z);

enum novas_planet novas_planet_for_name(const char *name);


// <================= END of SuperNOVAS API =====================>



#include "solarsystem.h"


// <================= SuperNOVAS internals ======================>

/// \cond PRIVATE
#ifdef __NOVAS_INTERNAL_API__

#  include <stdio.h>
#  include <math.h>

#define _CONSTS_

#define HALF_PI             (0.5 * M_PI)
#define ERAD_AU             (ERAD/AU)

#define XYZ_VECTOR_SIZE     (3 * sizeof(double))

// Use shorthand definitions for our constants
#define JD_J2000            NOVAS_JD_J2000
#define C                   NOVAS_C
#define AU_SEC              NOVAS_AU_SEC
#define C_AUDAY             NOVAS_C_AU_PER_DAY
#define AU                  NOVAS_AU
#define AU_KM               NOVAS_AU_KM
#define GS                  NOVAS_G_SUN
#define GE                  NOVAS_G_EARTH
#define ERAD                NOVAS_EARTH_RADIUS
#define EF                  NOVAS_EARTH_FLATTENING
#define ANGVEL              NOVAS_EARTH_ANGVEL

// Various locally used physical units
#define DAY                 86400.0               ///< [s] seconds in a day
#define DAY_HOURS           24.0
#define DEG360              360.0
#define JULIAN_YEAR_DAYS    365.25
#define JULIAN_CENTURY_DAYS 36525.0
#define ARCSEC              ASEC2RAD
#define DEGREE              DEG2RAD
#define HOURANGLE           (M_PI / 12.0)
#define MAS                 (1e-3 * ASEC2RAD)

// On some older platform NAN may not be defined, so define it here if need be
#ifndef NAN
#  define NAN               (0.0/0.0)
#endif

#  ifndef THREAD_LOCAL
#    if __STDC_VERSION__ >= 201112L
#      define THREAD_LOCAL _Thread_local          ///< C11 standard for thread-local variables
#    elif __GNUC__ >= 3 && __GNUC_MINOR__ >= 3
#      define THREAD_LOCAL __thread               ///< pre C11 gcc >= 3.3 standard for thread-local variables
#    else
#      define THREAD_LOCAL                        ///< no thread-local variables
#    endif
#  endif


int novas_trace(const char *loc, int n, int offset);
double novas_trace_nan(const char *loc);
void novas_set_errno(int en, const char *from, const char *desc, ...);
int novas_error(int ret, int en, const char *from, const char *desc, ...);

/**
 * Propagates an error (if any) with an offset. If the error is non-zero, it returns with the offset
 * error value. Otherwise it keeps going as if it weren't even there...
 *
 * @param n     {int} error code or the call that produces the error code
 * @param d     {int} offset with which the error is propagated
 *
 * @sa error_return
 */
#  define prop_error(loc, n, d) { \
  int __ret = novas_trace(loc, n, d); \
  if (__ret != 0) \
    return __ret; \
}

double novas_vlen(const double *v);
double novas_vdist(const double *v1, const double *v2);
double novas_vdot(const double *v1, const double *v2);

int polar_dxdy_to_dpsideps(double jd_tt, double dx, double dy, double *dpsi, double *deps);

extern int novas_inv_max_iter;

#endif /* __NOVAS_INTERNAL_API__ */
/// \endcond

#endif /* _NOVAS_ */
