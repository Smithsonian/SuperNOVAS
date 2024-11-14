/**
 * @file
 *
 * @author G. Kaplan and A. Kovacs
 *
 *  SuperNOVAS header for custom solar-system ephemeris calculations for major planets plus
 *  the Sun, Moon, and the Solar-system barycenter (and as of v1.2 also the Earth-Moon
 *  Barycenter and the barycenter of the Pluto system).
 *
 *  The source files solsys-calceph.c and solsys-cspice.c provide implementations that
 *  interface with the CALCEPH C library and the NAIF CSPICE Toolkit, respectively. CSPICE is
 *  the canocical library for handling JPL (SPK) ephemeris data, while CALCEPH is a more
 *  modern tool, which allows handling most types of JPL ephemerides, as well INPOP 2.0/3.0
 *  format data files.
 *
 *  The source files solsys1.c, solsys2.c, solsys3.c and solsys-ephem.c provide various legacy
 *  implementations that users may use (some require additional sources, or user-specific
 *  implementations).
 *
 *  If the standard implementations are compiled with the DEFAULT_SOLSYS option set (see
 *  `config.mk`), then the library is compiled with that version providing a built-in default
 *  implementation (the default is to use `solsys3.c`, which is a self-contained orbital
 *  calculation for the Sun and Earth only).
 *
 *  Additionally, users may set their custom choice of major planet ephemeris handler at
 *  runtime via the set_planet_provider() and/or set_planet_provider_hp() functions. They may
 *  also define custom handlers for all other types of Solar-system objects (i.e.
 *  `NOVAS_EPHEM_OBJECT` types) via set_ephem_provider().
 *
 *  Based on the NOVAS C Edition, Version 3.1:
 *
 *  U. S. Naval Observatory<br>
 *  Astronomical Applications Dept.<br>
 *  Washington, DC<br>
 *  <a href="http://www.usno.navy.mil/USNO/astronomical-applications">
 *  http://www.usno.navy.mil/USNO/astronomical-applications</a>
 *
 *  @sa solsys-calceph.c
 *  @sa solsys-cspice.c
 *  @sa solsys1.c
 *  @sa solsys2.c
 *  @sa solsys3.c
 *  @sa solsys-ephem.c
 */

#ifndef _SOLSYS_
#define _SOLSYS_

#include <novas.h>

/**
 * Solar-system body IDs to use as object.number with NOVAS_EPHEM_OBJECT types. JPL ephemerides
 * use <a href="https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/naif_ids.html">NAIF IDs</a>
 * to identify objects in the Solar-system, which is thus the most widely adopted convention for
 * numbering Solar-system bodies. But other numbering systems also exists, for example the
 * CALCEPH library uses its own convention for the numbering of asteroids.
 *
 * @sa object
 * @sa NOVAS_EPHEM_OBJECT
 */
enum novas_id_type {
  NOVAS_ID_NAIF = 0,  ///< If the ephemeris provider should use NAIF IDs
  NOVAS_ID_CALCEPH    ///< If the ephemeris provider should use CALCEPH IDs
};

/// Number of different Solar-system body ID types enumerated
#define NOVAS_ID_TYPES      (NOVAS_ID_CALCEPH + 1)

/**
 * Provides the position and velocity of major planets (as well as the Sun, Moon, Solar-system
 * Barycenter, and other barycenters). This version provides positions and velocities at regular
 * precision (see NOVAS_REDUCED_PRECISION).
 *
 * Since this is a function that may be provided by existing custom user implementations, we
 * keep the original argument types for compatibility, hence 'short' instead of the more
 * informative enums).
 *
 * @param jd_tdb        [day] Barycentric Dynamical Time (TDB) based Julian date
 * @param body          Major planet number (or that for the Sun, Moon, or an appropriate
 *                      barycenter), as defined by enum novas_planet, e.g. NOVAS_MARS
 *                      (4), NOVAS_SUN (10) or NOVAS_SSB (0).
 * @param origin        NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1) relative to which to
 *                      return positions and velocities. (For compatibility with existing NOVAS
 *                      C compatible user implementations, we keep the original NOVAS C argument
 *                      type here).
 * @param[out] position [AU] Position vector of 'body' at 'tjd'; equatorial rectangular
 *                      coordinates in AU referred to the mean equator and equinox of J2000.0.
 * @param[out] velocity [AU/day] Velocity vector of 'body' at 'tjd'; equatorial rectangular
 *                      system referred to the mean equator and equinox of J2000.0, in AU/Day.
 * @return              0 if successful, -1 if there is a required function is not provided
 *                      (errno set to ENOSYS), 1 if the input Julian date ('tjd') is out of
 *                      range, 2 if 'body' is invalid, or 3 if the ephemeris data cannot be
 *                      produced for other reasons.
 *
 * @sa set_planet_provider()
 * @sa ephemeris()
 * @sa novas_solarsystem_hp_func
 */
typedef short (*novas_planet_provider)(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position, double *velocity);


/**
 * Provides the position and velocity of major planets (as well as the Sun, Moon, Solar-system
 * Barycenter, and other barycenters). This version provides positions and velocities at high
 * precision (see NOVAS_FULL_PRECISION).
 *
 * Since this is a function that may be provided by existing custom user implementations, we
 * keep the original argument types for compatibility, hence 'short' instead of the more
 * informative enums).
 *
 * @param jd_tdb        [day] Barycentric Dynamical Time (TDB) based Julian date, broken into
 *                      high and low order components, respectively. Typically, as the integer
 *                      and fractional parts for the highest precision.
 * @param body          Major planet number (or that for the Sun, Moon, or an appropriate
 *                      barycenter), as defined by enum novas_planet, e.g. NOVAS_MARS
 *                      (4), NOVAS_SUN (10) or NOVAS_SSB (0).
 * @param origin        NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1) relative to which to
 *                      return positions and velocities. (For compatibility with existing NOVAS
 *                      C compatible user implementations, we keep the original NOVAS C argument
 *                      type here).
 * @param[out] position [AU] Position vector of 'body' at 'tjd'; equatorial rectangular
 *                      coordinates in AU referred to the mean equator and equinox of J2000.0.
 * @param[out] velocity [AU/day] Velocity vector of 'body' at 'tjd'; equatorial rectangular
 *                      system referred to the mean equator and equinox of J2000.0, in AU/Day.
 * @return              0 if successful, -1 if there is a required function is not provided
 *                      (errno set to ENOSYS), 1 if the input Julian date ('tjd') is out of
 *                      range, 2 if 'body' is invalid, or 3 if the ephemeris data cannot be
 *                      produced for other reasons.
 *
 * @sa set_planet_provider_hp()
 * @sa novas_solarsystem_func
 * @sa ephemeris()
 */
typedef short (*novas_planet_provider_hp)(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin, double *position, double *velocity);



/**
 * Function to obtain ephemeris data for minor planets, which are not handled by the
 * solarsystem() type calls. The library does not provide a default implementation, but users
 * can provide their own, either as a default statically compiled readeph() implementation,
 * or else a dynamically defined one via ephemeris_set_reader().
 *
 * Note, that implementations would typically use either the name or the ID argument
 * to identify the object for which ephemeris data is requested. As such you only need
 * to specify the one that is going to be used.
 *
 * @param name          The name of the solar-system body (in case the ephemeris provider
 *                      supports lookup by name), or NULL to force ID based lookup.
 * @param id            The ID number of the solar-system body for which the position in
 *                      desired. (Typically a NAIF ID, or else an appropriate ID for the
 *                      implementation -- corresponding minor planet objects should be created
 *                      with the same type of ID.). A value of -1 can be used to force name
 *                      based lookups (provided the implementation supports it).
 * @param jd_tdb_high   [day] The high-order part of Barycentric Dynamical Time (TDB) based
 *                      Julian date for which to find the position and velocity. Typically
 *                      this may be the integer part of the Julian date for high-precision
 *                      calculations, or else the entire Julian date for reduced precision.
 * @param jd_tdb_low    [day] The low-order part of Barycentric Dynamical Time (TDB) based
 *                      Julian date for which to find the position and velocity. Typically
 *                      this may be the fractional part of the Julian date for high-precision
 *                      calculations, or else 0.0 if the date is defined entirely by the
 *                      high-order component for reduced precision.
 * @param[out] origin   Set to NOVAS_BARYCENTER or NOVAS_HELIOCENTER to indicate relative to
 *                      which the ephemeris positions/velocities are reported.
 * @param[out] pos      [AU] position 3-vector to populate with rectangular equatorial
 *                      coordinates in AU. It may be NULL if position is not required.
 * @param[out] vel      [AU/day] velocity 3-vector to populate in rectangular equatorial
 *                      coordinates in AU/day. It may be NULL if velocities are not required.
 * @return              0 if successful, -1 if any of the pointer arguments are NULL, or some
 *                      non-zero value if the was an error s.t. the position and velocity
 *                      vector should not be used.
 *
 * @sa set_ephem_provider()
 * @sa ephemeris()
 * @sa NOVAS_EPHEM_OBJECT
 * @sa solsys-ephem.c
 *
 * @since 1.0
 * @author Attila Kovacs
 */
typedef int (*novas_ephem_provider)(const char *name, long id, double jd_tdb_high, double jd_tdb_low, enum novas_origin *origin, double *pos, double *vel);



/**
 * Provides a default ephemeris implementation to handle position and velocity calculations
 * for minor planets, which are not handled by the solarsystem() type calls. The library does
 * not provide a default implementation, but users can provide their own, either as a default
 * statically compiled readeph() implementation, or else a dynamically defined one via
 * ephemeris_set_reader().
 *
 * You can set the built-in implementation for the library by setting the DEFAULT_READEPH
 * variable in the Makefile.
 *
 * @deprecated This old ephemeris reader is prone to memory leaks, and lacks some useful
 *             functionality. Users are strongly encouraged to use the new
 *             `novas_ephem_provider` instead, which can provide dynamically configured
 *             implementations at runtime.
 *
 * @param mp            The ID number of the solar-system body for which the position are
 *                      desired.
 * @param name          The name of the solar-system body (usually upper-case)
 * @param jd_tdb        [day] Barycentric Dynamical Time (TDB) based Julian date for which to
 *                      find the position and velocity.
 * @param[out] error    Pointer to integer to populate with the error status: 0 if successful,
 *                      -1 if any of the pointer arguments are NULL, or some non-zero value
 *                      if the was an error s.t. the position and velocity vector should not
 *                      be used.
 * @return              [AU, AU/day] A newly allocated 6-vector in rectangular equatorial
 *                      coordinates, containing the heliocentric position coordinates in AU,
 *                      followed by hte heliocentric velocity components in AU/day. The caller
 *                      is responsible for calling free() on the returned value when it is no
 *                      longer needed.
 *
 * @sa novas_ephem_provider
 * @sa ephemeris()
 * @sa NOVAS_EPHEM_OBJECT
 *
 * @since 1.0
 * @author Attila Kovacs
 */
double *readeph(int mp, const char *name, double jd_tdb, int *error);


int set_planet_provider(novas_planet_provider func);

int set_planet_provider_hp(novas_planet_provider_hp func);

int set_ephem_provider(novas_ephem_provider func);

novas_ephem_provider get_ephem_provider();


/**
 * A default implementation for regular (reduced) precision handling of major planets, Sun,
 * Moon and the Solar-system barycenter. See DEFAULT_SOLSYS in Makefile to choose the
 * implementation that is built into with the library as a default. Applications can define
 * their own preferred implementations at runtime via set_planet_provider().
 *
 * Since this is a function that may be provided by existing custom user implementations, we
 * keep the original argument types for compatibility, hence 'short' instead of the more
 * informative enums).
 *
 * @param jd_tdb        [day] Barycentric Dynamical Time (TDB) based Julian date
 * @param body          Major planet number (or that for the Sun, Moon, or an appropriate
 *                      barycenter), as defined by enum novas_planet, e.g. NOVAS_MARS
 *                      (4), NOVAS_SUN (10) or NOVAS_SSB (0). (For compatibility
 *                      with existing NOVAS C compatible user implementations, we keep the
 *                      original NOVAS C argument type here).
 * @param origin        NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1) relative to which to
 *                      return positions and velocities. (For compatibility with existing NOVAS
 *                      C compatible user implementations, we keep the original NOVAS C argument
 *                      type here).
 * @param[out] position [AU] Position vector of 'body' at 'tjd'; equatorial rectangular
 *                      coordinates in AU referred to the mean equator and equinox of J2000.0.
 * @param[out] velocity [AU/day] Velocity vector of 'body' at 'tjd'; equatorial rectangular
 *                      system referred to the mean equator and equinox of J2000.0, in AU/Day.
 * @return              0 if successful, -1 if there is a required function is not provided
 *                      (errno set to ENOSYS), 1 if the input Julian date ('tjd') is out of
 *                      range, 2 if 'body' is invalid, or 3 if the ephemeris data cannot be
 *                      produced for other reasons.
 *
 * @sa novas_planet
 * @sa solarsystem_hp()
 * @sa set_planet_provider()
 * @sa place()
 * @sa ephemeris()
 */
short solarsystem(double jd_tdb, short body, short origin, double *position, double *velocity);


/**
 * A default implementation for high precision handling of major planets, Sun, Moon and the
 * Solar-system barycenter (and other barycenters). See DEFAULT_SOLSYS in Makefile to choose
 * the implementation that is built into the library as a default. Applications can define their
 * own preferred implementations at runtime via set_planet_provider_hp().
 *
 * Since this is a function that may be provided by existing custom user implementations, we
 * keep the original argument types for compatibility, hence 'short' instead of the more
 * informative enums).
 *
 *
 * @param jd_tdb        [day] Barycentric Dynamical Time (TDB) based Julian date, broken into
 *                      high and low order components, respectively. Typically, as the integer
 *                      and fractional parts for the highest precision.
 * @param body          Major planet number (or that for the Sun, Moon, or an appropriate
 *                      barycenter), as defined by enum novas_planet, e.g. NOVAS_MARS
 *                      (4), NOVAS_SUN (10) or NOVAS_SSB (0). (For compatibility with
 *                      existing NOVAS C compatible user implementations, we keep the original
 *                      NOVAS C argument type here).
 * @param origin        NOVAS_BARYCENTER (0) or NOVAS_HELIOCENTER (1) relative to which to
 *                      return positions and velocities. (For compatibility with existing NOVAS
 *                      C compatible user implementations, we keep the original NOVAS C argument
 *                      type here).
 * @param[out] position [AU] Position vector of 'body' at 'tjd'; equatorial rectangular
 *                      coordinates in AU referred to the mean equator and equinox of J2000.0.
 * @param[out] velocity [AU/day] Velocity vector of 'body' at 'tjd'; equatorial rectangular
 *                      system referred to the mean equator and equinox of J2000.0, in AU/Day.
 * @return              0 if successful, -1 if there is a required function is not provided
 *                      (errno set to ENOSYS), or some other error code (NOVAS C was not very
 *                      consistent here...)
 *
 * @sa solarsystem()
 * @sa set_planet_provider_hp()
 * @sa place()
 * @sa ephemeris()
 */
short solarsystem_hp(const double jd_tdb[2], short body, short origin, double *position, double *velocity);


short earth_sun_calc(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position, double *velocity);

short earth_sun_calc_hp(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin, double *position, double *velocity);

void enable_earth_sun_hp(int value);

short planet_ephem_provider(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position, double *velocity);

short planet_ephem_provider_hp(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin, double *position, double *velocity);

short planet_eph_manager(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position, double *velocity);

short planet_eph_manager_hp(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin, double *position, double *velocity);

short planet_jplint(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position, double *velocity);

short planet_jplint_hp(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin, double *position, double *velocity);


// Added in v1.2 --------------------------------->

novas_planet_provider get_planet_provider();

novas_planet_provider_hp get_planet_provider_hp();

// in naif.c
enum novas_planet naif_to_novas_planet(long id);

long novas_to_naif_planet(enum novas_planet id);

long novas_to_dexxx_planet(enum novas_planet id);

/// \cond PRIVATE


// <================= SuperNOVAS internals ======================>

#  ifdef __NOVAS_INTERNAL_API__

/// NAIF ID for the geocenter
#define NAIF_EARTH      399

/// NAIF ID for the Moon
#define NAIF_MOON       301

/// NAIF_ID for the Sun
#define NAIF_SUN        10

/// NAIF ID for the Solar-System Barycenter (SSB)
#define NAIF_SSB        0

/// NAIF ID for the Earth-Moon Barycenter (EMB)
#define NAIF_EMB        3

/// NAIF ID for the barycenter of the Pluto system
#define NAIF_PLUTO_BARYCENTER   9

/**
 * The function to use to provide planet ephemeris data.
 *
 * @sa set_planet_provider()
 * @sa planet_call_hp
 *
 * @since 1.0
 * @author Attila Kovacs
 */
extern novas_planet_provider planet_call;

/**
 * The default 'fallback' function to use to provide high-precision planet ephemeris data.
 *
 * @sa set_planet_provider_hp()
 * @sa planet_call
 *
 * @since 1.0
 * @author Attila Kovacs
 */
extern novas_planet_provider_hp planet_call_hp;

#  endif /* __NOVAS_INTERNAL_API__ */

/// \endcond

#endif /* _SOLSYS_ */
