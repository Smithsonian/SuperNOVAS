/**
 *  Legacy NOVAS C ephemeris provider module, using the JPL PLEPH (Fortran) library via the jplint() / jplihp()
 *  interface defined in `jplint.f`.
 */

#ifndef LEGACY_SOLSYS1_H_
#define LEGACY_SOLSYS1_H_

short planet_jplint(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *restrict position,
        double *restrict velocity);

short planet_jplint_hp(const double jd_tdb[restrict 2], enum novas_planet body, enum novas_origin origin,
        double *restrict position, double *restrict velocity);


#endif /* LEGACY_SOLSYS1_H_ */
