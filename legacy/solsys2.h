/**
 * @file
 *
 * @date Created  on Sep 24, 2025
 * @author Attila Kovacs
 *
 *   Legacy NOVAS C ephemeris provider interface via `eph_manager`, supporting JPL planetary ephemerides DE200 --
 *   DE421.
 */

#ifndef SOLSYS2_H_
#define SOLSYS2_H_

short planet_eph_manager(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *restrict position,
        double *restrict velocity);

short planet_eph_manager_hp(const double jd_tdb[restrict 2], enum novas_planet body, enum novas_origin origin,
        double *restrict position, double *restrict velocity);

#endif /* SOLSYS2_H_ */
