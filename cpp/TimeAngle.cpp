/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */


#include "supernovas.h"

using namespace novas;

namespace supernovas {

TimeAngle::TimeAngle(double radians)
: Angle(radians) {
  if(_rad < 0.0) _rad += TWOPI;
}

TimeAngle::TimeAngle(const std::string& str)
: Angle(novas_str_hours(str.c_str()) * Unit::hourAngle) {}

TimeAngle::TimeAngle(const Angle& angle)
: Angle(angle) {}

double TimeAngle::hours() const {
  return _rad / Unit::hourAngle;
}

double TimeAngle::minutes() const {
  return hours() * 60.0;
}

double TimeAngle::seconds() const {
  return hours() * 3600.0;
}

std::string TimeAngle::to_string(enum novas_separator_type separator, int decimals) const {
  char s[100] = {'\0'};
  novas_print_hms(hours(), separator, decimals, s, sizeof(s));
  return std::string(s);
}

TimeAngle TimeAngle::operator+(const Interval& offset) const {
  return TimeAngle(rad() + offset.hours() * Unit::hourAngle);
}

TimeAngle TimeAngle::operator-(const Interval& offset) const {
  return TimeAngle(rad() - offset.hours() * Unit::hourAngle);
}

static const TimeAngle _zero = TimeAngle(0.0);

const TimeAngle& TimeAngle::zero() {
  return _zero;
}

} // namespace supernovas
