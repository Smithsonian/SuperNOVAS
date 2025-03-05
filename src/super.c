/**
 * @file
 *
 * @date Created  on Aug 24, 2024
 * @author Attila Kovacs
 *
 *  Various SuperNOVAS only functions, which are not integral to the functionality of novas.c,
 *  and thus can live in a separate, more manageably sized, module.
 */

// We'll use gcc major version as a proxy for the glibc library to decide which feature macro to use.
// gcc 5.1 was released 2015-04-22...
#if defined(__GNUC__) && (__GNUC__ < 5)
#  define _BSD_SOURCE             ///< strcasecmp() feature macro for glibc <= 2.19
#else
#  define _DEFAULT_SOURCE         ///< strcasecmp() feature macro starting glibc 2.20 (2014-09-08)
#endif

#include <math.h>
#include <errno.h>
#include <string.h>
#include <strings.h>              // strcasecmp() / strncasecmp()
#include <ctype.h>                // toupper()

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"

#if __Lynx__ && __powerpc__
// strcasecmp() is not defined on PowerPC / LynxOS 3.1
int strcasecmp(const char *s1, const char *s2);
#endif

/// \endcond



/**
 * Computes the gravitationally undeflected position of an observed source position due to the
 * specified Solar-system bodies.
 *
 * REFERENCES:
 * <ol>
 * <li>Klioner, S. (2003), Astronomical Journal 125, 1580-1597, Section 6.</li>
 * </ol>
 *
 * @param pos_app     [AU] Apparent position 3-vector of observed object, with respect to origin at
 *                    observer (or the geocenter), referred to ICRS axes, components
 *                    in AU.
 * @param pos_obs     [AU] Position 3-vector of observer (or the geocenter), with respect to
 *                    origin at solar system barycenter, referred to ICRS axes,
 *                    components in AU.
 * @param planets     Apparent planet data containing positions and velocities for the major
 *                    gravitating bodies in the solar-system.
 * @param[out] out    [AU] Nominal position vector of observed object, with respect to origin at
 *                    observer (or the geocenter), referred to ICRS axes, without gravitational
 *                    deflection, components in AU. It can be the same vector as the input, but not
 *                    the same as pos_obs.
 * @return            0 if successful, -1 if any of the pointer arguments is NULL.
 *
 * @sa obs_planets()
 * @sa grav_planets()
 * @sa novas_app_to_geom()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int grav_undo_planets(const double *pos_app, const double *pos_obs, const novas_planet_bundle *restrict planets, double *out) {
  static const char *fn = "grav_undo_planets";

  const double tol = 1e-13;
  double pos_def[3] = {0}, pos0[3] = {0};
  double l;
  int i;

  if(!pos_app || !pos_obs)
    return novas_error(-1, EINVAL, fn, "NULL input 3-vector: pos_app=%p, pos_obs=%p", pos_app, pos_obs);

  if(!planets)
    return novas_error(-1, EINVAL, fn, "NULL input planet data");

  if(!out)
    return novas_error(-1, EINVAL, fn, "NULL output 3-vector: out=%p", out);

  l = novas_vlen(pos_app);
  if(l == 0.0) {
    if(out != pos_app)
      memcpy(out, pos_app, XYZ_VECTOR_SIZE);
    return 0;        // Source is same as observer. No deflection.
  }

  memcpy(pos0, pos_app, sizeof(pos0));

  for(i = 0; i < novas_inv_max_iter; i++) {
    int j;

    prop_error(fn, grav_planets(pos0, pos_obs, planets, pos_def), 0);

    if(novas_vdist(pos_def, pos_app) / l < tol) {
      memcpy(out, pos0, sizeof(pos0));
      return 0;
    }

    for(j = 3; --j >= 0;)
      pos0[j] -= pos_def[j] - pos_app[j];
  }

  return novas_error(-1, ECANCELED, fn, "failed to converge");
}

/**
 * Computes the gravitationally undeflected position of an observed source position due to the
 * major gravitating bodies in the solar system.  This function valid for an observed body within
 * the solar system as well as for a star.
 *
 * If 'accuracy' is set to zero (full accuracy), three bodies (Sun, Jupiter, and Saturn) are
 * used in the calculation.  If the reduced-accuracy option is set, only the Sun is used in
 * the calculation.  In both cases, if the observer is not at the geocenter, the deflection
 * due to the Earth is included.
 *
 * The number of bodies used at full and reduced accuracy can be set by making a change to
 * the code in this function as indicated in the comments.
 *
 * REFERENCES:
 * <ol>
 * <li>Klioner, S. (2003), Astronomical Journal 125, 1580-1597, Section 6.</li>
 * </ol>
 *
 * @param jd_tdb      [day] Barycentric Dynamical Time (TDB) based Julian date
 * @param accuracy    NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1)
 * @param pos_app     [AU] Apparent position 3-vector of observed object, with respect to origin at
 *                    observer (or the geocenter), referred to ICRS axes, components
 *                    in AU.
 * @param pos_obs     [AU] Position 3-vector of observer (or the geocenter), with respect to
 *                    origin at solar system barycenter, referred to ICRS axes,
 *                    components in AU.
 * @param[out] out    [AU] Nominal position vector of observed object, with respect to origin at
 *                    observer (or the geocenter), referred to ICRS axes, without gravitational
 *                    deflection, components in AU. It can be the same vector as the input, but not
 *                    the same as pos_obs.
 * @return            0 if successful, -1 if any of the pointer arguments is NULL (errno = EINVAL)
 *                    or if the result did not converge (errno = ECANCELED), or else an error from
 *                    obs_planets().
 *
 * @sa grav_def()
 * @sa novas_app_to_geom()
 * @sa set_planet_provider()
 * @sa set_planet_provider_hp()
 * @sa grav_bodies_full_accuracy
 * @sa grav_bodies_reduced_accuracy
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int grav_undef(double jd_tdb, enum novas_accuracy accuracy, const double *pos_app, const double *pos_obs, double *out) {
  static const char *fn = "grav_undef";

  novas_planet_bundle planets = {0};
  int pl_mask = (accuracy == NOVAS_FULL_ACCURACY) ? grav_bodies_full_accuracy : grav_bodies_reduced_accuracy;

  if(!pos_app || !out)
    return novas_error(-1, EINVAL, fn, "NULL source position 3-vector: pos_app=%p, out=%p", pos_app, out);

  prop_error(fn, obs_planets(jd_tdb, accuracy, pos_obs, pl_mask, &planets), 0);
  prop_error(fn, grav_undo_planets(pos_app, pos_obs, &planets, out), 0);
  return 0;
}

/**
 * Populates and object data structure with the data for a catalog source. The input source
 * must be defined with ICRS coordinates. To create objects with other types of coordiantes
 * use `make_cat_object_epoch()` instead.
 *
 * @param star          Pointer to structure to populate with the catalog data for a celestial
 *                      object located outside the solar system, specified with ICRS coordinates.
 * @param[out] source   Pointer to the celestial object data structure to be populated.
 * @return              0 if successful, or -1 if either argument is NULL, or else 5 if
 *                      'name' is too long.
 *
 * @sa make_cat_object_sys()
 * @sa make_cat_entry()
 * @sa make_planet()
 * @sa make_ephem_object()
 * @sa novas_geom_posvel()
 * @sa place()
 *
 * @since 1.1
 * @author Attila Kovacs
 *
 */
int make_cat_object(const cat_entry *star, object *source) {
  if(!star || !source)
    return novas_error(-1, EINVAL, "make_cat_object", "NULL parameter: star=%p, source=%p", star, source);
  make_object(NOVAS_CATALOG_OBJECT, star->starnumber, star->starname, star, source);
  return 0;
}

static int cat_to_icrs(cat_entry *restrict star, const char *restrict system) {
  if(strcasecmp(system, "ICRS") != 0) {
    double jd = novas_epoch(system);
    if(isnan(jd))
      return novas_trace("cat_to_icrs", -1, 0);

    if(jd != NOVAS_JD_J2000)
      transform_cat(CHANGE_EPOCH, jd, star, NOVAS_JD_J2000, NOVAS_SYSTEM_FK5, star);

    // Then convert J2000 coordinates to ICRS (also in place). Here the dates don't matter...
    transform_cat(CHANGE_J2000_TO_ICRS, 0.0, star, 0.0, NOVAS_SYSTEM_ICRS, star);
  }
  return 0;
}

/**
 * Populates and object data structure with the data for a catalog source for a given system of
 * catalog coordinates.
 *
 * @param star          Pointer to structure to populate with the catalog data for a celestial
 *                      object located outside the solar system.
 * @param system         Input catalog coordinate system epoch, e.g. "ICRS", "B1950.0", "J2000.0",
 *                      "FK4", "FK5", or "HIP". In general, any Besselian or Julian year epoch can
 *                      be used by year (e.g. "B1933.193" or "J2022.033"), or else the fixed value
 *                      listed. If 'B' or 'J' is ommitted in front of the epoch year, then Besselian
 *                      epochs are assumed prior to 1984.0.
 * @param[out] source   Pointer to the celestial object data structure to be populated with
 *                      the corresponding ICRS catalog coordinates, after appying proper-motion
 *                      and precession corrections as appropriate.
 * @return              0 if successful, or -1 if any argument is NULL or if the input 'system' is
 *                      invalid, or else 5 if 'name' is too long.
 *
 * @sa make_cat_object()
 * @sa make_redshifted_object_sys()
 * @sa novas_jd_for_epoch()
 * @sa make_cat_entry()
 * @sa place()
 * @sa NOVAS_SYSTEM_ICRS
 * @sa NOVAS_SYSTEM_HIP
 * @sa NOVAS_SYSTEM_J2000
 * @sa NOVAS_SYSTEM_B1950
 *
 * @since 1.3
 * @author Attila Kovacs
 */
int make_cat_object_sys(const cat_entry *star, const char *restrict system, object *source) {
  static const char *fn = "make_cat_object_epoch";

  if(!system)
    return novas_error(-1, EINVAL, fn, "coordinate system is NULL");

  prop_error(fn, make_cat_object(star, source), 0);
  prop_error(fn, cat_to_icrs(&source->star, system), 0);

  return 0;
}

/**
 * Sets a celestial object to be a Solar-system ephemeris body. Typically this would be used to define
 * minor planets, asteroids, comets and planetary satellites.
 *
 * @param name          Name of object. By default converted to upper-case, unless novas_case_sensitive()
 *                      was called with a non-zero argument. Max. SIZE_OF_OBJ_NAME long, including
 *                      termination. If the ephemeris provider uses names, then the name should match
 *                      those of the ephemeris provider -- otherwise it is not important.
 * @param num           Solar-system body ID number (e.g. NAIF). The number should match the needs of the
 *                      ephemeris provider used with NOVAS. (If the ephemeris provider is by name and not
 *                      ID number, then the number here is not important).
 * @param[out] body     Pointer to structure to populate.
 * @return              0 if successful, or else -1 if the 'body' pointer is NULL or the name
 *                      is too long.
 *
 *
 * @sa set_ephem_provider()
 * @sa make_planet()
 * @sa make_cat_entry()
 * @sa novas_geom_posvel()
 * @sa place()
 *
 * @since 1.0
 * @author Attila Kovacs
 */
int make_ephem_object(const char *name, long num, object *body) {
  prop_error("make_ephem_object", (make_object(NOVAS_EPHEM_OBJECT, num, name, NULL, body) ? -1 : 0), 0);
  return 0;
}


/**
 * Sets a celestial object to be a Solar-system orbital body. Typically this would be used to define
 * minor planets, asteroids, comets, or even planetary satellites.
 *
 * @param name          Name of object. It may be NULL if not relevant.
 * @param num           Solar-system body ID number (e.g. NAIF). It is not required and can be set e.g. to
 *                      -1 if not relevant to the caller.
 * @param orbit         The orbital parameters to adopt. The data will be copied, not referenced.
 * @param[out] body     Pointer to structure to populate.
 * @return              0 if successful, or else -1 if the 'orbit' or 'body' pointer is NULL or the name
 *                      is too long.
 *
 *
 * @sa novas_orbit_posvel()
 * @sa make_planet()
 * @sa make_ephem_object()
 * @sa novas_geom_posvel()
 * @sa place()
 *
 * @since 1.2
 * @author Attila Kovacs
 */
int make_orbital_object(const char *name, long num, const novas_orbital *orbit, object *body) {
  static const char *fn = "make_orbital_object";
  if(!orbit)
    return novas_error(-1, EINVAL, fn, "Input orbital elements is NULL");
  prop_error(fn, (make_object(NOVAS_ORBITAL_OBJECT, num, name, NULL, body) ? -1 : 0), 0);
  body->orbit = *orbit;
  return 0;
}

/**
 * Sets the orientation of an orbital system using the RA and DEC coordinates of the pole
 * of the Laplace (or else equatorial) plane relative to which the orbital elements are
 * defined. Orbital parameters of planetary satellites normally include the R.A. and
 * declination of the pole of the local Laplace plane in which the Keplerian orbital elements
 * are referenced.
 *
 * The system will become referenced to the equatorial plane, the relative obliquity is set
 * to (90&deg; - `dec`), while the argument of the ascending node ('Omega') is set to
 * (90&deg; + `ra`).
 *
 * NOTES:
 * <ol>
 * <li>You should not expect much precision from the long-range orbital approximations for
 * planetary satellites. For applications that require precision at any level, you should rely
 * on appropriate ephemerides, or else on up-to-date short-term orbital elements.</li>
 * </ol>
 *
 * @param type  Coordinate reference system in which `ra` and `dec` are defined (e.g. NOVAS_GCRS).
 * @param ra    [h] the R.A. of the pole of the oribtal reference plane.
 * @param dec   [deg] the declination of the pole of the oribtal reference plane.
 * @param[out]  sys   Orbital system
 * @return      0 if successful, or else -1 (errno will be set to EINVAL) if the output `sys`
 *              pointer is NULL.
 *
 * @author Attila Kovacs
 * @since 1.2
 *
 * @sa make_orbital_object()
 */
int novas_set_orbsys_pole(enum novas_reference_system type, double ra, double dec, novas_orbital_system *restrict sys) {
  if (!sys)
    return novas_error(-1, EINVAL, "novas_set_orbsys_pole", "input system is NULL");

  sys->plane = NOVAS_EQUATORIAL_PLANE;
  sys->type = type;
  sys->obl = remainder(90.0 - dec, DEG360);
  sys->Omega = remainder(15.0 * ra + 90.0, DEG360);

  return 0;
}

/**
 * Populates a celestial object data structure with the parameters for a redhifted catalog
 * source, such as a distant quasar or galaxy. It is similar to `make_cat_object()` except
 * that it takes a Doppler-shift (z) instead of radial velocity and it assumes no parallax
 * and no proper motion (appropriately for a distant redshifted source). The catalog name
 * is set to `EXT` to indicate an extragalactic source, and the catalog number defaults to 0.
 * The user may change these default field values as appropriate afterwards, if necessary.
 *
 * @param name        Object name (less than SIZE_OF_OBJ_NAME in length). It may be NULL.
 * @param ra          [h] Right ascension of the object (hours).
 * @param dec         [deg] Declination of the object (degrees).
 * @param z           Redhift value (&lambda;<sub>obs</sub> / &lambda;<sub>rest</sub> - 1 =
 *                    f<sub>rest</sub> / f<sub>obs</sub> - 1).
 * @param[out] source Pointer to structure to populate.
 * @return            0 if successful, or 5 if 'name' is too long, else -1 if the 'source'
 *                    pointer is NULL.
 *
 * @sa make_redshifted_object_sys()
 * @sa novas_v2z()
 *
 * @since 1.2
 * @author Attila Kovacs
 */
int make_redshifted_cat_entry(const char *name, double ra, double dec, double z, cat_entry *source) {
  static const char *fn = "make_redshifted_cat_entry";

  double v =  novas_z2v(z);

  if(isnan(v))
    return novas_error(-1, EINVAL, fn, "invalid redshift value: %f\n", z);

  prop_error(fn, make_cat_entry(name, "EXT", 0, ra, dec, 0.0, 0.0, 0.0, v, source), 0);
  return 0;
}

/**
 * Populates a celestial object data structure with the parameters for a redhifted catalog
 * source, such as a distant quasar or galaxy. It is similar to `make_cat_object()` except
 * that it takes a Doppler-shift (z) instead of radial velocity and it assumes no parallax
 * and no proper motion (appropriately for a distant redshifted source). The catalog name
 * is set to `EXT` to indicate an extragalactic source, and the catalog number defaults to 0.
 * The user may change these default field values as appropriate afterwards, if necessary.
 *
 * @param name        Object name (less than SIZE_OF_OBJ_NAME in length). It may be NULL.
 * @param ra          [h] ICRS Right ascension of the object (hours).
 * @param dec         [deg] ICRS Declination of the object (degrees).
 * @param z           Redhift value (&lambda;<sub>obs</sub> / &lambda;<sub>rest</sub> - 1 =
 *                    f<sub>rest</sub> / f<sub>obs</sub> - 1).
 * @param[out] source Pointer to structure to populate.
 * @return            0 if successful, or 5 if 'name' is too long, else -1 if the 'source'
 *                    pointer is NULL.
 *
 * @sa make_redshifted_object_sys()
 * @sa make_cat_object()
 * @sa novas_v2z()
 *
 * @since 1.2
 * @author Attila Kovacs
 */
int make_redshifted_object(const char *name, double ra, double dec, double z, object *source) {
  static const char *fn = "make_redshifted_source";

  cat_entry c;

  prop_error(fn, make_redshifted_cat_entry(name, ra, dec, z, &c), 0);
  prop_error(fn, make_cat_object(&c, source), 0);
  return 0;
}

/**
 * Populates a celestial object data structure with the parameters for a redhifted catalog
 * source, such as a distant quasar or galaxy, for a given system of catalog coordinates.
 *
 * @param name          Object name (less than SIZE_OF_OBJ_NAME in length). It may be NULL.
 * @param ra            [h] ICRS Right ascension of the object (hours).
 * @param dec           [deg] ICRS Declination of the object (degrees).
 * @param system        Input catalog coordinate system epoch, e.g. "ICRS", "B1950.0", "J2000.0",
 *                      "FK4", "FK5", or "HIP". In general, any Besselian or Julian year epoch
 *                      can be used by year (e.g. "B1933.193" or "J2022.033"), or else the fixed
 *                      value listed. If 'B' or 'J' is ommitted in front of the epoch year, then
 *                      Besselian epochs are assumed prior to 1984.0.
 * @param z             Redhift value (&lambda;<sub>obs</sub> / &lambda;<sub>rest</sub> - 1 =
 *                      f<sub>rest</sub> / f<sub>obs</sub> - 1).
 * @param[out] source   Pointer to the celestial object data structure to be populated with
 *                      the corresponding ICRS catalog coordinates.
 * @return              0 if successful, or -1 if any of the pointer arguments is NULL or the
 *                      'system' is invalid, or else 1 if 'type' is invalid, 2 if 'number' is
 *                      out of legal range or 5 if 'name' is too long.
 *
 *
 * @sa make_redshifted_object()
 * @sa make_cat_object_sys()
 * @sa novas_jd_for_epoch()
 * @sa place()
 * @sa NOVAS_SYSTEM_ICRS
 * @sa NOVAS_SYSTEM_HIP
 * @sa NOVAS_SYSTEM_J2000
 * @sa NOVAS_SYSTEM_B1950
 *
 * @since 1.3
 * @author Attila Kovacs
 */
int make_redshifted_object_sys(const char *name, double ra, double dec, const char *restrict system, double z, object *source) {
  static const char *fn = "make_redshifted_object_epoch";

  if(!system)
    return novas_error(-1, EINVAL, fn, "coordinate system is NULL");

  prop_error(fn, make_redshifted_object(name, ra, dec, z, source), 0);
  prop_error(fn, cat_to_icrs(&source->star, system), 0);

  return 0;
}

/**
 * Populates an 'observer' data structure for an observer moving relative to the surface of Earth,
 * such as an airborne observer. Airborne observers have an earth fixed momentary location,
 * defined by longitude, latitude, and altitude, the same was as for a stationary observer on
 * Earth, but are moving relative to the surface, such as in an aircraft or balloon observatory.
 *
 * @param location    Current longitude, latitude and altitude, and local weather (temperature and pressure)
 * @param vel         [km/s] Surface velocity.
 * @param[out] obs    Pointer to data structure to populate.
 * @return            0 if successful, or -1 if the output argument is NULL.
 *
 * @sa make_observer_at geocenter()
 * @sa make_observer_in_space()
 * @sa make_observer_on_surface()
 * @sa make_solar_system_observer()
 * @sa novas_calc_geometric_position()
 * @sa place()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int make_airborne_observer(const on_surface *location, const double *vel, observer *obs) {
  in_space motion = IN_SPACE_INIT;

  if(!vel)
    return novas_error(-1, EINVAL, "make_airborne_observer", "NULL velocity");

  memcpy(motion.sc_vel, vel, sizeof(motion.sc_vel));

  prop_error("make_airborne_observer", make_observer(NOVAS_AIRBORNE_OBSERVER, location, &motion, obs), 0);
  return 0;
}

/**
 * Populates an 'observer' data structure, for an observer situated on a near-Earth spacecraft,
 * with the specified geocentric position and velocity vectors. Solar-system observers are similar
 * to observers in Earth-orbit but their momentary position and velocity is defined relative to
 * the Solar System Barycenter, instead of the geocenter.
 *
 * @param sc_pos        [AU] Solar-system barycentric (x, y, z) position vector in ICRS.
 * @param sc_vel        [AU/day] Solar-system barycentric (x, y, z) velocity vector in ICRS.
 * @param[out] obs      Pointer to the data structure to populate
 * @return          0 if successful, or -1 if the output argument is NULL.
 *
 * @sa make_observer_in_space()
 * @sa make_observer_on_surface()
 * @sa make_observer_at_geocenter()
 * @sa make_airborne_observer()
 * @sa novas_calc_geometric_position()
 * @sa place()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int make_solar_system_observer(const double *sc_pos, const double *sc_vel, observer *obs) {
  static const char *fn = "make_observer_in_space";
  in_space loc;
  prop_error(fn, make_in_space(sc_pos, sc_vel, &loc), 0);
  prop_error(fn, make_observer(NOVAS_SOLAR_SYSTEM_OBSERVER, NULL, &loc, obs), 0);
  return 0;
}

/**
 * Returns the NOVAS planet ID for a given name (case insensitive), or -1 if no match is found.
 *
 * @param name    The planet name, or that for the "Sun", "Moon" or "SSB" (case insensitive).
 *                The spelled out "Solar System Barycenter" is also recognized with either spaces,
 *                hyphens ('-') or underscores ('_') separating the case insensitive words.
 * @return        The NOVAS major planet ID, or -1 (errno set to EINVAL) if the input name is
 *                NULL or if there is no match for the name provided.
 *
 * @author Attila Kovacs
 * @since 1.2
 *
 * @sa make_planet()
 */
enum novas_planet novas_planet_for_name(const char *restrict name) {
  static const char *fn = "novas_planet_for_name()";
  static const char *names[] = NOVAS_PLANET_NAMES_INIT;

  char *tok;
  int i;

  if(!name)
    return novas_error(-1, EINVAL, fn, "Input name is NULL");

  if(!name[0])
    return novas_error(-1, EINVAL, fn, "Input name is empty");

  for(i = 0; i < NOVAS_PLANETS; i++)
    if(strcasecmp(name, (const char *) names[i]) == 0)
      return i;

  // Check for Solar System Barycenter (and variants)
  tok = strtok(strdup(name), " \t-_");
  if(strcasecmp("solar", tok) == 0) {
    tok = strtok(NULL, " \t-_");
    if(tok && strcasecmp("system", tok) == 0) {
      tok = strtok(NULL, " \t-_");
      if(tok && strcasecmp("barycenter", tok) == 0)
        return NOVAS_SSB;
    }
  }

  return novas_error(-1, EINVAL, fn, "No match for name: '%s'", name);
}



/**
 * Returns the horizontal Parallactic Angle (PA) calculated for a gorizontal Az/El location of the sky. The PA
 * is the angle between the local horizontal coordinate directions and the local true-of-date equatorial
 * coordinate directions at the given location. The polar wobble is not included in the calculation.
 *
 * The Parallactic Angle is sometimes referrred to as the Vertical Position Angle (VPA). Both define the
 * same quantity.
 *
 * @param az    [deg] Azimuth angle
 * @param el    [deg] Elevation angle
 * @param lat   [deg] Geodetic latitude of observer
 * @return      [deg] Parallactic Angle (PA). I.e., the clockwise position angle of the declination direction
 *              w.r.t. the elevation axis in the horizontal system. Same as the the clockwise position angle
 *              of the elevation direction w.r.t. the declination axis in the equatorial system.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_epa()
 * @sa novas_h2e_offset()
 */
double novas_hpa(double az, double el, double lat) {
  double s, c;

  lat *= DEGREE;
  az *= DEGREE;
  el *= DEGREE;

  s = sin(lat);
  c = cos(lat);

  return atan2(-c * sin(az), s * cos(el) - c * sin(el) * cos(az)) / DEGREE;
}

/**
 * Returns the equatorial Parallactic Angle (PA) calculated for an R.A./Dec location of the sky at a given
 * sidereal time. The PA is the angle between the local horizontal coordinate directions and the local
 * true-of-date equatorial coordinate directions, at the given location and time. The polar wobble is not
 * included in the calculation.
 *
 * The Parallactic Angle is sometimes referrred to as the Vertical Position Angle (VPA). Both define the
 * same quantity.
 *
 * @param ha      [h] Hour angle (LST - RA) i.e., the difference between the Local (apparent) Sidereal Time
 *                and the apparent (true-of-date) Right Ascension of observed source.
 * @param dec     [deg] Apparent (true-of-date) declination of observed source
 * @param lat     [deg] Geodetic latitude of observer
 * @return        [deg] Parallactic Angle (PA). I.e., the clockwise position angle of the elevation direction
 *                w.r.t. the declination axis in the equatorial system. Same as the clockwise position angle
 *                of the declination direction w.r.t. the elevation axis, in the horizontal system.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_hpa()
 * @sa novas_lst()
 * @sa novas_e2h_offset()
 */
double novas_epa(double ha, double dec, double lat) {
  double coslat;

  ha *= HOURANGLE;
  lat *= DEGREE;
  dec *= DEGREE;

  coslat = cos(lat);
  return atan2(coslat * sin(ha), sin(lat) * cos(dec) - coslat * sin(dec) * cos(ha)) / DEGREE;
}

/**
 * Converts coordinate offsets, from the local horizontal system to local equatorial offsets.
 * Converting between local flat projections and spherical coordinates usually requires a WCS
 * projection.
 *
 * REFERENCES:
 * <ol>
 * <li>Calabretta, M.R., & Greisen, E.W., (2002), Astronomy & Astrophysics, 395, 1077-1122.</li>
 * </ol>
 *
 * @param daz         [arcsec] Projected offset position in the azimuth direction. The projected
 *                    offset between two azimuth positions at the same reference elevation is
 *                    &delta;Az = (Az2 - Az1) * cos(El<sub>0</sub>).
 * @param del         [arcsec] projected offset position in the elevation direction
 * @param pa          [deg] Parallactic Angle
 * @param[out] dra    [arcsec] Output offset position in the local true-of-date R.A. direction. It
 *                    can be a pointer to one of the input coordinates, or NULL if not required.
 * @param[out] ddec   [arcsec] Output offset position in the local true-of-date declination
 *                    direction. It can be a pointer to one of the input coordinates, or NULL if not
 *                    required.
 * @return            0
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_e2h_offset()
 * @sa novas_hpa()
 */
int novas_h2e_offset(double daz, double del, double pa, double *restrict dra, double *restrict ddec) {
  double dx = daz, dy = del, c, s;

  pa *= DEGREE;
  c = cos(pa);
  s = sin(pa);

  if(dra)
    *dra =  s * dy - c * dx;
  if(ddec)
    *ddec = s * dx + c * dy;

  return 0;
}

/**
 * Converts coordinate offsets, from the local equatorial system to local horizontal offsets.
 * Converting between local flat projections and spherical coordinates usually requires a WCS
 * projection.
 *
 * REFERENCES:
 * <ol>
 * <li>Calabretta, M.R., & Greisen, E.W., (2002), Astronomy & Astrophysics, 395, 1077-1122.</li>
 * </ol>
 *
 * @param dra         [arcsec] Projected ffset position in the apparent true-of-date R.A. direction.
 *                    E.g. The projected offset between two RA coordinates at a same reference
 *                    declination, is
 *                    &delta;RA = (RA2 - RA1) * cos(Dec<sub>0</sub>)
 * @param ddec        [arcsec] Projected offset position in the apparent true-of-date declination
 *                    direction.
 * @param pa          [deg] Parallactic Angle
 * @param[out] daz    [arcsec] Output offset position in the local azimuth direction. It can be a pointer
 *                    to one of the input coordinates, or NULL if not required.
 * @param[out] del    [arcsec] Output offset position in the local elevation direction. It can be a
 *                    pointer to one of the input coordinates, or NULL if not required.
 * @return            0
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_h2e_offset()
 * @sa novas_epa()
 */
int novas_e2h_offset(double dra, double ddec, double pa, double *restrict daz, double *restrict del) {
  return novas_h2e_offset(dra, ddec, pa, daz, del);
}

/**
 * Returns a Solar-system body's distance from the Sun, and optionally also the rate of recession.
 * It may be useful, e.g. to calculate the body's heating from the Sun.
 *
 * @param jd_tdb      [day] Barycentric Dynamical Time (TDB) based Julian date. You may want to
 *                    use a time that is antedated to when the observed light originated from the
 *                    source.
 * @param source      Observed Solar-system source
 * @param[out] rate   [AU/day] (optional) Returned rate of recession from Sun
 * @return            [AU] Distance from the Sun, or NAN if not a Solar-system source.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_solar_power()
 * @sa novas_solar_illum()
 */
double novas_helio_dist(double jd_tdb, const object *restrict source, double *restrict rate) {
  static const char *fn = "novas_helio_dist";

  const double jd2[2] = { jd_tdb, 0.0 };
  double pos[3], vel[3], d;

  if(rate)
    *rate = NAN;

  if(!source) {
    novas_error(0, EINVAL, fn, "input source is NULL");
    return NAN;
  }

  if(source->type == NOVAS_CATALOG_OBJECT) {
    novas_error(0, EINVAL, fn, "input source is not a Solar-system body: type %d", source->type);
    return NAN;
  }

  if(ephemeris(jd2, source, NOVAS_HELIOCENTER, NOVAS_REDUCED_ACCURACY, pos, vel) != 0)
    return novas_trace_nan(fn);

  d = novas_vlen(pos);
  if(!d) {
    // The Sun itself...
    if(rate)
      *rate = 0.0;
    return 0.0;
  }

  if(rate)
    *rate = novas_vlen(vel);
  return d;

}

/**
 * Returns the typical incident Solar power on a Solar-system body at the time of observation.
 *
 * @param jd_tdb  [day] Barycentric Dynamical Time (TDB) based Julian date. You may want to
 *                use a time that is antedated to when the observed light originated (
 *                was reflected) from the source.
 * @param source  Observed Solar-system source
 * @return        [W/m<sup>2</sup>] Incident Solar power on the illuminated side of the object,
 *                or NAN if not a Solar-system source or if the source is the Sun itself.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_solar_illum()
 */
double novas_solar_power(double jd_tdb, const object *restrict source) {
  double d = novas_helio_dist(jd_tdb, source, NULL);
  return NOVAS_SOLAR_CONSTANT / (d * d);
}

/**
 * Returns the angular separation of two locations on a sphere.
 *
 * @param lon1    [deg] longitude of first location
 * @param lat1    [deg] latitude of first location
 * @param lon2    [deg] longitude of second location
 * @param lat2    [deg] latitude of second location
 * @return        [deg] the angular separation of the two locations.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_equ_sep()
 * @sa novas_sun_angle()
 * @sa novas_moon_angle()
 */
double novas_sep(double lon1, double lat1, double lon2, double lat2) {
  double c = sin(lat1 * DEGREE) * sin(lat2 * DEGREE) + cos(lat1 * DEGREE) * cos(lat2 * DEGREE) * cos((lon1 - lon2) * DEGREE);
  return atan2(sqrt(1.0 - c * c), c) / DEGREE;
}

/**
 * Returns the angular separation of two equatorial locations on a sphere.
 *
 * @param ra1     [h] right ascension of first location
 * @param dec1    [deg] declination of first location
 * @param ra2     [h] right ascension of second location
 * @param dec2    [deg] declination of second location
 * @return        [deg] the angular separation of the two locations.
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_sep()
 * @sa novas_sun_angle()
 * @sa novas_moon_angle()
 */
double novas_equ_sep(double ra1, double dec1, double ra2, double dec2) {
  return novas_sep(15.0 * ra1, dec1, 15.0 * ra2, dec2);
}

/**
 * Converts rectangular telescope x,y,z (absolute or relative) coordinates (in ITRS) to equatorial
 * u,v,w projected coordinates for a specified line of sight.
 *
 * x,y,z are Cartesian coordinates w.r.t the Greenwich meridian. The directions are x: long=0, lat=0;
 * y: long=90, lat=0; z: lat=90.
 *
 * u,v,w are Cartesian coordinates (u,v) along the local equatorial R.A. and declination directions as
 * seen from a direction on the sky (w).
 *
 * @param xyz           [arb.u.] Absolute or relative x,y,z coordinates (double[3]).
 * @param ha            [h] Hourangle (LST - RA) i.e., the difference between the Local
 *                      (apparent) Sidereal Time and the apparent (true-of-date) Right
 *                      Ascension of observed source.
 * @param dec           [deg] Apparent (true-of-date) declination of source
 * @param[out] uvw      [arb.u.] Converted u,v,w coordinates (double[3]) in same units as xyz.
 *                      It may be the same vector as the input.
 *
 * @return              0 if successful, or else -1 if either vector argument is NULL
 *                      (errno will be set to EINVAL)
 *
 * @since 1.3
 * @author Attila Kovacs
 */
int novas_xyz_to_uvw(const double *xyz, double ha, double dec, double *uvw) {
  prop_error("novas_xyz_to_uvw", novas_xyz_to_los(xyz, -15.0 * ha, dec, uvw), 0);
  return 0;
}

