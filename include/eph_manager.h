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

extern short int KM;

extern int IPT[3][12], LPT[3];

extern long int  NRL, NP, NV;
extern long int RECORD_LENGTH;

extern double SS[3], JPLAU, PC[18], VC[18], TWOT, EM_RATIO;
extern double *BUFFER;

extern FILE *EPHFILE;

/*
   Function prototypes
 */

short int ephem_open (const char *ephem_name,
        double *jd_begin, double *jd_end,
        short int *de_number);

short int ephem_close (void);

short int planet_ephemeris (const double tjd[2], short int target,
        short int center,
        double *position, double *velocity);

short int state (const double *jed, short int target,
        double *target_pos, double *target_vel);

void interpolate (const double *buf, const double *t, long int ncm, long int na,
        double *position, double *velocity);

void split (double tt, double *fr);

#endif
