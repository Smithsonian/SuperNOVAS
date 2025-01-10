/**
 * @file
 *
 * @date Created  on Jun 23, 2024
 * @author Attila Kovacs
 * @since 1.1
 *
 *  SuperNOVAS routines for higher-level and efficient repeat coordinate transformations using
 *  observer frames. Observer frames represent an observer location at a specific astronomical
 *  time (instant), which can be re-used again and again to calculate or transform positions of
 *  celestial sources in a a range of astronomical coordinate systems.
 *
 *  @sa timescale.c
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include "novas.h"

#define XI0       (-0.0166170 * ARCSEC)         ///< Frame bias term &xi;<sub>0</sub>
#define ETA0      (-0.0068192 * ARCSEC)         ///< Frame bias term &eta;<sub>0</sub>
#define DA0       (-0.01460 * ARCSEC)           ///< Frame bias term da<sub>0</sub>

/// \cond PRIVATE
#define FRAME_DEFAULT       0                   ///< frame.state value we set to indicate the frame is not configured
#define FRAME_INITIALIZED   0xdeadbeadcafeba5e  ///< frame.state for a properly initialized frame.
#define GEOM_TO_APP         1                   ///< Geometric to apparent conversion
#define APP_TO_GEOM         (-1)                ///< Apparent to geometric conversion
/// \endcond

static int cmp_sys(enum novas_reference_system a, enum novas_reference_system b) {
  // GCRS=0, TOD=1, CIRS=2, ICRS=3, J2000=4, MOD=5
  // TOD->-3, MOD->-2, J2000->-1, GCRS/ICRS->0, CIRS->1
  static const int index[] = { 0, -3, 1, 0, -1, -2 };

  if(a < 0 || a >= NOVAS_REFERENCE_SYSTEMS)
    return novas_error(-2, EINVAL, "cmp_sys", "Invalid reference system (#1): %d", a);

  if(b < 0 || b >= NOVAS_REFERENCE_SYSTEMS)
    return novas_error(-2, EINVAL, "cmp_sys", "Invalid reference system (#2): %d", b);

  if(index[a] == index[b])
    return 0;

  return index[a] < index[b] ? -1 : 1;
}

static int matrix_transform(const double *in, const novas_matrix *matrix, double *out) {
  double v[3];
  int i;

  memcpy(v, in, sizeof(v));

  for(i = 3; --i >= 0;)
    out[i] = (matrix->M[i][0] * v[0]) + (matrix->M[i][1] * v[1]) + (matrix->M[i][2] * v[2]);

  return 0;
}


static int matrix_inv_rotate(const double *in, const novas_matrix *matrix, double *out) {
  // IMPORTANT! use only with unitary matrices.
  double v[3];
  int i;

  memcpy(v, in, sizeof(v));

  for(i = 3; --i >= 0;)
    out[i] = (matrix->M[0][i] * v[0]) + (matrix->M[1][i] * v[1]) + (matrix->M[2][i] * v[2]);

  return 0;
}

static int invert_matrix(const novas_matrix *A, novas_matrix *I) {
  double idet;
  int i;

  I->M[0][0] = A->M[1][1] * A->M[2][2] - A->M[2][1] * A->M[1][2];
  I->M[1][0] = A->M[2][0] * A->M[1][2] - A->M[1][0] * A->M[2][2];
  I->M[2][0] = A->M[1][0] * A->M[2][1] - A->M[2][0] * A->M[1][1];

  I->M[0][1] = A->M[2][1] * A->M[0][2] - A->M[0][1] * A->M[2][2];
  I->M[1][1] = A->M[0][0] * A->M[2][2] - A->M[2][0] * A->M[0][2];
  I->M[2][1] = A->M[2][0] * A->M[0][1] - A->M[0][0] * A->M[2][1];

  I->M[0][2] = A->M[0][1] * A->M[1][2] - A->M[1][1] * A->M[0][2];
  I->M[1][2] = A->M[1][0] * A->M[0][2] - A->M[0][0] * A->M[1][2];
  I->M[2][2] = A->M[0][0] * A->M[1][1] - A->M[1][0] * A->M[0][1];

  idet = 1.0 / (A->M[0][0] * I->M[0][0] + A->M[0][1] * I->M[1][0] + A->M[0][2] * I->M[2][0]);

  for(i = 3; --i >= 0;) {
    I->M[i][0] *= idet;
    I->M[i][1] *= idet;
    I->M[i][2] *= idet;
  }

  return 0;
}

static int set_frame_tie(novas_frame *frame) {
  // 'xi0', 'eta0', and 'da0' are ICRS frame biases in arcseconds taken
  // from IERS (2003) Conventions, Chapter 5.
  static const double ax = ETA0;
  static const double ay = -XI0;
  static const double az = -DA0;
  static const double X = ax * ax, Y = ay * ay, Z = az * az;

  novas_matrix *T = &frame->icrs_to_j2000;

  T->M[0][0] = (1.0 - 0.5 * (Y + Z));
  T->M[0][1] = -az;
  T->M[0][2] = ay;

  T->M[1][0] = az;
  T->M[1][1] = (1.0 - 0.5 * (X + Z));
  T->M[1][2] = -ax;

  T->M[2][0] = -ay;
  T->M[2][1] = ax;
  T->M[2][2] = (1.0 - 0.5 * (X + Y));

  return 0;
}

static int set_gcrs_to_cirs(novas_frame *frame) {
  static const char *fn = "set_gcrs_to_cirs";
  const double jd_tdb = novas_get_time(&frame->time, NOVAS_TDB);
  double r_cio;
  short sys;

  novas_matrix *T = &frame->gcrs_to_cirs;

  prop_error(fn, cio_location(jd_tdb, frame->accuracy, &r_cio, &sys), 0);
  prop_error(fn, cio_basis(jd_tdb, r_cio, sys, frame->accuracy, &T->M[0][0], &T->M[1][0], &T->M[2][0]), 10);

  return 0;
}

static int set_precession(novas_frame *frame) {
  // 't' is time in TDB centuries between the two epochs.
  const double t = (novas_get_time(&frame->time, NOVAS_TDB) - NOVAS_JD_J2000) / JULIAN_CENTURY_DAYS;
  const double eps0 = 84381.406 * ARCSEC;

  // Numerical coefficients of psi_a, omega_a, and chi_a, along with
  // epsilon_0, the obliquity at J2000.0, are 4-angle formulation from
  // Capitaine et al. (2003), eqs. (4), (37), & (39).
  const double psia = ((((-0.0000000951 * t + 0.000132851) * t - 0.00114045) * t - 1.0790069) * t + 5038.481507) * t * ARCSEC;
  const double omegaa = ((((+0.0000003337 * t - 0.000000467) * t - 0.00772503) * t + 0.0512623) * t - 0.025754) * t * ARCSEC + eps0;
  const double chia = ((((-0.0000000560 * t + 0.000170663) * t - 0.00121197) * t - 2.3814292) * t + 10.556403) * ARCSEC * t;

  const double sa = sin(eps0);
  const double ca = cos(eps0);
  const double sb = sin(-psia);
  const double cb = cos(-psia);
  const double sc = sin(-omegaa);
  const double cc = cos(-omegaa);
  const double sd = sin(chia);
  const double cd = cos(chia);

  novas_matrix *T = &frame->precession;

  // Compute elements of precession rotation matrix equivalent to
  // R3(chi_a) R1(-omega_a) R3(-psi_a) R1(epsilon_0).
  T->M[0][0] = cd * cb - sb * sd * cc;
  T->M[0][1] = cd * sb * ca + sd * cc * cb * ca - sa * sd * sc;
  T->M[0][2] = cd * sb * sa + sd * cc * cb * sa + ca * sd * sc;

  T->M[1][0] = -sd * cb - sb * cd * cc;
  T->M[1][1] = -sd * sb * ca + cd * cc * cb * ca - sa * cd * sc;
  T->M[1][2] = -sd * sb * sa + cd * cc * cb * sa + ca * cd * sc;

  T->M[2][0] = sb * sc;
  T->M[2][1] = -sc * cb * ca - sa * cc;
  T->M[2][2] = -sc * cb * sa + cc * ca;

  return 0;
}

static int set_nutation(novas_frame *frame) {
  const double cm = cos(frame->mobl);
  const double sm = sin(frame->mobl);
  const double ct = cos(frame->tobl);
  const double st = sin(frame->tobl);
  const double cp = cos(frame->dpsi0);
  const double sp = sin(frame->dpsi0);

  novas_matrix *T = &frame->nutation;

  // Nutation rotation matrix follows.
  T->M[0][0] = cp;
  T->M[0][1] = -sp * cm;
  T->M[0][2] = -sp * sm;

  T->M[1][0] = sp * ct;
  T->M[1][1] = cp * cm * ct + sm * st;
  T->M[1][2] = cp * sm * ct - cm * st;

  T->M[2][0] = sp * st;
  T->M[2][1] = cp * cm * st - sm * ct;
  T->M[2][2] = cp * sm * st + cm * ct;

  return 0;
}

static int set_obs_posvel(novas_frame *frame) {
  int res = obs_posvel(novas_get_time(&frame->time, NOVAS_TDB), frame->time.ut1_to_tt, frame->accuracy, &frame->observer,
          frame->earth_pos, frame->earth_vel, frame->obs_pos, frame->obs_vel);

  frame->v_obs = novas_vlen(frame->obs_vel);
  frame->beta = frame->v_obs / C_AUDAY;
  frame->gamma = sqrt(1.0 - frame->beta * frame->beta);
  return res;
}


static int frame_aberration(const novas_frame *frame, int dir, double *pos) {
  const double pos0[3] = { pos[0], pos[1], pos[2] };
  double d;
  int i;

  if(frame->v_obs == 0.0)
    return 0;

  d = novas_vlen(pos);
  if(d == 0.0)
    return 0;

  // Iterate as necessary (for inverse only)
  for(i = 0; i < novas_inv_max_iter; i++) {
    const double p = frame->beta * novas_vdot(pos, frame->obs_vel) / (d * frame->v_obs);
    const double q = (1.0 + p / (1.0 + frame->gamma)) * d / C_AUDAY;
    const double r = 1.0 + p;

    if(dir < 0) {
      const double pos1[3] = { pos[0], pos[1], pos[2] };

      // Apparent to geometric
      pos[0] = (r * pos0[0] - q * frame->obs_vel[0]) / frame->gamma;
      pos[1] = (r * pos0[1] - q * frame->obs_vel[1]) / frame->gamma;
      pos[2] = (r * pos0[2] - q * frame->obs_vel[2]) / frame->gamma;

      // Iterate, since p, q, r are defined by unaberrated position.
      if(novas_vdist(pos, pos1) < 1e-13 * d)
        return 0;
    }
    else {
      // Geometric to apparent
      pos[0] = (frame->gamma * pos0[0] + q * frame->obs_vel[0]) / r;
      pos[1] = (frame->gamma * pos0[1] + q * frame->obs_vel[1]) / r;
      pos[2] = (frame->gamma * pos0[2] + q * frame->obs_vel[2]) / r;
      return 0; // No need to iterate...
    }
  }

  return novas_error(-1, ECANCELED, "frame_aberration", "failed to converge");
}


static int is_frame_initialized(const novas_frame *frame) {
  if(!frame) return 0;
  return frame->state == FRAME_INITIALIZED;
}

/**
 * Sets up a observing frame for a specific observer location, time of observation, and accuracy
 * requirement. The frame is initialized using the currently configured planet ephemeris provider
 * function (see set_planet_provider() and set_planet_provider_hp()), and in case of reduced
 * accuracy mode, the currently configured IAU nutation model provider (see
 * set_nutation_lp_provider()).
 *
 * Note, that to construct full accuracy frames, you will need a high-precision ephemeris provider
 * for the major planets (not just the default Earth/Sun), as without it, gravitational bending
 * around massive plannets cannot be accounted for, and therefore &mu;as accuracy cannot be ensured,
 * in general. Attempting to construct a high-accuracy frame without a high-precision ephemeris
 * provider for the major planets will result in an error in the 10--40 range from the required
 * ephemeris() call.
 *
 *
 * @param accuracy    Accuracy requirement, NOVAS_FULL_ACCURACY (0) for the utmost precision or
 *                    NOVAS_REDUCED_ACCURACY (1) if ~1 mas accuracy is sufficient.
 * @param obs         Observer location
 * @param time        Time of observation
 * @param dx          [mas] Earth orientation parameter, polar offset in x.
 * @param dy          [mas] Earth orientation parameter, polar offset in y.
 * @param[out] frame  Pointer to the observing frame to configure.
 * @return            0 if successful,
 *                    10--40: error is 10 + the error from ephemeris(),
 *                    40--50: error is 40 + the error from geo_posvel(),
 *                    50--80: error is 50 + the error from sidereal_time(),
 *                    80--90 error is 80 + error from cio_location(),
 *                    90--100 error is 90 + error from cio_basis().
 *                    or else -1 if there was an error (errno will indicate the
 *                    type of error).
 *
 * @sa novas_change_observer()
 * @sa novas_sky_pos()
 * @sa novas_geom_posvel()
 * @sa novas_make_transform()
 * @sa set_planet_provider()
 * @sa set_planet_provider_hp()
 * @sa set_nutation_lp_provider()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_make_frame(enum novas_accuracy accuracy, const observer *obs, const novas_timespec *time, double dx, double dy,
        novas_frame *frame) {
  static const char *fn = "novas_make_frame";
  static const object earth = NOVAS_EARTH_INIT;
  static const object sun = NOVAS_SUN_INIT;

  double tdb2[2];
  double mobl, tobl, ee, dpsi, deps;
  long ijd_ut1;
  double fjd_ut1;

  if(accuracy < 0 || accuracy > NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  if(!obs || !time)
    return novas_error(-1, EINVAL, fn, "NULL input parameter: obs=%p, time=%p", obs, time);

  if(!frame)
    return novas_error(-1, EINVAL, fn, "NULL output frame");

  if(obs->where < 0 || obs->where >= NOVAS_OBSERVER_PLACES)
    return novas_error(-1, EINVAL, fn, "invalid observer location: %d", obs->where);

  frame->accuracy = accuracy;
  frame->time = *time;

  tdb2[0] = time->ijd_tt;
  tdb2[1] = time->fjd_tt + tt2tdb(time->ijd_tt + time->fjd_tt) / DAY;

  // Various calculated quantities for frame transformations
  e_tilt(tdb2[0] + tdb2[1], frame->accuracy, &mobl, &tobl, &ee, NULL, NULL);

  frame->mobl = mobl * DEGREE;
  frame->tobl = tobl * DEGREE;
  frame->ee = ee * DEGREE;

  nutation_angles((tdb2[0] + tdb2[1] - NOVAS_JD_J2000) / JULIAN_CENTURY_DAYS, accuracy, &dpsi, &deps);

  frame->dpsi0 = dpsi * ARCSEC;
  frame->deps0 = deps * ARCSEC;
  frame->dx = dx;
  frame->dy = dy;

  fjd_ut1 = novas_get_split_time(time, NOVAS_UT1, &ijd_ut1);
  frame->era = era(ijd_ut1, fjd_ut1);

  prop_error(fn, sidereal_time(ijd_ut1, fjd_ut1, time->ut1_to_tt, NOVAS_TRUE_EQUINOX, EROT_GST, frame->accuracy, &frame->gst), 50);

  set_frame_tie(frame);
  set_precession(frame);
  set_nutation(frame);

  prop_error(fn, set_gcrs_to_cirs(frame), 80);

  // Barycentric Earth and Sun positions and velocities
  prop_error(fn, ephemeris(tdb2, &sun, NOVAS_BARYCENTER, accuracy, frame->sun_pos, frame->sun_vel), 10);
  prop_error(fn, ephemeris(tdb2, &earth, NOVAS_BARYCENTER, accuracy, frame->earth_pos, frame->earth_vel), 10);

  frame->state = FRAME_INITIALIZED;

  prop_error(fn, novas_change_observer(frame, obs, frame), 40);

  return 0;
}

/**
 * Change the observer location for an observing frame.
 *
 * @param orig        Pointer to original observing frame
 * @param obs         New observer location
 * @param[out] out    Observing frame to populate with a original frame data and new observer
 *                    location. It can be the same as the input.
 * @return            0 if successfule or else an an error code from geo_posvel()
 *                    (errno will also indicate the type of error).
 *
 * @sa novas_make_frame()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_change_observer(const novas_frame *orig, const observer *obs, novas_frame *out) {
  static const char *fn = "novas_change_observer";
  double jd_tdb;
  int pl_mask;

  if(!orig || !obs || !out)
    return novas_error(-1, EINVAL, fn, "NULL parameter: orig=%p, obs=%p, out=%p", orig, obs, out);

  if(!is_frame_initialized(orig))
    return novas_error(-1, EINVAL, fn, "frame at %p not initialized", out);

  if(out != orig)
    *out = *orig;

  out->state = FRAME_DEFAULT;
  out->observer = *obs;

  pl_mask = (out->accuracy == NOVAS_FULL_ACCURACY) ? grav_bodies_full_accuracy : grav_bodies_reduced_accuracy;

  prop_error(fn, set_obs_posvel(out), 0);

  jd_tdb = novas_get_time(&out->time, NOVAS_TDB);
  prop_error(fn, obs_planets(jd_tdb, out->accuracy, out->obs_pos, pl_mask, &out->planets), 0);

  out->state = FRAME_INITIALIZED;
  return 0;
}

static int icrs_to_sys(const novas_frame *frame, double *pos, enum novas_reference_system sys) {
  switch(sys) {
    case NOVAS_ICRS:
    case NOVAS_GCRS:
      return 0;

    case NOVAS_CIRS:
      matrix_transform(pos, &frame->gcrs_to_cirs, pos);
      return 0;

    case NOVAS_J2000:
    case NOVAS_MOD:
    case NOVAS_TOD:
      //gcrs_to_j2000(pos, pos);
      matrix_transform(pos, &frame->icrs_to_j2000, pos);
      if(sys == NOVAS_J2000) return 0;

      matrix_transform(pos, &frame->precession, pos);
      if(sys == NOVAS_MOD) return 0;

      matrix_transform(pos, &frame->nutation, pos);
      return 0;
  }

  return novas_error(-1, EINVAL, "icrs_to_sys", "invalid reference system: %d", sys);
}

/**
 * Calculates the geometric position and velocity vectors, relative to the observer, for a source
 * in the given observing frame, in the specified coordinate system of choice. The geometric
 * position includes proper motion, and for solar-system bodies it is antedated for light travel
 * time, so it effectively represents the geometric position as seen by the observer. However, the
 * geometric does not include aberration correction, nor gravitational deflection.
 *
 * If you want apparent positions, which account for aberration and gravitational deflection,
 * use novas_skypos() instead.
 *
 * You can also use novas_transform_vector() to convert the output position and velocity vectors
 * to a dfferent coordinate system of choice afterwards if you want the results expressed in
 * more than one coordinate system.
 *
 * It implements the same geometric transformations as `place()` but at a reduced computational
 * cost. See `place()` for references.
 *
 * NOTES:
 * <ol>
 * <li>If `sys` is `NOVAS_TOD` (true equator and equinox of date), the less precise old (pre IAU
 * 2006) method is used, with the Lieske et al. 1977 nutation model, matching the behavior of the
 * original NOVAS C place() for that system. To obtain more precise TOD coordinates, set `sys` to
 * `NOVAS_CIRS` here, and follow with cirs_to_tod() after.</li>
 * </ol>
 *
 * @param source        Pointer to a celestial source data structure that is observed
 * @param frame         Observer frame, defining the location and time of observation
 * @param sys           The coordinate system in which to return positions and velocities.
 * @param[out] pos      [AU] Calculated geometric position vector of the source relative
 *                      to the observer location, in the designated coordinate system. It may be
 *                      NULL if not required.
 * @param[out] vel      [AU/day] The calculated velocity vector of the source relative to
 *                      the observer in the designated coordinate system. It must be distinct from
 *                      the pos output vector, and may be NULL if not required.
 * @return              0 if successful, or else -1 if any of the arguments is invalid,
 *                      50--70 error is 50 + error from light_time2().
 *
 * @sa novas_geom_to_app()
 * @sa novas_sky_pos()
 * @sa novas_transform_vector()
 * @sa place()
 * @sa cirs_to_tod()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_geom_posvel(const object *source, const novas_frame *frame, enum novas_reference_system sys, double *pos, double *vel) {
  static const char *fn = "novas_geom_posvel";

  double jd_tdb, t_light;
  double pos1[3], vel1[3];

  if(!source || !frame)
    return novas_error(-1, EINVAL, fn, "NULL argument: source=%p, frame=%p", source, frame);

  if(pos == vel)
    return novas_error(-1, EINVAL, fn, "identical output pos and vel 3-vectors @ %p.", pos);

  if(!is_frame_initialized(frame))
    return novas_error(-1, EINVAL, fn, "frame at %p not initialized", frame);

  if(frame->accuracy != NOVAS_FULL_ACCURACY && frame->accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", frame->accuracy);
  // Compute 'jd_tdb', the TDB Julian date corresponding to 'jd_tt'.
  jd_tdb = novas_get_time(&frame->time, NOVAS_TDB);

  // ---------------------------------------------------------------------
  // Find geometric position of observed object (ICRS)
  // ---------------------------------------------------------------------
  if(source->type == NOVAS_CATALOG_OBJECT) {
    // Observed object is star.
    double dt = 0.0;

    // Get position of star updated for its space motion.
    starvectors(&source->star, pos1, vel1);

    dt = d_light(pos1, frame->obs_pos);
    proper_motion(NOVAS_JD_J2000, pos1, vel1, (jd_tdb + dt), pos1);

    // Get position of star wrt observer (corrected for parallax).
    bary2obs(pos1, frame->obs_pos, pos1, &t_light);
  }
  else {
    int got = 0;

    // If we readily have the requested planet data in the frame, use it.
    if(source->type == NOVAS_PLANET)
      if(frame->planets.mask & (1 << source->number)) {
        memcpy(pos1, &frame->planets.pos[source->number][0], sizeof(pos1));
        memcpy(vel1, &frame->planets.vel[source->number][0], sizeof(vel1));
        got = 1;
      }

    // Otherwise, get the position of body wrt observer, antedated for light-time.
    if(!got) {
      prop_error(fn, light_time2(jd_tdb, source, frame->obs_pos, 0.0, frame->accuracy, pos1, vel1, &t_light), 50);
    }
  }

  if(pos) {
    prop_error(fn, icrs_to_sys(frame, pos1, sys), 0);
    memcpy(pos, pos1, sizeof(pos1));
  }
  if(vel) {
    prop_error(fn, icrs_to_sys(frame, vel1, sys), 0);
    memcpy(vel, vel1, sizeof(vel1));
  }

  return 0;
}


/**
 * Calculates an apparent location on sky for the source. The position takes into account the
 * proper motion (for sidereal soure), or is antedated for light-travel time (for Solar-System
 * bodies). It also applies an appropriate aberration correction and gravitational deflection of
 * the light.
 *
 * To calculate corresponding local horizontal coordinates, you can pass the output RA/Dec
 * coordinates to novas_app_to_hor(). Or to calculate apparent coordinates in other systems,
 * you may pass the result to novas_transform_sy_pos() after.
 *
 * And if you want geometric positions instead (not corrected for aberration or gravitational
 * deflection), you may want to use novas_geom_posvel() instead.
 *
 * The approximate 'inverse' of this function is novas_app_to_geom().
 *
 * This function implements the same aberration and gravitational deflection corrections as
 * `place()`, but at reduced computational cost. See `place()` for references. Unlike `place()`,
 * however, the output always reports the distance calculated from the parallax for sidereal
 * sources. Note also, that while `place()` does not apply aberration and gravitational deflection
 * corrections when `sys` is NOVAS_ICRS (3), this routine will apply those corrections consistently
 * for all coordinate systems (and you can use novas_geom_posvel() instead to get positions without
 * aberration or deflection in any system).
 *
 * NOTES:
 * <ol>
 * <li>If `sys` is `NOVAS_TOD` (true equator and equinox of date), the less precise old (pre IAU
 * 2006) method is used, with the Lieske et al. 1977 nutation model, matching the behavior of the
 * original NOVAS C place() for that system. To obtain more precise TOD coordinates, set `sys` to
 * `NOVAS_CIRS` here, and follow with cirs_to_tod() / cirs_to_app_ra() on the `out->r_hat` /
 * `out->ra` respectively after (or you can use just convert one of the quantities, and use
 * radec2vector() or vector2radec() to get the other even faster).</li>
 * </ol>
 *
 * @param object        Pointer to a celestial object data structure that is observed
 * @param frame         The observer frame, defining the location and time of observation
 * @param sys           The coordinate system in which to return the apparent sky location
 * @param[out] out      Pointer to the data structure which is populated with the calculated
 *                      apparent location in the designated coordinate system.
 * @return              0 if successful,
 *                      50--70 error is 50 + error from light_time2(),
 *                      70--80 error is 70 + error from grav_def(),
 *                      or else -1 (errno will indicate the type of error).
 *
 * @sa novas_geom_to_app()
 * @sa novas_app_to_hor()
 * @sa place()
 * @sa cirs_to_tod()
 * @sa cirs_to_app_ra()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_sky_pos(const object *object, const novas_frame *frame, enum novas_reference_system sys, sky_pos *out) {
  static const char *fn = "novas_sky_pos";

  double d_sb, pos[3], vel[3], vpos[3];

  if(!object || !frame || !out)
    return novas_error(-1, EINVAL, "NULL argument: object=%p, frame=%p, out=%p", (void *) object, frame, out);

  if(!is_frame_initialized(frame))
    return novas_error(-1, EINVAL, fn, "frame at %p not initialized", frame);

  if(frame->accuracy != NOVAS_FULL_ACCURACY && frame->accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", frame->accuracy);

  prop_error(fn, novas_geom_posvel(object, frame, NOVAS_ICRS, pos, vel), 0);

  out->dis = novas_vlen(pos);

  // ---------------------------------------------------------------------
  // Compute radial velocity (all vectors in ICRS).
  // ---------------------------------------------------------------------
  if(object->type == NOVAS_CATALOG_OBJECT) {
    d_sb = out->dis;
  }
  else {
    int k;

    // Calculate distance to Sun.
    d_sb = 0.0;
    for(k = 3; --k >= 0;) {
      double d = frame->sun_pos[k] - (frame->obs_pos[k] + pos[k]);
      d_sb += d * d;
    }
    d_sb = sqrt(d_sb);
  }

  // ---------------------------------------------------------------------
  // Compute direction in which light is emitted from the source
  // ---------------------------------------------------------------------
  if(object->type == NOVAS_CATALOG_OBJECT) {
    // For sidereal sources the 'velocity' position is the same as the geometric position.
    memcpy(vpos, pos, sizeof(pos));
  }
  else {
    double psrc[3]; // Barycentric position of Solar-system source (antedated)
    int i;

    // A.K.: For this we calculate gravitational deflection of the observer seen from the source
    // i.e., reverse tracing the light to find the direction in which it was emitted.
    for(i = 3; --i >= 0;) {
      vpos[i] = -pos[i];
      psrc[i] = pos[i] + frame->obs_pos[i];
    }

    // vpos -> deflected direction in which observer is seen from source.
    prop_error(fn, grav_planets(vpos, psrc, &frame->planets, vpos), 70);

    // vpos -> direction in which light was emitted from observer's perspective...
    for(i = 3; --i >= 0;)
      vpos[i] = -vpos[i];
  }

  prop_error(fn, novas_geom_to_app(frame, pos, sys, out), 70);

  out->rv = rad_vel2(object, vpos, vel, pos, frame->obs_vel, novas_vdist(frame->obs_pos, frame->earth_pos),
          novas_vdist(frame->obs_pos, frame->sun_pos), d_sb);

  return 0;
}



/**
 * Converts an geometric position in ICRS to an apparent position on sky, by applying appropriate
 * corrections for aberration and gravitational deflection for the observer's frame. Unlike
 * `place()` the output reports the distance calculated from the parallax for sidereal sources.
 * The radial velocity of the output is set to NAN (if needed use novas_sky_pos() instead).
 *
 * @param frame     The observer frame, defining the location and time of observation
 * @param pos       [AU] Geometric position of source in ICRS coordinates
 * @param sys       The coordinate system in which to return the apparent sky location
 * @param[out] out  Pointer to the data structure which is populated with the calculated
 *                  apparent location in the designated coordinate system. It may be the
 *                  same pounter as the input position.
 * @return          0 if successful, or an error from grav_def2(),
 *                  or else -1 (errno will indicate the type of error).
 *
 * @sa novas_sky_pos()
 * @sa novas_app_to_geom()
 * @sa novas_app_to_hor()
 * @sa novas_geom_posvel()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_geom_to_app(const novas_frame *frame, const double *pos, enum novas_reference_system sys, sky_pos *out) {
  const char *fn = "novas_geom_to_app";
  double pos1[3];
  int i;

  if(!pos || !frame || !out)
    return novas_error(-1, EINVAL, "NULL argument: pos=%p, frame=%p, out=%p", (void *) pos, frame, out);

  if(!is_frame_initialized(frame))
    return novas_error(-1, EINVAL, fn, "frame at %p not initialized", frame);

  if(frame->accuracy != NOVAS_FULL_ACCURACY && frame->accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", frame->accuracy);

  // Compute gravitational deflection and aberration.
  prop_error(fn, grav_planets(pos, frame->obs_pos, &frame->planets, pos1), 0);

  // Aberration correction
  frame_aberration(frame, GEOM_TO_APP, pos1);

  // Transform position to output system
  prop_error(fn, icrs_to_sys(frame, pos1, sys), 0);

  vector2radec(pos1, &out->ra, &out->dec);

  out->dis = novas_vlen(pos1);
  out->rv = NAN;

  for(i = 3; --i >= 0;)
    out->r_hat[i] = pos1[i] / out->dis;

  return 0;

}

/**
 * Converts an observed apparent position vector in the specified coordinate system to local
 * horizontal coordinates in the specified observer frame. The observer must be located on the
 * surface of Earth, or else the call will return with an error. The caller may optionally supply
 * a refraction model of choice to calculate an appropriate elevation angle that includes a
 * refraction correction for Earth's atmosphere. If no such model is provided the calculated
 * elevation will be the astrometric elevation without a refraction correction.
 *
 * @param frame       Observer frame, defining the time and place of observation (on Earth).
 * @param sys         Astronomical coordinate system in which the observed position is given.
 * @param ra          [h] Observed apparent right ascension (R.A.) coordinate
 * @param dec         [deg] Observed apparent declination coordinate
 * @param ref_model   An appropriate refraction model, or NULL to calculate unrefracted elevation.
 *                    Depending on the refraction model, you might want to make sure that the
 *                    weather parameters were set when the observing frame was defined.
 * @param[out] az     [deg] Calculated azimuth angle. It may be NULL if not required.
 * @param[out] el     [deg] Calculated elevation angle. It may be NULL if not required.
 * @return            0 if successful, or else an error from tod_to_itrs() or cirs_to_itrs(), or
 *                    -1 (errno will indicate the type of error).
 *
 * @sa novas_hor_to_app()
 * @sa novas_app_to_geom()
 * @sa novas_standard_refraction()
 * @sa novas_optical_refraction()
 * @sa novas_radio_refraction()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_app_to_hor(const novas_frame *frame, enum novas_reference_system sys, double ra, double dec, RefractionModel ref_model,
        double *az, double *el) {
  static const char *fn = "novas_app_to_hor";
  const novas_timespec *time;
  double pos[3];
  double az0, za0;

  if(!frame)
    return novas_error(-1, EINVAL, fn, "NULL observing frame");

  if(!az && !el) {
    return novas_error(-1, EINVAL, fn, "Both output pointers (az, el) are NULL");
  }

  if(!is_frame_initialized(frame))
    return novas_error(-1, EINVAL, fn, "frame at %p not initialized", frame);

  if(frame->observer.where != NOVAS_OBSERVER_ON_EARTH && frame->observer.where != NOVAS_AIRBORNE_OBSERVER) {
    return novas_error(-1, EINVAL, fn, "observer not on Earth: where=%d", frame->observer.where);
  }

  time = (novas_timespec *) &frame->time;

  radec2vector(ra, dec, 1.0, pos);

  switch(sys) {
    case NOVAS_J2000:
      matrix_transform(pos, &frame->precession, pos); // @suppress("No break at end of case")
      /* fallthrough */
    case NOVAS_MOD:
      matrix_transform(pos, &frame->nutation, pos); // @suppress("No break at end of case")
      /* fallthrough */
    case NOVAS_TOD:
      spin(15.0 * frame->gst, pos, pos);
      break;

    case NOVAS_ICRS:
    case NOVAS_GCRS:
      matrix_transform(pos, &frame->gcrs_to_cirs, pos); // @suppress("No break at end of case")
      /* fallthrough */
    case NOVAS_CIRS:
      spin(frame->era, pos, pos);
      break;

    default:
      return novas_error(-1, EINVAL, fn, "invalid coordinate system: %d", sys);
  }

  itrs_to_hor(&frame->observer.on_surf, pos, &az0, &za0);

  if(ref_model)
    za0 -= ref_model(time->ijd_tt + time->fjd_tt, &frame->observer.on_surf, NOVAS_REFRACT_ASTROMETRIC, 90.0 - za0);

  if(az)
    *az = az0;
  if(el)
    *el = 90.0 - za0;

  return 0;
}

/**
 * Converts an observed azimuth and elevation coordinate to right ascension (R.A.) and declination
 * coordinates expressed in the coordinate system of choice. The observer must be located on the
 * surface of Earth, or else the call will return with an error. The caller may optionally supply
 * a refraction model of choice to calculate an appropriate elevation angle that includes a
 * refraction correction for Earth's atmosphere. If no such model is provided, the provided
 * elevation value will be assumed to be an astrometric elevation without a refraction correction.
 *
 * @param frame       Observer frame, defining the time and place of observation (on Earth).
 * @param az          [deg] Observed azimuth angle. It may be NULL if not required.
 * @param el          [deg] Observed elevation angle. It may be NULL if not required.
 * @param ref_model   An appropriate refraction model, or NULL to assume unrefracted elevation.
 *                    Depending on the refraction model, you might want to make sure that the
 *                    weather parameters were set when the observing frame was defined.
 * @param sys         Astronomical coordinate system in which the output is R.A. and declination
 *                    values are to be calculated.
 * @param[out] ra     [h] Calculated apparent right ascension (R.A.) coordinate
 * @param[out] dec    [deg] Calculated apparent declination coordinate
 * @return            0 if successful, or else an error from itrs_to_tod() or itrs_to_cirs(), or
 *                    -1 (errno will indicate the type of error).
 *
 * @sa novas_app_to_hor()
 * @sa novas_app_to_geom()
 * @sa novas_standard_refraction()
 * @sa novas_optical_refraction()
 * @sa novas_radio_refraction()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_hor_to_app(const novas_frame *frame, double az, double el, RefractionModel ref_model, enum novas_reference_system sys,
        double *ra, double *dec) {
  static const char *fn = "novas_hor_to_app";
  const novas_timespec *time;
  double pos[3];

  if(!frame)
    return novas_error(-1, EINVAL, fn, "NULL observing frame");

  if(!ra && !dec) {
    return novas_error(-1, EINVAL, fn, "Both output pointers (ra, dec) are NULL");
  }

  if(!is_frame_initialized(frame))
    return novas_error(-1, EINVAL, fn, "frame at %p not initialized", frame);

  if(frame->observer.where != NOVAS_OBSERVER_ON_EARTH && frame->observer.where != NOVAS_AIRBORNE_OBSERVER) {
    return novas_error(-1, EINVAL, fn, "observer not on Earth: where=%d", frame->observer.where);
  }

  time = (novas_timespec *) &frame->time;

  if(ref_model)
    el -= ref_model(time->ijd_tt + time->fjd_tt, &frame->observer.on_surf, NOVAS_REFRACT_OBSERVED, el);

  // az, el to ITRS pos
  hor_to_itrs(&frame->observer.on_surf, az, 90.0 - el, pos);

  // ITRS to TOD or CIRS...
  spin(cmp_sys(sys, NOVAS_GCRS) < 0 ? -15.0 * frame->gst : -frame->era, pos, pos);

  // Continue to convert TOD / CIRS to output system....
  switch(sys) {
    case NOVAS_TOD:
      break;

    case NOVAS_MOD:
      matrix_inv_rotate(pos, &frame->nutation, pos);
      break;

    case NOVAS_J2000:
      matrix_inv_rotate(pos, &frame->nutation, pos);
      matrix_inv_rotate(pos, &frame->precession, pos);
      break;

    case NOVAS_CIRS:
      break;

    case NOVAS_ICRS:
    case NOVAS_GCRS:
      matrix_inv_rotate(pos, &frame->gcrs_to_cirs, pos);
      break;

    default:
      return novas_error(-1, EINVAL, fn, "invalid coordinate system: %d", sys);
  }

  vector2radec(pos, ra, dec);
  return 0;
}

/**
 * Converts an observed apparent sky position of a source to an ICRS geometric position, by
 * undoing the gravitational deflection and aberration corrections.
 *
 * @param frame           The observer frame, defining the location and time of observation
 * @param sys             The reference system in which the observed position is specified.
 * @param ra              [h] Observed ICRS right-ascension of the source
 * @param dec             [deg] Observed ICRS declination of the source
 * @param dist            [AU] Observed distance from observer. A value of &lt;=0 will translate
 *                        to 10<sup>15</sup> AU (around 5 Gpc).
 * @param[out] geom_icrs  [AU] The corresponding geometric position for the source, in ICRS.
 * @return              0 if successful, or else an error from grav_undef2(), or -1 (errno will
 *                      indicate the type of error).
 *
 * @sa novas_geom_to_app()
 * @sa novas_hor_to_app()
 * @sa novas_geom_to_hor()
 * @sa novas_transform_vector()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_app_to_geom(const novas_frame *frame, enum novas_reference_system sys, double ra, double dec, double dist, double *geom_icrs) {
  static const char *fn = "novas_apparent_to_nominal";
  double app_pos[3];

  if(!frame || !geom_icrs)
    return novas_error(-1, EINVAL, fn, "NULL argument: frame=%p, geom_icrs=%p", frame, geom_icrs);

  if(!is_frame_initialized(frame))
    return novas_error(-1, EINVAL, fn, "frame at %p not initialized", frame);

  if(sys < 0 || sys >= NOVAS_REFERENCE_SYSTEMS)
    return novas_error(-1, EINVAL, fn, "invalid reference system: %d", sys);

  if(dist <= 0.0) dist = 1e15;

  // 3D apparent position
  radec2vector(ra, dec, dist, app_pos);

  // Convert apparent position to ICRS...
  switch(sys) {
    case NOVAS_CIRS:
      matrix_inv_rotate(app_pos, &frame->gcrs_to_cirs, app_pos);
      break;

    case NOVAS_TOD:
      matrix_inv_rotate(app_pos, &frame->nutation, app_pos); // @suppress("No break at end of case")
      /* fallthrough */
    case NOVAS_MOD:
      matrix_inv_rotate(app_pos, &frame->precession, app_pos); // @suppress("No break at end of case")
      /* fallthrough */
    case NOVAS_J2000:
      matrix_inv_rotate(app_pos, &frame->icrs_to_j2000, app_pos);
      break;
    default:
      // nothing to do.
      ;
  }

  // Undo aberration correction
  frame_aberration(frame, APP_TO_GEOM, app_pos);

  // Undo gravitational deflection and aberration.
  prop_error(fn, grav_undo_planets(app_pos, frame->obs_pos, &frame->planets, geom_icrs), 0);

  return 0;
}


static int cat_transform(novas_transform *transform, const novas_matrix *component, int dir) {
  int i;
  double T[3][3];

  memcpy(T, transform->matrix.M, sizeof(T));
  memset(transform->matrix.M, 0, sizeof(transform->matrix.M));

  for(i = 3; --i >= 0;) {
    int j;
    for(j = 3; --j >= 0;) {
      int k;
      for(k = 3; --k >= 0;)
        transform->matrix.M[i][j] += T[i][k] * (dir < 0 ? component->M[j][k] : component->M[k][j]);
    }
  }

  return 0;
}

/**
 * Calculates a transformation matrix that can be used to convert positions and velocities from
 * one coordinate reference system to another.
 *
 * @param frame           Observer frame, defining the location and time of observation
 * @param from_system     Original coordinate reference system
 * @param to_system       New coordinate reference system
 * @param[out] transform  Pointer to the transform data structure to populate.
 * @return                0 if successful, or else -1 if there was an error (errno will indicate
 *                        the type of error).
 *
 * @sa novas_transform_vector()
 * @sa novas_transform_sky_pos()
 * @sa novas_invert_transform()
 * @sa novas_geom_posvel()
 * @sa novas_app_to_geom()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_make_transform(const novas_frame *frame, enum novas_reference_system from_system, enum novas_reference_system to_system,
        novas_transform *transform) {
  static const char *fn = "novas_calc_transform";
  int i, dir;

  if(!frame || !transform)
    return novas_error(-1, EINVAL, fn, "NULL argument: frame=%p, transform=%p", frame, transform);

  if(!is_frame_initialized(frame))
    return novas_error(-1, EINVAL, fn, "frame at %p not initialized", frame);

  if(to_system < 0 || to_system >= NOVAS_REFERENCE_SYSTEMS)
    return novas_error(-1, EINVAL, fn, "invalid reference system (to): %d\n", to_system);

  transform->frame = *frame;
  transform->from_system = from_system;
  transform->to_system = to_system;

  memset(&transform->matrix, 0, sizeof(transform->matrix));

  // Identity matrix
  for(i = 3; --i >= 0;)
    transform->matrix.M[i][i] = 1.0;

  // We treat ICRS and GCRS as the same here
  // The aberration and gravitational bending are accounted for in
  // novas_calc_apparent
  if(from_system == NOVAS_ICRS)
    from_system = NOVAS_GCRS;
  if(to_system == NOVAS_ICRS)
    to_system = NOVAS_GCRS;

  dir = cmp_sys(to_system, from_system);

  if(dir == 0)
    return 0;

  if(dir < 0) {
    switch(from_system) {
      case NOVAS_CIRS:
        cat_transform(transform, &frame->gcrs_to_cirs, -1);
        if(to_system == NOVAS_GCRS)
          return 0; // @suppress("No break at end of case")
        /* fallthrough */

      case NOVAS_GCRS:
        cat_transform(transform, &frame->icrs_to_j2000, 1);
        if(to_system == NOVAS_J2000)
          return 0; // @suppress("No break at end of case")
        /* fallthrough */

      case NOVAS_J2000:
        cat_transform(transform, &frame->precession, 1);
        if(to_system == NOVAS_MOD)
          return 0; // @suppress("No break at end of case")
        /* fallthrough */

      case NOVAS_MOD:
        cat_transform(transform, &frame->nutation, 1);
        return 0;

      default:
        // nothing to do...
        ;
    }
  }
  else {
    switch(from_system) {
      case NOVAS_TOD:
        cat_transform(transform, &frame->nutation, -1);
        if(to_system == NOVAS_MOD)
          return 0; // @suppress("No break at end of case")
        /* fallthrough */

      case NOVAS_MOD:
        cat_transform(transform, &frame->precession, -1);
        if(to_system == NOVAS_J2000)
          return 0; // @suppress("No break at end of case")
        /* fallthrough */

      case NOVAS_J2000:
        cat_transform(transform, &frame->icrs_to_j2000, -1);
        if(to_system == NOVAS_GCRS)
          return 0; // @suppress("No break at end of case")
        /* fallthrough */

      case NOVAS_GCRS:
        cat_transform(transform, &frame->gcrs_to_cirs, 1);
        return 0;

      default:
        // nothing to do...
        ;
    }
  }

  return novas_error(-1, EINVAL, fn, "invalid reference system (from): %d\n", from_system);
}

/**
 * Inverts a novas coordinate transformation matrix.
 *
 * @param transform     Pointer to a coordinate transformation matrix.
 * @param[out] inverse  Pointer to a coordinate transformation matrix to populate with the inverse
 *                      transform. It may be the same as the input.
 * @return              0 if successful, or else -1 if the was an error (errno will indicate the
 *                      type of error).
 *
 * @sa novas_make_transform()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_invert_transform(const novas_transform *transform, novas_transform *inverse) {
  novas_transform orig;

  if(!transform || !inverse)
    return novas_error(-1, EINVAL, "novas_invert_transform", "NULL argument: transform=%p, inverse=%p", transform, inverse);

  orig = *transform;
  *inverse = orig;
  invert_matrix(&orig.matrix, &inverse->matrix);

  return 0;
}


/**
 * Transforms a position or velocity 3-vector from one coordinate reference system to another.
 *
 * @param in          Input 3-vector in the original coordinate reference system
 * @param transform   Pointer to a coordinate transformation matrix
 * @param[out] out    Output 3-vector in the new coordinate reference system. It may be the same
 *                    as the input.
 * @return            0 if successful, or else -1 if there was an error (errno will indicate the
 *                    type of error).
 *
 * @sa novas_make_transform()
 * @sa novas_transform_skypos()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_transform_vector(const double *in, const novas_transform *transform, double *out) {
  static const char *fn = "novas_matrix_transform";

  if(!transform || !in  || !out)
    return novas_error(-1, EINVAL, fn, "NULL parameter: in=%p, transform=%p, out=%p", in, transform, out);

  prop_error(fn, matrix_transform(in, &transform->matrix, out), 0);

  return 0;
}

/**
 * Transforms a position or velocity 3-vector from one coordinate reference system to another.
 *
 * @param in          Input apparent position on sky in the original coordinate reference system
 * @param transform   Pointer to a coordinate transformation matrix
 * @param[out] out    Output apparent position on sky in the new coordinate reference system.
 *                    It may be the same as the input.
 * @return            0 if successful, or else -1 if there was an error (errno will indicate the
 *                    type of error).
 *
 * @sa novas_make_transform()
 * @sa novas_transform_vector()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_transform_sky_pos(const sky_pos *in, const novas_transform *transform, sky_pos *out) {
  static const char *fn = "novas_matrix_transform";

  if(!transform || !in  || !out)
    return novas_error(-1, EINVAL, fn, "NULL parameter: in=%p, transform=%p, out=%p", in, transform, out);

  prop_error(fn, matrix_transform(in->r_hat, &transform->matrix, out->r_hat), 0);
  vector2radec(out->r_hat, &out->ra, &out->dec);

  return 0;
}
