/**
 * @file
 *
 * @date Created  on Oct 13, 2025
 * @author Attila Kovacs
 */

#include <iostream>

#include "TestUtil.hpp"

using namespace novas;

int main() {
  TestUtil test = TestUtil("Evolution");

  int n = 0;


  if(!test.check("invalid(pos)", !Evolution(NAN, -2.0, 3.0).is_valid())) n++;
  if(!test.check("invalid(vel)", !Evolution(1.0, NAN, 3.0).is_valid())) n++;
  if(!test.check("invalid(acc)", !Evolution(1.0, -2.0, NAN).is_valid())) n++;

  Evolution e(1.0, -2.0, 3.0);
  if(!test.check("is_valid()", e.is_valid())) n++;
  if(!test.equals("value(0.0)", e.value(Interval(0.0)), 1.0)) n++;
  if(!test.equals("rate(0.0)", e.rate(Interval(0.0)), -2.0)) n++;
  if(!test.equals("acceleration(0.0)", e.acceleration(), 3.0)) n++;
  if(!test.equals("value(1.0)", e.value(Interval(1.0)), 2.0)) n++;
  if(!test.equals("rate(1.0)", e.rate(Interval(1.0)), 1.0)) n++;
  if(!test.equals("value(-1.0)", e.value(Interval(-1.0)), 6.0)) n++;
  if(!test.equals("rate(-1.0)", e.rate(Interval(-1.0)), -5.0)) n++;

  e = Evolution::stationary(1.23);
  if(!test.check("stationary()", e.is_valid())) n++;
  if(!test.equals("value(stationary)", e.value(Interval(1.0)), 1.23)) n++;
  if(!test.equals("rate(stationary)", e.rate(Interval(1.0)), 0.0)) n++;
  if(!test.equals("acceleration(stationary)", e.acceleration(), 0.0)) n++;

  test = TestUtil("HorizontalTrack");

  Evolution xe(NAN);
  Evolution lon(1.0, 2.0), lat(-1.1, 2.1), r(1.2, -2.2);

  if(!test.check("invalid(time)", !HorizontalTrack(Time::invalid(), Interval(1.0), lon, lat, r).is_valid())) n++;
  if(!test.check("invalid(range)", !HorizontalTrack(Time::j2000(), Interval(NAN), lon, lat, r).is_valid())) n++;
  if(!test.check("invalid(lon)", !HorizontalTrack(Time::j2000(), Interval(1.0), xe, lat, r).is_valid())) n++;
  if(!test.check("invalid(lat)", !HorizontalTrack(Time::j2000(), Interval(1.0), lon, xe, r).is_valid())) n++;
  if(!test.check("invalid(r)", !HorizontalTrack(Time::j2000(), Interval(1.0), lon, lat, xe).is_valid())) n++;

  Interval ip(1.0 / Constant::pi);
  Time t1 = Time::j2000() + ip;

  HorizontalTrack h(Time::j2000(), Interval(1.0), lon, lat, r);
  if(!test.check("is_valid()", h.is_valid())) n++;
  if(!test.check("reference_time()", h.reference_time() == Time::j2000())) n++;
  if(!test.check("range()", h.range() == Interval(1.0))) n++;
  if(!test.check("is_valid_at(OK)", h.is_valid_at(t1))) n++;
  if(!test.check("is_valid_at(!)", !h.is_valid_at(t1 + 1.0))) n++;

  if(!test.equals("longitude_evolution()", h.longitude_evolution().value(ip), lon.value(ip), 1e-15)) n++;
  if(!test.equals("latitude_evolution()", h.latitude_evolution().value(ip), lat.value(ip), 1e-15)) n++;
  if(!test.equals("distance_evolution()", h.distance_evolution().value(ip), r.value(ip), 1e-15)) n++;

  std::optional<Angle> oa = h.longitude_at(t1);
  if(!test.check("longitude_at()", oa.has_value())) n++;
  if(!test.equals("longitude_at().value()", oa.value().rad(), lon.value(ip), 1e-15)) n++;

  oa = h.latitude_at(t1);
  if(!test.check("latitude_at()", oa.has_value())) n++;
  if(!test.equals("latitude_at().value()", oa.value().rad(), lat.value(ip), 1e-15)) n++;

  std::optional<Coordinate> oc = h.distance_at(t1);
  if(!test.check("distance_at()", oc.has_value())) n++;
  if(!test.equals("distance_at().value()", oc.value().m(), r.value(ip), 1e-15)) n++;

  std::optional<ScalarVelocity> ov = h.radial_velocity_at(t1);
  if(!test.check("radial_velocity_at()", ov.has_value())) n++;
  if(!test.equals("radial_velocity_at().value()", ov.value().m_per_s(), r.rate(ip), 1e-15)) n++;

  if(!test.equals("redshift_at()", h.redshift_at(t1), novas_v2z(r.rate(ip) / (Unit::km / Unit::s)), 1e-15)) n++;

  std::optional<Horizontal> oh = h.projected_at(t1);
  if(!test.check("projected_at()", oh.has_value())) n++;
  if(!test.equals("projected_at().azimuth()", oh.value().azimuth().rad(), lon.value(ip), 1e-15)) n++;
  if(!test.equals("projected_at().elevation()", oh.value().elevation().rad(), lat.value(ip), 1e-15)) n++;


  Time t2 = Time::j2000() - Interval(1.1);
  if(!test.check("longitude_at(range)", !h.longitude_at(t2).has_value())) n++;
  if(!test.check("latitude_at(range)", !h.latitude_at(t2).has_value())) n++;
  if(!test.check("distance_at(range)", !h.distance_at(t2).has_value())) n++;
  if(!test.check("radial_velocity_at(range)", !h.radial_velocity_at(t2).has_value())) n++;
  if(!test.check("redshift_at(range)", isnan(h.redshift_at(t2)))) n++;
  if(!test.check("projected_at(range)", !h.projected_at(t2).has_value())) n++;



  test = TestUtil("EquatorialTrack");

  std::cout << "Track.cpp: " << (n > 0 ? "FAILED" : "OK") << "\n";
  return n;
}
