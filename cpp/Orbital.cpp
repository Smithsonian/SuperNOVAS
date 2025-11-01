/**
 * @file
 *
 * @date Created  on Oct 9, 2025
 * @author Attila Kovacs
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

using namespace novas;

namespace supernovas {

OrbitalSystem::OrbitalSystem(enum novas::novas_reference_plane plane, const Planet& center) : _system({}) {
  static const char *fn = "OrbitalSystem()";

  if((unsigned) plane >= NOVAS_REFERENCE_PLANES)
    novas_error(0, EINVAL, fn, "input reference plane is invalid: %d", plane);
  else if(!center.is_valid())
    novas_error(0, EINVAL, fn, "center planet is invalid: %d", center.novas_id());
  else _valid = true;

  _system.plane = plane;
  _system.center = center.novas_id();
}

OrbitalSystem::OrbitalSystem(const novas::novas_orbital_system *system) {
  static const char *fn = "OrbitalSystem()";

  if(!system)
    novas_error(0, EINVAL, fn, "input system is NULL");
  else if(!center().is_valid())
    novas_error(0, EINVAL, fn, "input system center is invalid: %d", (int) system->center);
  else if((unsigned) system->plane >= NOVAS_REFERENCE_PLANES)
    novas_error(0, EINVAL, fn, "input reference plane is invalid: %d", (int) system->plane);
  else if((unsigned) system->type >= NOVAS_REFERENCE_SYSTEMS)
    novas_error(0, EINVAL, fn, "input system coordinate type is invalid: %d", (int) system->type);
  else if(isnan(system->obl))
    novas_error(0, EINVAL, fn, "input system obliquity is NAN");
  else if(isnan(system->Omega))
    novas_error(0, EINVAL, fn, "input system Omega is NAN");
  else
    _valid = true;

  if(system)
    _system = *system;
}


Planet OrbitalSystem::center() const {
  return Planet(_system.center);
}

Angle OrbitalSystem::obliquity() const {
  return Angle(_system.obl);
}

Angle OrbitalSystem::ascending_node() const {
  return Angle(_system.Omega);
}

OrbitalSystem& OrbitalSystem::orientation(double obliquity_rad, double ascending_node_rad, const EquatorialSystem& system) {
  static const char *fn = "OrbitalSystem::orinetation";

  _valid = false;

  if(isnan(obliquity_rad))
    novas_error(0, EINVAL, fn, "input obliquity is NAN");
  else if(isnan(ascending_node_rad))
    novas_error(0, EINVAL, fn, "input ascending node is NAN");
  else if(!system.is_valid())
    novas_error(0, EINVAL, fn, "input equatorial system is invalid");
  else
    _valid = true;

  _system.type = system.reference_system();
  _system.obl = obliquity_rad;
  _system.Omega = ascending_node_rad;

  return *this;
}

OrbitalSystem& OrbitalSystem::orientation(const Angle& obliquity, const Angle& ascending_node, const EquatorialSystem& system) {
  return orientation(obliquity.rad(), ascending_node.rad(), system);
}

OrbitalSystem OrbitalSystem::equatorial(const Planet& center) {
  return OrbitalSystem(NOVAS_EQUATORIAL_PLANE, center);
}

OrbitalSystem OrbitalSystem::ecliptic(const Planet& center) {
  return OrbitalSystem(NOVAS_ECLIPTIC_PLANE, center);
}

OrbitalSystem OrbitalSystem::from_novas_orbital_system(const novas::novas_orbital_system *system) {
  return OrbitalSystem(system);
}




Orbital::Orbital(const novas_orbital *orbit) {
  static const char *fn = "Orbital()";

  if(!orbit)
    novas_error(0, EINVAL, fn, "input orbit is NULL");
  else if(!system().is_valid())
    novas_error(0, EINVAL, fn, "input orbital system is invalid");
  else if(isnan(orbit->jd_tdb))
    novas_error(0, EINVAL, fn, "input orbit->jd_tdb is NAN");
  else if(isnan(orbit->a))
    novas_error(0, EINVAL, fn, "input orbit->a is NAN");
  else if(isnan(orbit->M0))
    novas_error(0, EINVAL, fn, "input orbit->M0 is NAN");
  else if(isnan(orbit->n))
    novas_error(0, EINVAL, fn, "input orbit->n is NAN");
  else if(orbit->n == 0.0)
    novas_error(0, EINVAL, fn, "input orbit->n is 0");
  else if(orbit->n < 0.0)
    novas_error(0, EINVAL, fn, "input orbit->n is negative");
  else if(isnan(orbit->e))
    novas_error(0, EINVAL, fn, "input orbit->e is NAN");
  else if(orbit->e < 0.0)
    novas_error(0, EINVAL, fn, "input orbit->e is negative");
  else if(isnan(orbit->omega))
    novas_error(0, EINVAL, fn, "input orbit->omega is NAN");
  else if(isnan(orbit->i))
    novas_error(0, EINVAL, fn, "input orbit->i is NAN");
  else if(isnan(orbit->Omega))
    novas_error(0, EINVAL, fn, "input orbit->Omega is NAN");
  else
    _valid = true;

  if(orbit)
    _orbit = *orbit;
}

Orbital::Orbital(const OrbitalSystem& system, double jd_tdb, double semi_major_m,
        double mean_anom_rad, double period_s) {
  static const char *fn = "Orbital()";

  if(!system.is_valid())
    novas_error(0, EINVAL, fn, "input orbital system is invalid");
  else if(isnan(jd_tdb))
    novas_error(0, EINVAL, fn, "input reference time is NAN");
  else if(isnan(semi_major_m))
    novas_error(0, EINVAL, fn, "input semi major axis is NAN");
  else if(isnan(mean_anom_rad))
    novas_error(0, EINVAL, fn, "input mean anomaly is NAN");
  else if(isnan(period_s))
    novas_error(0, EINVAL, fn, "input period is NAN");
  else if(period_s == 0.0)
    novas_error(0, EINVAL, fn, "input period is zero");
  else if(period_s < 0.0)
    novas_error(0, EINVAL, fn, "input period is negative");
  else
    _valid = true;

  _orbit.system = *system._novas_orbital_system();
  _orbit.jd_tdb = jd_tdb;
  _orbit.a = semi_major_m / Unit::au;
  _orbit.M0 = mean_anom_rad / Unit::deg;
  _orbit.n = 360.0 / (period_s / Unit::day);
}

Orbital::Orbital(const OrbitalSystem& system, const Time& ref_time, const Distance& semi_major,
        const Angle& mean_anom, const Interval& periodT)
: Orbital(system, ref_time.jd(NOVAS_TDB), semi_major.m(), mean_anom.rad(), periodT.seconds()) {}


OrbitalSystem Orbital::system() const {
  return OrbitalSystem::from_novas_orbital_system(&_orbit.system);
}

double Orbital::reference_jd_tdb() const {
  return _orbit.jd_tdb;
}

Distance Orbital::semi_major_axis() const {
  return Distance(_orbit.a * Unit::au);
}

Angle Orbital::reference_mean_anomaly() const {
  return Angle(_orbit.M0 * Unit::deg);
}

double Orbital::mean_motion() const {
  return _orbit.n * Unit::deg / Unit::day;
}

Interval Orbital::period() const {
  return Interval(Constant::twoPi / mean_motion());
}

double Orbital::eccentricity() const {
  return _orbit.e;
}

Angle Orbital::periapsis() const {
  return Angle(_orbit.omega * Unit::deg);
}

Angle Orbital::inclination() const {
  return Angle(_orbit.i * Unit::deg);
}

Angle Orbital::ascending_node() const {
  return Angle(_orbit.Omega * Unit::deg);
}

Interval Orbital::apsis_period() const {
  return Interval(_orbit.apsis_period * Unit::day);
}

Interval Orbital::node_period() const {
  return Interval(_orbit.node_period * Unit::day);
}

double Orbital::apsis_rate() const {
  return Constant::twoPi / apsis_period().seconds();
}

double Orbital::node_rate() const {
  return Constant::twoPi / node_period().seconds();
}

Position Orbital::position(const Time& time, enum novas_accuracy accuracy) const {
  double p[3] = {0.0};

  if(novas_orbit_posvel(time.jd(), &_orbit, accuracy, p, NULL) != 0) {
    novas_trace_invalid("Orbital::position");
    return Position::invalid();
  }

  return Position(p, Unit::au);
}

Velocity Orbital::velocity(const Time& time, enum novas_accuracy accuracy) const {
  double v[3] = {0.0};

  if(novas_orbit_posvel(time.jd(), &_orbit, accuracy, NULL, v) != 0) {
    novas_trace_invalid("Orbital::velocity");
    return Velocity::invalid();
  }

  return Velocity(v, Unit::au / Unit::day);
}


Orbital& Orbital::eccentricity(double e, double periapsis_rad) {
  static const char *fn = "Orbital::eccentricity";

  _valid = false;

  if(isnan(e))
    novas_error(0, EINVAL, fn, "input eccentricity is NAN");
  if(e < 0.0)
    novas_error(0, EINVAL, fn, "input eccentricity is negative");
  else if(isnan(periapsis_rad))
    novas_error(0, EINVAL, fn, "input periapsis is NAN");
  else
    _valid = true;

  _orbit.e = e;
  _orbit.omega = periapsis_rad / Unit::deg;

  return *this;
}

Orbital& Orbital::eccentricity(double e, const Angle& periapsis_angle) {
  return eccentricity(e, periapsis_angle.rad());
}

Orbital& Orbital::inclination(double angle_rad, double ascending_node_rad) {
  static const char *fn = "Orbital::inclination";

  _valid = false;

  if(isnan(angle_rad))
    novas_error(0, EINVAL, fn, "input inclination angle is NAN");
  else if(isnan(ascending_node_rad))
    novas_error(0, EINVAL, fn, "input ascending node is NAN");
  else
    _valid = true;

  _orbit.i = angle_rad / Unit::deg;
  _orbit.Omega = ascending_node_rad / Unit::deg;

  return *this;
}

Orbital& Orbital::inclination(const Angle& angle, const Angle& ascending_node_angle) {
  return inclination(angle.rad(), ascending_node_angle.rad());
}

Orbital& Orbital::apsis_period(double seconds) {
  if(seconds < 0.0) {
    novas_error(0, EINVAL, "Orbital::apsis_period", "input period is negative");
    _valid = false;
  }
  _orbit.apsis_period = seconds / Unit::day;
  return *this;
}

Orbital& Orbital::apsis_period(const Interval& periodT) {
  return apsis_period(periodT.seconds());
}

Orbital& Orbital::apsis_rate(double rad_per_sec) {
  return apsis_period(Constant::twoPi / rad_per_sec);
}

Orbital& Orbital::node_period(double seconds) {
  if(seconds < 0.0) {
    novas_error(0, EINVAL, "Orbital::node_period", "input inclination angle is negative");
    _valid = false;
  }
  _orbit.node_period = seconds / Unit::day;
  return *this;
}

Orbital& Orbital::node_period(const Interval& periodT) {
  return node_period(periodT.seconds());
}

Orbital& Orbital::node_rate(double rad_per_sec) {
  return node_period(Constant::twoPi / rad_per_sec);
}

Orbital Orbital::with_mean_motion(const OrbitalSystem& system, double jd_tdb, double a, double M0, double rad_per_sec) {
  return Orbital(system, jd_tdb, a, M0, Constant::twoPi / rad_per_sec);
}

Orbital Orbital::with_mean_motion(const OrbitalSystem& system, const Time& time, const Angle& a, const Angle& M0, double rad_per_sec) {
  return Orbital::with_mean_motion(system, time.jd(NOVAS_TDB), a.rad(), M0.rad(), rad_per_sec);
}

Orbital Orbital::from_novas_orbit(const novas_orbital *orbit) {
  return Orbital(orbit);
}



} // namespace supernovas


