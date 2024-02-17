/**
 *
 * @author G. Kaplan and A. Kovacs
 *
 *  SuperNOVAS header for managing 1997 version of JPL ephemerides specifically for solsys2.c.
 *
 *  Based on the NOVAS C Edition, Version 3.1,  U. S. Naval Observatory
 *  Astronomical Applications Dept.
 *  Washington, DC
 *  <a href="http://www.usno.navy.mil/USNO/astronomical-applications">http://www.usno.navy.mil/USNO/astronomical-applications</a>
 */

#ifndef _EPHMAN_
#define _EPHMAN_

#include <stdio.h>

#if COMPAT
#  include <math.h>
#  include <stdlib.h>
#endif


/// Planet codes for JPL DE ephemeris files.
enum de_planet {
  DE_MERCURY = 0, ///< Number for Mercury in the JPL DE ephemeris files
  DE_VENUS,       ///< Number for Venus in the JPL DE ephemeris files
  DE_EARTH,       ///< Number for Earth in the JPL DE ephemeris files
  DE_MARS,        ///< Number for Mars in the JPL DE ephemeris files
  DE_JUPITER,     ///< Number for Jupiter in the JPL DE ephemeris files
  DE_SATURN,      ///< Number for Saturn in the JPL DE ephemeris files
  DE_URANUS,      ///< Number for Uranus in the JPL DE ephemeris files
  DE_NEPTUNE,     ///< Number for Neptune in the JPL DE ephemeris files
  DE_PLUTO,       ///< Number for Pluto in the JPL DE ephemeris files
  DE_MOON,        ///< Number for Moon in the JPL DE ephemeris files
  DE_SUN,         ///< Number for Sun in the JPL DE ephemeris files
  DE_SSB,         ///< Number for Solar System Barycenter (SSB) in the JPL DE ephemeris files
  DE_EMB,         ///< Number for Earth-Moon Barycenter Earth in the JPL DE ephemeris files
  DE_NUTATIONS    ///< Number for Nutations in the JPL DE ephemeris files
};


#define DE_PLANETS (DE_NUTATIONS + 1)


// External variables
extern short KM;

extern int IPT[3][12], LPT[3];

extern long NRL, NP, NV;
extern long RECORD_LENGTH;

extern double SS[3], JPLAU, PC[18], VC[18], TWOT, EM_RATIO;
extern double *BUFFER;

extern FILE *EPHFILE;


short ephem_open(const char *ephem_name, double *jd_begin, double *jd_end, short *de_number);

short ephem_close(void);

short planet_ephemeris(const double tjd[2], enum de_planet target, enum de_planet origin, double *position, double *velocity);

short state(const double *jed, enum de_planet target, double *target_pos, double *target_vel);

int interpolate(const double *buf, const double *t, long ncm, long na, double *position, double *velocity);

int split(double tt, double *fr);

#endif
