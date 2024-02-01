/**
 *
 * @author G. Kaplan and A. Kovacs
 *
 *  SuperNOVAS header for managing 1997 version of JPL ephemerides specifically for solsys2.c.
 *
 *  Basded on the NOVAS C Edition, Version 3.1,  U. S. Naval Observatory
 *  Astronomical Applications Dept.
 *  Washington, DC
 *  <a href="http://www.usno.navy.mil/USNO/astronomical-applications">http://www.usno.navy.mil/USNO/astronomical-applications</a>
 */

#ifndef _EPHMAN_
#define _EPHMAN_

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/*
   External variables
 */

extern short KM;

extern int IPT[3][12], LPT[3];

extern long  NRL, NP, NV;
extern long RECORD_LENGTH;

extern double SS[3], JPLAU, PC[18], VC[18], TWOT, EM_RATIO;
extern double *BUFFER;

extern FILE *EPHFILE;

/*
   Function prototypes
 */

short ephem_open (const char *ephem_name,
        double *jd_begin, double *jd_end,
        short *de_number);

short ephem_close (void);

short planet_ephemeris (const double tjd[2], short target,
        short center,
        double *position, double *velocity);

short state (const double *jed, short target,
        double *target_pos, double *target_vel);

void interpolate (const double *buf, const double *t, long ncm, long na,
        double *position, double *velocity);

void split (double tt, double *fr);

#endif
