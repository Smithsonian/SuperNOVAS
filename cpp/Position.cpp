/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include <cstring>

#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
#include "supernovas.h"


using namespace supernovas;


Position::Position(double x, double y, double z)
: Vector(x, y, z) {}


Position::Position(const double pos[3], double unit)
: Vector(pos[0] * unit, pos[1] * unit, pos[2] * unit) {}

double Position::x_m() const {
  return _component[0];
}

double Position::y_m() const {
  return _component[1];
}

double Position::z_m() const {
  return _component[2];
}

Distance Position::distance() const {
  return Distance(abs());
}


Position Position::inv() const {
  return Position(-_component[0], -_component[1], -_component[2]);
}

Spherical Position::as_spherical() const {
  double longitude = atan2(_component[1], _component[0]);
  double xy = hypot(_component[0], _component[1]);
  double latitude = atan2(_component[2], xy);
  return Spherical(isnan(longitude) ? 0.0 : longitude, isnan(latitude) ? 0.0 : latitude, abs());
}

Position Position::origin() {
  return Position(0.0, 0.0, 0.0);
}

std::string Position::str() const {
  return "POS ( " + Distance(x_m()).str() + ", " + Distance(y_m()).str() + ", " + Distance(z_m()).str() + ")";
}

Position operator+(const Position& l, const Position& r) {
  return Position(l.x_m() + r.x_m(), l.y_m() + r.y_m(), l.z_m() + r.z_m());
}

Position operator-(const Position& l, const Position& r) {
  return Position(l.x_m() - r.x_m(), l.y_m() - r.y_m(), l.z_m() - r.z_m());
}
