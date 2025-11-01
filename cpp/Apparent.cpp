/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
/// \endcond

#include "supernovas.h"

using namespace novas;

namespace supernovas {

static bool is_valid_rv(const char *fn, double rv_ms) {
  if(isnan(rv_ms))
    return novas_error(0, EINVAL, fn, "input radial velocity is NAN");
  else if(fabs(rv_ms) > Constant::c)
    return novas_error(0, EINVAL, fn, "input radial velocity exceeds the speed of light: %g m/s", rv_ms);
  return true;
}

static bool is_valid_sky_pos(const char *fn, const sky_pos *p) {
  if(isnan(p->ra))
    return novas_error(0, EINVAL, fn, "input pos->ra is NAN");

  else if(isnan(p->dec))
    return novas_error(0, EINVAL, fn, "input pos->dec is NAN");

  else if(!(p->dis > 0))
    return novas_error(0, EINVAL, fn, "input pos->dis is invalid: %g AU", p->dis / Unit::au);

  else if(isnan(p->rv))
    return novas_error(0, EINVAL, fn, "input pos->rv is NAN");

  else if(p->rv * Unit::au / Unit::day > Constant::c)
    return novas_error(0, EINVAL, fn, "input radial velocity exceeds the speed of light: %g m/s", p->rv * Unit::au / Unit::day);

  return true;
}


Apparent::Apparent(const Frame& f, enum novas_reference_system system)
: _frame(f), _sys(system) {
  static const char *fn = "Apparent(frame, system)";

  if(!f.is_valid())
    novas_error(0, EINVAL, fn, "frame is invalid");
  else if(system < 0 || system > NOVAS_REFERENCE_SYSTEMS)
    novas_error(0, EINVAL, fn, "system %d is invalid", _sys);
  else
    _valid = true;
}

Apparent::Apparent(sky_pos p, const Frame& f, enum novas_reference_system system)
: Apparent(f, system) {
  static const char *fn = "Apparent(frame, sky_pos, system)";

  if(!f.is_valid())
    novas_error(0, EINVAL, fn, "input frame is invalid");
  else
    _valid = true;

  _pos = p;

  // Always recalculate r_hat to ensure it's consistent with ra/dec
  radec2vector(_pos.ra, _pos.dec, 1.0, _pos.r_hat);
}

Apparent::Apparent(double ra_rad, double dec_rad, const Frame& frame, double rv_ms, enum novas_reference_system system)
: Apparent(frame, system) {
  static const char *fn = "Apparent(frame, eq, rv, system)";

  if(isnan(ra_rad))
    novas_error(0, EINVAL, fn, "input RA is NAN");
  else if(isnan(dec_rad))
    novas_error(0, EINVAL, fn, "input RA is NAN");

  _valid = is_valid_rv(fn, rv_ms);

  _pos.ra = ra_rad / Unit::hourAngle;
  _pos.dec = dec_rad / Unit::deg;
  _pos.rv = rv_ms / (Unit::km / Unit::sec);
  _pos.dis = NOVAS_DEFAULT_DISTANCE;

  radec2vector(_pos.ra, _pos.dec, 1.0, _pos.r_hat);
}

Apparent Apparent::cirs(double ra_rad, double dec_rad, const Frame& frame, double rv_ms) {
  return Apparent(ra_rad, dec_rad, frame, rv_ms, NOVAS_CIRS);
}

Apparent Apparent::cirs(const Angle& ra, const Angle& dec, const Frame& frame, const Speed& rv) {
  return Apparent(ra.rad(), dec.rad(), frame, rv.m_per_s(), NOVAS_CIRS);
}

Apparent Apparent::tod(double ra_rad, double dec_rad, const Frame& frame, double rv_ms) {
  return Apparent(ra_rad, dec_rad, frame, rv_ms, NOVAS_TOD);
}

Apparent Apparent::tod(const Angle& ra, const Angle& dec, const Frame& frame, const Speed& rv) {
  return Apparent(ra.rad(), dec.rad(), frame, rv.m_per_s(), NOVAS_TOD);
}

const Frame& Apparent::frame() const {
  return _frame;
}

enum novas_reference_system Apparent::system() const {
  return _sys;
}

const sky_pos *Apparent::_sky_pos() const {
  return &_pos;
}

Position Apparent::xyz() const {
  return Position(_pos.r_hat, _pos.dis * Unit::au);
}

Speed Apparent::radial_velocity() const {
  return Speed(_pos.rv * Unit::au / Unit::day);
}

Distance Apparent::distance() const {
  return Distance(_pos.dis * Unit::au);
}

Equatorial Apparent::equatorial() const {
  return Equatorial(_pos.ra * Unit::hourAngle, _pos.dec * Unit::deg,
          EquatorialSystem::for_reference_system(_sys, _frame.time().jd()).value(), _pos.dis * Unit::au);
}

Ecliptic Apparent::ecliptic() const {
  return equatorial().as_ecliptic();
}

Galactic Apparent::galactic() const {
  return equatorial().as_galactic();
}

std::optional<Horizontal> Apparent::horizontal() const {
  static const char *fn = "Apparent::horizontal";

  if(!_frame.observer().is_geodetic()) {
    novas_error(0, EINVAL, fn, "cannot convert for non-geodetic observer frame");
    return std::nullopt;
  }

  double ra = 0.0, dec = 0.0, az = 0.0, el = 0.0;

  // pos.ra / pos.dec may be NAN for ITRS / TIRS...
  vector2radec(_pos.r_hat, &ra, &dec);

  if(novas_app_to_hor(_frame._novas_frame(), _sys, ra, dec, NULL, &az, &el) != 0) {
    novas_trace_invalid(fn);
    return std::nullopt;
  }

  return Horizontal(az * Unit::deg, el * Unit::deg, _pos.dis * Unit::au);
}

std::optional<Apparent> Apparent::from_sky_pos(sky_pos pos, const Frame& frame, enum novas_reference_system system) {
  if(!is_valid_sky_pos("Apparent::from_sky_pos", &pos))
    return std::nullopt;

  return Apparent(pos, frame, system);
}

static const Apparent _invalid = Apparent::tod(NAN, NAN, Frame::invalid(), NAN);
const Apparent& Apparent::invalid() {
  return _invalid;
}

} //namespace supernovas

