/**
 * @file
 *
 * @date Created  on Mar 6, 2025
 * @author Attila Kovacs
 *
 *  Function relating to the use of orbital elements
 */

#include <errno.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"
/// \endcond


/**
 * Change xzy vectors to the new polar orientation. &theta, &phi define the orientation of the input pole in the output system.
 *
 * @param in        input 3-vector in the original system (pole = z)
 * @param theta     [deg] polar angle of original pole in the new system
 * @param phi       [deg] azimuthal angle of original pole in the new system
 * @param[out] out  output 3-vector in the new (rotated) system. It may be the same vector as the input.
 * @return          0
 *
 */
static int change_pole(const double *in, double theta, double phi, double *out) {
  double x, y, z;
  double ca, sa, cb, sb;

  x = in[0];
  y = in[1];
  z = in[2];

  theta *= DEGREE;
  phi *= DEGREE;

  ca = cos(phi);
  sa = sin(phi);
  cb = cos(theta);
  sb = sin(theta);

  out[0] = ca * x - sa * (cb * y + sb * z);
  out[1] = sa * x + ca * (cb * y - sb * z);
  out[2] = sb * y + cb * z;

  return 0;
}

/**
 * Converts equatorial coordinates of a given type to GCRS equatorial coordinates
 *
 * @param jd_tdb    [day] Barycentric Dynamical Time (TDB) based Julian Date
 * @param sys       the type of equator assumed for the input (mean, true, or GCRS).
 * @param[in, out] vec  vector to change to GCRS.
 * @return          0 if successful, or else -1 (errno set to EINVAL) if the 'sys'
 *                  argument is invalid.
 *
 * @author Attila Kovacs
 * @since 1.2
 */
static int equ2gcrs(double jd_tdb, enum novas_reference_system sys, double *vec) {
  switch(sys) {
    case NOVAS_GCRS:
    case NOVAS_ICRS:
      return 0;
    case NOVAS_CIRS:
      return cirs_to_gcrs(jd_tdb, NOVAS_REDUCED_ACCURACY, vec, vec);
    case NOVAS_J2000:
      return j2000_to_gcrs(vec, vec);
    case NOVAS_TOD:
      return tod_to_gcrs(jd_tdb, NOVAS_REDUCED_ACCURACY, vec, vec);
    case NOVAS_MOD:
      return mod_to_gcrs(jd_tdb, vec, vec);
    default:
      return novas_error(-1, EINVAL, "equ2gcrs", "invalid reference system: %d", sys);
  }
}

/**
 * Convert coordinates in an orbital system to GCRS equatorial coordinates
 *
 * @param jd_tdb        [day] Barycentric Dynamic Time (TDB) based Julian Date
 * @param sys           Orbital system specification
 * @param accuracy      NOVAS_FULL_ACCURACY or NOVAS_REDUCED_ACCURACY
 * @param[in, out] vec  Coordinates
 * @return              0 if successful, or else an error from ecl2equ_vec().
 *
 * @author Attila Kovacs
 * @since 1.2
 */
static int orbit2gcrs(double jd_tdb, const novas_orbital_system *sys, enum novas_accuracy accuracy, double *vec) {
  static const char *fn = "orbit2gcrs";

  if(accuracy != NOVAS_FULL_ACCURACY && accuracy != NOVAS_REDUCED_ACCURACY)
    return novas_error(-1, EINVAL, fn, "invalid accuracy: %d", accuracy);

  if(sys->obl)
    change_pole(vec, sys->obl, sys->Omega, vec);

  if(sys->plane == NOVAS_ECLIPTIC_PLANE) {
    enum novas_equator_type eq;
    double jd = jd_tdb;

    switch(sys->type) {
      case NOVAS_GCRS:
      case NOVAS_ICRS:
        eq = NOVAS_GCRS_EQUATOR;
        jd = NOVAS_JD_J2000;
        break;
      case NOVAS_J2000:
        eq = NOVAS_TRUE_EQUATOR;
        jd = NOVAS_JD_J2000;
        break;
      case NOVAS_TOD:
      case NOVAS_CIRS:
        eq = NOVAS_TRUE_EQUATOR;
        break;
      case NOVAS_MOD:
        eq = NOVAS_MEAN_EQUATOR;
        break;
      default:
        return novas_error(-1, EINVAL, fn, "invalid reference system: %d", sys->type);
    }

    ecl2equ_vec(jd, eq, accuracy, vec, vec);
  }
  else if(sys->plane != NOVAS_EQUATORIAL_PLANE)
    return novas_error(-1, EINVAL, fn, "invalid orbital system reference plane type: %d", sys->type);

  prop_error(fn, equ2gcrs(jd_tdb, sys->type, vec), 0);

  return 0;
}

/**
 * Calculates a rectangular equatorial position and velocity vector for the given orbital elements for the
 * specified time of observation.
 *
 * REFERENCES:
 * <ol>
 * <li>https://ssd.jpl.nasa.gov/planets/approx_pos.html</li>
 * <li>https://en.wikipedia.org/wiki/Orbital_elements</li>
 * <li>https://orbitalofficial.com/</li>
 * <li>https://downloads.rene-schwarz.com/download/M001-Keplerian_Orbit_Elements_to_Cartesian_State_Vectors.pdf</li>
 * </ol>
 *
 * @param jd_tdb    [day] Barycentric Dynamic Time (TDB) based Julian date
 * @param orbit     Orbital parameters
 * @param accuracy  NOVAS_FULL_ACCURACY (0) or NOVAS_REDUCED_ACCURACY (1).
 * @param[out] pos  [AU] Output ICRS equatorial position vector, or NULL if not required
 * @param[out] vel  [AU/day] Output ICRS equatorial velocity vector, or NULL if not required
 * @return          0 if successful, or else -1 if the orbital parameters are NULL
 *                  or if the position and velocity output vectors are the same or the orbital
 *                  system is ill defined (errno set to EINVAL), or if the calculation did not converge (errno set to
 *                  ECANCELED), or
 *
 * @sa ephemeris()
 * @sa novas_geom_posvel()
 * @sa place()
 * @sa make_orbital_object()
 *
 * @author Attila Kovacs
 * @since 1.2
 */
int novas_orbit_posvel(double jd_tdb, const novas_orbital *restrict orbit, enum novas_accuracy accuracy,
        double *restrict pos, double *restrict vel) {
  static const char *fn = "novas_orbit_posvel";

  double dt, M, E, nu, r, omega, Omega;
  double cO, sO, ci, si, co, so;
  double xx, yx, zx, xy, yy, zy;
  int i = novas_inv_max_iter;

  if(!orbit)
    return novas_error(-1, EINVAL, fn, "input orbital elements is NULL");

  if(pos == vel)
    return novas_error(-1, EINVAL, fn, "output pos = vel (@ %p)", pos);

  dt = (jd_tdb - orbit->jd_tdb);
  E = M = remainder(orbit->M0 + orbit->n * dt, DEG360) * DEGREE;

  // Iteratively determine E, using Newton-Raphson method...
  while(--i >= 0) {
    double esE = orbit->e * sin(E);
    double ecE = orbit->e * cos(E);
    double dE = (E - esE - M) / (1.0 - ecE);

    E -= dE;
    if(fabs(dE) < EPREC)
      break;
  }

  if(i < 0)
    return novas_error(-1, ECANCELED, fn, "Eccentric anomaly convergence failure");

  nu = 2.0 * atan2(sqrt(1.0 + orbit->e) * sin(0.5 * E), sqrt(1.0 - orbit->e) * cos(0.5 * E));
  r = orbit->a * (1.0 - orbit->e * cos(E));

  omega = orbit->omega * DEGREE;
  if(orbit->apsis_period > 0.0)
    omega += TWOPI * remainder(dt / orbit->apsis_period, 1.0);

  Omega = orbit->Omega * DEGREE;
  if(orbit->node_period > 0.0)
    Omega += TWOPI * remainder(dt / orbit->node_period, 1.0);

  // pos = Rz(-Omega) . Rx(-i) . Rz(-omega) . orb
  cO = cos(Omega);
  sO = sin(Omega);
  ci = cos(orbit->i * DEGREE);
  si = sin(orbit->i * DEGREE);
  co = cos(omega);
  so = sin(omega);

  // Rotation matrix
  // See https://en.wikipedia.org/wiki/Euler_angles
  // (note the Wikipedia has opposite sign convention for angles...)
  xx = cO * co - sO * ci * so;
  yx = sO * co + cO * ci * so;
  zx = si * so;

  xy = -cO * so - sO * ci * co;
  yy = -sO * so + cO * ci * co;
  zy = si * co;

  if(pos) {
    double x = r * cos(nu);
    double y = r * sin(nu);

    // Perform rotation
    pos[0] = xx * x + xy * y;
    pos[1] = yx * x + yy * y;
    pos[2] = zx * x + zy * y;

    prop_error(fn, orbit2gcrs(jd_tdb, &orbit->system, accuracy, pos), 0);
  }

  if(vel) {
    double v = orbit->n * DEGREE * orbit->a * orbit->a / r;    // [AU/day]
    double x = -v * sin(E);
    double y = v * sqrt(1.0 - orbit->e * orbit->e) * cos(E);

    // Perform rotation
    vel[0] = xx * x + xy * y;
    vel[1] = yx * x + yy * y;
    vel[2] = zx * x + zy * y;

    prop_error(fn, orbit2gcrs(jd_tdb, &orbit->system, accuracy, vel), 0);
  }

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


