/**
 * @file
 *
 * @date Created  on Feb 18, 2024
 * @author Attila Kovacs
 */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <string.h>

#include "novas.h"

#define J2000   2451545.0


static observer obs;
static object source;
static double tdb = J2000;
static double ut12tt = 69.0;

static double xp = 1.0;
static double yp = -2.0;

// Initialized quantities.
static double pos0[3];

static enum novas_origin ephem_origin;

static short dummy_planet_hp(const double *jd_tdb, enum novas_planet body, enum novas_origin origin, double *position, double *velocity) {
  memset(position, 0, 3 * sizeof(double));
  memset(velocity, 0, 3 * sizeof(double));
  position[0] = body % 10;
  velocity[1] = 0.01 * (body % 10);
  return 0;
}

static short dummy_planet(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position, double *velocity) {
  double tdb2[2] = { tdb };
  return dummy_planet_hp(tdb2, body, origin, position, velocity);
}

static int dummy_ephem(const char *name, long id, double jd_tdb_high, double jd_tdb_low, enum novas_origin *origin, double *pos, double *vel) {
  *origin = ephem_origin;
  memset(pos, 0, 3 * sizeof(double));
  memset(vel, 0, 3 * sizeof(double));
  pos[0] = id % 100;
  vel[1] = 0.01 * (id % 100);
  return 0;
}

static int check_equal_pos(const double *posa, const double *posb, double tol) {
  int i;

  for(i = 0; i < 3; i++) if(fabs(posa[i] - posb[i]) > tol) {
    fprintf(stderr, "  A[%d] = %.9g vs B[%d] = %.9g\n", i, posa[i], i, posb[i]);
    return i + 1;
  }
  return 0;
}

static int is_ok(const char *func, int error) {
  if(error) fprintf(stderr, "ERROR %d! %s (source = %s, from = %d)\n", error, func, source.name, obs.where);
  return !error;
}

static double vlen(double *pos) {
  return sqrt(pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2]);
}

static int test_gcrs_j2000_gcrs() {
  double pos1[3];

  if(!is_ok("gcrs_to_j2000", gcrs_to_j2000(pos0, pos1))) return 1;
  if(!is_ok("j2000_to_gcrs", j2000_to_gcrs(pos1, pos1))) return 1;
  if(!is_ok("gcrs_j2000_gcrs", check_equal_pos(pos0, pos1, 1e-9 * vlen(pos0)))) return 1;
  return 0;
}

static int test_j2000_tod_j2000() {
  double pos1[3];

  if(!is_ok("j2000_to_tod", j2000_to_tod(tdb, 0, pos0, pos1))) return 1;
  if(!is_ok("tod_to_j2000", tod_to_j2000(tdb, 0, pos1, pos1))) return 1;
  if(!is_ok("j2000_tod_j2000", check_equal_pos(pos0, pos1, 1e-9 * vlen(pos0)))) return 1;
  return 0;
}

static int test_tod_itrs_tod() {
  double pos1[3];

  if(!is_ok("tod_to_itrs", tod_to_itrs(tdb, 0.0, ut12tt, 0, xp, yp, pos0, pos1))) return 1;
  if(!is_ok("itrs_to_tod", itrs_to_tod(tdb, 0.0, ut12tt, 0, xp, yp, pos1, pos1))) return 1;
  if(!is_ok("tod_itrs_tod", check_equal_pos(pos0, pos1, 1e-9 * vlen(pos0)))) return 1;
  return 0;
}

static int test_gcrs_cirs_gcrs() {
  double pos1[3];

  if(!is_ok("gcrs_to_cirs", gcrs_to_cirs(tdb, 0, pos0, pos1))) return 1;
  if(!is_ok("cirs_to_gcrs", cirs_to_gcrs(tdb, 0, pos1, pos1))) return 1;
  if(!is_ok("gcrs_cirs_gcrs", check_equal_pos(pos0, pos1, 1e-9 * vlen(pos0)))) return 1;
  return 0;
}


static int test_cirs_itrs_cirs() {
  double pos1[3];

  if(!is_ok("cirs_to_itrs", cirs_to_itrs(tdb, 0.0, ut12tt, 0, xp, yp, pos0, pos1))) return 1;
  if(!is_ok("itrs_to_cirs", itrs_to_cirs(tdb, 0.0, ut12tt, 0, xp, yp, pos1, pos1))) return 1;
  if(!is_ok("cirs_itrs_cirs", check_equal_pos(pos0, pos1, 1e-9 * vlen(pos0)))) return 1;
  return 0;
}

static int test_itrs_hor_itrs() {
  int a;

  if(obs.where != NOVAS_OBSERVER_ON_EARTH) return 0;

  for(a = 0; a < 360; a += 30) {
    double az = 0.0, za = 0.0, p[3] = {}, pos1[3];

    p[0] = cos(a * DEG2RAD);
    p[1] = sin(a * DEG2RAD);

    if(!is_ok("itrs_to_hor", itrs_to_hor(&obs.on_surf, p, &az, &za))) return 1;
    if(!is_ok("hor_to_itrs", hor_to_itrs(&obs.on_surf, az, za, pos1))) return 1;
    if(!is_ok("itrs_hor_itrs", check_equal_pos(p, pos1, 1e-9))) return 1;

    if(!is_ok("itrs_to_hor:az:null", itrs_to_hor(&obs.on_surf, p, NULL, &za))) return 1;
    if(!is_ok("itrs_to_hor:za:null", itrs_to_hor(&obs.on_surf, p, &az, NULL))) return 1;
  }
  return 0;
}

static int test_equ2hor() {
  int a;

  if(obs.where != NOVAS_OBSERVER_ON_EARTH) return 0;

  for(a = 0; a < 24.0; a += 3) {
    int d;
    for(d = -90; d <= 90; d += 30) {
      double ra = a, dec = d, az, za, rar, decr;

      if(!is_ok("itrs_to_hor:rar:null", equ2hor(tdb, 0.0, NOVAS_REDUCED_ACCURACY, 0.0, 0.0, &obs.on_surf, NOVAS_STANDARD_ATMOSPHERE, ra, dec, &az, &za, NULL, &decr))) return 1;
      if(!is_ok("itrs_to_hor:decr:null", equ2hor(tdb, 0.0, NOVAS_REDUCED_ACCURACY, 0.0, 0.0, &obs.on_surf, NOVAS_STANDARD_ATMOSPHERE, ra, dec, &az, &za, &rar, NULL))) return 1;
    }
  }

  return 0;
}

static int test_aberration() {
  double p[3], v[3] = {};

  if(source.type != NOVAS_PLANET) return 0;

  memcpy(p, pos0, sizeof(p));
  if(!is_ok("aberration:corner", aberration(p, v, 0.0, p))) return 1;

  return 0;
}

static int test_starvectors() {
  double p[3], v[3] = {};

  if(source.type != NOVAS_CATALOG_OBJECT) return 0;

  if(!is_ok("starvectors:pos:null", starvectors(&source.star, NULL, v))) return 1;
  if(!is_ok("starvectors:vel:null", starvectors(&source.star, p, NULL))) return 1;

  return 0;
}

static int test_terra() {
  double p[3], v[3];

  if(obs.where != NOVAS_OBSERVER_ON_EARTH) return 0;

  if(!is_ok("terra:pos:null", terra(&obs.on_surf, 0.0, NULL, v))) return 1;
  if(!is_ok("terra:vel:null", terra(&obs.on_surf, 0.0, p, NULL))) return 1;

  return 0;
}

static int test_geo_posvel() {
  double p[3], v[3];

  if(obs.where != NOVAS_OBSERVER_ON_EARTH) return 0;

  if(!is_ok("geo_posvel:pos:null", geo_posvel(tdb, 0.0, NOVAS_FULL_ACCURACY, &obs, NULL, v))) return 1;
  if(!is_ok("geo_posvel:vel:null", geo_posvel(tdb, 0.0, NOVAS_FULL_ACCURACY, &obs, p, NULL))) return 1;

  return 0;
}

static int test_bary2obs() {
  double pobs[3];

  if(obs.where != NOVAS_OBSERVER_ON_EARTH) return 0;

  if(!is_ok("bary2obs:terra", geo_posvel(tdb, 0.0, NOVAS_FULL_ACCURACY, &obs, pobs, NULL))) return 1;
  if(!is_ok("bary2obs:lighttime:null", bary2obs(pos0, pobs, pobs, NULL))) return 1;

  return 0;
}

static int test_tod_vs_cirs() {
  double pos1[3];

  if(!is_ok("gcrs_to_j2000", gcrs_to_j2000(pos0, pos1))) return 1;
  if(!is_ok("j2000_to_tod", j2000_to_tod(tdb, 0, pos1, pos1))) return 1;
  if(!is_ok("tod_to_itrs", tod_to_itrs(tdb, 0.0, ut12tt, 0, xp, yp, pos1, pos1))) return 1;
  if(!is_ok("itrs_to_cirs", itrs_to_cirs(tdb, 0.0, ut12tt, 0, xp, yp, pos1, pos1))) return 1;
  if(!is_ok("cirs_to_gcrs", cirs_to_gcrs(tdb, 0, pos1, pos1))) return 1;

  if(!is_ok("tod_vs_cirs", check_equal_pos(pos0, pos1, 1e-9 * vlen(pos0)))) return 1;

  return 0;
}

static int test_equ_ecl() {
  int a;

  for(a = 0; a < 24; a += 3) {
    double ra0 = a, dec0 = 0.0, elon, elat, ra, dec;

    if(!is_ok("equ2ecl", equ2ecl(tdb, NOVAS_GCRS_EQUATOR, NOVAS_FULL_ACCURACY, ra0, dec0, &elon, &elat))) return 1;
    if(!is_ok("ecl2equ", ecl2equ(tdb, NOVAS_GCRS_EQUATOR, NOVAS_FULL_ACCURACY, elon, elat, &ra, &dec))) return 1;
    if(!is_ok("equ_ecl_equ:ra", fabs(remainder(ra - ra0, 24.0)) > 1e-8)) return 1;
    if(!is_ok("equ_ecl_equ:dec", fabs(dec - dec0) > 1e-7)) return 1;
  }

  return 0;
}


static int test_equ_gal() {
  int a;

  for(a = 0; a < 24; a += 3) {
    double ra0 = a, dec0 = 0.0, glon, glat, ra, dec;

    if(!is_ok("equ2gal", equ2gal(ra0, dec0, &glon, &glat))) return 1;
    if(!is_ok("gal2equ", gal2equ(glon, glat, &ra, &dec))) return 1;
    if(!is_ok("equ_gal_equ:ra", fabs(remainder(ra - ra0, 24.0)) > 1e-8)) return 1;
    if(!is_ok("equ_gal_equ:dec", fabs(dec - dec0) > 1e-7)) return 1;
  }

  return 0;
}

static int test_place_star() {
  int i;

  for(i = 0; i < 4; i++) {
    sky_pos posa = {}, posb = {};
    if(!is_ok("place_star", place_star(tdb, &source.star, &obs, ut12tt, i, 1, &posa))) return 1;
    if(!is_ok("place_star:control", place(tdb, &source, &obs, ut12tt, i, 1, &posb))) return 1;
    if(!is_ok("place_star:check", check_equal_pos(posa.r_hat, posb.r_hat, 1e-9))) return 1;
  }

  return 0;
}


static int test_place_icrs() {
  int i;

  if(obs.where != NOVAS_OBSERVER_AT_GEOCENTER) return 0;

  for(i = 0; i < 4; i++) {
    sky_pos posa = {}, posb = {};
    if(!is_ok("place_icrs", place_icrs(tdb, &source, 1, &posa))) return 1;
    if(!is_ok("place_icrs:control", place(tdb, &source, &obs, ut12tt, NOVAS_ICRS, 1, &posb))) return 1;
    if(!is_ok("place_icrs:check", check_equal_pos(posa.r_hat, posb.r_hat, 1e-9))) return 1;
  }

  return 0;
}

static int test_place_gcrs() {
  int i;

  if(obs.where != NOVAS_OBSERVER_AT_GEOCENTER) return 0;

  for(i = 0; i < 4; i++) {
    sky_pos posa = {}, posb = {};
    if(!is_ok("place_gcrs", place_gcrs(tdb, &source, 1, &posa))) return 1;
    if(!is_ok("place_gcrs:control", place(tdb, &source, &obs, ut12tt, NOVAS_GCRS, 1, &posb))) return 1;
    if(!is_ok("place_gcrs:check", check_equal_pos(posa.r_hat, posb.r_hat, 1e-9))) return 1;
  }

  return 0;
}

static int test_place_cirs() {
  int i;

  if(obs.where != NOVAS_OBSERVER_AT_GEOCENTER) return 0;

  for(i = 0; i < 4; i++) {
    sky_pos posa = {}, posb = {};
    if(!is_ok("place_cirs", place_cirs(tdb, &source, 1, &posa))) return 1;
    if(!is_ok("place_cirs:control", place(tdb, &source, &obs, ut12tt, NOVAS_CIRS, 1, &posb))) return 1;
    if(!is_ok("place_cirs:check", check_equal_pos(posa.r_hat, posb.r_hat, 1e-9))) return 1;
  }

  return 0;
}

static int test_place_tod() {
  int i;

  if(obs.where != NOVAS_OBSERVER_AT_GEOCENTER) return 0;

  for(i = 0; i < 4; i++) {
    sky_pos posa = {}, posb = {};
    if(!is_ok("place_tod", place_tod(tdb, &source, 1, &posa))) return 1;
    if(!is_ok("place_tod:control", place(tdb, &source, &obs, ut12tt, NOVAS_TOD, 1, &posb))) return 1;
    if(!is_ok("place_tod:check", check_equal_pos(posa.r_hat, posb.r_hat, 1e-9))) return 1;
  }

  return 0;
}

static int test_radec_star() {
  int i;

  for(i = 0; i < 4; i++) {
    sky_pos posa = {}, posb = {};
    double ra, dec, rv;
    if(!is_ok("radec_star", radec_star(tdb, &source.star, &obs, ut12tt, i, 1, &ra, &dec, &rv))) return 1;
    radec2vector(ra, dec, 1.0, posa.r_hat);

    if(!is_ok("radec_star:control", place(tdb, &source, &obs, ut12tt, i, 1, &posb))) return 1;
    if(!is_ok("radec_star:check", check_equal_pos(posa.r_hat, posb.r_hat, 1e-9))) return 1;
    if(!is_ok("radec_star:check_rv", fabs(rv - posb.rv) > 1e-6)) return 1;

    if(!is_ok("radec_star:ra:null", radec_star(tdb, &source.star, &obs, ut12tt, i, 1, NULL, &dec, &rv))) return 1;
    if(!is_ok("radec_star:dec:null", radec_star(tdb, &source.star, &obs, ut12tt, i, 1, &ra, NULL, &rv))) return 1;
    if(!is_ok("radec_star:rv:null", radec_star(tdb, &source.star, &obs, ut12tt, i, 1, &ra, &dec, NULL))) return 1;
  }

  return 0;
}

static int test_source() {
  int n = 0;

  if(test_gcrs_j2000_gcrs()) n++;
  if(test_j2000_tod_j2000()) n++;
  if(test_tod_itrs_tod()) n++;

  if(test_gcrs_cirs_gcrs()) n++;
  if(test_cirs_itrs_cirs()) n++;

  if(test_tod_vs_cirs()) n++;

  if(test_place_star()) n++;
  if(test_place_icrs()) n++;
  if(test_place_gcrs()) n++;
  if(test_place_cirs()) n++;
  if(test_place_tod()) n++;

  if(test_radec_star()) n++;

  if(test_equ2hor()) n++;
  if(test_aberration()) n++;
  if(test_starvectors()) n++;

  return n;
}


static int test_make_planet() {
  object mars;

  make_planet(NOVAS_MARS, &mars);

  if(!is_ok("make_planet:type", mars.type != NOVAS_PLANET)) return 1;
  if(!is_ok("make_planet:number", mars.number != NOVAS_MARS)) return 1;
  if(!is_ok("make_planet:name", strcasecmp(mars.name, "Mars"))) return 1;

  return 0;
}


static int test_precession() {
  double pos1[3], pos2[3];

  if(!is_ok("precssion:2-step", precession(tdb, pos0, NOVAS_JD_B1950, pos1))) return 1;

  precession(tdb, pos0, NOVAS_JD_J2000, pos2);
  precession(NOVAS_JD_J2000, pos2, NOVAS_JD_B1950, pos2);

  if(!is_ok("radec_star:check", check_equal_pos(pos1, pos2, 1e-9 * vlen(pos1)))) return 1;

  return 0;
}



static int test_radec_planet() {
  int i;
  object sun;

  make_planet(NOVAS_SUN, &sun);

  for(i = 0; i < 4; i++) {
    sky_pos posa = {}, posb = {};
    double ra, dec, dis, rv;
    if(!is_ok("radec_planet", radec_planet(tdb, &sun, &obs, ut12tt, i, 1, &ra, &dec, &dis, &rv))) return 1;
    radec2vector(ra, dec, 1.0, posa.r_hat);

    if(!is_ok("radec_planet:control", place(tdb, &sun, &obs, ut12tt, i, 1, &posb))) return 1;
    if(!is_ok("radec_planet:check", check_equal_pos(posa.r_hat, posb.r_hat, 1e-9))) return 1;
    if(!is_ok("radec_planet:check_dist", fabs(dis - posb.dis) > 1e-6 * posb.dis)) return 1;
    if(!is_ok("radec_planet:check_rv", fabs(rv - posb.rv) > 1e-6)) return 1;

    if(!is_ok("radec_planet:ra:null", radec_planet(tdb, &sun, &obs, ut12tt, i, 1, NULL, &dec, &dis, &rv))) return 1;
    if(!is_ok("radec_planet:dec:null", radec_planet(tdb, &sun, &obs, ut12tt, i, 1, &ra, NULL, &dis, &rv))) return 1;
    if(!is_ok("radec_planet:dis:null", radec_planet(tdb, &sun, &obs, ut12tt, i, 1, &ra, &dec, NULL, &rv))) return 1;
    if(!is_ok("radec_planet:rv:null", radec_planet(tdb, &sun, &obs, ut12tt, i, 1, &ra, &dec, &dis, NULL))) return 1;
  }

  return 0;
}


static int test_observers() {
  double ps[3] = { 100.0, 30.0, 10.0 }, vs[3] = { 10.0 };
  int n = 0;

  if(test_precession()) n++;
  if(test_radec_planet()) n++;

  if(test_equ_ecl()) n++;
  if(test_equ_gal()) n++;

  make_observer_at_geocenter(&obs);
  n += test_source();

  make_observer_on_surface(20.0, -15.0, 0.0, 0.0, 1000.0, &obs);
  n += test_source();
  if(test_itrs_hor_itrs()) n++;
  if(test_terra()) n++;
  if(test_geo_posvel()) n++;
  if(test_bary2obs()) n++;

  make_observer_in_space(ps, vs, &obs);
  n += test_source();

  return n;
}



static int test_sources() {
  cat_entry star;
  int n = 0;

  printf(" Testing date %.3f\n", (tdb - J2000));

  make_cat_entry("22+20", "TST", 1001, 22.0, 20.0, 3.0, -2.0, 5.0, 10.0, &star);
  if(make_object(2, star.starnumber, star.starname, &star, &source) != 0) return -1;
  n += test_observers();

  make_cat_entry("16-20", "TST", 1001, 16.0, -20.0, 3.0, -2.0, 5.0, 10.0, &star);
  if(make_object(2, star.starnumber, star.starname, &star, &source) != 0) return -1;
  n += test_observers();

  make_cat_entry("08+03", "TST", 1001, 8.0, 3.0, -3.0, 2.0, -5.0, -10.0, &star);
  if(make_object(2, star.starnumber, star.starname, &star, &source) != 0) return -1;
  n += test_observers();


  return n;
}


static int test_get_ut1_to_tt() {
  double dut1 = get_ut1_to_tt(37, 0.123);
  double dutexp = NOVAS_TAI_TO_TT + 37.0 + 0.123;

  if(!is_ok("get_ut1_to_tt", fabs(dut1 - dutexp) > 1e-9)) return 1;

  return 0;
}

static int test_get_utc_to_tt() {
  double dutc = get_utc_to_tt(37);
  double dutexp = NOVAS_TAI_TO_TT + 37.0;

  if(!is_ok("get_ut1_to_tt", fabs(dutc - dutexp) > 1e-9)) return 1;

  return 0;
}

static int test_nutation_lp_provider() {
  double t = (tdb - NOVAS_JD_J2000) / 36525.0;
  double de, dp, de0, dp0;

  int status = 1;


  if(!is_ok("nutation_lp_provider:set_nutation_lp_provider", set_nutation_lp_provider(iau2000b))) goto cleanup; // @suppress("Goto statement used")
  if(!is_ok("nutation_lp_provider:nutation_angles", nutation_angles(t, NOVAS_REDUCED_ACCURACY, &de, &dp))) goto cleanup; // @suppress("Goto statement used")
  if(!is_ok("nutation_lp_provider:iau2000b", iau2000b(tdb, 0.0, &de0, &dp0))) goto cleanup; // @suppress("Goto statement used")

  de0 /= ASEC2RAD;
  dp0 /= ASEC2RAD;

  if(!is_ok("nutation_lp_provider:check_de", fabs(de - de0) > 1e-7)) goto cleanup; // @suppress("Goto statement used")
  if(!is_ok("nutation_lp_provider:check_dp", fabs(dp - dp0) > 1e-7)) goto cleanup; // @suppress("Goto statement used")

  status = 0;

  cleanup:

  set_nutation_lp_provider(nu2000k);
  return status;
}


static int test_cal_date() {
  short y, m, d;
  double h;

  if(!is_ok("cal_date:y:null", cal_date(tdb, NULL, &m, &d, &h))) return 1;
  if(!is_ok("cal_date:m:null", cal_date(tdb, &y, NULL, &d, &h))) return 1;
  if(!is_ok("cal_date:d:null", cal_date(tdb, &y, &m, NULL, &h))) return 1;
  if(!is_ok("cal_date:h:null", cal_date(tdb, &y, &m, &d, NULL))) return 1;

  return 0;
}

static int test_dates() {
  double offsets[] = {-10000.0, 0.0, 10000.0, 10000.0, 10000.01 };
  int i, n = 0;

  if(test_get_ut1_to_tt()) n++;
  if(test_get_utc_to_tt()) n++;
  if(test_nutation_lp_provider()) n++;
  if(test_cal_date()) n++;

  for(i = 0; i < 5; i++) {
    tdb = J2000 + offsets[i];
    n += test_sources();
  }

  if(n) fprintf(stderr, " -- FAILED! %d errors\n", n);
  else fprintf(stderr, " -- OK\n");
  return n;
}

static int test_refract_astro() {
  int i;
  observer o;

  if(!is_ok("refract_astro:init", make_observer_on_surface(10.0, 20.0, 2000.0, -10.0, 900.0, &o))) return 1;

  for(i = 10; i < 90; i++) {
    double za = i;
    int j;

    for(j = 0; j < 3; j++) {
      double r = refract_astro(&o.on_surf, j, za);
      double r1 = refract(&o.on_surf, j, za - r);

      if(!is_ok("refract_astro", fabs(r - r1) > 1e-4)) {
        fprintf(stderr, "  za = %d, option = %d, r = %.6f, r1 = %.6f\n", i, j, r, r1);
        return 1;
      }
    }
  }

  return 0;
}

static int test_case() {
  object o;

  make_object(NOVAS_PLANET, NOVAS_EARTH, "Earth", NULL, &o);
  if(!is_ok("test_case:default", strcmp(o.name, "EARTH"))) return 1;

  novas_case_sensitive(1);
  make_object(NOVAS_PLANET, NOVAS_EARTH, "Earth", NULL, &o);
  if(!is_ok("test_case:sensitive", strcmp(o.name, "Earth"))) return 1;

  novas_case_sensitive(0);
  make_object(NOVAS_PLANET, NOVAS_EARTH, "Earth", NULL, &o);
  if(!is_ok("test_case:insensitive", strcmp(o.name, "EARTH"))) return 1;

  return 0;
}

static int test_make_ephem_object() {
  object body;

  make_ephem_object("Ceres", 1000001, &body);

  if(!is_ok("make_ephem_object:type", body.type != NOVAS_EPHEM_OBJECT)) return 1;
  if(!is_ok("make_ephem_object:number", body.number != 1000001)) return 1;
  if(!is_ok("make_ephem_object:name", strcasecmp(body.name, "Ceres"))) return 1;

  return 0;
}

static int test_make_cat_entry() {
  cat_entry c;

  if(!is_ok("make_cat_entry:name:null", make_cat_entry(NULL, "TST", 1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, &c))) return 1;
  if(!is_ok("make_cat_entry:cat:null", make_cat_entry("test", NULL, 1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, &c))) return 1;

  return 0;
}

static int test_make_object() {
  object o;
  cat_entry c;

  if(!is_ok("make_object:name:null", make_object(NOVAS_CATALOG_OBJECT, 1, NULL, &c, &o))) return 1;

  return 0;
}

static int test_transform_cat() {
  cat_entry in, out;

  if(!is_ok("transform_cat:noid", transform_cat(CHANGE_J2000_TO_ICRS, 0.0, &in, 0.0, NULL, &out))) return 1;
  if(!is_ok("transform_cat:same", transform_cat(CHANGE_J2000_TO_ICRS, 0.0, &in, 0.0, "TR", &in))) return 1;

  return 0;
}

static int test_planet_provider() {
  int status = 1;
  object mars;
  double p[3], v[3], p0[3], v0[3];
  double tdb2[2] = { tdb };

  make_planet(NOVAS_MARS, &mars);

  if(!is_ok("planet_provider:set_planet_provider", set_planet_provider(dummy_planet))) goto cleanup; // @suppress("Goto statement used")
  if(!is_ok("planet_provider:set_planet_provider_hp", set_planet_provider_hp(dummy_planet_hp))) goto cleanup; // @suppress("Goto statement used")

  if(!is_ok("planet_provider:ephemeris", ephemeris(tdb2, &mars, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, p, v))) goto cleanup; // @suppress("Goto statement used")
  if(!is_ok("planet_provider:control", dummy_planet(tdb, NOVAS_MARS, NOVAS_BARYCENTER, p0, v0))) goto cleanup; // @suppress("Goto statement used")
  if(!is_ok("planet_provider:check_pos", check_equal_pos(p, p0, 1e-9 * vlen(p0)))) goto cleanup; // @suppress("Goto statement used")
  if(!is_ok("planet_provider:check_vel", check_equal_pos(v, v0, 1e-9 * vlen(v0)))) goto cleanup; // @suppress("Goto statement used")

  if(!is_ok("planet_provider:ephemeris_hp", ephemeris(tdb2, &mars, NOVAS_BARYCENTER, NOVAS_FULL_ACCURACY, p, v))) goto cleanup; // @suppress("Goto statement used")
  if(!is_ok("planet_provider:control_hp", dummy_planet_hp(tdb2, NOVAS_MARS, NOVAS_BARYCENTER, p0, v0))) goto cleanup; // @suppress("Goto statement used")
  if(!is_ok("planet_provider:check_pos_hp", check_equal_pos(p, p0, 1e-9 * vlen(p0)))) goto cleanup; // @suppress("Goto statement used")
  if(!is_ok("planet_provider:check_vel_hp", check_equal_pos(v, v0, 1e-9 * vlen(v0)))) goto cleanup; // @suppress("Goto statement used")

  status = 0;

  cleanup:

  set_planet_provider(earth_sun_calc);
  set_planet_provider_hp(earth_sun_calc_hp);
  return status;
}


static int test_ephem_provider() {
  novas_ephem_provider prior = get_ephem_provider();

  object body;
  double p[3], v[3], p0[3], v0[3];
  double tdb2[2] = { tdb };
  int status = 1;
  enum novas_origin o;

  make_ephem_object("Dummy", 1000001, &body);

  if(!is_ok("ephem_provider:set_ephem_provider", set_ephem_provider(dummy_ephem))) goto cleanup; // @suppress("Goto statement used")

  for(ephem_origin = 0; ephem_origin < 2; ephem_origin++) {
    if(!is_ok("planet_provider:ephemeris", ephemeris(tdb2, &body, NOVAS_BARYCENTER, NOVAS_FULL_ACCURACY, p, v))) goto cleanup; // @suppress("Goto statement used")
    if(!is_ok("planet_provider:control", dummy_ephem(body.name, body.number, tdb, 0.0, &o, p0, v0))) goto cleanup; // @suppress("Goto statement used")
    if(o == NOVAS_BARYCENTER) {
      if(!is_ok("planet_provider:check_pos", check_equal_pos(p, p0, 1e-9 * vlen(p0)))) goto cleanup; // @suppress("Goto statement used")
      if(!is_ok("planet_provider:check_vel", check_equal_pos(v, v0, 1e-9 * vlen(v0)))) goto cleanup; // @suppress("Goto statement used")
    }

    if(!is_ok("planet_provider:ephemeris", ephemeris(tdb2, &body, NOVAS_HELIOCENTER, NOVAS_FULL_ACCURACY, p, v))) goto cleanup; // @suppress("Goto statement used")
    if(o == NOVAS_BARYCENTER) {
      fprintf(stderr, " Expecing diffent A/B, twice:\n");
      if(!is_ok("planet_provider:check_pos", !check_equal_pos(p, p0, 1e-9 * vlen(p0)))) goto cleanup; // @suppress("Goto statement used")
      if(!is_ok("planet_provider:check_vel", !check_equal_pos(v, v0, 1e-9 * vlen(v0)))) goto cleanup; // @suppress("Goto statement used")
      fprintf(stderr, " OK.\n");
    }
  }

  status = 0;

  cleanup:

  set_ephem_provider(prior);
  return status;
}


static int test_enable_earth_sun_calc_hp() {
  double tdb2[2] = { tdb }, p[3], v[3], p0[3], v0[3];
  int status = 1;

  enable_earth_sun_hp(1);

  if(!is_ok("enable_earth_sun_hp", earth_sun_calc(tdb, NOVAS_SUN, NOVAS_BARYCENTER, p0, v0))) goto cleanup; // @suppress("Goto statement used")
  if(!is_ok("enable_earth_sun_hp", earth_sun_calc_hp(tdb2, NOVAS_SUN, NOVAS_BARYCENTER, p, v))) goto cleanup; // @suppress("Goto statement used")
  if(!is_ok("enable_earth_sun_hp:check_pos", check_equal_pos(p, p0, 1e-9 * vlen(p0)))) goto cleanup; // @suppress("Goto statement used")
  if(!is_ok("enable_earth_sun_hp:check_vel", check_equal_pos(v, v0, 1e-9 * vlen(v0)))) goto cleanup; // @suppress("Goto statement used")

  status = 0;

  cleanup:

  enable_earth_sun_hp(0);
  return status;
}

static int test_ira_equinox() {
  double e1 = ira_equinox(tdb, NOVAS_MEAN_EQUINOX, NOVAS_FULL_ACCURACY);
  double e2 = ira_equinox(tdb, NOVAS_MEAN_EQUINOX, NOVAS_FULL_ACCURACY);

  if(!is_ok("ira_equinox", e1 != e2)) return 1;
  return 0;
}


static int test_iau2000a() {
  double dpsi, deps;

  if(!is_ok("iau2000a:dspi:null", iau2000a(tdb, 0.0, NULL, &deps))) return 1;
  if(!is_ok("iau2000a:deps:null", iau2000a(tdb, 0.0, &dpsi, NULL))) return 1;

  return 0;
}

static int test_iau2000b() {
  double dpsi, deps;

  if(!is_ok("iau2000a:dspi:null", iau2000b(tdb, 0.0, NULL, &deps))) return 1;
  if(!is_ok("iau2000a:deps:null", iau2000b(tdb, 0.0, &dpsi, NULL))) return 1;

  return 0;
}

static int test_nu2000k() {
  double dpsi, deps;

  if(!is_ok("iau2000a:dspi:null", nu2000k(tdb, 0.0, NULL, &deps))) return 1;
  if(!is_ok("iau2000a:deps:null", nu2000k(tdb, 0.0, &dpsi, NULL))) return 1;

  return 0;
}

static int test_tdb2tt() {
  double tt, d;

  if(!is_ok("tdb2tt:tt:null", tdb2tt(tdb, NULL, &d))) return 1;
  if(!is_ok("tdb2tt:dt:null", tdb2tt(tdb, &tt, NULL))) return 1;

  return 0;
}

int main() {
  int n = 0;

  make_object(NOVAS_CATALOG_OBJECT, 0, "None", NULL, &source);

  if(test_make_cat_entry()) n++;
  if(test_make_object()) n++;
  if(test_make_planet()) n++;
  if(test_make_ephem_object()) n++;
  if(test_transform_cat()) n++;
  if(test_refract_astro()) n++;
  if(test_case()) n++;
  if(test_planet_provider()) n++;
  if(test_ephem_provider()) n++;
  if(test_enable_earth_sun_calc_hp()) n++;
  if(test_ira_equinox()) n++;
  if(test_iau2000a()) n++;
  if(test_iau2000b()) n++;
  if(test_nu2000k()) n++;
  if(test_tdb2tt()) n++;

  n += test_dates();

  return n;
}
