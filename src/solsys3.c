/*
 Naval Observatory Vector Astrometry Software (NOVAS)
 C Edition, Version 3.1

 solsys3.c: Self-contained Sun-Earth ephemeris

 U. S. Naval Observatory
 Astronomical Applications Dept.
 Washington, DC
 http://www.usno.navy.mil/USNO/astronomical-applications
 */

#ifndef _NOVAS_
#include "novas.h"
#endif

#include <math.h>
#include <string.h>

/// \cond PRIVATE
#define T0        NOVAS_T0
/// \endcond

/*
 Additional function prototype.
 */

void sun_eph(double jd, double *ra, double *dec, double *dis);

/********solarsystem */

short int solarsystem3(double jd_tdb, enum novas_major_planet body, enum novas_origin origin, double *position, double *velocity)
/*
 ------------------------------------------------------------------------

 PURPOSE:
 Provides the position and velocity of the Earth at epoch 'jd_tdb'
 by evaluating a closed-form theory without reference to an
 external file.  This function can also provide the position
 and velocity of the Sun.

 REFERENCES:
 Kaplan, G. H. "NOVAS: Naval Observatory Vector Astrometry
 Subroutines"; USNO internal document dated 20 Oct 1988;
 revised 15 Mar 1990.
 Explanatory Supplement to The Astronomical Almanac (1992).

 INPUT
 ARGUMENTS:
 jd_tdb (double)
 TDB Julian date.
 body (short int)
 Body identification number.
 Set 'body' = 0 or 'body' = 1 or 'body' = 10 for the Sun.
 Set 'body' = 2 or 'body' = 3 for the Earth.
 origin (short int)
 Origin code
 = 0 ... solar system barycenter
 = 1 ... center of mass of the Sun
 OUTPUT
 ARGUMENTS:
 position[3] (double)
 Position vector of 'body' at 'jd_tdb'; equatorial rectangular
 coordinates in AU referred to the mean equator and equinox
 of J2000.0.
 velocity[3] (double)
 Velocity vector of 'body' at 'jd_tdb'; equatorial rectangular
 system referred to the mean equator and equinox of J2000.0,
 in AU/Day.

 RETURNED
 VALUE:
 (short int)
 0...Everything OK.
 1...Input Julian date ('jd_tdb') out of range.
 2...Invalid value of 'body'.

 GLOBALS
 USED:
 T0, TWOPI.       novascon.c

 FUNCTIONS
 CALLED:
 sun_eph          solsys3.c
 radec2vector     novas.c
 precession       novas.c
 sin              math.h
 cos              math.h
 fabs             math.h
 fmod             math.h

 VER./DATE/
 PROGRAMMER:
 V1.0/05-96/JAB (USNO/AA) Convert to C; substitute new theory of
 Sun.
 V1.1/06-98/JAB (USNO/AA) Updated planetary masses & mean elements.
 V1.2/04-09/JAB (USNO/AA) Updated value of obliquity at J2000.0.
 V1.3/11-09/WKP (USNO/AA) Updated output argument names to
 'position' and 'velocity'.
 V1.4/11-09/JAB (USNO/AA) Update barycenter computation.
 V1.5/02-11/JLB (USNO/AA) Reformatted description of origin for
 consistency with other documentation.


 NOTES:
 1. This function is the "C" version of Fortran NOVAS routine
 'solsys' version 3.

 ------------------------------------------------------------------------
 */
{
  short int i;

  /*
   The arrays below contain masses and orbital elements for the four
   largest planets -- Jupiter, Saturn, Uranus, and Neptune --  (see
   Explanatory Supplement (1992), p. 316) with angles in radians.  These
   data are used for barycenter computations only.
   */

  static const double pm[4] = { 1047.349, 3497.898, 22903.0, 19412.2 };
  static const double pa[4] = { 5.203363, 9.537070, 19.191264, 30.068963 };
  static const double pe[4] = { 0.048393, 0.054151, 0.047168, 0.008586 };
  static const double pj[4] = { 0.022782, 0.043362, 0.013437, 0.030878 };
  static const double po[4] = { 1.755036, 1.984702, 1.295556, 2.298977 };
  static const double pw[4] = { 0.257503, 1.613242, 2.983889, 0.784898 };
  static const double pl[4] = { 0.600470, 0.871693, 5.466933, 5.321160 };
  static const double pn[4] = { 1.450138e-3, 5.841727e-4, 2.047497e-4, 1.043891e-4 };

  /*
   'obl' is the obliquity of ecliptic at epoch J2000.0 in degrees.
   */

  static const double obl = 23.4392794444;

  static double tlast = 0.0;
  static double tmass, a[3][4], b[3][4], vbary[3];

  /*
   Initialize constants.
   Initial value of 'tmass' is mass of Sun plus four inner planets.
   */

  if(tlast < 1.0) {
    const double oblr = obl * TWOPI / 360.0;
    const double se = sin(oblr);
    const double ce = cos(oblr);

    tmass = 1.0 + 5.977e-6;

    for(i = 0; i < 4; i++) {
      /*
       Compute sine and cosine of orbital angles.
       */
      const double si = sin(pj[i]);
      const double ci = cos(pj[i]);
      const double sn = sin(po[i]);
      const double cn = cos(po[i]);
      const double sw = sin(pw[i] - po[i]);
      const double cw = cos(pw[i] - po[i]);
      /*
       Compute p and q vectors (see Brouwer & Clemence (1961), Methods of
       Celestial Mechanics, pp. 35-36.)
       */

      const double p1 = cw * cn - sw * sn * ci;
      const double p2 = (cw * sn + sw * cn * ci) * ce - sw * si * se;
      const double p3 = (cw * sn + sw * cn * ci) * se + sw * si * ce;
      const double q1 = -sw * cn - cw * sn * ci;
      const double q2 = (-sw * sn + cw * cn * ci) * ce - cw * si * se;
      const double q3 = (-sw * sn + cw * cn * ci) * se + cw * si * ce;
      const double roote = sqrt(1.0 - pe[i] * pe[i]);

      a[0][i] = pa[i] * p1;
      a[1][i] = pa[i] * p2;
      a[2][i] = pa[i] * p3;
      b[0][i] = pa[i] * roote * q1;
      b[1][i] = pa[i] * roote * q2;
      b[2][i] = pa[i] * roote * q3;

      tmass += 1.0 / pm[i];
    }

    tlast = 1.0;
  }

  /*
   Check if input Julian date is within range (within 3 centuries of J2000).
   */

  if((jd_tdb < 2340000.5) || (jd_tdb > 2560000.5)) return 1;

  /*
   Form heliocentric coordinates of the Sun or Earth, depending on
   'body'.  Velocities are obtained from crude numerical differentiation.
   */

  if((body == 0) || (body == 1) || (body == 10)) /* Sun */
  {
    for(i = 0; i < 3; i++)
      position[i] = velocity[i] = 0.0;
  }

  else if((body == 2) || (body == 3)) { /* Earth */
    double p[3][3];
    for(i = 0; i < 3; i++) {
      const double qjd = jd_tdb + (double) (i - 1) * 0.1;
      double ras, decs, diss, pos1[3];

      sun_eph(qjd, &ras, &decs, &diss);
      radec2vector(ras, decs, diss, pos1);
      precession(qjd, pos1, T0, position);
      p[i][0] = -position[0];
      p[i][1] = -position[1];
      p[i][2] = -position[2];
    }
    for(i = 0; i < 3; i++) {
      position[i] = p[1][i];
      velocity[i] = (p[2][i] - p[0][i]) / 0.2;
    }
  }

  else return 2;

  /*
   If 'origin' = 0, move origin to solar system barycenter.

   Solar system barycenter coordinates are computed from Keplerian
   approximations of the coordinates of the four largest planets.
   */

  if(origin == 0) {
    static double pbary[3];

    if(fabs(jd_tdb - tlast) >= 1.0e-06) {

      for(i = 0; i < 3; i++)
        pbary[i] = vbary[i] = 0.0;

      /*
       The following loop cycles once for each of the four planets.
       */

      for(i = 0; i < 4; i++) {
        double pplan[3], vplan[3], f;
        /*
         Compute mean longitude, mean anomaly, and eccentric anomaly.
         */

        const double e = pe[i];
        const double mlon = pl[i] + pn[i] * (jd_tdb - T0);
        const double ma = fmod((mlon - pw[i]), TWOPI);
        const double u = ma + e * sin(ma) + 0.5 * e * e * sin(2.0 * ma);
        const double sinu = sin(u);
        const double cosu = cos(u);

        /*
         Compute velocity factor.
         */

        const double anr = pn[i] / (1.0 - e * cosu);

        /*
         Compute planet's position and velocity wrt eq & eq J2000.
         */

        pplan[0] = a[0][i] * (cosu - e) + b[0][i] * sinu;
        pplan[1] = a[1][i] * (cosu - e) + b[1][i] * sinu;
        pplan[2] = a[2][i] * (cosu - e) + b[2][i] * sinu;
        vplan[0] = anr * (-a[0][i] * sinu + b[0][i] * cosu);
        vplan[1] = anr * (-a[1][i] * sinu + b[1][i] * cosu);
        vplan[2] = anr * (-a[2][i] * sinu + b[2][i] * cosu);

        /*
         Compute mass factor and add in to total displacement.
         */

        f = 1.0 / (pm[i] * tmass);

        pbary[0] += pplan[0] * f;
        pbary[1] += pplan[1] * f;
        pbary[2] += pplan[2] * f;
        vbary[0] += vplan[0] * f;
        vbary[1] += vplan[1] * f;
        vbary[2] += vplan[2] * f;
      }

      tlast = jd_tdb;
    }

    for(i = 0; i < 3; i++) {
      if(position) position[i] -= pbary[i];
      if(velocity) velocity[i] -= vbary[i];
    }
  }

  return 0;
}

/********solarsystem_hp */

short int solarsystem3_hp(const double jd_tdb[2], enum novas_major_planet body, enum novas_origin origin, double *position,
        double *velocity) {

  /*
   Set the value of 'action' according to the rules explained under
   NOTES in the prolog.
   */

  const int action = 1;

  if(action == 2) { /* Return low-precision position and velocity */
    double p[3], v[3];

    int error = solarsystem(jd_tdb[0] + jd_tdb[1], body, origin, p, v);
    if(error) return (error);

    if(position) memcpy(position, p, sizeof(p));
    if(velocity) memcpy(velocity, v, sizeof(v));

    return 0;
  }

  return 3;
}

/********sun_eph */

void sun_eph(double jd,

double *ra, double *dec, double *dis)
/*
 ------------------------------------------------------------------------

 PURPOSE:
 To compute equatorial spherical coordinates of Sun referred to
 the mean equator and equinox of date.

 REFERENCES:
 Bretagnon, P. and Simon, J.L. (1986).  Planetary Programs and
 Tables from -4000 to + 2800. (Richmond, VA: Willmann-Bell).
 Kaplan, G.H. (2005). US Naval Observatory Circular 179.

 INPUT
 ARGUMENTS:
 jd (double)
 Julian date on TDT or ET time scale.

 OUTPUT
 ARGUMENTS:
 ra (double)
 Right ascension referred to mean equator and equinox of date
 (hours).
 dec (double)
 Declination referred to mean equator and equinox of date
 (degrees).
 dis (double)
 Geocentric distance (AU).

 RETURNED
 VALUE:
 None.

 GLOBALS
 USED:
 T0, TWOPI, ASEC2RAD

 FUNCTIONS
 CALLED:
 sin           math.h
 cos           math.h
 asin          math.h
 atan2         math.h

 VER./DATE/
 PROGRAMMER:
 V1.0/08-94/JAB (USNO/AA)
 V1.1/05-96/JAB (USNO/AA): Compute mean coordinates instead of
 apparent.
 V1.2/01-07/JAB (USNO/AA): Use 'ASEC2RAD' instead of 'RAD2SEC'.
 V1.3/04-09/JAB (USNO/AA): Update the equation for mean
 obliquity of the ecliptic, and correct
 longitude based on a linear fit to DE405
 in the interval 1900-2100 (see notes).

 NOTES:
 1. Quoted accuracy is 2.0 + 0.03 * T^2 arcsec, where T is
 measured in units of 1000 years from J2000.0.  See reference.
 2. The obliquity equation is updated to equation 5.12 of the
 second reference.
 3. The linear fit to DE405 primarily corrects for the
 difference between "old" (Lieske) and "new" (IAU 2006)
 precession.  The difference, new - old, is -0.3004 arcsec/cy.

 ------------------------------------------------------------------------
 */
{
  short int i;

  double sum_lon = 0.0;
  double sum_r = 0.0;
  const double factor = 1.0e-07;
  double u, lon, t, emean, sin_lon;

  struct sun_con {
    double l;
    double r;
    double alpha;
    double nu;
  };

  static const struct sun_con con[50] = { { 403406.0, 0.0, 4.721964, 1.621043 }, { 195207.0, -97597.0, 5.937458, 62830.348067 }, { 119433.0,
          -59715.0, 1.115589, 62830.821524 }, { 112392.0, -56188.0, 5.781616, 62829.634302 }, { 3891.0, -1556.0, 5.5474, 125660.5691 }, {
          2819.0, -1126.0, 1.5120, 125660.9845 }, { 1721.0, -861.0, 4.1897, 62832.4766 }, { 0.0, 941.0, 1.163, 0.813 }, { 660.0, -264.0,
          5.415, 125659.310 }, { 350.0, -163.0, 4.315, 57533.850 }, { 334.0, 0.0, 4.553, -33.931 }, { 314.0, 309.0, 5.198, 777137.715 }, {
          268.0, -158.0, 5.989, 78604.191 }, { 242.0, 0.0, 2.911, 5.412 }, { 234.0, -54.0, 1.423, 39302.098 },
          { 158.0, 0.0, 0.061, -34.861 }, { 132.0, -93.0, 2.317, 115067.698 }, { 129.0, -20.0, 3.193, 15774.337 }, { 114.0, 0.0, 2.828,
                  5296.670 }, { 99.0, -47.0, 0.52, 58849.27 }, { 93.0, 0.0, 4.65, 5296.11 }, { 86.0, 0.0, 4.35, -3980.70 }, { 78.0, -33.0,
                  2.75, 52237.69 }, { 72.0, -32.0, 4.50, 55076.47 }, { 68.0, 0.0, 3.23, 261.08 }, { 64.0, -10.0, 1.22, 15773.85 }, { 46.0,
                  -16.0, 0.14, 188491.03 }, { 38.0, 0.0, 3.44, -7756.55 }, { 37.0, 0.0, 4.37, 264.89 }, { 32.0, -24.0, 1.14, 117906.27 }, {
                  29.0, -13.0, 2.84, 55075.75 }, { 28.0, 0.0, 5.96, -7961.39 }, { 27.0, -9.0, 5.09, 188489.81 },
          { 27.0, 0.0, 1.72, 2132.19 }, { 25.0, -17.0, 2.56, 109771.03 }, { 24.0, -11.0, 1.92, 54868.56 }, { 21.0, 0.0, 0.09, 25443.93 }, {
                  21.0, 31.0, 5.98, -55731.43 }, { 20.0, -10.0, 4.03, 60697.74 }, { 18.0, 0.0, 4.27, 2132.79 }, { 17.0, -12.0, 0.79,
                  109771.63 }, { 14.0, 0.0, 4.24, -7752.82 }, { 13.0, -5.0, 2.01, 188491.91 }, { 13.0, 0.0, 2.65, 207.81 }, { 13.0, 0.0,
                  4.98, 29424.63 }, { 12.0, 0.0, 0.93, -7.99 }, { 10.0, 0.0, 2.21, 46941.14 }, { 10.0, 0.0, 3.59, -68.29 }, { 10.0, 0.0,
                  1.50, 21463.25 }, { 10.0, -9.0, 2.55, 157208.40 } };

  /*
   Define the time units 'u', measured in units of 10000 Julian years
   from J2000.0, and 't', measured in Julian centuries from J2000.0.
   */

  u = (jd - T0) / 3652500.0;
  t = u * 100.0;

  /*
   Compute longitude and distance terms from the series.
   */

  for(i = 0; i < 50; i++) {
    const double arg = con[i].alpha + con[i].nu * u;
    sum_lon += con[i].l * sin(arg);
    sum_r += con[i].r * cos(arg);
  }

  /*
   Compute longitude, latitude, and distance referred to mean equinox
   and ecliptic of date.  Apply correction to longitude based on a
   linear fit to DE405 in the interval 1900-2100.
   */

  lon = 4.9353929 + 62833.1961680 * u + factor * sum_lon;
  lon += ((-0.1371679461 - 0.2918293271 * t) * ASEC2RAD);

  lon = fmod(lon, TWOPI);
  if(lon < 0.0) lon += TWOPI;

  *dis = 1.0001026 + factor * sum_r;

  /*
   Compute mean obliquity of the ecliptic.
   */

  emean = (84381.406 + (-46.836769 + (-0.0001831 + 0.00200340 * t) * t) * t) * ASEC2RAD;

  /*
   Compute equatorial spherical coordinates referred to the mean equator
   and equinox of date.
   */

  sin_lon = sin(lon);
  *ra = atan2((cos(emean) * sin_lon), cos(lon)) * RAD2DEG;
  *ra = fmod(*ra, 360.0);
  if(*ra < 0.0) *ra += 360.0;
  *ra = *ra / 15.0;

  *dec = asin(sin(emean) * sin_lon) * RAD2DEG;

  return;
}

#ifdef DEFAULT_SOLSYS
short int default_solarsystem(double jd_tdb, enum novas_major_planet body, enum novas_origin origin, double *position, double *velocity) {
  return solarsystem3(jd_tdb, body, origin, position, velocity);
}

short int default_solarsystem_hp(const double jd_tdb[2], enum novas_major_planet body, enum novas_origin origin, double *position, double *velocity) {
  return solarsystem3_hp(jd_tdb, body, origin, position, velocity);
}
#else
short int solarsystem(double jd_tdb, short int body, short int origin, double *position, double *velocity) {
  return solarsystem3(jd_tdb, body, origin, position, velocity);
}

short int solarsystem_hp(const double jd_tdb[2], short int body, short int origin, double *position, double *velocity) {
  return solarsystem3_hp(jd_tdb, body, origin, position, velocity);
}
#endif

