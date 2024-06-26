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

/// \cond PRIVATE
#define FRAME_DEFAULT       0           ///< frame.state value we set to indicate the frame is not configured
#define FRAME_INITIALIZED   0xcafecafe  ///< frame.state for a properly initialized frame.
/// \endcond

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

static int matrix_transform(const double *in, const novas_matrix *matrix, double *out) {
  static const char *fn = "novas_matrix_transform";
  double orig[3];
  int i;

  if(!in || !out)
    return novas_error(-1, EINVAL, fn, "NULL coordinate: in=%p, out=%p", in, out);
  if(!matrix)
    return novas_error(-1, EINVAL, fn, "NULL matrix pointer");

  memcpy(orig, in, sizeof(orig));

  for(i = 3; --i >= 0;)
    out[i] = matrix->M[i][0] * orig[0] + matrix->M[i][1] * orig[1] + matrix->M[i][2] * orig[2];

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
    I->M[i][0] *= idet;
    I->M[i][1] *= idet;
    I->M[i][2] *= idet;
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
    frame->gcrs_to_cirs.M[i][0] = T[0][i];
    frame->gcrs_to_cirs.M[i][1] = T[1][i];
    frame->gcrs_to_cirs.M[i][2] = T[2][i];
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

static int is_frame_initialized(const novas_frame *frame) {
  if(!frame) return 0;
  return frame->state == FRAME_INITIALIZED;
}

/**
 * Sets up a observing frame for a specific observer location, time of observation, and accuracy
 * requirement.
 *
 *
 * @param accuracy    Accuracy requirement, NOVAS_FULL_ACCURACY (0) for the utmost precision or
 *                    NOVAS_REDUCED_ACCURACY (1) if ~1 mas accuracy is sufficient.
 * @param obs         Observer location
 * @param time        Time of observation
 * @param dx          [mas] Earth orientation parameter, polar offset in x.
 * @param dy          [mas] Earth orientation parameter, polar offset in y.
 * @param[out] frame  Pointer to the observing frame to set up.
 * @return            0 if successful, or else -1 if there was an error (errno will indicate the
 *                    type of error).
 *
 * @sa novas_change_observer()
 * @sa novas_icrs_posvel()
 * @sa novas_make_transform()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_make_frame(enum novas_accuracy accuracy, const observer *obs, const novas_timespec *time, double dx, double dy,
        novas_frame *frame) {
  static const char *fn = "novas_create_frame";
  static const object earth = { NOVAS_PLANET, NOVAS_EARTH, "Earth" };
  static const object sun = { NOVAS_PLANET, NOVAS_SUN, "Sun" };

  double tdb2[2];
  double mobl, tobl, ee, dpsi, deps;
  long ijd_ut1;
  double fjd_ut1;
  int err;

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
  tdb2[1] = time->fjd_tt + tt2tdb(time->ijd_tt + time->fjd_tt);

  // Various calculated quantities for frame transformations
  e_tilt(tdb2[0] + tdb2[1], frame->accuracy, &mobl, &tobl, &ee, &dpsi, &deps);

  frame->mobl = mobl * DEGREE;
  frame->tobl = tobl * DEGREE;
  frame->ee = ee * DEGREE;

  // Do not include polar wobble here.
  novas_dxdy_to_dpsideps(time->ijd_tt + time->fjd_tt, 0.0, 0.0, &frame->dpsi0, &frame->deps0);

  frame->dx = dx;
  frame->dy = dy;

  fjd_ut1 = novas_get_split_time(time, NOVAS_UT1, &ijd_ut1);
  frame->era = era(ijd_ut1, fjd_ut1);

  err = sidereal_time(ijd_ut1, fjd_ut1, time->ut1_to_tt, NOVAS_TRUE_EQUINOX, EROT_GST, frame->accuracy, &frame->gst);
  if(err != 0) {
    novas_trace(fn, err, 0);
    return -1;
  }

  set_frame_tie(frame);
  set_precession(frame);
  set_nutation(frame);
  set_aberration(frame);

  prop_error(fn, set_gcrs_to_cirs(frame), 0);

  // Barycentric Earth and Sun positions and velocities
  prop_error(fn, ephemeris(tdb2, &sun, NOVAS_BARYCENTER, accuracy, &frame->pos[NOVAS_SUN][0], &frame->vel[NOVAS_SUN][0]), 0);
  prop_error(fn, ephemeris(tdb2, &earth, NOVAS_BARYCENTER, accuracy, &frame->pos[NOVAS_EARTH][0], &frame->vel[NOVAS_EARTH][0]), 0);

  frame->state = FRAME_INITIALIZED;

  novas_change_observer(frame, obs, frame);

  return 0;
}

/**
 * Change the observer location for an observing frame.
 *
 * @param orig        Pointer to original observing frame
 * @param obs         New observer location
 * @param[out] out    Observing frame to populate with a original frame data and new observer
 *                    location. It can be the same as the input.
 * @return            0 if successfule or else an an error code (errno will indicate the type of
 *                    error).
 *
 * @sa novas_make_frame()
 */
int novas_change_observer(const novas_frame *orig, const observer *obs, novas_frame *out) {
  static const char *fn = "novas_change_observer";
  if(!orig || !obs || !out)
    return novas_error(-1, EINVAL, fn, "NULL parameter: orig=%p, obs=%p, out=%p", orig, obs, out);

  if(!is_frame_initialized(orig))
    return novas_error(-1, EINVAL, fn, "frame at %p not initialized", out);

  if(out != orig)
    *out = *orig;

  out->state = FRAME_DEFAULT;
  out->observer = *obs;
  prop_error(fn, set_obs_posvel(out), 0);

  out->state = FRAME_INITIALIZED;
  return 0;
}

/**
 * Calculates the geomtric position and velocity vectors for a source in the given observing
 * frame, in the specified coordinate system of choice. The geomtreic position includes proper
 * motion, and for solar-system bodies it is antedated for light travel time, so it effectively
 * represents the geometric position as seen by the observer. However, the geometric does not
 * include aberration correction, nor gravitational deflection.
 *
 * If you want apparent positions, which account for aberration and gravitational deflection,
 * you can pass the result to novas_skypos() after.
 *
 * You can also use novas_transform_posvel() to convert the output ICRS position and velocity vectors
 * to a dfferent coordinate system of choice after also.
 *
 * @param source    Pointer to a celestial source data structure that is observed
 * @param frame     Observer frame, defining the location and time of observation
 * @param sys       Desired reference coordinate system in which to return the coordinates
 * @param[out] pos  [AU] Calculated position vector of the source relative to the observer
 *                  location. It may be NULL if not required.
 * @param[out] vel  [AU/day] The calculated velocity vector of the source relative to the
 *                  observer. It may be NULL if not required.
 * @return          0 if successful, or an error from light_time2(), or else -1 (errno will
 *                  indicate the type of error).
 *
 * @sa novas_make_frame()
 * @sa novas_sky_pos()
 * @sa novas_transform_vector()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_posvel(const object *source, const novas_frame *frame, enum novas_reference_system sys, double *pos, double *vel) {
  static const char *fn = "novas_icrs_posvel";

  double jd_tdb, t_light;
  const observer *obs;
  double pos1[3], vel1[3];

  if(!source || !frame || !pos || !vel)
    return novas_error(-1, EINVAL, fn, "NULL argument: source=%p, frame=%p, pos=%p, vel=%p", source, frame, pos, vel);

  obs = (observer*) &frame->observer;

  if(!is_frame_initialized(frame))
      return novas_error(-1, EINVAL, fn, "frame at %p not initialized", frame);

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

  if(sys == NOVAS_CIRS) {
    matrix_transform(pos1, &frame->gcrs_to_cirs, pos1);
    matrix_transform(pos1, &frame->gcrs_to_cirs, pos1);
  }
  else if(cmp_sys(sys, NOVAS_ICRS) != 0) {
    matrix_transform(pos1, &frame->icrs_to_j2000, pos1);
    matrix_transform(pos1, &frame->icrs_to_j2000, pos1);
    if(sys == NOVAS_J2000)
      goto finished; // @suppress("Goto statement used")

    matrix_transform(pos1, &frame->precession, pos1);
    matrix_transform(pos1, &frame->precession, pos1);
    if(sys == NOVAS_MOD)
      goto finished; // @suppress("Goto statement used")

    matrix_transform(pos1, &frame->nutation, pos1);
    matrix_transform(pos1, &frame->nutation, pos1);
  }

  finished:

  if(pos)
    memcpy(pos, pos1, sizeof(pos1));
  if(vel)
    memcpy(vel, vel1, sizeof(vel1));

  return 0;
}

/**
 * Converts geometric positions to apparent location on sky, by applying an aberration correction
 * and gravitational deflection. To calculate corresponding local horizontal coordinates, you can
 * pass the output RA/Dec coordinates to novas_to_horizontal().
 *
 * @param object        Pointer to a celestial object data structure that is observed
 * @param frame         The observer frame, defining the location and time of observation
 * @param pos           [AU] Geometric position 3-vector of the observed source relative to the
 *                      observer
 * @param vel           [AU/day] Geometric velocity 3-vector of the observed source relative to
 *                      the observer
 * @param[out] output   Pointer to the data structure which is populated with the calculated
 *                      apparent location.
 * @return              0 if successful, or an error from grav_def(), or else -1 (errno will
 *                      indicate the type of error).
 *
 * @sa novas_to_horizontal()
 * @sa novas_posvel()
 * @sa novas_apparent_to_geometric()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_sky_pos(const object *object, const novas_frame *frame, const double *pos, const double *vel, sky_pos *output) {
  static const char *fn = "novas_calc_apparent";
  enum novas_observer_place loc;
  double jd_tdb, id, d_sb;
  double pos1[3];
  int i;

  if(!object || !pos || !frame || !output)
    return novas_error(-1, EINVAL, "NULL argument: object=%p, pos=%p, frame=%p, output=%p", (void*) object, pos, frame, output);

  if(!is_frame_initialized(frame))
      return novas_error(-1, EINVAL, fn, "frame at %p not initialized", frame);

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


static double novas_refraction(enum novas_refraction_model model, const on_surface *loc, enum novas_refraction_type type, double el) {
  if(!loc)
    return novas_error(-1, EINVAL, "novas_refraction", "NULL on surface observer location");

  if(type == NOVAS_REFRACT_OBSERVED)
    return refract(loc, NOVAS_WEATHER_AT_LOCATION, 90.0 - el);

  return refract_astro(loc, NOVAS_WEATHER_AT_LOCATION, 90.0 - el);
}

/**
 * Returns an optical refraction correction for a standard atmosphere.
 *
 * @param j_tt      [day] Terrestrial Time (TT) based Julian data of observation
 * @param loc       Pointer to structure defining the observer's location on earth, and local weather
 * @param type      Whether the input elevation is observed or astrometric: REFRACT_OBSERVED (-1) or
 *                  REFRACT_ASTROMETRIC (0).
 * @param el        [deg] Astrometric (unrefracted) source elevation
 * @return          [arcsec] Estimated refraction, or NAN if there was an error (it should also
 *                  set errno to indicate the type of error).
 *
 * @sa novas_to_horizontal()
 * @sa novas_optical_refraction()
 * @sa NOVAS_STANDARD_ATMOSPHERE()
 * @sa refract()
 * @sa refract_astro()
 */
double novas_standard_refraction(double j_tt, const on_surface *loc, enum novas_refraction_type type, double el) {
  double dz = novas_refraction(NOVAS_STANDARD_ATMOSPHERE, loc, type, el);
  if(isnan(dz))
    novas_trace("novas_optical_refraction", -1, 0);
  return dz;
}

/**
 * Returns an optical refraction correction using the weather parameters defined for the observer location.
 *
 * @param j_tt      [day] Terrestrial Time (TT) based Julian data of observation
 * @param loc       Pointer to structure defining the observer's location on earth, and local weather
 * @param type      Whether the input elevation is observed or astrometric: REFRACT_OBSERVED (-1) or
 *                  REFRACT_ASTROMETRIC (0).
 * @param el        [deg] Astrometric (unrefracted) source elevation
 * @return          [arcsec] Estimated refraction, or NAN if there was an error (it should also
 *                  set errno to indicate the type of error).
 *
 * @sa novas_to_horizontal()
 * @sa novas_optical_refraction()
 * @sa NOVAS_STANDARD_ATMOSPHERE()
 * @sa refract()
 * @sa refract_astro()
 */
double novas_optical_refraction(double j_tt, const on_surface *loc, enum novas_refraction_type type, double el) {
  double dz = novas_refraction(NOVAS_WEATHER_AT_LOCATION, loc, type, el);
  if(isnan(dz))
    novas_trace("novas_optical_refraction", -1, 0);
  return dz;
}

/**
 * Converts an observed position vector in the specified coordinate system to local horizontal
 * coordinates in the specified observer frame. The observer must be located on the surface of
 * Earth, or else the call will return with an error. The caller may optionally supply a
 * refraction model of choice to calculate an appropriate elevation angle that includes a
 * refraction correction for Earth's atmosphere. If no such model is provided the calculated
 * elevation will be the astrometric elevation without a refraction correction.
 *
 * @param ra          [h] Right ascension coordinate
 * @param dec         [deg] Declination angle
 * @param sys         Astronomical coordinate system in which the position is supplied.
 * @param frame       Observer frame, defining the time and place of observation (on Earth).
 * @param ref_model   An appropriate refraction model, or NULL to calculate unrefracted elevation.
 *                    If you use a refraction model, you might want to make sure that the weather
 *                    parameters were set when the observing frame was defined.
 * @param[out] az     [deg] Calculated azimuth angle. It may be NULL if not required.
 * @param[out] el     [deg] Calculated elevation angle. It may be NULL if not required.
 * @return            0 if successful, or else an error from tod_to_itrs() or cirs_to_itrs(), or
 *                    -1 (errno will indicate the type of error).
 *
 * @sa novas_posvel()
 * @sa novas_sky_pos()
 * @sa novas_standard_refraction()
 * @sa novas_optical_refraction()
 */
int novas_to_horizontal(enum novas_reference_system sys, double ra, double dec, const novas_frame *frame, RefractionModel ref_model,
        double *az, double *el) {
  static const char *fn = "novas_horizontal";
  const novas_timespec *time;
  double pos[3];
  double az0, za0, del = 0.0;

  if(!frame)
    return novas_error(-1, EINVAL, fn, "NULL observing frame");

  if(!az && !el) {
    return novas_error(-1, EINVAL, fn, "Both output pointers (az, el) are NULL");
  }

  if(!is_frame_initialized(frame))
      return novas_error(-1, EINVAL, fn, "frame at %p not initialized", frame);

  if(frame->observer.where != NOVAS_OBSERVER_ON_EARTH) {
    return novas_error(-1, EINVAL, fn, "observer not on Earth: where=%d", frame->observer.where);
  }

  time = (novas_timespec *) &frame->time;

  radec2vector(ra, dec, 1.0, pos);

  switch(sys) {
    case NOVAS_J2000:
      matrix_transform(pos, &frame->precession, pos);
      /* no break */
    case NOVAS_MOD:
      matrix_transform(pos, &frame->nutation, pos);
      /* no break */
    case NOVAS_TOD:
      prop_error(fn, tod_to_itrs(time->ijd_tt, time->fjd_tt, time->ut1_to_tt, frame->accuracy, frame->dx, frame->dy, pos, pos), 0);
      break;

    case NOVAS_ICRS:
    case NOVAS_GCRS:
      matrix_transform(pos, &frame->gcrs_to_cirs, pos);
      /* no break */
    case NOVAS_CIRS:
      prop_error(fn, cirs_to_itrs(time->ijd_tt, time->fjd_tt, time->ut1_to_tt, frame->accuracy, frame->dx, frame->dy, pos, pos), 0);
      break;
    default:
      return novas_error(-1, EINVAL, fn, "invalid coordinate system: %d", sys);
  }

  itrs_to_hor(&frame->observer.on_surf, pos, &az0, &za0);

  if(ref_model) {
    del = ref_model(time->ijd_tt + time->fjd_tt, &frame->observer.on_surf, NOVAS_REFRACT_ASTROMETRIC, 90.0 - za0);
    if(isnan(del))
      return novas_trace(fn, -1, 0);
  }

  if(az)
    *az = az0;
  if(el)
    *el = 90.0 - za0 + del;

  return 0;
}

/**
 * Convers an observed apparent position for a source to a geometric position, by undoing
 * the gravitational deflection and aberration corrections.
 *
 *
 * @param app_pos       [AU] Apparent observed position of source (appropriately scaled to
 *                      distance)
 * @param frame         The observer frame, defining the location and time of observation
 * @param[out] geom_pos [AU] The corresponding geometric position for the source. It may be the
 *                      same as the input.
 * @return              0 if successful, or else an error from grav_undef(), or -1 (errno will
 *                      indicate the type of error).
 *
 * @sa novas_sky_pos()
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

  if(!is_frame_initialized(frame))
      return novas_error(-1, EINVAL, fn, "frame at %p not initialized", frame);

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



/**
 * Calculates a transformation matrix that can be used to convert positions and velocities from
 * one coordinate reference system to another.
 *
 * @param from_system     Original coordinate reference system
 * @param to_system       New coordinate reference system
 * @param frame           Observer frame, defining the location and time of observation
 * @param[out] transform  Pointer to the transform data structure to populate.
 * @return                0 if successful, or else -1 if there was an error (errno will indicate
 *                        the type of error).
 *
 * @sa novas_transform_vector()
 * @sa novas_icrs_posvel()
 * @sa novas_invert_transform()
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_make_transform(enum novas_reference_system from_system, enum novas_reference_system to_system, const novas_frame *frame,
        novas_transform *transform) {
  static const char *fn = "novas_calc_transform";
  int i, dir;

  if(!frame || !transform)
    return novas_error(-1, EINVAL, fn, "NULL argument: frame=%p, transform=%p", frame, transform);

  if(!is_frame_initialized(frame))
      return novas_error(-1, EINVAL, fn, "frame at %p not initialized", frame);

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
 * @param[out] inverse  Pointer to a coordinate transformation matrix to populate with the inverse
 *                      transform.
 * @return              0 if successful, or else -1 if the was an error (errno will indicate the
 *                      type of error).
 *
 * @sa novas_make_transform()
 * @sa novas_icrs_posvel()
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
 *
 * @since 1.1
 * @author Attila Kovacs
 */
int novas_transform_vector(const double *in, const novas_transform *transform, double *out) {
  static const char *fn = "novas_matrix_transform";

  if(!transform)
    return novas_error(-1, EINVAL, fn, "NULL transform pointer");

  prop_error(fn, matrix_transform(in, &transform->matrix, out), 0);

  return 0;
}

