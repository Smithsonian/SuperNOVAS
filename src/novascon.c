/**
 * @file
 *
 * @author G. Kaplan and A. Kovacs
 *
 *  SuperNOVAS implementation for numerical constants that used to be used internally in novas.c.
 *
 *  @deprecated Use your own version for the selection of the constant you need, expressed in whatever units your application
 *              desires. We should not force you to adopt the internally used convention of NOVAS, not to mention the high
 *              chance of namespace conglicts with the super-simplistic naming scheme here. You are better off without this.
 *
 *  C Edition, Version 3.1,  U. S. Naval Observatory
 *  Astronomical Applications Dept.
 *  Washington, DC
 *  <a href="http://www.usno.navy.mil/USNO/astronomical-applications">http://www.usno.navy.mil/USNO/astronomical-applications</a>
 */
#include "novas.h"
#include "novascon.h"

const double T0 = NOVAS_JD_J2000;
const double C = NOVAS_C;
const double AU_SEC = NOVAS_AU_SEC;
const double C_AUDAY = NOVAS_C_AU_PER_DAY;
const double AU = NOVAS_AU;
const double AU_KM = NOVAS_AU_KM;
const double GS = NOVAS_GS;
const double GE = NOVAS_GE;
const double ERAD = NOVAS_ERAD;
const double F = NOVAS_EFLAT;
const double ANGVEL = NOVAS_ANGVEL;
const double RMASS[12] = NOVAS_RMASS_INIT;











