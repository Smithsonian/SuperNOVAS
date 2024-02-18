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
static short accuracy;
static double ut12tt = 69.0;

static double xp = 1.0;
static double yp = -2.0;

// Initialized quantities.
static double lst, pos0[3], vel0[3], epos[3], evel[3], pobs[3], vobs[3];

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

  if(!is_ok("tod_to_itrs", tod_to_itrs(tdb, ut12tt, 0, xp, yp, pos0, pos1))) return 1;
  if(!is_ok("itrs_to_tod", itrs_to_tod(tdb, ut12tt, 0, xp, yp, pos1, pos1))) return 1;
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

  if(!is_ok("cirs_to_itrs", cirs_to_itrs(tdb, ut12tt, 0, xp, yp, pos0, pos1))) return 1;
  if(!is_ok("itrs_to_cirs", itrs_to_cirs(tdb, ut12tt, 0, xp, yp, pos1, pos1))) return 1;
  if(!is_ok("cirs_itrs_cirs", check_equal_pos(pos0, pos1, 1e-9 * vlen(pos0)))) return 1;
  return 0;
}

static int test_itrs_hor_itrs() {
  double az = 0.0, za = 0.0, pos1[3], l = vlen(pos0);
  int i;

  if(obs.where != NOVAS_OBSERVER_ON_EARTH) return 0;


  if(!is_ok("itrs_to_hor", itrs_to_hor(&obs.on_surf, pos0, &az, &za))) return 1;
  if(!is_ok("hor_to_itrs", hor_to_itrs(&obs.on_surf, az, za, pos1))) return 1;

  for(i=0; i < 3; i++) pos1[i] *= l;

  if(!is_ok("cirs_itrs_cirs", check_equal_pos(pos0, pos1, 1e-9 * l))) return 1;
  return 0;
}

static int test_tod_vs_cirs() {
  double pos1[3];

  if(!is_ok("gcrs_to_j2000", gcrs_to_j2000(pos0, pos1))) return 1;
  if(!is_ok("j2000_to_tod", j2000_to_tod(tdb, 0, pos1, pos1))) return 1;
  if(!is_ok("tod_to_itrs", tod_to_itrs(tdb, ut12tt, 0, xp, yp, pos1, pos1))) return 1;
  if(!is_ok("itrs_to_cirs", itrs_to_cirs(tdb, ut12tt, 0, xp, yp, pos1, pos1))) return 1;
  if(!is_ok("cirs_to_gcrs", cirs_to_gcrs(tdb, 0, pos1, pos1))) return 1;

  if(!is_ok("tod_vs_cirs", check_equal_pos(pos0, pos1, 1e-9 * vlen(pos0)))) return 1;

  return 0;
}

static int test_equ_gal() {
  double ra, dec, glon, glat, pos1[3];

  vector2radec(pos0, &ra, &dec);
  if(!is_ok("equ2gal", equ2gal(ra, dec, &glon, &glat))) return 1;
  if(!is_ok("gal2equ", gal2equ(glon, glat, &ra, &dec))) return 1;
  radec2vector(ra, dec, vlen(pos0), pos1);

  if(!is_ok("tod_vs_cirs", check_equal_pos(pos0, pos1, 1e-9 * vlen(pos0)))) return 1;

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

  if(test_itrs_hor_itrs()) n++;

  if(test_equ_gal()) n++;

  if(test_place_star()) n++;
  if(test_place_icrs()) n++;
  if(test_place_gcrs()) n++;
  if(test_place_cirs()) n++;
  if(test_place_tod()) n++;

  if(test_radec_star()) n++;

  return n;
}


static int test_precession() {
  double pos1[3], pos2[3];

  if(!is_ok("precssion:2-step", precession(tdb, pos0, NOVAS_JD_B1950, pos1))) return 1;

  precession(tdb, pos0, NOVAS_JD_J2000, pos2);
  precession(NOVAS_JD_J2000, pos2, NOVAS_JD_B1950, pos2);

  if(!is_ok("radec_star:check", check_equal_pos(pos1, pos2, 1e-9 * vlen(pos1)))) return 1;

  return 0;
}

static int test_observers() {
  double ps[3] = { 100.0, 30.0, 10.0 }, vs[3] = { 10.0 };
  int n = 0;

  test_precession();

  make_observer_at_geocenter(&obs);
  n += test_source();

  make_observer_on_surface(20.0, -15.0, 0.0, 0.0, 1000.0, &obs);
  n += test_source();

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


static int test_dates() {
  double offsets[] = {-10000.0, 0.0, 10000.0, 10000.0, 10000.01 };
  int i, n = 0;

  if(test_get_ut1_to_tt()) n++;
  if(test_get_utc_to_tt()) n++;

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

  make_object(NOVAS_MAJOR_PLANET, NOVAS_EARTH, "Earth", NULL, &o);
  if(!is_ok("test_case:default", strcmp(o.name, "EARTH"))) return 1;

  novas_case_sensitive(1);
  make_object(NOVAS_MAJOR_PLANET, NOVAS_EARTH, "Earth", NULL, &o);
  if(!is_ok("test_case:sensitive", strcmp(o.name, "Earth"))) return 1;

  novas_case_sensitive(0);
  make_object(NOVAS_MAJOR_PLANET, NOVAS_EARTH, "Earth", NULL, &o);
  if(!is_ok("test_case:insensitive", strcmp(o.name, "EARTH"))) return 1;

  return 0;
}

int main() {
  int n = 0;

  make_object(NOVAS_CATALOG_OBJECT, 0, "None", NULL, &source);

  if(test_refract_astro()) n++;
  if(test_case()) n++;
  n += test_dates();

  return n;
}
