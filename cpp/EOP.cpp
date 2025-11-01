/**
 * @file
 *
 * @date Created  on Sep 30, 2025
 * @author Attila Kovacs
 */

#include "supernovas.h"


namespace supernovas {

class EOP {
private:
  int _leap = 0;
  Angle _xp, _yp;
  double _t;

  double _dxp = 0.0, _dyp = 0.0, _dt = 0.0;  // [mas, s] Applied corrections, in novas units.

public:

  EOP(int leap_seconds, double dut1 = 0.0, double xp = 0.0, double yp = 0.0)
  : _leap(leap_seconds), _xp(xp), _yp(yp), _t(dut1) {}

  EOP(int leap_seconds, double dut1, const Angle& xp, const Angle& yp)
  : EOP(leap_seconds, dut1, xp.rad(), yp.rad()) {}

  int leap_seconds() const {
    return _leap;
  }

  const Angle& xp() const {
    return _xp;
  }

  const Angle& yp() const {
    return _yp;
  }

  double dUT1() const {
    return _t;
  }

  EOP itrf_transformed(int from_year, int to_year) const {
    double xp1, yp1, t1;
    novas_itrf_transform_eop(from_year, _xp.arcsec(), _yp.arcsec(), _t, to_year, &xp1, &yp1, &t1);
    return EOP(_leap, t1, xp1 * Unit::arcsec, yp1 * Unit::arcsec);
  }

  EOP diurnal_corrected(const Time& time) const {
    double dxp, dyp, dt;
    novas_diurnal_eop_at_time(time._novas_timespec(), &dxp, &dyp, &dt);

    // Apply correction rel. to prior correction.
    EOP eop = EOP(_leap, _t + dt - _dt, (_xp.rad() + dxp - _dxp) * Unit::arcsec, (_yp.rad() + dyp - _dyp) * Unit::arcsec);

    eop._dxp = dxp;
    eop._dyp = dyp;
    eop._dt = dt;
    return eop;
  }
};
} // namespace supernovas


