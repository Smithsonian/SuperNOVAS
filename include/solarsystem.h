/**
 * @file
 *
 * SuperNOVAS Solar-system related types, definitions, and functions (included in @ref novas.h).
 *
 * Solar-system objects come in multiple varieties from the perspective of SuperNOVAS:
 *
 *  1. Major planets, plus the Sun, Moon, Solar-system Barycenter (SSB), and since v1.2 also
 *     the Earth-Moon Barycenter (EMB), and the Pluto system barycenter.
 *
 *  2. Ephemeris objects, which historically have been all sources other than the major planets
 *     above. NOVAS treated these apart from the major planets, and so we follow.
 *
 *  3. Keplerian orbital elements (since v1.2), which can be used as an alternative to
 *     ephemerides. With short term projections, using up-to-date orbital elements, such as
 *     published by the IAU Minor Planet Center, one may calculate positions for asteroids,
 *     comets, and near-Earth objects (NEOs) with similar accuracy as with ephemerides. However,
 *     long-term application, orbital elements are typically much less accurate.
 *
 * Accordingly, this header defines various functions for the above types of Solar-system objects.
 *
 * High-precision calculations of Solar-system sources typically require access to precise
 * ephemeris data. Users may configure SuperNOVAS with different ephemeris provider functions for
 * the major planets and other ephemeris object, respectively (see `novas_planet_provider`,
 * `novas_planet_provider_hp`, and `novas_ephem_provider` types). These providers may be selected
 * and changed at runtime, including custom user supplied implementations. One may also select a
 * default provider at build time, if needded, for better legacy support for old NOVAS C
 * applications without runtime selection (our default is to rely on the self-contained @ref
 * solsys3.c, which calculates approximate positions for the Earth and Sun only). We however
 * recommend you stick to runtime configuration only, if possible.
 *
 * Besides the high-precision position and velocity calculations, you can do a lot more with
 * Solar-system sources also. For example, you can calculate the Moon's phase, or the angle
 * between an observed source and the Sun or Moon (often a consideration in deciding whether to
 * observe a source), or calculate the solar illumination fraction for planets (especially the
 * inner ones), or the typical incident solar power on the illuminated side. And, even without a
 * high-precison ephemeris provider, you may still calculate the position of the major planets
 * with arcminute level accuracy also.
 *
 * @author Attila Kovacs and G. Kaplan
 */

#ifndef _SOLSYS_
#define _SOLSYS_

/// \cond PRIVATE
#if __STDC_VERSION__ < 199901L
#  define restrict                        ///< No 'restrict' keyword prior to C99
#endif
/// \endcond


/// [W/m<sup>2</sup>] The Solar Constant i.e., typical incident Solar power on Earth.
/// The value of 1367 Wm<sup>−2</sup> was adopted by the World Radiation Center
/// (Gueymard, 2004).
/// @since 1.3
/// @sa novas_solar_power()
#define NOVAS_SOLAR_CONSTANT      1367.0

/**
 * String array initializer for Major planet names, matching the enum novas_planet. E.g.
 *
 * ```c
 * const char *planet_names[] = NOVAS_PLANET_NAMES_INIT;
 * ```
 *
 * @hideinitializer
 * @since 1.2
 * @author Attila Kovacs
 *
 * @sa enum novas_planet
 */
#define NOVAS_PLANET_NAMES_INIT { \
  "SSB", "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune", "Pluto", \
  "Sun", "Moon", "EMB", "Pluto-Barycenter" }

/**
 * Array initializer for mean planet radii in meters, matching the enum novas_planet. E.g.
 *
 * ```c
 * const double *planet_radii[] = NOVAS_PLANET_RADII_INIT;
 * ```
 *
 * REFERENCES:
 * <ol>
 * <li>https://orbital-mechanics.space/reference/planetary-parameters.html, Table 3.</li>
 * <li>B. A. Archinal, et al.,
 * Report of the IAU Working Group on Cartographic Coordinates and Rotational Elements:
 * 2015. Celestial Mechanics and Dynamical Astronomy, 130(3):22, March 2018.
 * doi:10.1007/s10569-017-9805-5.</li>
 * <li>B. A. Archinal, et al.,
 * Report of the IAU Working Group on Cartographic Coordinates and Rotational Elements:
 * 2015. Celestial Mechanics and Dynamical Astronomy, 130(3):22, March 2018.
 * doi:10.1007/s10569-017-9805-5.
 * </ol>
 *
 * @hideinitializer
 * @since 1.5
 * @author Attila Kovacs
 *
 * @sa enum novas_planet, NOVAS_PLANET_NAMES_INIT, NOVAS_RMASS_INIT
 */
#define NOVAS_PLANET_RADII_INIT { \
  0.0, \
  2440530.0, 6051800.0, 6378136.6, 3396190.0, 71492000.0, 60268000.0, 25559000.0, 24764000.0, 1188300.0, \
  695700000.0, \
  1737400.0, \
  0.0, 0.0 \
}

/**
 * Reciprocal masses of solar system bodies, from DE-405 (Sun mass / body mass).
 * [0]: Earth/Moon barycenter (legacy from NOVAS C), MASS[1] = Mercury, ...,
 * [9]: Pluto (barycenter), [10]: Sun, [11]: Moon.
 * Barycentric reciprocal masses (index 12, 13) are not set.
 *
 * NOTES:
 * <ol>
 * <li>The values have been updated to used DE440 data (Park et al. 2021) in 1.4.
 * </ol>
 *
 * REFERENCES:
 * <ol>
 * <li>Ryan S. Park et al 2021 AJ 161 105, DOI 10.3847/1538-3881/abd414</li>
 * <li>IERS Conventions, Chapter 3, Table 3.1</li>
 * </ol>
 *
 * @hideinitializer
 * @since 1.2
 * @author Attila Kovacs
 *
 * @sa enum novas_planet, NOVAS_PLANET_NAMES_INIT, NOVAS_PLANET_RAII_INIT, NOVAS_PLANETS_GRAV_Z_INIT
 */
#define NOVAS_RMASS_INIT  { \
      328900.559708565, \
      6023657.9450387, 408523.718656268, 332946.048773067, 3098703.54671961, \
      1047.348631244, 3497.9018007932, 22902.9507834766, 19412.2597758766, 136045556.16738, \
      1.0, 27068702.9548773, \
      0.0, 0.0 }

/**
 * Gravitational redshifts for major planets (and Moon and Sun) for light emitted at surface
 * and detected at a large distance away. Barycenters are not considered, and for Pluto the
 * redshift for the Pluto system is assumed for distant observers.
 *
 * @sa enum novas_planet
 * @sa NOVAS_PLANETS
 *
 * @hideinitializer
 * @since 1.1.1
 * @author Attila Kovacs
 */
#define NOVAS_PLANET_GRAV_Z_INIT { \
  0.0, 1.0047e-10, 5.9724e-10, 7.3050e-10, 1.4058e-10, 2.0166e-8, 7.2491e-9, 2.5420e-9, \
  3.0893e-9, 9.1338e-12, 2.120483e-6, 3.1397e-11, 0.0, 0.0 }


/**
 * Solar-system body IDs to use as object.number with @ref NOVAS_EPHEM_OBJECT types. JPL ephemerides
 * use <a href="https://naif.jpl.nasa.gov/pub/naif/toolkit_docs/C/req/naif_ids.html">NAIF IDs</a>
 * to identify objects in the Solar-system, which is thus the most widely adopted convention for
 * numbering Solar-system bodies. But other numbering systems also exists, for example the
 * CALCEPH library uses its own convention for the numbering of asteroids.
 *
 * @sa object, NOVAS_EPHEM_OBJECT, NOVAS_ID_TYPES
 *
 * @author Attila Kovacs
 * @since 1.2
 */
enum novas_id_type {
  NOVAS_ID_NAIF = 0,  ///< If the ephemeris provider should use NAIF IDs
  NOVAS_ID_CALCEPH    ///< If the ephemeris provider should use CALCEPH IDs
};

/**
 * Number of different Solar-system body ID types enumerated
 *
 * @hideinitializer
 * @author Attila Kovacs
 * @since 1.2
 *
 * @sa enum novas_id_type
 */
#define NOVAS_ID_TYPES      (NOVAS_ID_CALCEPH + 1)

 /**
  * Default set of gravitating bodies to use for deflection calculations in reduced accuracy mode.
  * (only apply gravitational deflection for the Sun.)
  *
  * @since 1.1
  * @author Attila Kovacs
  *
  * @sa grav_bodies_reduced_accuracy, grav_planets(), grav_undo_planets()
  */
 #define DEFAULT_GRAV_BODIES_REDUCED_ACCURACY   ( (1 << NOVAS_SUN) )

 /**
  * Default set of gravitating bodies to use for deflection calculations in full accuracy mode.
  *
  * @since 1.1
  * @author Attila Kovacs
  *
  * @sa grav_bodies_full_accuracy, grav_planets(), grav_undo_planets()
  */
 #define DEFAULT_GRAV_BODIES_FULL_ACCURACY      ( DEFAULT_GRAV_BODIES_REDUCED_ACCURACY | (1 << NOVAS_JUPITER) | (1 << NOVAS_SATURN) )

 /**
  * Current set of gravitating bodies to use for deflection calculations in reduced accuracy mode. Each
  * bit signifies whether a given body is to be accounted for as a gravitating body that bends light,
  * such as the bit `(1 << NOVAS_JUPITER)` indicates whether or not Jupiter is considered as a deflecting
  * body. You should also be sure that you provide ephemeris data for bodies that are designated for the
  * deflection calculation.
  *
  * @since 1.1
  *
  * @sa grav_def(), grav_planets(), DEFAULT_GRAV_BODIES_REDUCED_ACCURACY
  * @sa set_ephem_provider()
  */
 extern int grav_bodies_reduced_accuracy;

 /**
  * Current set of gravitating bodies to use for deflection calculations in full accuracy mode. Each
  * bit signifies whether a given body is to be accounted for as a gravitating body that bends light,
  * such as the bit `(1 << NOVAS_JUPITER)` indicates whether or not Jupiter is considered as a deflecting
  * body. You should also be sure that you provide ephemeris data for bodies that are designated for the
  * deflection calculation.
  *
  * @since 1.1
  *
  * @sa grav_def(), grav_planets(), DEFAULT_GRAV_BODIES_FULL_ACCURACY
  * @sa set_ephem_provider_hp()
  */
 extern int grav_bodies_full_accuracy;


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
 * @sa set_planet_provider(), ephemeris(), novas_solarsystem_hp_func
 * @sa make_planet(), novas_sky_pos(), novas_geom_posvel()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
typedef short (*novas_planet_provider)(double jd_tdb, enum novas_planet body, enum novas_origin origin,
        double *restrict position, double *restrict velocity);


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
 * @sa set_planet_provider_hp(), novas_solarsystem_func, ephemeris()
 * @sa make_planet(), novas_sky_pos(), novas_geom_posvel(), grav_planets(), grav_undo_planets()
 * @since 1.0
 * @author Attila Kovacs
 */
typedef short (*novas_planet_provider_hp)(const double jd_tdb[2], enum novas_planet body, enum novas_origin origin,
        double *restrict position, double *restrict velocity);


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
 * @sa set_ephem_provider(), ephemeris(), NOVAS_EPHEM_OBJECT, solsys-ephem.c
 * @sa make_ephem_object(), novas_sky_pos(), novas_geom_posvel()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
typedef int (*novas_ephem_provider)(const char *name, long id, double jd_tdb_high, double jd_tdb_low,
        enum novas_origin *restrict origin, double *restrict pos, double *restrict vel);



#ifndef _EXCLUDE_DEPRECATED
/**
 * @deprecated This old ephemeris reader is prone to memory leaks, and lacks some useful
 *             functionality. Users are strongly encouraged to use the new
 *             `novas_ephem_provider` / `novas_set_ephem_provider()` instead, for dynamically
 *             configured implementations at runtime. This prototype is provided only to extend
 *             support for legacy NOVAS applications only, where an inplementation had to be
 *             linked always.
 *
 * Legacy NOVAS C function to handle position and velocity calculations for Solar-system sources,
 * beyond the major planets. This function can be defined by an external module, as per the NOVAS
 * C way, provided you compile SuperNOVAS with the `READPH_SOURCE` option set to specify the
 * source(s) that implement it (in `config.mk` or the environment). If `READEPH_SOURCE` in not
 * defined during the build of SuperNOVAS, then this function will not be used by __SuperNOVAS__,
 * nor will be implemented by it.
 *
 * @param mp            The ID number of the solar-system body for which the position are
 *                      desired. An actual implementation might use this and/or the name to
 *                      identify the object.
 * @param name          The name of the solar-system body (usually upper-case). An actual
 *                      implementation might use this and/or `mp` to identify the object.
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
 *
 */
double *readeph(int mp, const char *restrict name, double jd_tdb, int *restrict error);
#endif

int set_planet_provider(novas_planet_provider func);

int set_planet_provider_hp(novas_planet_provider_hp func);

int set_ephem_provider(novas_ephem_provider func);

novas_ephem_provider get_ephem_provider();

#ifndef _EXCLUDE_DEPRECATED
/**
 * @deprecated (<i>legacy function</i>) Use `set_planet_provider()` instead to specify what
 *             function should be used to calculate ephemeris positions for major planets.
 *
 * Legacy NOVAS C function for obtaining planet position / velocity data.  This function can be
 * defined by external modules, as per the NOVAS C way, provided you compile SuperNOVAS with the
 * `SOLSYS_SOURCE` option set to specify the source(s) that implement it (in `config.mk` or the
 * environment). If `SOLSYS_SOURCE` in not defined during the build of SuperNOVAS, then this
 * function will be provided by the currently configured `novas_planet_provider` function, such as
 * `earth_sun_calc()` if not configured otherwise.
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
 * @sa novas_planet, solarsystem_hp(), set_planet_provider(), ephemeris()
 * @sa novas_sky_pos(), novas_geom_posvel()
 */
short solarsystem(double jd_tdb, short body, short origin, double *restrict position, double *restrict velocity);
#endif

#ifndef _EXCLUDE_DEPRECATED
/**
 * @deprecated (<i>legacy function</i>) Use `set_planet_provider_hp()` instead to specify what
 *             function should be used to calculate high-precision ephemeris positions for major
 *             planets.
 *
 * Legacy NOVAS C function for obtaining high-precision planet position / velocity data. This
 * function can be defined by external modules, as per the NOVAS C way, provided you compile
 * SuperNOVAS with the `SOLSYS_SOURCE` option set to specify the source(s) that implement it (in
 * `config.mk` or the environment). If `SOLSYS_SOURCE` in not defined during the build of
 * SuperNOVAS, then this function will be provided by the currently configured
 * `novas_planet_provider_hp` function, such as `earth_sun_calc()_hp` if not configured otherwise.
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
 *                      consistent here...
 *
 * @sa solarsystem(), set_planet_provider_hp(), ephemeris()
 * @sa novas_sky_pos(), novas_geom_posvel(), grav_planets(), grav_undo_planets()
 */
short solarsystem_hp(const double jd_tdb[restrict 2], short body, short origin, double *restrict position, double *restrict velocity);
#endif


short earth_sun_calc(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *restrict position,
        double *restrict velocity);

short earth_sun_calc_hp(const double jd_tdb[restrict 2], enum novas_planet body, enum novas_origin origin,
        double *restrict position, double *restrict velocity);

void enable_earth_sun_hp(int value);

short planet_ephem_provider(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *restrict position,
        double *restrict velocity);

short planet_ephem_provider_hp(const double jd_tdb[restrict 2], enum novas_planet body, enum novas_origin origin,
        double *restrict position, double *restrict velocity);

short ephemeris(const double *restrict jd_tdb, const object *restrict body, enum novas_origin origin,
        enum novas_accuracy accuracy, double *restrict pos, double *restrict vel);



// Added in v1.2 --------------------------------->

novas_planet_provider get_planet_provider();

novas_planet_provider_hp get_planet_provider_hp();

// in naif.c
enum novas_planet naif_to_novas_planet(long id);

long novas_to_naif_planet(enum novas_planet id);

long novas_to_dexxx_planet(enum novas_planet id);

enum novas_planet novas_planet_for_name(const char *restrict name);


// Added in v1.3 --------------------------------->

// in orbital.c
int novas_set_orbsys_pole(enum novas_reference_system type, double ra, double dec, novas_orbital_system *restrict sys);

int make_orbital_object(const char *name, long num, const novas_orbital *orbit, object *body);

int novas_orbit_posvel(double jd_tdb, const novas_orbital *restrict orbit, enum novas_accuracy accuracy,
        double *restrict pos, double *restrict vel);

double novas_helio_dist(double jd_tdb, const object *restrict source, double *restrict rate);

double novas_solar_power(double jd_tdb, const object *restrict source);

double novas_solar_illum(const object *restrict source, const novas_frame *restrict frame);

double novas_sun_angle(const object *restrict source, const novas_frame *restrict frame);

double novas_moon_angle(const object *restrict source, const novas_frame *restrict frame);


// Added in v1.4 --------------------------------->

// in orbit.c
int novas_orbit_native_posvel(double jd_tdb, const novas_orbital *restrict orbit, double *restrict pos, double *restrict vel);

// in planets.c
int novas_make_planet_orbit(enum novas_planet id, double jd_tdb, novas_orbital *restrict orbit);

int novas_make_moon_orbit(double jd_tdb, novas_orbital *restrict orbit);

int novas_approx_heliocentric(enum novas_planet id, double jd_tdb, double *restrict pos, double *restrict vel);

int novas_approx_sky_pos(enum novas_planet id, const novas_frame *restrict frame, enum novas_reference_system sys, sky_pos *restrict out);

double novas_moon_phase(double jd_tdb);

double novas_next_moon_phase(double phase, double jd_tdb);


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


#  endif /* __NOVAS_INTERNAL_API__ */
/// \endcond

#endif /* _SOLSYS_ */
