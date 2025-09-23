/**
 *
 * @author G. Kaplan and A. Kovacs
 *
 *  SuperNOVAS header for managing 1997 version of JPL ephemerides specifically for solsys1.c.
 *
 *  Based on the NOVAS C Edition, Version 3.1:
 *
 *  U. S. Naval Observatory<br>
 *  Astronomical Applications Dept.<br>
 *  Washington, DC<br>
 *  <a href="http://www.usno.navy.mil/USNO/astronomical-applications">
 *  http://www.usno.navy.mil/USNO/astronomical-applications</a>
 *
 * @sa eph_manager.c
 * @sa solsys1.c
 */

#ifndef _EPHMAN_
#define _EPHMAN_

#include <stdio.h>

#if COMPAT
#  include <math.h>
#  include <stdlib.h>
#endif


/**
 * Planet codes for JPL DE ephemeris files.
 *
 * @sa DE_PLANETS, eph_manager.c
 *
 * @author Attila Kovacs
 * @since 1.0
 *
 * @sa planet_ephemeris()
 */
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

/**
 * Number planets defined for eph_manager.c functions.
 *
 * @sa de_planet
 *
 * @author Attila Kovacs
 * @since 1.0
 */
#define DE_PLANETS (DE_NUTATIONS + 1)


#ifndef _EXCLUDE_DEPRECATED

/// \cond PRIVATE
// External variables ------------------------------>

/// @deprecated (<i>for internal use</i>) This variable should never have been exposed to users.
///             But since NOVAS C did, we follow.
extern short KM;

/// @deprecated (<i>for internal use</i>) This variable should never have been exposed to users.
///             But since NOVAS C did, we follow.
extern int IPT[3][12];

/// @deprecated (<i>for internal use</i>) This variable should never have been exposed to users.
///             But since NOVAS C did, we follow.
extern int LPT[3];

/// @deprecated (<i>for internal use</i>) This variable should never have been exposed to users.
///             But since NOVAS C did, we follow.
extern long NRL;

/// @deprecated (<i>for internal use</i>) This variable should never have been exposed to users.
///             But since NOVAS C did, we follow.
extern long NP;

/// @deprecated (<i>for internal use</i>) This variable should never have been exposed to users.
///             But since NOVAS C did, we follow.
extern long NV;

/// @deprecated (<i>for internal use</i>) This variable should never have been exposed to users.
///             But since NOVAS C did, we follow.
extern long RECORD_LENGTH;

/// @deprecated (<i>for internal use</i>) This variable should never have been exposed to users.
///             But since NOVAS C did, we follow.
extern double SS[3];

/// @deprecated (<i>for internal use</i>) This variable should never have been exposed to users.
///             But since NOVAS C did, we follow.
extern double JPLAU;

/// @deprecated (<i>for internal use</i>) This variable should never have been exposed to users.
///             But since NOVAS C did, we follow.
extern double PC[18];

/// @deprecated (<i>for internal use</i>) This variable should never have been exposed to users.
///             But since NOVAS C did, we follow.
extern double VC[18];

/// @deprecated (<i>for internal use</i>) This variable should never have been exposed to users.
///             But since NOVAS C did, we follow.
extern double TWOT;

/// @deprecated (<i>for internal use</i>) This variable should never have been exposed to users.
///             But since NOVAS C did, we follow.
extern double EM_RATIO;

/// @deprecated (<i>for internal use</i>) This variable should never have been exposed to users.
///             But since NOVAS C did, we follow.
extern double *BUFFER;

/// @deprecated (<i>for internal use</i>) This variable should never have been exposed to users.
///             But since NOVAS C did, we follow.
extern FILE *EPHFILE;


short state(const double *jed, enum de_planet target, double *target_pos, double *target_vel);

int interpolate(const double *buf, const double *t, long ncf, long na, double *position, double *velocity);

int split(double tt, double *fr);

/// \endcond


short ephem_open(const char *ephem_name, double *jd_begin, double *jd_end, short *de_number);

short ephem_close(void);

short planet_ephemeris(const double tjd[2], enum de_planet target, enum de_planet origin, double *position, double *velocity);


#endif /* _EXCLUDE_DEPRECATED */

#endif /* _EPHMAN_ */
