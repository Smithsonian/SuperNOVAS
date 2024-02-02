/**
 * @file
 *
 * @author G. Kaplan and A. Kovacs
 * @version 0.9.0
 *
 *  SuperNOVAS astrometry softwate based on the Naval Observatory Vector Astrometry Software (NOVAS).
 *  It has been modified to fix outstanding issues and to make it easier to use.
 *
 *
 *  Based on the NOVAS C Edition, Version 3.1,  U. S. Naval Observatory
 *  Astronomical Applications Dept.
 *  Washington, DC
 *  <a href="http://www.usno.navy.mil/USNO/astronomical-applications">http://www.usno.navy.mil/USNO/astronomical-applications</a>
 */

#ifndef _NOVAS_
#define _NOVAS_

#include <math.h>   // for M_PI
#include <stdlib.h> // NULL

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

#define SUPERNOVAS_MAJOR_VERSION  0       ///< API major version
#define SUPERNOVAS_MINOR_VERSION  0       ///< API major version
#define SUPERNOVAS_SUBVERSION     0       ///< Integer sub version of the release
#define SUPERNOVAS_VERSION_EXTRA  ""      ///< Additional release information in version, e.g. "-1", or "-rc1".

/// The version string for this library
#define SUPERNOVAS_VERSION_STRING #SUPERNOVAS_MAJOR_VERSION "." #SUPERNOVAS_MINOR_VERSION "." \
                                  #SUPERNOVAS_SUBVERSION SUPERNOVAS_RELEASE

#define NOVAS_MAJOR_VERSION       3       ///< Major version of NOVAS on which this library is based
#define NOVAS_MINOR_VERSION       1       ///< Minor version of NOVAS on which this library is based

/// The version string of the upstream NOVAS library on which this library is based.
#define NOVAS_VERSION_STRING      #NOVAS_MAJOR_VERSION "." NOVAS_MINOR_VERSION

#define NOVAS_CIO_CACHE_SIZE      1024    ///< [pts] cache size for GCRS CIO locator data (16 bytes per point).

/// [day] Julian date at J2000
#define NOVAS_JD_J2000    2451545.0

/// [day] Julian date at B1950
#define NOVAS_JD_B1950    2433282.42345905

/// [day] Julian date at B1900
#define NOVAS_JD_B1900    15019.81352

/// [m/s] Speed of light in meters/second is a defining physical constant.
#define NOVAS_C           299792458.0

/// [m] Astronomical unit in meters.  Value is AU_SEC * C.
#define NOVAS_AU          1.4959787069098932e+11

/// [AU] Light-time for one astronomical unit (AU) in seconds, from DE-405.
#define NOVAS_AU_SEC      ( NOVAS_AU / NOVAS_C )

/// [AU/day] Speed of light in AU/day.  Value is 86400 / AU_SEC.
#define NOVAS_C_AU_PER_DAY     ( 86400.0 / AU_SEC )

/// [km] Astronomical Unit in kilometers.
#define NOVAS_AU_KM       ( 1e-3 * NOVAS_AU )

/// [m<sup>3</sup>/s<sup>2</sup>] Heliocentric gravitational constant in meters^3 / second^2, from DE-405.
#define NOVAS_GS          1.32712440017987e+20

/// [m<sup>3</sup>/s<sup>2</sup>] Geocentric gravitational constant in meters^3 / second^2, from DE-405.
#define NOVAS_GE          3.98600433e+14

/// [m] Radius of Earth in meters from IERS Conventions (2003).
#define NOVAS_ERAD        6378136.6

/// Earth ellipsoid flattening from IERS Conventions (2003). Value is 1 / 298.25642.
#define NOVAS_EFLAT       (1.0 / 298.25642)

/// [rad/s] Rotational angular velocity of Earth in radians/sec from IERS Conventions (2003).
#define NOVAS_ANGVEL      7.2921150e-5

/// [s] TAI - GPS time offset
#define NOVAS_GPS_TO_TAI  19.0

/// [s] TT - TAI time offset
#define NOVAS_TAI_TO_TT   32.187



/// Reciprocal masses of solar system bodies, from DE-405 (Sun mass / body mass).
/// [0]: Earth/Moon barycenter, MASS[1] = Mercury, ...,
/// [9]: Pluto, [10]: Sun, [11]: Moon.
#define NOVAS_RMASS_INIT  { \
      328900.561400, 6023600.0, 408523.71, 332946.050895, 3098708.0, 1047.3486, 3497.898, \
      22902.98, 19412.24, 135200000.0, 1.0, 27068700.387534 }

#if !COMPAT
// If we are not in the strict compatibility mode, where constants are defined
// as variables in novascon.h (with implementation in novascon.c), then define
// them here

#  ifndef TWOPI
#    define TWOPI             (2.0 * M_PI)    ///< 2&pi;
#  endif

#  ifndef ASEC360
/// [arcsec] Number of arcseconds in 360 degrees.
#    define ASEC360           (360 * 60 * 60)
#  endif

#  ifndef DEG2RAD
/// [rad/deg] 1 degree in radians
#    define DEG2RAD           (M_PI / 180.0)
#  endif

#  ifndef RAD2DEG
/// [deg/rad] 1 radian in degrees
#    define RAD2DEG           (1.0 / DEG2RAD)
#  endif

#  ifndef ASEC2RAD
/// [rad/arcsec] 1 arcsecond in radians
#    define ASEC2RAD          (DEG2RAD / 3600.0)
#  endif

#endif

/**
 * The type of astronomical objects distinguied by the NOVAS library.
 *
 */
enum novas_object_type {
  NOVAS_MAJOR_PLANET = 0,     ///< A major planet, and also including the Sun an the moon Moon. @sa novas_planets
  NOVAS_MINOR_PLANET, ///< A minor planet, which may only be handled via ephemeris data, and may require specific user-provided implementation.
  NOVAS_DISTANT_OBJECT        ///< Any non-solar system object that may be handled via 'catalog' coordinates, such as a star or a quasar.
};

#define NOVAS_OBJECT_TYPES        (NOVAS_DISTANT_OBJECT + 1) ///< The number of object types distinguished by NOVAS.

/**
 * Enumeration for the 'major planet' numbers in NOVAS to use as the solar-system body number whenever
 * the object type is NOVAS_MAJOR_PLANET.
 *
 * @sa NOVAS_MAJOR_PLANET
 * @sa NOVAS_MAJOR_PLANET_NAMES_INIT
 */
enum novas_planet {
  NOVAS_BARYCENTER_POS = 0, ///< Solar-system barycenter position ID
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

#define NOVAS_PLANETS       (NOVAS_MOON + 1)      ///< The number of major planets defined in NOVAS.

/**
 * String array initializer for Major planet names, matching the enum novas_planet. E.g.
 *
 * \code
 * char *planet_names[] = NOVAS_MAJOR_PLANET_NAMES_INIT;
 * \endcode
 *
 *
 * @sa novas_majot_planet
 */
#define NOVAS_PLANET_NAMES_INIT { "Barycenter", "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune", "Pluto", "Sun", "Moon" }

/**
 * Types of places on and around Earth that may serve a a reference position for the observation.
 *
 */
enum novas_observer_place {
  NOVAS_OBSERVER_AT_GEOCENTER = 0, ///< Calculate coordinates as if observing from the geocenter for location and Earth rotation independent coordinates.
  NOVAS_OBSERVER_ON_EARTH,            ///< Observer is at a location that is in the rotating frame of Earth.

  /**
   * Observer is on Earth orbit, with a position and velocity vector relative to geocenter.
   * This may also be appropriate for observatories at the L2 or other Earth-based Langrange
   * points.
   */
  NOVAS_OBSERVER_IN_EARTH_ORBIT
};

#define NOVAS_OBSERVER_PLACES     (NOVAS_IN_SPACE + 1)  ///< The number of observer place types supported

/**
 * The basic types of positional coordinate reference systems supported by NOVAS. These determine only how the celestial pole
 * is to be located, but not how velocities are to be referenced. specific pos-vel coordinates are referenced to
 * an 'astro_frame', which must specify one of the values defined here.
 *
 * @sa astro_frame
 */
enum novas_reference_system {
  NOVAS_GCRS = 0, ///< Geocentric Celestial Reference system. Essentially the same as ICRS, but with velocities referenced to Earth's orbiting frame.
  NOVAS_TOD,     ///< True equinox Of Date: dynamical system of the true equator, with its origin at the true equinox (pre IAU 2006 system).
  NOVAS_CIRS, ///< Celestial Intermediate Reference System: dynamical system of the true equator, with its origin at the CIO (preferred since IAU 2006)
  NOVAS_ICRS      ///< International Celestiual Reference system. The equatorial system fixed to the frame of distant quasars.
};

/**
 * Constants that determine the type of equator to be used for the coordinate system.
 */
enum novas_equator_type {
  NOVAS_MEAN_EQUATOR, ///< True equinox Of Date (TOD): dynamical system of the true equator, with its origin at the true equinox (pre IAU 2006 system).
  NOVAS_TRUE_EQUATOR, ///< Celestial Intermediate Reference System (CIRS): dynamical system of the true equator, with its origin at the CIO (preferred since IAU 2006)
  NOVAS_ICRS_EQUATOR  ///< International Celestiual Reference system (ICRS). The equatorial system fixed to the frame of distant quasars.
};

#define NOVAS_REFERENCE_SYSTEMS   (NOVAS_ICRS + 1)    ///< The number of basic coordinate reference systems in NOVAS.

/**
 * Constants to control the precision of NOVAS nutation calculations.
 *
 */
enum novas_accuracy {
  NOVAS_FULL_ACCURACY = 0, ///< Use full precision calculations to micro-arcsecond accuracy. It can be computationally intensive when using the dynamical equator.
  NOVAS_REDUCED_ACCURACY ///< Calculate with truncated terms. It can be significantly faster if a few milliarcsecond accuracy is sufficient.
};

/**
 * Constants that determine whether refraction calculations should use a standard atmospheric model, or whatever
 * weather parameters have been been specified for the observing location.
 *
 * @sa on_surface
 */
enum novas_refraction_model {
  NOVAS_STANDARD_ATMOSPHERE = 0, ///< Uses a standard atmospheric model, ignoring all weather values defined for the specific observing location
  NOVAS_WEATHER_AT_LOCATION       ///< Uses the weather parameters that are specified together with the observing location.
};

/**
 * Constants that determine the type of rotation measure to use.
 */
enum novas_earth_rotation_measure {
  EROT_GST = 0,                   ///< Use GST as the rotation measure, relative to the true equinox (before IAU 20006 standard)
  EROT_ERA                        ///< Use Earth Rotation Angle (ERA) as the rotation measure, relative to the CIO (new IAU 2006 standard)
};

/**
 * The convention in which the celestial pole offsets are defined for polar wobble.
 *
 */
enum novas_pole_offset_type {
  POLE_OFFSETS_DPSI_DEPS = 1,     ///< Offsets are &Delta;d&psi;, &Delta;d&epsilon; pairs (pre IAU 2006 precession-nutation model).
  POLE_OFFSETS_X_Y                ///< Offsets are dx, dy pairs (IAU 2006 precession-nutation model)
};

/**
 * The type of equinox to be calculated
 *
 */
enum novas_equinox_type {
  NOVAS_MEAN_EQUINOX = 0,         /// Mean equinox
  NOVAS_TRUE_EQUINOX              /// True apparent equinox
};

/**
 * The origin of the NOVAS_ICRS system for referencing velocities.
 */
enum novas_origin {
  NOVAS_BARYCENTER = 0,           ///< Origin at the Solar-system baricenter (i.e. BCRS)
  NOVAS_HELIOCENTER               ///< Origin at the center of the Sun.
};
#define NOVAS_ORIGIN_TYPES            (NOVAS_HELIOCENTER + 1)     ///< the number of different ICSR origins available in NOVAS.

#define BARYC     NOVAS_BARYCENTER      ///< @deprecated Old definition of the Barycenter origin. Same as NOVAS_BARYCENTER.
#define HELIOC    NOVAS_HELIOCENTER     ///< @deprecated Old definition of the Center of Sun as the origin. Same as NOVAS_HELIOCENTER.

/**
 * The types of coordinate transformations available for tranform_cat().
 *
 * @sa transform_cat()
 */
enum novas_transform_type {
  /// Updates the star's data to account for the star's space motion between
  /// the first and second dates, within a fixed reference frame.
  CHANGE_EPOCH = 1,

  /// applies a rotation of the reference frame
  /// corresponding to precession between the first and second dates,
  /// but leaves the star fixed in space.
  CHANGE_EQUATOR_EQUINOX,

  /// The combined equivalent of CHANGE_EPOCH and CHANGE_EQUATOR_EQUINOX together.
  CHANGE_EQUATOR_EQUINOX_EPOCH,

  /// A fixed rotation about very small angles (<0.1 arcsecond) to take data from the
  /// dynamical system of J2000.0 to the ICRS.
  CHANGE_J2000_TO_ICRS,

  /// The inverse transformation of J2000_TO_ICRS
  CHANGE_ICRS_TO_J2000
};

#define NOVAS_TRANSFORM_TYPES         (ICRS_TO_J2000 + 1)         ///< The number of coordinate transfor types in NOVAS.

/**
 * System in which CIO location is calculated.
 */
enum novas_cio_location_type {
  CIO_VS_GCRS = 1,     ///< The location of the CIO relative to the GCRS frame.
  CIO_VS_EQUINOX       ///< The location of the CIO relative to the true equinox in the dynamical frame
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
  WOBBLE_ITRS_TO_TIRS = 0,    ///< use for wobble() to change from ITRS to TIRS.
  WOBBLE_TIRS_TO_ITRS         ///< to use for wobble() to change from TIRS to ITRS. (You can use any non-zero value as well.)
};

/**
 * Direction constant to use for frame_tie(), to determine the direction of transformation between J2000 and
 * ICRS coordinates.
 *
 * @sa frame_tie()
 * @sa TIE_J2000_TO_ICRS
 */
enum novas_frametie_direction {
  TIE_ICRS_TO_J2000 = -1, ///< Change coordinates from ICRS to the J2000 (dynamical) frame. (You can also use any negative value for the same effect).
  TIE_J2000_TO_ICRS    ///< Change coordinates from J2000 (dynamical) frame to the ICRS. (You can use any value &gt;=0 for the same effect).
};

/**
 * Direction constant for nutation(), between mean and true equatorial coordinates.
 *
 */
enum novas_nutation_direction {
  NUTATE_TRUE_TO_MEAN = -1, ///< Change from true equator to mean equator (i.e. undo wobble corrections). You may use any non-zero value as well.
  NUTATE_MEAN_TO_TRUE       ///< Change from mean equator to true equator (i.e. apply wobble corrections)
};

/**
 * Fundamental arguments of the Sun and Moon, from Simon section 3.4(b.3), precession = 5028.8200 arcsec/cy)
 */
typedef struct {
  double l;           ///< [rad] mean anomaly of the Moon
  double l1;          ///< [rad] mean anomaly of the Sun
  double F;           ///< [rad] mean argument of the latitude of the Moon
  double D;           ///< [rad] mean elongation of the Moon from the Sun
  double Omega;       ///< [rad] mean longitude of the Moon's ascending node.
} novas_fundamental_args;

#define SIZE_OF_OBJ_NAME 51     ///< Maximum bytes in object names including string termination.
#define SIZE_OF_CAT_NAME 4      ///< Maximum bytes in catalof IDs including string termination.

/**
 * Basic astrometric data for any celestial object
 * located outside the solar system; the catalog
 * data for a star
 *
 */
typedef struct {
  char starname[SIZE_OF_OBJ_NAME];  ///< name of celestial object
  char catalog[SIZE_OF_CAT_NAME];   ///< catalog designator (e.g., HIP)
  long starnumber;              ///< integer identifier assigned to object
  double ra;                        ///< [h] ICRS right ascension (hours)
  double dec;                       ///< [deg] ICRS declination (degrees)
  double promora;                   ///< [mas/yr] ICRS proper motion in right ascension (milliarcseconds/year)
  double promodec;                  ///< [mas/yr] ICRS proper motion in declination (milliarcseconds/year)
  double parallax;                  ///< [mas] parallax (milliarcseconds)
  double radialvelocity;            ///< [km/s] radial velocity (km/s)
} cat_entry;

/**
 * Celestial object of interest
 */
typedef struct {
  enum novas_object_type type;    ///< NOVAS object type
  long number;                ///< enum novas_planet, or minor planet ID (e.g. NAIF), or star catalog ID.
  char name[SIZE_OF_OBJ_NAME];    ///< name of the object (0-terminated)
  cat_entry star;                 ///< basic astrometric data for any celestial objecr located outside the solar system, such as a star
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
  double longitude;     ///< [deg] geodetic (ITRS) longitude; east positive (degrees)
  double height;        ///< [m] altitude above sea level (meters)
  double temperature;   ///< [C] temperature (degrees Celsius); for optical refraction
  double pressure;      ///< [mbar] atmospheric pressure (millibars); for optical refraction
} on_surface;

/**
 * data for an observer's location on Earth orbit
 *
 */
typedef struct {
  double sc_pos[3];     ///< [km] geocentric position vector (x, y, z), component in km
  double sc_vel[3];     ///< [km] geocentric velocity vector (x_dot, y_dot, z_dot), components in km/s
} in_space;

/**
 * Observer location (somewhere around Earth).
 *
 */
typedef struct {
  enum novas_observer_place where;    ///< observer location type

  /// structure containing data for an observer's location on the surface of the Earth
  /// (if where = NOVAS_OBSERVER_ON_EARTH)
  on_surface on_surf;

  /// data for an observer's location on Earth orbit.
  /// (if where = NOVAS_OBSERVER_IN_SPACE)
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
 * Right ascension of the Celestial Intermediate Origin (CIO) with respect to the GCRS
 *
 */
typedef struct {
  double jd_tdb;    ///< [day] Barycentric Dynamical Time (TDB) based Julian date.
  double ra_cio;    ///< [arcsec] right ascension of the CIO with respect to the GCRS (arcseconds)
} ra_of_cio;

/**
 * Fully defines the astronomical frame for which coordinates (including velocities) are calculated.
 *
 * @author Attila Kovacs
 * @since 1.0
 */
typedef struct {
  enum novas_reference_system basis_system;     ///< Coordindate system type.
  enum novas_origin origin;                     ///< Location of origin (if type is NOVAS_ICRS)
  observer location;                            ///< Location of observer (if type is not NOVAS_ICRS)
  double jd_tdb;                                ///< [day] Barycentric Dynamical Time (TDB) based Julian date of observation.
  double ut1_to_tt;                            ///< [s] TT - UT1 time difference (if observer is on the surface of Earth, otherwise ignored.
} astro_frame;

/**
 * Macro for converting epoch year to TT-based Julian date
 *
 * @param epoch           [yr] Astronomical epoch, e.g. 1950.0, or 2000.0, or 2024.08736
 * @return                [day] Terrestrial Time (TT) based Julian date
 */
#define EPOCH_TO_JD(epoch)                      (JD2000 + (epoch - 2000.0) * JULIAN_YEAR_DAYS )

/**
 * Initializer for astro_frame for Barycentric Celestrial Reference System (BCRS)
 *
 */
#define FRAME_INIT_BCRS                         { NOVAS_ICRS, NOVAS_BARICENTRIC }

/**
 * Initializer for astro_frame for International Celestrial Reference System (ICRS), with its origin at the
 * center of the Sun
 *
 */
#define FRAME_INIT_ICRS                         { NOVAS_ICRS, NOVAS_HELIOCDENTRIC }

/**
 * Initializer for astro_frame for Geocentric Celestrial Reference System (ICRS), with its origin at the
 * center of Earth
 *
 */
#define FRAME_INIT_GCRS                         { NOVAS_GCRS }

/**
 * Initializer for astro_frame for the mean dynamical equator at a given epoch
 *
 * @param epoch           [yr] Astronomical epoch, e.g. 1950.0, or 2000.0, or 2024.08736
 */
#define FRAME_INIT_CIRS(epoch)                  { NOVAS_CIRS, -1, NOVAS_OBSERVER_AT_GEOCENTER, EPOCH_TO_JD(epoch) }

/**
 * Initializer for astro_frame for True equinox Of Date (TOD)system at a given epoch
 *
 * @param epoch           [yr] Astronomical epoch, e.g. 1950.0, or 2000.0, or 2024.08736
 */
#define FRAME_INIT_TOD(epoch)                   { NOVAS_TOD, -1, NOVAS_OBSERVER_AT_GEOCENTER, EPOCH_TO_JD(epoch) }

/**
 * Initializer for astro_frame for a topocentric frame at a given location in Earth's rotating frame
 * and at a specific instant of time.
 *
 * @param jd_tt           [day] Precise Terrestrial Time (TT) based Julian date of observation
 * @param surface_loc     Pointer to an on_surface structure defining the observer location on Earth
 */
#define FRAME_INIT_APPARENT_TOPO(jd_tt, surface_loc)        { NOVAS_CIRS, -1, { NOVAS_OBSERVER_ON_SURFACE, surface_loc}, jd_tt + tt2tdb(jd_tt) / 86400.0 }

/**
 * Initializer for astro_frame for an Earth orbit frame.
 *
 * @param jd_tdb          [day] Precise Barycentric Dynamical Time (TDB) based Julian date of observation
 * @param space_loc       Pointer to an in_space structure defining the observer location in Earth orbit
 */
#define FRAME_INIT_APPARENT_NEAR_EARTH(jd_tdb, space_loc)    { NOVAS_CIRS, -1, { NOVAS_OBSERVER_IN_SPACE, space_loc}, jd_tdb }

short app_star(double jd_tt, const cat_entry *star, enum novas_accuracy accuracy, double *ra, double *dec);

short virtual_star(double jd_tt, const cat_entry *star, enum novas_accuracy accuracy, double *ra, double *dec);

short astro_star(double jd_tt, const cat_entry *star, enum novas_accuracy accuracy, double *ra, double *dec);

short app_planet(double jd_tt, const object *ss_body, enum novas_accuracy accuracy, double *ra, double *dec, double *dis);

short virtual_planet(double jd_tt, const object *ss_body, enum novas_accuracy accuracy, double *ra, double *dec, double *dis);

short astro_planet(double jd_tt, const object *ss_body, enum novas_accuracy accuracy, double *ra, double *dec, double *dis);

short topo_star(double jd_tt, double ut1_to_tt, const cat_entry *star, const on_surface *position, enum novas_accuracy accuracy, double *ra,
        double *dec);

short local_star(double jd_tt, double ut1_to_tt, const cat_entry *star, const on_surface *position, enum novas_accuracy accuracy,
        double *ra, double *dec);

short topo_planet(double jd_tt, const object *ss_body, double ut1_to_tt, const on_surface *position, enum novas_accuracy accuracy,
        double *ra, double *dec, double *dis);

short local_planet(double jd_tt, const object *ss_body, double ut1_to_tt, const on_surface *position, enum novas_accuracy accuracy,
        double *ra, double *dec, double *dis);

short mean_star(double jd_tt, double ra, double dec, enum novas_accuracy accuracy, double *ira, double *idec);

short place(double jd_tt, const object *cel_object, const observer *location, double ut1_to_tt, enum novas_reference_system coord_sys,
        enum novas_accuracy accuracy, sky_pos *output);

int equ2gal(double rai, double deci, double *glon, double *glat);

short equ2ecl(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double ra, double dec, double *elon,
        double *elat);

short equ2ecl_vec(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, const double *pos1, double *pos2);

short ecl2equ_vec(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, const double *pos1, double *pos2);

int equ2hor(double jd_ut1, double ut1_to_tt, enum novas_accuracy accuracy, double xp, double yp, const on_surface *location, double ra,
        double dec, short ref_option, double *zd, double *az, double *rar, double *decr);

short gcrs2equ(double jd_tt, enum novas_equator_type coord_sys, enum novas_accuracy accuracy, double rag, double decg, double *ra,
        double *dec);

short sidereal_time(double jd_high, double jd_low, double ut1_to_tt, enum novas_equinox_type gst_type,
        enum novas_earth_rotation_measure method, enum novas_accuracy accuracy, double *gst);

double era(double jd_high, double jd_low);

short ter2cel(double jd_ut_high, double jd_ut_low, double ut1_to_tt, enum novas_earth_rotation_measure method, enum novas_accuracy accuracy,
        enum novas_reference_system option, double xp, double yp, const double *vec1, double *vec2);

short cel2ter(double jd_ut_high, double jd_ut_low, double ut1_to_tt, enum novas_earth_rotation_measure method, enum novas_accuracy accuracy,
        enum novas_reference_system option, double xp, double yp, const double *vec1, double *vec2);

int spin(double angle, const double *pos1, double *pos2);

int wobble(double tjd, enum novas_wobble_direction direction, double xp, double yp, const double *pos1, double *pos2);

int terra(const on_surface *location, double lst, double *pos, double *vel);

int e_tilt(double jd_tdb, enum novas_accuracy accuracy, double *mobl, double *tobl, double *ee, double *dpsi, double *deps);

short cel_pole(double tjd, enum novas_pole_offset_type type, double dpole1, double dpole2);

double ee_ct(double jd_high, double jd_low, enum novas_accuracy accuracy);

int frame_tie(const double *pos1, enum novas_frametie_direction direction, double *pos2);

int proper_motion(double jd_tdb1, const double *pos, const double *vel, double jd_tdb2, double *pos2);

int bary2obs(const double *pos, const double *pos_obs, double *pos2, double *lighttime);

short geo_posvel(double jd_tt, double ut1_to_tt, enum novas_accuracy accuracy, const observer *obs, double *pos, double *vel);

short light_time(double jd_tdb, const object *ss_object, const double *pos_obs, double tlight0, enum novas_accuracy accuracy, double *pos,
        double *tlight);

double d_light(const double *pos1, const double *pos_obs);

short grav_def(double jd_tdb, enum novas_observer_place loc_code, enum novas_accuracy accuracy, const double *pos1, const double *pos_obs,
        double *pos2);

int grav_vec(const double *pos1, const double *pos_obs, const double *pos_body, double rmass, double *pos2);

int aberration(const double *pos, const double *ve, double lighttime, double *pos2);

int rad_vel(const object *cel_object, const double *pos, const double *vel, const double *vel_obs, double d_obs_geo, double d_obs_sun,
        double d_obj_sun, double *rv);

short precession(double jd_tdb1, const double *pos1, double jd_tdb2, double *pos2);

int nutation(double jd_tdb, enum novas_nutation_direction direction, enum novas_accuracy accuracy, const double *pos, double *pos2);

int nutation_angles(double t, enum novas_accuracy accuracy, double *dpsi, double *deps);

int fund_args(double t, novas_fundamental_args *a);

double planet_lon(double t, enum novas_planet planet);

double accum_prec(double t);

double mean_obliq(double jd_tdb);

short vector2radec(const double *pos, double *ra, double *dec);

int radec2vector(double ra, double dec, double dist, double *vector);

int starvectors(const cat_entry *star, double *pos, double *vel);

double ut1_to_tt(int leap_seconds, double dut1);

int tdb2tt(double tdb_jd, double *tt_jd, double *secdiff);

double tt2tdb(double jd_tt);

short cio_ra(double jd_tt, enum novas_accuracy accuracy, double *ra_cio);

short cio_location(double jd_tdb, enum novas_accuracy accuracy, double *ra_cio, short *ref_sys);

short cio_basis(double jd_tdb, double ra_cio, enum novas_cio_location_type ref_sys, enum novas_accuracy accuracy, double *x, double *y,
        double *z);

short cio_array(double jd_tdb, long n_pts, ra_of_cio *cio);

double ira_equinox(double jd_tdb, enum novas_equinox_type equinox, enum novas_accuracy accuracy);

short ephemeris(const double jd_tdb[2], const object *cel_obj, enum novas_origin origin, enum novas_accuracy accuracy, double *pos,
        double *vel);

int transform_hip(const cat_entry *hipparcos, cat_entry *hip_2000);

short transform_cat(enum novas_transform_type, double date_incat, const cat_entry *incat, double date_newcat, const char *newcat_id,
        cat_entry *newcat);

int limb_angle(const double *pos_obj, const double *pos_obs, double *limb_ang, double *nadir_ang);

double refract(const on_surface *location, enum novas_refraction_model ref_option, double zd_obs);

double julian_date(short year, short month, short day, double hour);

int cal_date(double tjd, short *year, short *month, short *day, double *hour);

double norm_ang(double angle);

short make_cat_entry(const char *star_name, const char *catalog, long star_num, double ra, double dec, double pm_ra, double pm_dec,
        double parallax, double rad_vel, cat_entry *star);

short make_object(enum novas_object_type, int number, const char *name, const cat_entry *star_data, object *cel_obj);

short make_observer(enum novas_observer_place, const on_surface *obs_surface, const in_space *obs_space, observer *obs);

int make_observer_at_geocenter(observer *obs_at_geocenter);

int make_observer_on_surface(double latitude, double longitude, double height, double temperature, double pressure,
        observer *obs_on_surface);

int make_observer_in_space(const double *sc_pos, const double *sc_vel, observer *obs_in_space);

int make_on_surface(double latitude, double longitude, double height, double temperature, double pressure, on_surface *obs_surface);

int make_in_space(const double *sc_pos, const double *sc_vel, in_space *obs_space);

// Added API in SuperNOVAS
int calc_pos(const object *source, const observer *obs, double jd_tt, double ut1_to_tt, enum novas_reference_system system,
        enum novas_accuracy accuracy, sky_pos *pos);

int calc_frame_pos(const object *source, const astro_frame *frame, enum novas_accuracy accuracy, sky_pos *pos);

int calc_star_pos(const cat_entry *star, const observer *obs, double jd_tt, double ut1_to_tt, enum novas_reference_system system,
        enum novas_accuracy accuracy, sky_pos *pos);

int calc_icrs_pos(const object *source, double jd_tt, enum novas_accuracy accuracy, sky_pos *pos);

int calc_gcrs_pos(const object *source, double jd_tt, enum novas_accuracy accuracy, sky_pos *pos);

int calc_dynamical_pos(const object *source, double jd_tt, enum novas_accuracy accuracy, sky_pos *pos);

int cio_set_locator_file(const char *filename);

int nutation_set_lp(novas_nutate_func f);

#include "solarsystem.h"

#endif /* _NOVAS_ */
