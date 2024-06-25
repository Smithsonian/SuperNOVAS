/**
 * @file
 *
 * @date Created  on Jun 23, 2024
 * @author Attila Kovacs
 * @since 1.1
 *
 *  Routines for higher-level and efficient repeat coordinate transformations.
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

static int matrix_transform(const double *in, const novas_matrix *matrix, double *out) {
  static const char *fn = "novas_matrix_transform";
  double orig[3];
  int i;

  if(!in || !out)
    return novas_error(-1, EINVAL, fn, "NULL coordinate: in=%p, out=%p", in, out);
  if(!matrix)
    return novas_error(-1, EINVAL, fn, "NULL matrix pointer");

  memcpy(orig, in, sizeof(orig));

  for(i = 3; --i >= 0;) {
    int j;
    out[i] = 0.0;
    for(j = 3; --j >= 0;)
      out[i] += matrix->M[i][j] * orig[j];
  }

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
  I->M[2][1] = A->M[2][1] * A->M[0][2] - A->M[0][1] * A->M[2][2];
  I->M[0][2] = A->M[0][1] * A->M[1][2] - A->M[1][1] * A->M[0][2];
  I->M[1][2] = A->M[1][0] * A->M[0][2] - A->M[0][0] * A->M[1][2];
  I->M[2][2] = A->M[0][0] * A->M[1][1] - A->M[1][0] * A->M[0][1];

  idet = 1.0 / (A->M[0][0] * I->M[0][0] + A->M[1][1] * I->M[1][1] + A->M[2][2] * I->M[2][2]);

  for(i = 3; --i >= 0;) {
    int j;
    for(j = 3; --j >= 0;)
      I->M[i][j] *= idet;
  }

  return 0;
}

static int set_frame_tie(novas_frame *frame) {
  // 'xi0', 'eta0', and 'da0' are ICRS frame biases in arcseconds taken
  // from IERS (2003) Conventions, Chapter 5.
  static const double ax = -0.0166170 * ARCSEC; // eta0
  static const double ay = 0.0068192 * ARCSEC;  // -xi0
  static const double az = 0.01460 * ARCSEC;    // -da0
  static const double A[3] = { ax * ax, ay * ay, az * az };

  novas_matrix *T = &frame->icrs_to_j2000;

  memset(&frame->icrs_to_j2000, 0, sizeof(frame->icrs_to_j2000));

  T->M[0][0] = (1.0 - 0.5 * (A[1] + A[2]));
  T->M[0][1] = -az;
  T->M[0][2] = ay;
  T->M[1][0] = az;
  T->M[1][1] = (1.0 - 0.5 * (A[0] + A[2]));
  T->M[1][2] = -ax;
  T->M[2][0] = -ay;
  T->M[2][1] = ax;
  T->M[2][2] = (1.0 - 0.5 * (A[0] + A[1]));

  return 0;
}

static int set_gcrs_to_cirs(novas_frame *frame) {
  static const char *fn = "set_gcrs_to_cirs";
  const double jd_tdb = novas_get_time(&frame->time, NOVAS_TDB);
  double r_cio;
  short sys;
  double T[3][3];
  int i;

  prop_error(fn, cio_location(jd_tdb, frame->accuracy, &r_cio, &sys), 0);
  prop_error(fn, cio_basis(jd_tdb, r_cio, sys, frame->accuracy, &T[0][0], &T[1][0], &T[2][0]), 10);

  for(i = 3; --i >= 0;) {
    int j;
    for(j = 3; --j >= 0;)
      frame->gcrs_to_cirs.M[i][j] = T[j][i];
  }

  return 0;
}

static int set_precession(novas_frame *frame) {
  // 't' is time in TDB centuries between the two epochs.
  const double t = (novas_get_time(&frame->time, NOVAS_TDB) - NOVAS_JD_J2000) / JULIAN_CENTURY_DAYS;
  const double eps0 = 84381.406 * ARCSEC;

  // Numerical coefficients of psi_a, omega_a, and chi_a, along with
  // epsilon_0, the obliquity at J2000.0, are 4-angle formulation from
  // Capitaine et al. (2003), eqs. (4), (37), & (39).
  const double psia = ((((-0.0000000951 * t + 0.000132851) * t - 0.00114045) * t - 1.0790069) * t + 5038.481507) * t
          * ARCSEC;
  const double omegaa = ((((+0.0000003337 * t - 0.000000467) * t - 0.00772503) * t + 0.0512623) * t - 0.025754) * t
          * ARCSEC + eps0;
  const double chia = ((((-0.0000000560 * t + 0.000170663) * t - 0.00121197) * t - 2.3814292) * t + 10.556403) * ARCSEC
          * t;

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
  const double cp = cos(frame->dpsi);
  const double sp = sin(frame->dpsi);

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
  return obs_posvel(novas_get_time(&frame->time, NOVAS_TDB), frame->time.ut1_to_tt, &frame->observer, frame->accuracy,
          &frame->pos[NOVAS_EARTH][0], &frame->vel[NOVAS_EARTH][0], frame->obs_pos, frame->obs_vel);
}

static int set_aberration(novas_frame *frame) {
  novas_matrix *T;
  double *O, vemag, beta;
  int i;

  if(!frame)
    return novas_error(-1, EINVAL, "set_aberration", "NULL frame argument");

  T = &frame->aberration;
  O = frame->aberration_offset;

  memset(O, 0, XYZ_VECTOR_SIZE);

  vemag = novas_vlen(frame->obs_vel);
  beta = vemag / C_AUDAY;

  for(i = 0; i < 3; i++) {
    double cosd, gammai, p, q, r;
    int j;

    cosd = frame->obs_vel[i] / vemag;
    gammai = sqrt(1.0 - beta * beta);
    p = beta * cosd;
    q = (1.0 + p / (1.0 + gammai));
    r = 1.0 + p;

    for(j = 3; --j >= 0;) {
      T->M[j][i] = gammai / r;
      O[j] += q * frame->obs_vel[j] / r;
    }
  }

  invert_matrix(&frame->aberration, &frame->inv_aberration);

  return 0;
}

/**
 * Sets up a observing frame for a specific observer location, time of observation, and accuracy requirement.
 *
 * @param accuracy    Accuracy requirement, NOVAS_FULL_ACCURACY (0) for the utmost precision or
 *                    NOVAS_REDUCED_ACCURACY (1) if ~1 mas accuracy is sufficient.
 * @param obs         Observer location
 * @param time        Time of observation
 * @param[out] frame  Pointer to the observing frame to set up.
 * @return            0 if successful, or else -1 if there was an error (errno will indicate the type of
 *                    error).
 *
 * @sa novas_geometric_posvel()
 * @sa novas_set_transform()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_set_frame(enum novas_accuracy accuracy, const observer *obs, const novas_timespec *time, novas_frame *frame) {
  static const char *fn = "novas_create_frame";
  static const object earth = { NOVAS_PLANET, NOVAS_EARTH, "Earth" };
  static const object sun = { NOVAS_PLANET, NOVAS_SUN, "Sun" };

  double tdb2[2];
  double mobl, tobl, ee, dpsi, deps;

  if(accuracy < 0 || accuracy > NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  if(!time)
    return novas_error(-1, EINVAL, fn, "NULL input time specification");

  if(!frame)
    return novas_error(-1, EINVAL, fn, "NULL output frame");

  frame->accuracy = accuracy;
  frame->time = *time;
  frame->observer = *obs;

  tdb2[0] = time->ijd_tt;
  tdb2[1] = time->fjd_tt + tt2tdb(time->ijd_tt + time->fjd_tt);

  // Various calculated quantities for frame transformations
  e_tilt(tdb2[0] + tdb2[1], frame->accuracy, &mobl, &tobl, &ee, &dpsi, &deps);

  frame->mobl = mobl * DEGREE;
  frame->tobl = tobl * DEGREE;
  frame->ee = ee * DEGREE;
  frame->dpsi = dpsi * ARCSEC;
  frame->deps = deps * ARCSEC;

  set_frame_tie(frame);
  set_precession(frame);
  set_nutation(frame);
  set_aberration(frame);

  prop_error(fn, set_gcrs_to_cirs(frame), 0);

  // Barycentric Earth and Sun positions and velocities
  prop_error(fn,
          ephemeris(tdb2, &sun, NOVAS_BARYCENTER, accuracy, &frame->pos[NOVAS_SUN][0], &frame->vel[NOVAS_SUN][0]), 0);
  prop_error(fn,
          ephemeris(tdb2, &earth, NOVAS_BARYCENTER, accuracy, &frame->pos[NOVAS_EARTH][0], &frame->vel[NOVAS_EARTH][0]),
          0);

  set_obs_posvel(frame);

  return 0;
}

/**
 * Calculates the geometric position and velocity vectors for a source in the given observing frame. The geometric position is that
 * for a stationary observer (i.e. no aberration correction), and it does not include gravitational deflection. You can convert
 * the geometric position to an apparent location in a seconds step, to includes aberration and deflection, by passing the results onto
 * novas_skypos().
 *
 *
 * @param source    Pointer to a celestial source data structure that is observed
 * @param frame     The observer frame, defining the location and time of observation
 * @param sys       The desired reference coordinate system in which to return the coordinates
 * @param[out] pos  [AU] The calculated nominal position vector of the source relative to a non-moving observer location.
 * @param[out] vel  [AU/day] The calculated nominal velocity vector of the source relative to a non-moving observer location.
 * @return          0 if successful, or an error from light_time2(), or else -1 (errno will indicate the type of error).
 *
 * @sa novas_skypos()
 * @sa novas_set_transform()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_geometric_posvel(const object *source, const novas_frame *frame, enum novas_reference_system sys,
        double *pos, double *vel) {
  static const char *fn = "novas_calc_posvel";

  double jd_tdb, t_light;
  const observer *obs;
  double pos1[3], vel1[3];

  if(!source || !frame || !pos || !vel)
    return novas_error(-1, EINVAL, fn, "NULL argument: source=%p, frame=%p, pos=%p, vel=%p", source, frame, pos, vel);

  obs = (observer*) &frame->observer;

  if(frame->accuracy != NOVAS_FULL_ACCURACY && frame->accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", frame->accuracy);

  // ---------------------------------------------------------------------
  // Earth can only be an observed object when 'location' is not on Earth.
  // ---------------------------------------------------------------------
  if((source->type == NOVAS_PLANET) && (source->number == NOVAS_EARTH) && (obs->where != NOVAS_OBSERVER_AT_GEOCENTER)
          && (obs->where != NOVAS_OBSERVER_ON_EARTH))
    return novas_error(-1, EINVAL, fn, "invalid source type: %d", source->type);

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
    bary2obs(pos1, frame->obs_vel, pos1, &t_light);
  }
  else {
    // Get position of body wrt observer, antedated for light-time.
    prop_error(fn, light_time2(jd_tdb, source, frame->obs_pos, 0.0, frame->accuracy, pos1, vel1, &t_light), 0);
  }

  if(pos)
    memcpy(pos, pos1, sizeof(pos1));
  if(vel)
    memcpy(vel, vel1, sizeof(vel1));

  return 0;
}

/**
 * Converts nominal positions to apparent location on sky, by applying an aberration correction and gravitational
 * deflection.
 *
 * @param object        Pointer to a celestial object data structure that is observed
 * @param pos           [AU] Geometric position 3-vector of the observed source relative to the observer
 * @param vel           [AU/day] Geometric velocity 3-vector of the observed source relative to the observer
 * @param frame         The observer frame, defining the location and time of observation
 * @param[out] output   Pointer to the data structure which is populated with the calculated apparent location.
 * @return              0 if successful, or an error from grav_def(), or else -1 (errno will indicate the type
 *                      of error).
 *
 * @sa novas_geometric_posvel()
 * @sa novas_apparent_to_geometric()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_skypos(const object *object, const double *pos, const double *vel, const novas_frame *frame,
        sky_pos *output) {
  static const char *fn = "novas_calc_apparent";
  enum novas_observer_place loc;
  double jd_tdb, id, d_sb;
  double pos1[3];
  int i;

  if(!object || !pos || !frame || !output)
    return novas_error(-1, EINVAL, "NULL argument: object=%p, pos=%p, frame=%p, output=%p", (void*) object, pos, frame,
            output);

  jd_tdb = novas_get_time(&frame->time, NOVAS_TDB);

  if(object->type == NOVAS_CATALOG_OBJECT) {
    output->dis = 0.0;
    d_sb = novas_vlen(pos);
  }
  else {
    int k;

    output->dis = novas_vlen(pos) * C_AUDAY;

    // Calculate distance to Sun.
    d_sb = 0.0;
    for(k = 3; --k >= 0;) {
      double d = frame->pos[NOVAS_SUN][k] - (frame->obs_pos[k] + pos[k]);
      d_sb += d * d;
    }
    d_sb = sqrt(d_sb);
  }

  // ---------------------------------------------------------------------
  // Compute radial velocity (all vectors in ICRS).
  // ---------------------------------------------------------------------
  if(vel)
    rad_vel(object, pos, vel, frame->obs_vel, novas_vdist(frame->obs_pos, &frame->pos[NOVAS_EARTH][0]),
            novas_vdist(frame->obs_pos, &frame->pos[NOVAS_SUN][0]), d_sb, &output->rv);
  else
    output->rv = NAN;

  // ---------------------------------------------------------------------
  // Apply gravitational deflection of light and aberration.
  // ---------------------------------------------------------------------
  // Variable 'loc' determines whether Earth deflection is included.
  loc = frame->observer.where;
  if(loc == NOVAS_OBSERVER_ON_EARTH) {
    double frlimb;
    limb_angle(pos, &frame->pos[NOVAS_EARTH][0], NULL, &frlimb);
    if(frlimb < 0.8)
      loc = NOVAS_OBSERVER_AT_GEOCENTER;
  }

  // Compute gravitational deflection and aberration.
  prop_error(fn, grav_def(jd_tdb, loc, frame->accuracy, pos, frame->obs_pos, pos1), 0);

  matrix_transform(pos1, &frame->aberration, pos1);

  for(i = 3; --i >= 0;)
    pos1[i] += frame->aberration_offset[i];

  // ---------------------------------------------------------------------
  // Finish up.
  // ---------------------------------------------------------------------
  vector2radec(pos1, &output->ra, &output->dec);

  id = 1.0 / novas_vlen(pos1);
  for(i = 3; --i >= 0;)
    output->r_hat[i] = pos1[i] * id;

  return 0;
}

/**
 * Convers an observed apparent position for a source to a geometric position, by undoing
 * the gravitational deflection and aberration corrections.
 *
 *
 * @param app_pos       [AU] Apparent observed position of source (appropriately scaled to distance)
 * @param frame         The observer frame, defining the location and time of observation
 * @param[out] geom_pos [AU] The corresponding geometric position for the source.
 * @return              0 if successful, or else an error from grav_undef(), or -1 (errno will
 *                      indicate the type of error.
 *
 * @sa novas_skypos()
 * @sa place()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_apparent_to_geometric(const double *app_pos, const novas_frame *frame, double *geom_pos) {
  static const char *fn = "novas_apparent_to_nominal";
  enum novas_observer_place loc;
  double jd_tdb;
  int i;

  if(!app_pos || !frame || !geom_pos)
    return novas_error(-1, EINVAL, "NULL argument: app_pos=%p, frame=%p, nom_pos=%p", (void*) app_pos, frame, geom_pos);

  jd_tdb = novas_get_time(&frame->time, NOVAS_TDB);

  for(i = 3; --i >= 0;)
    geom_pos[i] -= frame->aberration_offset[i];

  matrix_transform(app_pos, &frame->inv_aberration, geom_pos);

  // ---------------------------------------------------------------------
  // Apply gravitational deflection of light and aberration.
  // ---------------------------------------------------------------------
  // Variable 'loc' determines whether Earth deflection is included.
  loc = frame->observer.where;
  if(loc == NOVAS_OBSERVER_ON_EARTH) {
    double frlimb;
    limb_angle(geom_pos, &frame->pos[NOVAS_EARTH][0], NULL, &frlimb);
    if(frlimb < 0.8)
      loc = NOVAS_OBSERVER_AT_GEOCENTER;
  }

  // Compute gravitational deflection and aberration.
  prop_error(fn, grav_undef(jd_tdb, loc, frame->accuracy, geom_pos, frame->obs_pos, geom_pos), 0);

  return 0;
}

static int cat_transform(novas_transform *transform, const novas_matrix *component, int dir) {
  int i;
  double T[3][3];

  memcpy(T, transform->matrix.M, sizeof(T));

  for(i = 3; --i >= 0;) {
    int j;
    for(j = 3; --j >= 0;) {
      int k;
      for(k = 3; --k >= 0;)
        transform->matrix.M[i][j] = T[i][k] * (dir < 0 ? component->M[j][k] : component->M[k][j]);
    }
  }

  return 0;
}

static int cmp_sys(enum novas_reference_system a, enum novas_reference_system b) {
  // GCRS=0, TOD=1, CIRS=2, ICRS=3, J2000=4, MOD=5
  // TOD->-3, MOD->-2, J2000->-1, GCRS->0, CIRS->1
  static const int index[] = { 0, -3, 1, 0, -1, -2 };

  if(a < 0 || a >= NOVAS_REFERENCE_SYSTEMS || b < 0 || b >= NOVAS_REFERENCE_SYSTEMS)
    return novas_error(-2, EINVAL, "cmp_sys", "Invalid reference system: a=%d, b=%d", a, b);

  if(index[a] == index[b])
    return 0;

  return index[a] < index[b] ? -1 : 1;
}

/**
 * Calculates a transformation matrix that can be used to convert positions and velocities from
 * one coordinate reference system to another.
 *
 * @param from_system     Original coordinate reference system
 * @param to_system       New coordinate reference system
 * @param frame           Observer frame, defining the location and time of observation
 * @param[out] transform  Pointer to the transform data structure to populate.
 * @return                0 if successful, or else -1 if there was an error (errno will indicate the
 *                        type of error).
 *
 * @sa novas_transform_pos()
 * @sa novas_transform_vel()
 * @sa novas_geometric_posvel()
 * @sa novas_invert_transform()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_set_transform(enum novas_reference_system from_system, enum novas_reference_system to_system,
        const novas_frame *frame, novas_transform *transform) {
  int i, dir;

  if(!frame || !transform)
    return novas_error(-1, EINVAL, "novas_calc_transform", "NULL argument: frame=%p, transform=%p", frame, transform);

  transform->frame = *frame;
  transform->from_system = from_system;
  transform->to_system = to_system;

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
  if(dir < -1)
    return -1;

  if(dir == 0)
    return 0;

  if(dir < 0) {
    switch(from_system) {
      case NOVAS_CIRS:
        cat_transform(transform, &frame->gcrs_to_cirs, -1);
        if(to_system == NOVAS_GCRS)
          return 0;
        /* no break */

      case NOVAS_GCRS:
        cat_transform(transform, &frame->icrs_to_j2000, 1);
        if(to_system == NOVAS_J2000)
          return 0;
        /* no break */

      case NOVAS_J2000:
        cat_transform(transform, &frame->precession, 1);
        if(from_system == NOVAS_MOD)
          return 0;
        /* no break */

      case NOVAS_MOD:
        cat_transform(transform, &frame->nutation, 1);
        return 0;

      default:
        /* NOT REACHED */
    }
  }

  else {
    switch(from_system) {
      case NOVAS_TOD:
        cat_transform(transform, &frame->nutation, -1);
        if(to_system == NOVAS_MOD)
          return 0;
        /* no break */

      case NOVAS_MOD:
        cat_transform(transform, &frame->precession, -1);
        if(to_system == NOVAS_MOD)
          return 0;
        /* no break */

      case NOVAS_J2000:
        cat_transform(transform, &frame->icrs_to_j2000, -1);
        if(to_system == NOVAS_GCRS)
          return 0;
        /* no break */

      case NOVAS_GCRS:
        cat_transform(transform, &frame->gcrs_to_cirs, 1);
        return 0;

      default:
        /* NOT REACHED */
    }
  }

  return 0; /* NOT REACHED */
}

/**
 * Inverts a novas coordinate transformation matrix.
 *
 * @param transform     Pointer to a coordinate transformation matrix.
 * @param[out] inverse  Pointer to a coordinate transformation matrix to populate with the inverse transform.
 * @return              0 if successful, or else -1 if the was an error (errno will indicate the type of
 *                      error).
 *
 * @sa novas_calc_transform()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_invert_transform(const novas_transform *transform, novas_transform *inverse) {
  novas_transform orig;

  if(!transform || !inverse)
    return novas_error(-1, EINVAL, "novas_invert_transform", "NULL argument: transform=%p, inverse=%p", transform,
            inverse);

  orig = *transform;
  *inverse = orig;
  invert_matrix(&orig.matrix, &inverse->matrix);

  return 0;
}

static int frame_pos(const novas_frame *frame, int dir, double *pos) {
  static const char *fn = "bary2frame_vel";
  int i;
  double epos[3];
  const observer *obs = &frame->observer;

  if(!frame->contains_planet[NOVAS_EARTH])
    return novas_error(-1, EINVAL, fn, "Uninitialized frame");

  if(obs->where == NOVAS_OBSERVER_ON_EARTH) {
    double lst = novas_get_time(&frame->time, NOVAS_UT1) * 24.0;
    prop_error(fn, terra(&obs->on_surf, lst, epos, NULL), 0);
  }
  else if(obs->where == NOVAS_OBSERVER_IN_EARTH_ORBIT) {
    return novas_error(-1, EINVAL, fn, "NULL spacecraft position");
  }

  for(i = 3; --i >= 0;) {
    // Earth orbital velocity...
    pos[i] += dir * frame->pos[NOVAS_EARTH][i];

    switch(obs->where) {
      case NOVAS_OBSERVER_ON_EARTH:
        pos[i] += dir * epos[i];
        break;
      case NOVAS_OBSERVER_IN_EARTH_ORBIT:
        // Spacecraft velocity
        pos[i] += dir * obs->near_earth.sc_pos[i];
        break;
      default:
        // Nothing to do.
    }
  }

  return 0;
}

static int frame_vel(const novas_frame *frame, int dir, double *v) {
  static const char *fn = "bary2frame_vel";
  const observer *obs = &frame->observer;
  int i;
  double vrot[3];

  if(!frame->contains_planet[NOVAS_EARTH])
    return novas_error(-1, EINVAL, fn, "Uninitialized frame");

  if(obs->where == NOVAS_OBSERVER_ON_EARTH) {
    double lst = novas_get_time(&frame->time, NOVAS_UT1) * 24.0;
    prop_error(fn, terra(&obs->on_surf, lst, NULL, vrot), 0);
  }
  else if(obs->where == NOVAS_OBSERVER_IN_EARTH_ORBIT) {
    return novas_error(-1, EINVAL, fn, "NULL spacecraft velocity");
  }

  for(i = 3; --i >= 0;) {
    // Earth orbital velocity...
    v[i] += dir * frame->vel[NOVAS_EARTH][i];

    switch(obs->where) {
      case NOVAS_OBSERVER_ON_EARTH:
        v[i] += dir * vrot[i];
        break;
      case NOVAS_OBSERVER_IN_EARTH_ORBIT:
        // Spacecraft velocity
        v[i] += dir * obs->near_earth.sc_vel[i];
        break;
      default:
        // nothing to do...
    }
  }

  return 0;
}

/**
 * Transforms a position vector from one coordinate reference system to another.
 *
 * @param in          Input position 3-vector in the original coordinate reference system
 * @param transform   Pointer to a coordinate transformation matrix
 * @param[out] out    Output position 3-vector in the new coordinate reference system
 * @return            0 if successful, or else -1 if there was an error (errno will
 *                    indicate the type of error).
 *
 * @sa novas_calc_transform()
 * @sa novas_transform_vel()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_transform_pos(const double *in, const novas_transform *transform, double *out) {
  static const char *fn = "novas_matrix_transform";

  if(!transform)
    return novas_error(-1, EINVAL, fn, "NULL transform pointer");

  memcpy(out, in, XYZ_VECTOR_SIZE);
  prop_error(fn, frame_pos(&transform->frame, -1, out), 0);
  prop_error(fn, matrix_transform(out, &transform->matrix, out), 0);
  prop_error(fn, frame_pos(&transform->frame, 1, out), 0);

  return 0;
}

/**
 * Transforms a velocity vector from one coordinate reference system to another.
 *
 * @param in          Input velocity 3-vector in the original coordinate reference system
 * @param transform   Pointer to a coordinate transformation matrix
 * @param[out] out    Output velocity 3-vector in the new coordinate reference system
 * @return            0 if successful, or else -1 if there was an error (errno will
 *                    indicate the type of error).
 *
 * @sa novas_calc_transform()
 * @sa novas_transform_pos()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_transform_vel(const double *in, const novas_transform *transform, double *out) {
  static const char *fn = "novas_transform_vel";

  if(!transform)
    return novas_error(-1, EINVAL, fn, "NULL transform pointer");

  memcpy(out, in, XYZ_VECTOR_SIZE);
  prop_error(fn, frame_vel(&transform->frame, -1, out), 0);
  prop_error(fn, matrix_transform(out, &transform->matrix, out), 0);
  prop_error(fn, frame_vel(&transform->frame, 1, out), 0);

  return 0;
}

