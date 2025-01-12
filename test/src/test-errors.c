/**
 * @date Created  on Feb 19, 2024
 * @author Attila Kovacs
 */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <string.h>

#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"

static int check(const char *func, int exp, int error) {
  if(error != exp) {
    fprintf(stderr, "ERROR! %s: expected %d, got %d\n", func, exp, error);
    return 1;
  }
  return 0;
}

static int check_nan(const char *func, double value) {
  if(!isnan(value)) {
    fprintf(stderr, "ERROR! %s: expected NAN, got %f\n", func, value);
    return 1;
  }
  return 0;
}

static int test_make_on_surface() {
  if(check("make_on_surface", -1, make_on_surface(0.0, 0.0, 0.0, 0.0, 0.0, NULL))) return 1;
  return 0;
}

static int test_make_in_space() {
  double p[3] = {1.0}, v[3] = {};
  in_space sp;
  int n = 0;

  if(check("make_in_space", -1, make_in_space(p, v, NULL))) n++;
  if(check("make_in_space:p", 0, make_in_space(NULL, v, &sp))) n++;
  if(check("make_in_space:v", 0, make_in_space(p, NULL, &sp))) n++;

  return n;
}

static int test_make_observer() {
  in_space sp = {};
  on_surface on = {};
  observer obs = {};
  int n = 0;

  if(check("make_observer:where", 1, make_observer(-1, &on, &sp, &obs))) n++;
  if(check("make_observer", -1, make_observer(NOVAS_OBSERVER_AT_GEOCENTER, &on, &sp, NULL))) n++;
  if(check("make_observer:on", -1, make_observer(NOVAS_OBSERVER_ON_EARTH, NULL, &sp, &obs))) n++;
  if(check("make_observer:eorb", -1, make_observer(NOVAS_OBSERVER_IN_EARTH_ORBIT, &on, NULL, &obs))) n++;
  if(check("make_observer:air:surf", -1, make_observer(NOVAS_AIRBORNE_OBSERVER, NULL, &sp, &obs))) n++;
  if(check("make_observer:air:vel", -1, make_observer(NOVAS_AIRBORNE_OBSERVER, &on, NULL, &obs))) n++;

  return n;
}

static int test_make_airborne_observer() {
  on_surface on = {};
  observer obs = {};
  int n = 0;

  if(check("make_airborne_observer:vel", -1, make_airborne_observer(&on, NULL, &obs))) n++;

  return n;
}

static int test_make_ephem_object() {
  object o;
  int n = 0;

  char longname[SIZE_OF_OBJ_NAME + 1];
  memset(longname, 'A', SIZE_OF_OBJ_NAME);

  if(check("make_ephem_object", -1, make_ephem_object("dummy", 1, NULL))) n++;
  if(check("make_ephem_object:name", -1, make_ephem_object(longname, 1, &o))) n++;

  return n;
}

static int test_make_planet() {
  object o;
  int n = 0;

  if(check("make_ephem_object:lo", -1, make_planet(-1, &o))) n++;
  if(check("make_ephem_object:hi", -1, make_planet(NOVAS_PLANETS, &o))) n++;

  return n;
}

static int test_make_cat_entry() {
  cat_entry c;
  int n = 0;

  char longname[SIZE_OF_OBJ_NAME + 1];
  char longcat[SIZE_OF_CAT_NAME + 1];

  memset(longname, 'A', SIZE_OF_OBJ_NAME);
  memset(longcat, 'A', SIZE_OF_CAT_NAME);

  if(check("make_cat_entry", -1, make_cat_entry("dummy", "cat", 1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, NULL))) n++;
  if(check("make_cat_entry:name", 1, make_cat_entry(longname, "cat", 1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, &c))) n++;
  if(check("make_cat_entry:catname", 2, make_cat_entry("dummy", longcat, 1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, &c))) n++;

  return n;
}

static int test_make_object() {
  cat_entry s = {};
  object o = {};
  int n = 0;

  char longname[SIZE_OF_OBJ_NAME + 1];
  memset(longname, 'A', SIZE_OF_OBJ_NAME);

  if(check("make_object", -1, make_object(NOVAS_PLANET, 1, "dummy", &s, NULL))) n++;
  if(check("make_object:star", -1, make_object(NOVAS_CATALOG_OBJECT, 1, "dummy", NULL, &o))) n++;
  if(check("make_object:type", 1, make_object(-1, 1, "dummy", &s, &o))) n++;
  if(check("make_object:pl:lo", 2, make_object(NOVAS_PLANET, -1, "dummy", NULL, &o))) n++;
  if(check("make_object:pl:lo", 2, make_object(NOVAS_PLANET, NOVAS_PLANETS, "dummy", NULL, &o))) n++;
  if(check("make_object:name", 5, make_object(NOVAS_PLANET, 1, longname, NULL, &o))) n++;

  return n;
}

static int test_make_cat_object() {
  cat_entry s;
  object source;
  int n = 0;

  make_cat_entry("test", "TST", 1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, &s);

  if(check("make_cat_object", -1, make_cat_object(&s, NULL))) n++;
  if(check("make_cat_object:star", -1, make_cat_object(NULL, &source))) n++;

  return n;
}

static int test_make_redshifted_object() {
  object source;
  int n = 0;

  if(check("make_redshifted_object", -1, make_redshifted_object("TEST", 0.0, 0.0, 0.0, NULL))) n++;
  if(check("make_redshifted_object:z:lo", -1, make_redshifted_object("TEST", 0.0, 0.0, -1.0, &source))) n++;

  return n;
}

static int test_v2z() {
  int n = 0;

  if(check_nan("v2z:hi", novas_v2z(NOVAS_C / 1000.0 + 0.01))) n++;
  return n;
}

static int test_z2v() {
  int n = 0;

  if(check_nan("z2v:-1", novas_z2v(-1.0))) n++;
  return n;
}

static int test_refract() {
  on_surface o = {};
  int n = 0;

  novas_debug(NOVAS_DEBUG_ON);
  fprintf(stderr, ">>> Expecting error message...\n");
  errno = 0;
  double r = refract(NULL, NOVAS_STANDARD_ATMOSPHERE, 30.0);
  if(check("refract:loc", 1, r == 0.0 && errno == EINVAL)) n++;
  novas_debug(NOVAS_DEBUG_OFF);

  errno = 0;
  r = refract(&o, -1, 30.0);
  if(check("refract:model", 1, r == 0.0 && errno == EINVAL)) n++;

  errno = 0;
  r = refract(&o, NOVAS_STANDARD_ATMOSPHERE, 91.01);
  if(check("refract:zd", 1, r == 0.0)) n++;

  return n;
}

static int test_refract_astro() {
  extern int novas_inv_max_iter;
  on_surface surf = {};
  int n = 0;

  novas_inv_max_iter = 0;
  if(check_nan("refract_astro:converge", refract_astro(&surf, NOVAS_STANDARD_ATMOSPHERE, 85.0))) n++;
  else if(check("refract_astro:converge:errno", ECANCELED, errno)) n++;

  novas_inv_max_iter = 100;

  return n;
}

static int test_inv_refract() {
  extern int novas_inv_max_iter;
  on_surface surf = {};
  int n = 0;

  novas_inv_max_iter = 0;
  if(check_nan("inv_refract:converge", novas_inv_refract(novas_optical_refraction, NOVAS_JD_J2000, &surf, NOVAS_REFRACT_OBSERVED, 5.0))) n++;
  else if(check("inv_refract:converge:errno", ECANCELED, errno)) n++;
  novas_inv_max_iter = 100;

  return n;
}

static int test_limb_angle() {
  double pos[3] = { 0.01 }, pn[3] = { -0.01 }, pz[3] = {}, a, b;
  int n = 0;

  if(check("limb_angle:pos_obj", -1, limb_angle(NULL, pos, &a, &b))) n++;
  if(check("limb_angle:pos_obs", -1, limb_angle(pos, NULL, &a, &b))) n++;
  if(check("limb_angle:obj:0", -1, limb_angle(pz, pos, &a, &b))) n++;
  if(check("limb_angle:obj:0", -1, limb_angle(pos, pz, &a, &b))) n++;

  // Null outputs OK
  if(check("limb_angle:limb", 0, limb_angle(pos, pos, NULL, &b))) n++;
  if(check("limb_angle:nadir", 0, limb_angle(pos, pos, &a, NULL))) n++;

  // corner case (cosd = -1)
  if(check("limb_angle:corner", 0, limb_angle(pos, pn, &a, NULL))) n++;

  return n;
}


static int test_transform_cat() {
  cat_entry c = {}, c1;
  int n = 0;

  char longname[SIZE_OF_OBJ_NAME + 1];
  memset(longname, 'A', SIZE_OF_OBJ_NAME);

  if(check("transform_cat:in", -1, transform_cat(PRECESSION, NOVAS_JD_B1950, NULL, NOVAS_JD_J2000, "FK5", &c))) n++;
  if(check("transform_cat:out", -1, transform_cat(PRECESSION, NOVAS_JD_B1950, &c, NOVAS_JD_J2000, "FK5", NULL))) n++;
  if(check("transform_cat:option", -1, transform_cat(-1, NOVAS_JD_B1950, &c, NOVAS_JD_J2000, "FK5", &c1))) n++;
  if(check("transform_cat:option:same", -1, transform_cat(-1, NOVAS_JD_B1950, &c, NOVAS_JD_J2000, "FK5", &c))) n++;
  if(check("transform_cat:name", 2, transform_cat(PRECESSION, NOVAS_JD_B1950, &c, NOVAS_JD_J2000, longname, &c))) n++;

  return n;
}

static int test_transform_hip() {
  cat_entry c = {};
  int n = 0;

  if(check("transform_hip:in", -1, transform_hip(NULL, &c))) n++;
  if(check("transform_hip:in", -1, transform_hip(&c, NULL))) n++;

  return n;
}

static int test_ephemeris() {
  double p[3], v[3];
  double tdb[2] = { NOVAS_JD_J2000 };
  object ceres;
  int n = 0;

  make_ephem_object("Ceres", 2000001, &ceres);

  if(check("ephemeris:body", -1, ephemeris(tdb, NULL, NOVAS_BARYCENTER, NOVAS_FULL_ACCURACY, p, v))) n++;
  if(check("ephemeris:jd", -1, ephemeris(NULL, &ceres, NOVAS_BARYCENTER, NOVAS_FULL_ACCURACY, p, v))) n++;
  if(check("ephemeris:pos", -1, ephemeris(tdb, &ceres, NOVAS_BARYCENTER, NOVAS_FULL_ACCURACY, NULL, v))) n++;
  if(check("ephemeris:vel", -1, ephemeris(tdb, &ceres, NOVAS_BARYCENTER, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("ephemeris:pos+vel", -1, ephemeris(tdb, &ceres, NOVAS_BARYCENTER, NOVAS_FULL_ACCURACY, NULL, NULL))) n++;
  if(check("ephemeris:pos=vel", -1, ephemeris(tdb, &ceres, NOVAS_BARYCENTER, NOVAS_FULL_ACCURACY, p, p))) n++;
  if(check("ephemeris:origin", 1, ephemeris(tdb, &ceres, -1, NOVAS_FULL_ACCURACY, p, v))) n++;

#ifndef DEFAULT_READEPH
  if(check("ephemeris:noephem", -1, ephemeris(tdb, &ceres, NOVAS_BARYCENTER, NOVAS_FULL_ACCURACY, p, v))) n++;
#endif

  return n;
}

static int test_j2000_to_tod() {
  double p[3];
  int n = 0;

  if(check("j2000_to_tod:in", -1, j2000_to_tod(0.0, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("j2000_to_tod:out", -1, j2000_to_tod(0.0, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("j2000_to_tod:accuracy", -1, j2000_to_tod(0.0, -1, p, p))) n++;

  return n;
}

static int test_tod_to_j2000() {
  double p[3] = {1.0};
  int n = 0;

  if(check("tod_to_j2000:in", -1, tod_to_j2000(0.0, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("tod_to_j2000:out", -1, tod_to_j2000(0.0, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("tod_to_j2000:accuracy", -1, tod_to_j2000(0.0, -1, p, p))) n++;

  return n;
}

static int test_gcrs_to_tod() {
  double p[3];
  int n = 0;

  if(check("gcrs_to_tod:in", -1, gcrs_to_tod(0.0, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("gcrs_to_tod:out", -1, gcrs_to_tod(0.0, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("gcrs_to_tod:accuracy", -1, gcrs_to_tod(0.0, -1, p, p))) n++;

  return n;
}

static int test_tod_to_gcrs() {
  double p[3] = {1.0};
  int n = 0;

  if(check("tod_to_gcrs:in", -1, tod_to_gcrs(0.0, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("tod_to_gcrs:out", -1, tod_to_gcrs(0.0, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("tod_to_gcrs:accuracy", -1, tod_to_gcrs(0.0, -1, p, p))) n++;

  return n;
}

static int test_gcrs_to_mod() {
  double p[3];
  int n = 0;

  if(check("gcrs_to_mod:in", -1, gcrs_to_mod(0.0, NULL, p))) n++;
  if(check("gcrs_to_mod:out", -1, gcrs_to_mod(0.0, p, NULL))) n++;

  return n;
}

static int test_mod_to_gcrs() {
  double p[3] = {1.0};
  int n = 0;

  if(check("mod_to_gcrs:in", -1, mod_to_gcrs(0.0, NULL, p))) n++;
  if(check("mod_to_gcrs:out", -1, mod_to_gcrs(0.0, p, NULL))) n++;

  return n;
}

static int test_gcrs_to_cirs() {
  double p[3] = {1.0};
  int n = 0;

  if(check("gcrs_to_cirs:in", -1, gcrs_to_cirs(0.0, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("gcrs_to_cirs:out", -1, gcrs_to_cirs(0.0, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("gcrs_to_cirs:accuracy", -1, gcrs_to_cirs(0.0, -1, p, p))) n++;

  return n;
}

static int test_cirs_to_gcrs() {
  double p[3];
  int n = 0;

  if(check("cirs_to_gcrs:in", -1, cirs_to_gcrs(0.0, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("cirs_to_gcrs:out", -1, cirs_to_gcrs(0.0, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("cirs_to_gcrs:accuracy", -1, cirs_to_gcrs(0.0, -1, p, p))) n++;

  return n;
}

static int test_cirs_to_app_ra() {
  int n = 0;
  if(check_nan("cirs_to_app_ra:accuracy:-1", cirs_to_app_ra(NOVAS_JD_J2000, -1, 0.0))) n++;
  if(check_nan("cirs_to_app_ra:accuracy:2", cirs_to_app_ra(NOVAS_JD_J2000, 2, 0.0))) n++;
  return n;
}

static int test_app_to_cirs_ra() {
  int n = 0;
  if(check_nan("app_to_cirs_ra:accuracy:-1", app_to_cirs_ra(NOVAS_JD_J2000, -1, 0.0))) n++;
  if(check_nan("app_to_cirs_ra:accuracy:2", app_to_cirs_ra(NOVAS_JD_J2000, 2, 0.0))) n++;
  return n;
}

static int test_set_planet_provider() {
  if(check("set_planet_provider", -1, set_planet_provider(NULL))) return 1;
  return 0;
}

static int test_set_planet_provider_hp() {
  if(check("set_planet_provider_hp", -1, set_planet_provider_hp(NULL))) return 1;
  return 0;
}

static int test_place_star() {
  cat_entry c = {};
  observer loc = {};
  sky_pos pos;
  int n = 0;

  if(check("place_star:in", -1, place_star(0.0, NULL, &loc, 0.0, NOVAS_GCRS, NOVAS_FULL_ACCURACY, &pos))) n++;
  if(check("place_star:out", -1, place_star(0.0, &c, &loc, 0.0, NOVAS_GCRS, NOVAS_FULL_ACCURACY, NULL))) n++;

  return n;
}

static int test_place() {
  object o = {};
  observer loc = {};
  sky_pos pos;
  int n = 0;

  if(check("place:object", -1, place(0.0, NULL, &loc, 0.0, NOVAS_GCRS, NOVAS_FULL_ACCURACY, &pos))) n++;
  if(check("place:sys:lo", 1, place(0.0, &o, &loc, 0.0, -1, NOVAS_FULL_ACCURACY, &pos))) n++;
  if(check("place:sys:hi", 1, place(0.0, &o, &loc, 0.0, NOVAS_REFERENCE_SYSTEMS, NOVAS_FULL_ACCURACY, &pos))) n++;
  if(check("place:accuracy", 2, place(0.0, &o, &loc, 0.0, NOVAS_GCRS, -1, &pos))) n++;

  return n;
}

static int test_radec_planet() {
  object o = {};
  observer loc = {};
  double ra, dec, dis, rv;

  o.type = NOVAS_CATALOG_OBJECT;
  if(check("radec_planet:cat", -1, radec_planet(NOVAS_JD_J2000, &o, &loc, 0.0, NOVAS_GCRS, NOVAS_REDUCED_ACCURACY, &ra, &dec, &dis, &rv))) return 1;

  return 0;
}

static int test_mean_star() {
  extern int novas_inv_max_iter;
  double x, y;
  int n = 0;

  if(check("mean_star:ira", -1, mean_star(0.0, 0.0, 0.0, NOVAS_FULL_ACCURACY, NULL, &y))) n++;
  if(check("mean_star:idec", -1, mean_star(0.0, 0.0, 0.0, NOVAS_FULL_ACCURACY, &x, NULL))) n++;

  novas_inv_max_iter = 0;
  if(check("mean_star:converge", 1, mean_star(NOVAS_JD_J2000, 0.0, 0.0, NOVAS_REDUCED_ACCURACY, &x, &y))) n++;
  else if(check("mean_star:converge:errno", ECANCELED, errno)) n++;
  novas_inv_max_iter = 100;

  return n;
}

static int test_equ2gal() {
  double x;
  int n = 0;

  if(check("equ2gal:lon", -1, equ2gal(0.0, 0.0, NULL, &x))) n++;
  if(check("equ2gal:lat", -1, equ2gal(0.0, 0.0, &x, NULL))) n++;

  return n;
}

static int test_gal2equ() {
  double x;
  int n = 0;

  if(check("gal2equ:ra", -1, gal2equ(0.0, 0.0, NULL, &x))) n++;
  if(check("gal2equ:dec", -1, gal2equ(0.0, 0.0, &x, NULL))) n++;

  return n;
}

static int test_equ2ecl() {
  double x;
  int n = 0;

  if(check("equ2ecl:lon", -1, equ2ecl(0.0, NOVAS_GCRS_EQUATOR, NOVAS_FULL_ACCURACY, 0.0, 0.0, NULL, &x))) n++;
  if(check("equ2ecl:lat", -1, equ2ecl(0.0, NOVAS_GCRS_EQUATOR, NOVAS_FULL_ACCURACY, 0.0, 0.0, &x, NULL))) n++;

  return n;
}


static int test_ecl2equ() {
  double x;
  int n = 0;

  if(check("ecl2equ:lon", -1, ecl2equ(0.0, NOVAS_GCRS_EQUATOR, NOVAS_FULL_ACCURACY, 0.0, 0.0, NULL, &x))) n++;
  if(check("ecl2equ:lat", -1, ecl2equ(0.0, NOVAS_GCRS_EQUATOR, NOVAS_FULL_ACCURACY, 0.0, 0.0, &x, NULL))) n++;

  return n;
}

static int test_equ2ecl_vec() {
  double p[3] = {1.0};
  int n = 0;

  if(check("equ2ecl_vec:in", -1, equ2ecl_vec(0.0, NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("equ2ecl_vec:out", -1, equ2ecl_vec(0.0, NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("equ2ecl_vec:accuracy", -1, equ2ecl_vec(0.0, NOVAS_MEAN_EQUATOR, -1, p, p))) n++;
  if(check("equ2ecl_vec:equator", 1, equ2ecl_vec(0.0, -1, NOVAS_FULL_ACCURACY, p, p))) n++;

  return n;
}

static int test_ecl2equ_vec() {
  double p[3] = {1.0};
  int n = 0;

  if(check("ecl2equ_vec:in", -1, ecl2equ_vec(0.0, NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("ecl2equ_vec:out", -1, ecl2equ_vec(0.0, NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("ecl2equ_vec:accuracy", -1, ecl2equ_vec(0.0, NOVAS_MEAN_EQUATOR, -1, p, p))) n++;
  if(check("ecl2equ_vec:equator", 1, ecl2equ_vec(0.0, -1, NOVAS_FULL_ACCURACY, p, p))) n++;

  return n;
}


static int test_itrs_to_hor() {
  on_surface loc = {};
  double p[3] = {}, az, za;
  int n = 0;

  if(check("itrs_to_hor:loc", -1, itrs_to_hor(NULL, p, &az, &za))) n++;
  if(check("itrs_to_hor:in", -1, itrs_to_hor(&loc, NULL, &az, &za))) n++;

  return n;
}

static int test_hor_to_itrs() {
  on_surface loc = {};
  double p[3];
  int n = 0;

  if(check("hor_to_itrs:loc", -1, hor_to_itrs(NULL, 0.0, 0.0, p))) n++;
  if(check("hor_to_itrs:in", -1, hor_to_itrs(&loc, 0.0, 0.0, NULL))) n++;

  return n;
}

static int test_equ2hor() {
  on_surface loc;
  double az, za, rar, decr;
  int n = 0;

  if(check("equ2hor:loc", -1, equ2hor(0.0, 0.0, NOVAS_FULL_ACCURACY, 0.0, 0.0, NULL, 0.0, 0.0, NOVAS_STANDARD_ATMOSPHERE, &az, &za, &rar, &decr))) n++;
  if(check("equ2hor:az", -1, equ2hor(0.0, 0.0, NOVAS_FULL_ACCURACY, 0.0, 0.0, &loc, 0.0, 0.0, NOVAS_STANDARD_ATMOSPHERE, NULL, &za, &rar, &decr))) n++;
  if(check("equ2hor:zd", -1, equ2hor(0.0, 0.0, NOVAS_FULL_ACCURACY, 0.0, 0.0, &loc, 0.0, 0.0, NOVAS_STANDARD_ATMOSPHERE, &az, NULL, &rar, &decr))) n++;

  return n;
}

static int test_gcrs2equ() {
  double ra, dec;
  int n = 0;

  if(check("gcrs2equ:ra", -1, gcrs2equ(0.0, NOVAS_DYNAMICAL_MOD, NOVAS_FULL_ACCURACY, 0.0, 0.0, NULL, &dec))) n++;
  if(check("gcrs2equ:dec", -1, gcrs2equ(0.0, NOVAS_DYNAMICAL_MOD, NOVAS_FULL_ACCURACY, 0.0, 0.0, &ra, NULL))) n++;
  if(check("gcrs2equ:sys", -1, gcrs2equ(0.0, -1, NOVAS_FULL_ACCURACY, 0.0, 0.0, &ra, &dec))) n++;

  return n;
}

static int test_sidereal_time() {
  double x;
  int n = 0;

  if(check("sidereal_time:out", -1, sidereal_time(0.0, 0.0, 0.0, NOVAS_MEAN_EQUINOX, EROT_GST, NOVAS_FULL_ACCURACY, NULL))) n++;
  if(check("sidereal_time:accuracy", 1, sidereal_time(0.0, 0.0, 0.0, NOVAS_MEAN_EQUINOX, EROT_GST, -1, &x))) n++;
  if(check("sidereal_time:erot", 2, sidereal_time(0.0, 0.0, 0.0, NOVAS_MEAN_EQUINOX, -1, NOVAS_FULL_ACCURACY, &x))) n++;

  return n;
}

static int test_ter2cel() {
  double p[3] = {1.0};
  int n = 0;

  if(check("ter2cel:in", -1, ter2cel(0.0, 0.0, 0.0, EROT_GST, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, NULL, p))) n++;
  if(check("ter2cel:out", -1, ter2cel(0.0, 0.0, 0.0, EROT_GST, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p, NULL))) n++;
  if(check("ter2cel:accuracy", 1, ter2cel(0.0, 0.0, 0.0, EROT_GST, -1, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p, p))) n++;
  if(check("ter2cel:erot", 2, ter2cel(0.0, 0.0, 0.0, -1, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p, p))) n++;

  return n;
}

static int test_cel2ter() {
  double p[3] = {1.0};
  int n = 0;

  if(check("cel2ter:in", -1, cel2ter(0.0, 0.0, 0.0, EROT_GST, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, NULL, p))) n++;
  if(check("cel2ter:out", -1, cel2ter(0.0, 0.0, 0.0, EROT_GST, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p, NULL))) n++;
  if(check("cel2ter:accuracy", 1, cel2ter(0.0, 0.0, 0.0, EROT_GST, -1, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p, p))) n++;
  if(check("cel2ter:erot", 2, cel2ter(0.0, 0.0, 0.0, -1, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p, p))) n++;

  return n;
}


static int test_spin() {
  double p[3] = {1.0};
  int n = 0;

  if(check("spin:in", -1, spin(0.0, NULL, p))) n++;
  if(check("spin:out", -1, spin(0.0, p, NULL))) n++;

  return n;
}

static int test_wobble() {
  double p[3] = {1.0};
  int n = 0;

  if(check("wobble:in", -1, wobble(0.0, WOBBLE_ITRS_TO_PEF, 0.0, 0.0, NULL, p))) n++;
  if(check("wobble:out", -1, wobble(0.0, WOBBLE_ITRS_TO_PEF, 0.0, 0.0, p, NULL))) n++;

  return n;
}

static int test_terra() {
  on_surface loc = {};
  double p[3], v[3];
  int n = 0;

  if(check("terra:loc", -1, terra(NULL, 0.0, p, v))) n++;
  if(check("terra:same", -1, terra(&loc, 0.0, p, p))) n++;

  return n;
}

static int test_e_tilt() {
  int n = 0;

  if(check("e_tilt:accuracy", -1, e_tilt(0.0, -1, NULL, NULL, NULL, NULL, NULL))) n++;

  return n;
}



static int test_cel_pole() {
  int n = 0;

  if(check("cel_pole:type", 1, cel_pole(0.0, -1, 0.0, 0.0))) n++;

  return n;
}

static int test_frame_tie() {
  double p[3] = {1.0};
  int n = 0;

  if(check("frame_tie:in", -1, frame_tie(NULL, 0, p))) n++;
  if(check("frame_tie:out", -1, frame_tie(p, 0, NULL))) n++;

  return n;
}


static int test_proper_motion() {
  double p[3] = {1.0}, v[3] = {1.0};
  int n = 0;

  if(check("frame_tie:p", -1, proper_motion(0.0, NULL, v, 1.0, p))) n++;
  if(check("frame_tie:v", -1, proper_motion(0.0, p, NULL, 1.0, p))) n++;
  if(check("frame_tie:out", -1, proper_motion(0.0, p, v, 1.0, NULL))) n++;

  return n;
}

static int test_bary2obs() {
  double p[3] = {}, po[3] = {}, out[3], lt;
  int n = 0;

  if(check("bary2obs:pos", -1, bary2obs(NULL, po, out, &lt))) n++;
  if(check("bary2obs:obs", -1, bary2obs(p, NULL, out, &lt))) n++;
  if(check("bary2obs:out", -1, bary2obs(p, po, NULL, &lt))) n++;

  return n;
}

static int test_geo_posvel() {
  observer o;
  double p[3], v[3];
  int n = 0;

  o.where = NOVAS_OBSERVER_ON_EARTH;
  if(check("geo_posvel:loc", -1, geo_posvel(0.0, 0.0, NOVAS_FULL_ACCURACY, NULL, p, v))) n++;
  if(check("geo_posvel:same", -1, geo_posvel(0.0, 0.0, NOVAS_FULL_ACCURACY, &o, p, p))) n++;
  if(check("geo_posvel:accuracy", 1, geo_posvel(0.0, 0.0, -1, &o, p, v))) n++;

  o.where = -1;
  if(check("geo_posvel:where", 2, geo_posvel(0.0, 0.0, NOVAS_FULL_ACCURACY, &o, p, v))) n++;

  return n;
}

static int test_light_time2() {
  extern int novas_inv_max_iter;
  object o;
  double pos[3] = {1.0}, p[3], v[3], t;
  int n = 0;

  make_planet(NOVAS_SUN, &o);

  if(check("light_time2:tout", -1, light_time2(0.0, &o, pos, 0.0, NOVAS_FULL_ACCURACY, p, v, NULL))) n++;
  if(check("light_time2:object", -1, light_time2(0.0, NULL, pos, 0.0, NOVAS_FULL_ACCURACY, p, v, &t))) n++;
  if(check("light_time2:pos", -1, light_time2(0.0, &o, NULL, 0.0, NOVAS_FULL_ACCURACY, p, v, &t))) n++;
  if(check("light_time2:same1", -1, light_time2(0.0, &o, pos, 0.0, NOVAS_FULL_ACCURACY, pos, v, &t))) n++;
  if(check("light_time2:same2", -1, light_time2(0.0, &o, pos, 0.0, NOVAS_FULL_ACCURACY, p, pos, &t))) n++;
  if(check("light_time2:same3", -1, light_time2(0.0, &o, pos, 0.0, NOVAS_FULL_ACCURACY, p, p, &t))) n++;

  novas_inv_max_iter = 0;
  if(check("light_time2:converge", 1, light_time2(0.0, &o, pos, 0.0, NOVAS_FULL_ACCURACY, p, v, &t))) n++;
  else if(check("light_time2:converge:errno", ECANCELED, errno)) n++;
  novas_inv_max_iter = 100;

  return n;
}

static int test_d_light() {
  double p[3] = {1.0};
  int n = 0;

  if(check_nan("d_light:1", d_light(NULL, p))) n++;
  if(check_nan("d_light:2", d_light(p, NULL))) n++;

  return n;
}

static int test_cio_array() {
  ra_of_cio x[5];
  int n = 0;

  if(check("cio_array:out", -1, cio_array(0.0, 5, NULL))) n++;
  if(check("cio_array:n_pts:lo", 3, cio_array(0.0, 1, x))) n++;
  if(check("cio_array:n_pts:hi", 3, cio_array(0.0, NOVAS_CIO_CACHE_SIZE + 1, x))) n++;

  set_cio_locator_file("blah");
  if(check("cio_array:file", 1, cio_array(0.0, 5, x))) n++;

  set_cio_locator_file("../cio_ra.bin");
  set_cio_locator_file("../cio_ra.bin"); // Test reopen also...
  if(check("cio_array:beg", 2, cio_array(0.0, 5, x))) n++;
  if(check("cio_array:end", 2, cio_array(1e20, 5, x))) n++;

  if(check("cio_array:corner:lo", 6, cio_array(2341952.6, 5, x))) n++;
  if(check("cio_array:corner:hi", 6, cio_array(2561137.4, 5, x))) n++;

  if(check("cio_array:corner:near", 0, cio_array(2341962.6, 5, x))) n++;

  set_cio_locator_file("bad-cio-data/empty");
  if(check("cio_array:bin:empty", 1, cio_array(2341952.6, 5, x))) n++;

  set_cio_locator_file("bad-cio-data/bad-1.bin");
  if(check("cio_array:bin:header", -1, cio_array(2341952.6, 2, x))) n++;

  set_cio_locator_file("bad-cio-data/bad-2.bin");
  if(check("cio_array:bin:incomplete", 6, cio_array(2341951.4, 2, x))) n++;
  if(check("cio_array:bin:seek", -1, cio_array(2341965.4, 2, x))) n++;

  set_cio_locator_file("bad-cio-data/bad-1.txt");
  if(check("cio_array:ascii:header", -1, cio_array(2341952.6, 2, x))) n++;

  set_cio_locator_file("bad-cio-data/bad-2.txt");
  if(check("cio_array:ascii:incomplete", 6, cio_array(2341951.4, 2, x))) n++;
  if(check("cio_array:ascii:seek", 2, cio_array(2341965.4, 2, x))) n++;

  set_cio_locator_file("bad-cio-data/bad-3.txt");
  if(check("cio_array:ascii:no-data", 1, cio_array(2341952.6, 2, x))) n++;

  set_cio_locator_file("bad-cio-data/bad-4.txt");
  if(check("cio_array:ascii:corrupt:first", -1, cio_array(2341952.6, 2, x))) n++;

  set_cio_locator_file("bad-cio-data/bad-5.txt");
  if(check("cio_array:ascii:corrupt", -1, cio_array(2341952.6, 2, x))) n++;

  return n;
}

static int test_cio_basis() {
  double x[3], y[3], z[3];
  int n = 0;

  if(check("cio_basis:x", -1, cio_basis(0.0, 0.0, CIO_VS_GCRS, NOVAS_FULL_ACCURACY, NULL, y, z))) n++;
  if(check("cio_basis:y", -1, cio_basis(0.0, 0.0, CIO_VS_GCRS, NOVAS_FULL_ACCURACY, x, NULL, z))) n++;
  if(check("cio_basis:z", -1, cio_basis(0.0, 0.0, CIO_VS_GCRS, NOVAS_FULL_ACCURACY, x, y, NULL))) n++;
  if(check("cio_basis:accuracy", -1, cio_basis(0.0, 0.0, CIO_VS_GCRS, -1, x, y, z))) n++;
  if(check("cio_basis:ref", 1, cio_basis(0.0, 0.0, -1, NOVAS_FULL_ACCURACY, x, y, z))) n++;

  return n;
}

static int test_cio_location() {
  double x;
  short type;
  int n = 0;

  if(check("cio_location:ra", -1, cio_location(0.0, NOVAS_FULL_ACCURACY, NULL, &type))) n++;
  if(check("cio_location:type", -1, cio_location(0.0, NOVAS_FULL_ACCURACY, &x, NULL))) n++;

  return n;
}

static int test_cio_ra() {
  double x;
  int n = 0;

  if(check("cio_location:ra", -1, cio_ra(0.0, NOVAS_FULL_ACCURACY, NULL))) n++;
  if(check("cio_location:accuracy", 1, cio_ra(0.0, -1, &x))) n++;

  return n;
}

static int test_starvectors() {
  cat_entry star;
  double p[3], v[3];
  int n = 0;

  if(check("starvectors:star", -1, starvectors(NULL, p, v))) n++;
  if(check("starvectors:pos+vel", -1, starvectors(NULL, NULL, NULL))) n++;
  if(check("starvectors:pos=vel", -1, starvectors(&star, p, p))) n++;

  return n;
}

static int test_radec2vector() {
  int n = 0;

  if(check("radec2vector", -1, radec2vector(0.0, 0.0, 1.0, NULL))) n++;

  return n;
}

static int test_vector2radec() {
  double p[3] = {}, ra, dec;
  int n = 0;

  if(check("vector2radec:vec", -1, vector2radec(NULL, &ra, &dec))) n++;

  if(check("vector2radec:zero", 1, vector2radec(p, &ra, &dec))) n++;

  p[2] = 1.0;
  if(check("vector2radec:pole", 2, vector2radec(p, &ra, &dec))) n++;
  if(check("vector2radec:pole:ra:null", 2, vector2radec(p, NULL, &dec))) n++;
  if(check("vector2radec:pole:dec:null", 2, vector2radec(p, &ra, NULL))) n++;

  return n;
}

static int test_planet_lon() {
  int n = 0;

  if(check_nan("planet_lon", planet_lon(0.0, -1))) n++;

  return n;
}

static int test_fund_args() {
  int n = 0;

  if(check("find_args", -1, fund_args(0.0, NULL))) n++;

  return n;
}

static int test_nutation_angles() {
  double x;
  int n = 0;

  if(check("nutation_angles:dpsi", -1, nutation_angles(0.0, NOVAS_FULL_ACCURACY, NULL, &x))) n++;
  if(check("nutation_angles:deps", -1, nutation_angles(0.0, NOVAS_FULL_ACCURACY, &x, NULL))) n++;

  return n;
}

static int test_set_nutation_lp_provider() {
  int n = 0;

  if(check("set_nutation_lp_provider", -1, set_nutation_lp_provider(NULL))) n++;

  return n;
}

static int test_nutation() {
  double p[3] = {1.0};
  int n = 0;

  if(check("nutation:in", -1, nutation(0.0, NUTATE_MEAN_TO_TRUE, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("nutation:out", -1, nutation(0.0, NUTATE_MEAN_TO_TRUE, NOVAS_FULL_ACCURACY, p, NULL))) n++;

  return n;
}

static int test_precession() {
  double p[3] = {1.0};
  int n = 0;

  if(check("precesion:in", -1, precession(0.0, NULL, 1.0, p))) n++;
  if(check("precesion:out", -1, precession(0.0, p, 1.0, NULL))) n++;

  return n;
}

static int test_rad_vel() {
  object o;
  double p[3] = {}, v[3] = {}, vo[3] = {}, rv;
  int n = 0;

  o.type = NOVAS_PLANET;

  if(check("rad_vel:object", -1, rad_vel(NULL, p, v, vo, 1.0, 1.0, 1.0, &rv))) n++;
  if(check("rad_vel:pos", -1, rad_vel(&o, NULL, v, vo, 1.0, 1.0, 1.0, &rv))) n++;
  if(check("rad_vel:vel", -1, rad_vel(&o, p, NULL, vo, 1.0, 1.0, 1.0, &rv))) n++;
  if(check("rad_vel:vobs", -1, rad_vel(&o, p, v, NULL, 1.0, 1.0, 1.0, &rv))) n++;
  if(check("rad_vel:out", -1, rad_vel(&o, p, v, vo, 1.0, 1.0, 1.0, NULL))) n++;

  o.type = -1;
  if(check("rad_vel", -1, rad_vel(&o, p, v, vo, 1.0, 1.0, 1.0, &rv))) n++;

  return n;
}

static int test_aberration() {
  double p[3] = {1.0}, v[3] = {};
  int n = 0;

  if(check("aberration:pos", -1, aberration(NULL, v, 0.0, p))) n++;
  if(check("aberration:vel", -1, aberration(p, NULL, 0.0, p))) n++;
  if(check("aberration:out", -1, aberration(p, v, 0.0, NULL))) n++;

  return n;
}

static int test_grav_vec() {
  double p[3] = {2.0}, po[3] = {0.0, 1.0}, pb[3] = {};
  int n = 0;

  if(check("grav_vec:pos", -1, grav_vec(NULL, po, pb, 1.0, p))) n++;
  if(check("grav_vec:po", -1, grav_vec(p, NULL, pb, 1.0, p))) n++;
  if(check("grav_vec:pb", -1, grav_vec(p, po, NULL, 1.0, p))) n++;
  if(check("grav_vec:out", -1, grav_vec(p, po, pb, 1.0, NULL))) n++;

  return n;
}

static int test_grav_def() {
  double p[3] = {2.0}, po[3] = {0.0, 1.0};
  int n = 0;

  if(check("grav_def:pos", -1, grav_def(NOVAS_JD_J2000, NOVAS_OBSERVER_AT_GEOCENTER, NOVAS_FULL_ACCURACY, NULL, po, p))) n++;
  if(check("grav_def:po", -1, grav_def(NOVAS_JD_J2000, NOVAS_OBSERVER_AT_GEOCENTER, NOVAS_FULL_ACCURACY, p, NULL, p))) n++;
  if(check("grav_def:out", -1, grav_def(NOVAS_JD_J2000, NOVAS_OBSERVER_AT_GEOCENTER, NOVAS_FULL_ACCURACY, p, po, NULL))) n++;

  if(check("grav_def:sun", 13, grav_def(NOVAS_JD_J2000, NOVAS_OBSERVER_AT_GEOCENTER, NOVAS_FULL_ACCURACY, p, po, p))) n++;

  fprintf(stderr, ">>> Expecting an error and trace...\n");
  novas_debug(NOVAS_DEBUG_EXTRA);
  enable_earth_sun_hp(1);
  if(check("grav_def:planets", 12, grav_def(NOVAS_JD_J2000, NOVAS_OBSERVER_AT_GEOCENTER, NOVAS_FULL_ACCURACY, p, po, p))) n++;
  enable_earth_sun_hp(0);
  novas_debug(NOVAS_DEBUG_OFF);

  return n;
}

static int test_grav_undef() {
  double p[3] = {2.0}, po[3] = {0.0, 1.0};
  int n = 0;

  if(check("grav_def:pos", -1, grav_undef(NOVAS_JD_J2000, NOVAS_FULL_ACCURACY, NULL, po, p))) n++;
  if(check("grav_def:po", -1, grav_undef(NOVAS_JD_J2000, NOVAS_FULL_ACCURACY, p, NULL, p))) n++;
  if(check("grav_def:out", -1, grav_undef(NOVAS_JD_J2000, NOVAS_FULL_ACCURACY, p, po, NULL))) n++;

  return n;
}

static int test_grav_init_planets() {
  novas_planet_bundle planets = {};
  double p[3] = {2.0};
  int n = 0;

  if(check("grav_init_planets:pos_obs", -1, obs_planets(NOVAS_JD_J2000, NOVAS_FULL_ACCURACY, NULL, 0, &planets))) n++;
  if(check("grav_init_planets:planets", -1, obs_planets(NOVAS_JD_J2000, NOVAS_FULL_ACCURACY, p, 0, NULL))) n++;

  return n;
}

static int test_grav_planets() {
  novas_planet_bundle planets = {};
  double p[3] = {2.0}, po[3] = {0.0, 1.0}, out[3] = {};
  int n = 0;

  if(check("grav_planets:pos_src", -1, grav_planets(NULL, po, &planets, out))) n++;
  if(check("grav_planets:pos_obs", -1, grav_planets(p, NULL, &planets, out))) n++;
  if(check("grav_planets:planets", -1, grav_planets(p, po, NULL, out))) n++;
  if(check("grav_planets:pos_src", -1, grav_planets(p, po, &planets, NULL))) n++;

  return n;
}

static int test_grav_undo_planets() {
  extern int novas_inv_max_iter;
  novas_planet_bundle planets = {};
  double p[3] = {2.0}, po[3] = {0.0, 1.0}, out[3] = {};
  int n = 0;

  if(check("grav_undo_planets:pos_app", -1, grav_undo_planets(NULL, po, &planets, out))) n++;
  if(check("grav_undo_planets:pos_obs", -1, grav_undo_planets(p, NULL, &planets, out))) n++;
  if(check("grav_undo_planets:planets", -1, grav_undo_planets(p, po, NULL, out))) n++;
    if(check("grav_undo_planets:pos_src", -1, grav_undo_planets(p, po, &planets, NULL))) n++;

  planets.mask = 1 << NOVAS_SUN;
  novas_inv_max_iter = 0;
  if(check("grav_undo_planets:converge", -1, grav_undo_planets(p, po, &planets, out))) n++;
  else if(check("grav_undo_planets:converge:errno", ECANCELED, errno)) n++;
  novas_inv_max_iter = 100;

  return n;
}

static int test_earth_sun_calc() {
  double p[3], v[3];
  int n = 0;

  if(check("earth_sun_calc:pos", -1, earth_sun_calc(NOVAS_JD_J2000, NOVAS_SUN, NOVAS_BARYCENTER, NULL, v))) n++;
  if(check("earth_sun_calc:vel", -1, earth_sun_calc(NOVAS_JD_J2000, NOVAS_SUN, NOVAS_BARYCENTER, p, NULL))) n++;
  if(check("earth_sun_calc:tdb:lo", 1, earth_sun_calc(2340000.0, NOVAS_SUN, NOVAS_BARYCENTER, p, v))) n++;
  if(check("earth_sun_calc:tdb:hi", 1, earth_sun_calc(2560001.0, NOVAS_SUN, NOVAS_BARYCENTER, p, v))) n++;
  if(check("earth_sun_calc:number", 2, earth_sun_calc(NOVAS_JD_J2000, NOVAS_JUPITER, NOVAS_BARYCENTER, p, v))) n++;

  return n;
}

static int test_earth_sun_calc_hp() {
  double p[3], v[3], tdb2[2] = { NOVAS_JD_J2000 };
  int n = 0;

  enable_earth_sun_hp(1);

  if(check("earth_sun_calc_hp:tdb", -1, earth_sun_calc_hp(NULL, NOVAS_SUN, NOVAS_BARYCENTER, p, v))) n++;
  if(check("earth_sun_calc_hp:pos", -1, earth_sun_calc_hp(tdb2, NOVAS_SUN, NOVAS_BARYCENTER, NULL, v))) n++;
  if(check("earth_sun_calc_hp:vel", -1, earth_sun_calc_hp(tdb2, NOVAS_SUN, NOVAS_BARYCENTER, p, NULL))) n++;
  if(check("earth_sun_calc_hp:number", 2, earth_sun_calc_hp(tdb2, NOVAS_JUPITER, NOVAS_BARYCENTER, p, v))) n++;

  enable_earth_sun_hp(0);

  return n;
}

static int test_sun_eph() {
  extern int sun_eph(double jd, double *ra, double *dec, double *dis);

  double ra, dec, dis;
  int n = 0;

  if(check("sun_eph:ra", -1, sun_eph(NOVAS_JD_J2000, NULL, &dec, &dis))) n++;
  if(check("sun_eph:dec", -1, sun_eph(NOVAS_JD_J2000, &ra, NULL, &dis))) n++;
  if(check("sun_eph:dis", -1, sun_eph(NOVAS_JD_J2000, &ra, &dec, NULL))) n++;

  return n;
}

static int test_obs_posvel() {
  observer obs;
  double x;
  int n = 0;

  make_observer_at_geocenter(&obs);

  if(check("obs_posvel:obs", -1, obs_posvel(NOVAS_JD_J2000, 0.0, NOVAS_REDUCED_ACCURACY, NULL, NULL, NULL, &x, NULL))) n++;
  if(check("obs_posvel:obs:pos+vel", -1, obs_posvel(NOVAS_JD_J2000, 0.0, NOVAS_REDUCED_ACCURACY, &obs, NULL, NULL, NULL, NULL))) n++;
  if(check("obs_posvel:obs:pos=vel", -1, obs_posvel(NOVAS_JD_J2000, 0.0, NOVAS_REDUCED_ACCURACY, &obs, NULL, NULL, &x, &x))) n++;

  obs.where = -1;
  if(check("obs_posvel:obs:where:-1", -1, obs_posvel(NOVAS_JD_J2000, 0.0, NOVAS_REDUCED_ACCURACY, &obs, NULL, NULL, &x, NULL))) n++;

  obs.where = NOVAS_OBSERVER_PLACES;
  if(check("obs_posvel:obs:where:hi", -1, obs_posvel(NOVAS_JD_J2000, 0.0, NOVAS_REDUCED_ACCURACY, &obs, NULL, NULL, &x, NULL))) n++;

  return n;
}

static int test_time() {
  novas_timespec time = {};
  int n = 0;
  long ijd = 0;

  if(check("time:set:time", -1, novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 37, 0.11, NULL))) n++;
  if(check("time:set:scale:-1", -1, novas_set_time(-1, NOVAS_JD_J2000, 37, 0.11, &time))) n++;
  if(check("time:set:scale:hi", -1, novas_set_time(NOVAS_TIMESCALES, NOVAS_JD_J2000, 37, 0.11, &time))) n++;

  if(check_nan("time:get:time", novas_get_time(NULL, NOVAS_TT))) n++;
  if(check_nan("time:get:scale:-1", novas_get_time(&time, -1))) n++;
  if(check_nan("time:get:scale:hi", novas_get_time(&time, NOVAS_TIMESCALES))) n++;

  if(check("time:get_unix_time:time", -1, novas_get_unix_time(NULL, &ijd))) n++;
  if(check("time:get_unix_time:time+ijd", -1, novas_get_unix_time(NULL, NULL))) n++;

  if(check("time:offset:time", -1, novas_offset_time(NULL, 0.1, &time))) n++;
  if(check("time:offset:out", -1, novas_offset_time(&time, 0.1, NULL))) n++;
  if(check("time:offset:both", -1, novas_offset_time(NULL, 0.1, NULL))) n++;

  if(check_nan("time:diff:t1", novas_diff_time(NULL, &time))) n++;
  if(check_nan("time:diff:t2", novas_diff_time(&time, NULL))) n++;
  if(check_nan("time:diff:both", novas_diff_time(NULL, NULL))) n++;

  if(check_nan("time:diff_tcg:t1", novas_diff_tcg(NULL, &time))) n++;
  if(check_nan("time:diff_tcg:t2", novas_diff_tcg(&time, NULL))) n++;
  if(check_nan("time:diff_tcg:both", novas_diff_tcg(NULL, NULL))) n++;

  if(check_nan("time:diff_tcb:t1", novas_diff_tcb(NULL, &time))) n++;
  if(check_nan("time:diff_tcb:t2", novas_diff_tcb(&time, NULL))) n++;
  if(check_nan("time:diff_tcb:both", novas_diff_tcb(NULL, NULL))) n++;


  return n;
}


static double switching_refraction(double jd_tt, const on_surface *loc, enum novas_refraction_type type, double el) {
  static int count;
  (void) jd_tt;
  (void) loc;
  (void) type;
  (void) el;
  return (count++) % 2 ? -0.1 : 0.1;
}


static int test_refraction() {
  int n = 0;
  on_surface obs = {};

  if(check_nan("stardard_refraction:loc", novas_standard_refraction(NOVAS_JD_J2000, NULL, NOVAS_REFRACT_OBSERVED, 10.0))) n++;
  if(check_nan("stardard_refraction:type:-2", novas_standard_refraction(NOVAS_JD_J2000, &obs, -2, 10.0))) n++;
  if(check_nan("stardard_refraction:type:1", novas_standard_refraction(NOVAS_JD_J2000, &obs, 1, 10.0))) n++;
  if(check_nan("stardard_refraction:el:neg", novas_standard_refraction(NOVAS_JD_J2000, &obs, 1, -10.0))) n++;

  if(check_nan("optical_refraction:loc", novas_optical_refraction(NOVAS_JD_J2000, NULL, NOVAS_REFRACT_OBSERVED, 10.0))) n++;
  if(check_nan("optical_refraction:type:-2", novas_optical_refraction(NOVAS_JD_J2000, &obs, -2, 10.0))) n++;
  if(check_nan("optical_refraction:type:1", novas_optical_refraction(NOVAS_JD_J2000, &obs, 1, 10.0))) n++;
  if(check_nan("optical_refraction:el:neg", novas_optical_refraction(NOVAS_JD_J2000, &obs, 1, -10.0))) n++;

  if(check_nan("radio_refraction:loc", novas_radio_refraction(NOVAS_JD_J2000, NULL, NOVAS_REFRACT_OBSERVED, 10.0))) n++;
  if(check_nan("radio_refraction:type:-2", novas_radio_refraction(NOVAS_JD_J2000, &obs, -2, 10.0))) n++;
  if(check_nan("radio_refraction:type:1", novas_radio_refraction(NOVAS_JD_J2000, &obs, 1, 10.0))) n++;
  if(check_nan("radio_refraction:el:neg", novas_radio_refraction(NOVAS_JD_J2000, &obs, 1, -10.0))) n++;

  if(check_nan("inv_refract:conv", novas_inv_refract(switching_refraction, NOVAS_JD_J2000, NULL, NOVAS_REFRACT_OBSERVED, 10.0))) n++;
  else if(check("inv_refract:conv:errno", ECANCELED, errno)) n++;

  fprintf(stderr, ">>> Expecting an error and trace...\n");
  novas_debug(1);
  novas_optical_refraction(NOVAS_JD_J2000, NULL, NOVAS_REFRACT_OBSERVED, 10.0);
  novas_debug(0);

  obs.humidity = -1.01;
  if(check_nan("radio_refraction:humidity:lo", novas_radio_refraction(NOVAS_JD_J2000, &obs, NOVAS_REFRACT_OBSERVED, 10.0))) n++;

  obs.humidity = 101.01;
  if(check_nan("radio_refraction:humidity:hi", novas_radio_refraction(NOVAS_JD_J2000, &obs, NOVAS_REFRACT_OBSERVED, 10.0))) n++;

  return n;
}

static int test_make_frame() {
  int n = 0;
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {};

  if(check("make_frame:obs", -1, novas_make_frame(NOVAS_REDUCED_ACCURACY, NULL, &ts, 0.0, 0.0, &frame))) n++;
  if(check("make_frame:time", -1, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, NULL, 0.0, 0.0, &frame))) n++;
  if(check("make_frame:frame", -1, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, NULL))) n++;
  if(check("make_frame:accuracy:-1", -1, novas_make_frame(-1, &obs, &ts, 0.0, 0.0, &frame))) n++;
  if(check("make_frame:accuracy:2", -1, novas_make_frame(2, &obs, &ts, 0.0, 0.0, &frame))) n++;

  obs.where = -1;
  if(check("make_frame:obs:where:-1", -1, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame))) n++;

  obs.where = NOVAS_OBSERVER_PLACES;
  if(check("make_frame:obs:where:hi", -1, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame))) n++;

  return n;
}

static int test_change_observer() {
  int n = 0;
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {}, out = {};

  make_observer_at_geocenter(&obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);

  if(check("change_observer:orig", -1, novas_change_observer(NULL, &obs, &out))) n++;
  if(check("change_observer:orig:init", -1, novas_change_observer(&frame, &obs, &out))) n++;

  if(check("change_observer:make_frame", 0, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame))) return 1;
  if(check("change_observer:orig:ok", 0, novas_change_observer(&frame, &obs, &out))) n++;

  if(check("change_observer:obs", -1, novas_change_observer(&frame, NULL, &out))) n++;
  if(check("change_observer:out", -1, novas_change_observer(&frame, &obs, NULL))) n++;

  return n;
}

static int test_make_transform() {
  int n = 0;
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {};
  novas_transform T = {};

  make_observer_at_geocenter(&obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);

  if(check("make_transform:frame", -1, novas_make_transform(NULL, NOVAS_ICRS, NOVAS_J2000, &T))) n++;
  if(check("make_transform:frame:init", -1, novas_make_transform(&frame, NOVAS_ICRS, NOVAS_J2000, &T))) n++;

  if(check("make_transform:frame", 0, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame))) n++;
  if(check("make_transform:out", -1, novas_make_transform(&frame, NOVAS_ICRS, NOVAS_J2000, NULL))) n++;

  if(check("make_transform:from:-1", -1, novas_make_transform(&frame, -1, NOVAS_J2000, &T))) n++;
  if(check("make_transform:from:hi", -1, novas_make_transform(&frame, NOVAS_REFERENCE_SYSTEMS, NOVAS_J2000, &T))) n++;

  if(check("make_transform:to:-1", -1, novas_make_transform(&frame, NOVAS_ICRS, -1, &T))) n++;
  if(check("make_transform:to:hi", -1, novas_make_transform(&frame, NOVAS_ICRS, NOVAS_REFERENCE_SYSTEMS, &T))) n++;

  return n;
}

static int test_geom_posvel() {
  int n = 0;
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {};
  object o = {};
  double pos[3] = {}, vel[3] = {};

  make_observer_at_geocenter(&obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);

  if(check("geom_posvel:frame", -1, novas_geom_posvel(&o, NULL, NOVAS_ICRS, pos, vel))) n++;
  if(check("geom_posvel:frame:init", -1, novas_geom_posvel(&o, &frame, NOVAS_ICRS, pos, vel))) n++;

  novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame);
  if(check("geom_posvel:frame:ok", 0, novas_geom_posvel(&o, &frame, NOVAS_ICRS, pos, vel))) n++;

  if(check("geom_posvel:object", -1, novas_geom_posvel(NULL, &frame, NOVAS_ICRS, pos, vel))) n++;
  if(check("geom_posvel:pos+vel", -1, novas_geom_posvel(&o, &frame, NOVAS_ICRS, NULL, NULL))) n++;
  if(check("geom_posvel:pos=vel", -1, novas_geom_posvel(&o, &frame, NOVAS_ICRS, pos, pos))) n++;
  if(check("geom_posvel:sys:-1", -1, novas_geom_posvel(&o, &frame, -1, pos, vel))) n++;
  if(check("geom_posvel:sys:hi", -1, novas_geom_posvel(&o, &frame, NOVAS_REFERENCE_SYSTEMS, pos, vel))) n++;

  frame.accuracy = -1;
  if(check("geom_posvel:frame:accuracy:-1", -1, novas_geom_posvel(&o, &frame, NOVAS_ICRS, pos, vel))) n++;

  frame.accuracy = 2;
  if(check("geom_posvel:frame:accuracy:2", -1, novas_geom_posvel(&o, &frame, NOVAS_ICRS, pos, vel))) n++;

#ifndef DEFAULT_READEPH
  frame.accuracy = NOVAS_REDUCED_ACCURACY;
  make_ephem_object("blah", 111111, &o);
  if(check("geom_posvel:ephem_object", -1, novas_geom_posvel(&o, &frame, NOVAS_ICRS, pos, vel))) n++;
#endif

  return n;
}

static int test_sky_pos() {
  int n = 0;
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {};
  object o = {};
  sky_pos out = {};

  make_observer_at_geocenter(&obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);

  if(check("sky_pos:frame", -1, novas_sky_pos(&o, NULL, NOVAS_ICRS, &out))) n++;
  if(check("sky_pos:frame:init", -1, novas_sky_pos(&o, &frame, NOVAS_ICRS, &out))) n++;

  novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame);
  if(check("sky_pos:frame:ok", 0, novas_sky_pos(&o, &frame, NOVAS_ICRS, &out))) n++;

  if(check("sky_pos:object", -1, novas_sky_pos(NULL, &frame, NOVAS_ICRS, &out))) n++;
  if(check("sky_pos:out", -1, novas_sky_pos(&o, &frame, NOVAS_ICRS, NULL))) n++;
  if(check("sky_pos:sys:-1", -1, novas_sky_pos(&o, &frame, -1, &out))) n++;
  if(check("sky_pos:sys:hi", -1, novas_sky_pos(&o, &frame, NOVAS_REFERENCE_SYSTEMS, &out))) n++;

  frame.accuracy = -1;
  if(check("sky_pos:frame:accuracy:-1", -1, novas_sky_pos(&o, &frame, NOVAS_ICRS, &out))) n++;

  frame.accuracy = 2;
  if(check("sky_pos:frame:accuracy:2", -1, novas_sky_pos(&o, &frame, NOVAS_ICRS, &out))) n++;

  frame.accuracy = NOVAS_FULL_ACCURACY;
  if(check("sky_pos:frame:accuracy:full", 73, novas_sky_pos(&o, &frame, NOVAS_ICRS, &out))) n++;

  return n;
}

static int test_app_to_geom() {
  extern int novas_inv_max_iter;
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {};
  double pos[3] = {};
  int n = 0;

  make_observer_at_geocenter(&obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);

  if(check("app_to_geom:frame", -1, novas_app_to_geom(NULL, NOVAS_ICRS, 1.0, 2.0, 10.0, pos))) n++;
  if(check("app_to_geom:frame:init", -1, novas_app_to_geom(&frame, NOVAS_ICRS, 1.0, 2.0, 10.0, pos))) n++;

  novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame);
  novas_inv_max_iter = 0;
  if(check("app_to_geom:frame:converge", -1, novas_app_to_geom(&frame, NOVAS_ICRS, 1.0, 2.0, 10.0, pos))) n++;
  else if(check("app_to_geom:frame:converge:errno", ECANCELED, errno)) n++;
  novas_inv_max_iter = 100;

  if(check("app_to_geom:pos", -1, novas_app_to_geom(&frame, NOVAS_ICRS, 1.0, 2.0, 10.0, NULL))) n++;
  if(check("app_to_geom:sys:-1", -1, novas_app_to_geom(&frame, -1, 1.0, 2.0, 10.0, pos))) n++;
  if(check("app_to_geom:sys:hi", -1, novas_app_to_geom(&frame, NOVAS_REFERENCE_SYSTEMS, 1.0, 2.0, 10.0, pos))) n++;

  return n;
}

static int test_geom_to_app() {
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {};
  sky_pos out = {};
  double pos[3] = {};
  int n = 0;

  make_observer_on_surface(1.0, 2.0, 3.0, 4.0, 1001.0, &obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);

  if(check("geom_to_app:frame", -1, novas_geom_to_app(NULL, pos, NOVAS_ICRS, &out))) n++;
  if(check("geom_to_app:frame:init", -1, novas_geom_to_app(&frame, pos, NOVAS_ICRS, &out))) n++;

  novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame);
  if(check("geom_to_app:frame:ok", 0, novas_geom_to_app(&frame, pos, NOVAS_ICRS, &out))) n++;

  if(check("geom_to_app:pos", -1, novas_geom_to_app(&frame, NULL, NOVAS_ICRS, &out))) n++;
  if(check("geom_to_app:sys:-1", -1, novas_geom_to_app(&frame, pos, -1, &out))) n++;
  if(check("geom_to_app:sys:hi", -1, novas_geom_to_app(&frame, pos, NOVAS_REFERENCE_SYSTEMS, &out))) n++;

  frame.accuracy = -1;
  if(check("geom_to_app:frame:accuracy:-1", -1, novas_geom_to_app(&frame, pos, NOVAS_ICRS, &out))) n++;

  frame.accuracy = 2;
  if(check("geom_to_app:frame:accuracy:2", -1, novas_geom_to_app(&frame, pos, NOVAS_ICRS, &out))) n++;

  return n;
}

static int test_app_to_hor() {
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {};
  double az, el;
  int n = 0;

  make_observer_on_surface(1.0, 2.0, 3.0, 4.0, 1001.0, &obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);

  if(check("app_to_hor:frame", -1, novas_app_to_hor(NULL, NOVAS_ICRS, 1.0, 2.0, NULL, &az, &el))) n++;
  if(check("app_to_hor:frame:init", -1, novas_app_to_hor(&frame, NOVAS_ICRS, 1.0, 2.0, NULL, &az, &el))) n++;

  novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame);
  if(check("app_to_hor:frame:ok", 0, novas_app_to_hor(&frame, NOVAS_ICRS, 1.0, 2.0, NULL, &az, &el))) n++;

  if(check("app_to_hor:az+el", -1, novas_app_to_hor(&frame, NOVAS_ICRS, 1.0, 2.0, NULL, NULL, NULL))) n++;
  if(check("app_to_hor:sys:-1", -1, novas_app_to_hor(&frame, -1, 1.0, 2.0, NULL, &az, &el))) n++;
  if(check("app_to_hor:sys:hi", -1, novas_app_to_hor(&frame, NOVAS_REFERENCE_SYSTEMS, 1.0, 2.0, NULL, &az, &el))) n++;

  frame.observer.where = NOVAS_OBSERVER_AT_GEOCENTER;
  if(check("app_to_hor:frame:obs:where", -1, novas_app_to_hor(&frame, NOVAS_ICRS, 1.0, 2.0, NULL, &az, &el))) n++;

  return n;
}

static int test_hor_to_app() {
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {};
  double ra, dec;
  int n = 0;

  make_observer_on_surface(1.0, 2.0, 3.0, 4.0, 1001.0, &obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);

  if(check("hor_to_app:frame", -1, novas_hor_to_app(NULL, 1.0, 2.0, NULL, NOVAS_ICRS, &ra, &dec))) n++;
  if(check("hor_to_app:frame:init", -1, novas_hor_to_app(&frame, 1.0, 2.0, NULL, NOVAS_ICRS, &ra, &dec))) n++;

  novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame);
  if(check("hor_to_app:frame:ok", 0, novas_hor_to_app(&frame, 1.0, 2.0, NULL, NOVAS_ICRS, &ra, &dec))) n++;

  if(check("hor_to_app:ra+dec", -1, novas_hor_to_app(&frame, 1.0, 2.0, NULL, NOVAS_ICRS, NULL, NULL))) n++;
  if(check("hor_to_app:sys:-1", -1, novas_hor_to_app(&frame, 1.0, 2.0, NULL, -1, &ra, &dec))) n++;
  if(check("hor_to_app:sys:hi", -1, novas_hor_to_app(&frame, 1.0, 2.0, NULL, NOVAS_REFERENCE_SYSTEMS, &ra, &dec))) n++;

  frame.observer.where = NOVAS_OBSERVER_AT_GEOCENTER;
  if(check("hor_to_app:frame:obs:where", -1, novas_hor_to_app(&frame, 1.0, 2.0, NULL, NOVAS_ICRS, &ra, &dec))) n++;

  return n;
}


static int test_transform_vector() {
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {};
  novas_transform T = {};
  double pos[3] = {};
  int n = 0;

  make_observer_at_geocenter(&obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);
  novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame);

  novas_make_transform(&frame, NOVAS_ICRS, NOVAS_J2000, &T);

  if(check("transform_vector:in", -1, novas_transform_vector(NULL, &T, pos))) n++;
  if(check("transform_vector:out", -1, novas_transform_vector(pos, &T, NULL))) n++;
  if(check("transform_vector:in+out", -1, novas_transform_vector(NULL, &T, NULL))) n++;
  if(check("transform_vector:in", -1, novas_transform_vector(pos, NULL, pos))) n++;

  return n;
}

static int test_transform_sky_pos() {
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {};
  novas_transform T = {};
  sky_pos pos = {};
  int n = 0;


  make_observer_at_geocenter(&obs);
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);
  novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame);

  novas_make_transform(&frame, NOVAS_ICRS, NOVAS_J2000, &T);

  if(check("transform_sky_pos:in", -1, novas_transform_sky_pos(NULL, &T, &pos))) n++;
  if(check("transform_sky_pos:out", -1, novas_transform_sky_pos(&pos, &T, NULL))) n++;
  if(check("transform_sky_pos:in+out", -1, novas_transform_sky_pos(NULL, &T, NULL))) n++;
  if(check("transform_sky_pos:in", -1, novas_transform_sky_pos(&pos, NULL, &pos))) n++;

  return n;
}

static int test_inv_transform() {
  novas_transform T = {};
  int n = 0;

  if(check("invert_transform:in", -1, novas_invert_transform(NULL, &T))) n++;
  if(check("invert_transform:out", -1, novas_invert_transform(&T, NULL))) n++;
  if(check("invert_transform:in+out", -1, novas_invert_transform(NULL, NULL))) n++;

  return n;
}

static int test_redshift_vrad() {
  int n = 0;

  if(check_nan("redshift_vrad", redshift_vrad(0.0, -1.0))) n++;
  return n;
}

static int test_unredshift_vrad() {
  int n = 0;

  if(check_nan("unredshift_vrad", unredshift_vrad(0.0, -1.0))) n++;
  return n;
}

static int test_z_add() {
  int n = 0;

  if(check_nan("z_add:z1", novas_z_add(-1.0, 0.0))) n++;
  if(check_nan("z_add:z2", novas_z_add(0.0, -1.0))) n++;
  if(check_nan("z_add:z1+z2", novas_z_add(-1.0, -1.0))) n++;
  return n;
}

static int test_z_inv() {
  int n = 0;

  if(check_nan("z_inv", novas_z_inv(-1.0))) n++;
  return n;
}

static int test_novas_to_naif_planet() {
  int n = 0;

  if(check("novas_to_naif_planet:-2", -1, novas_to_naif_planet(-2))) n++;
  if(check("novas_to_naif_planet:-1", -1, novas_to_naif_planet(-1))) n++;
  if(check("novas_to_naif_planet:hi", -1, novas_to_naif_planet(NOVAS_PLANETS))) n++;

  return n;
}

static int test_novas_to_dexxx_planet() {
  int n = 0;

  if(check("novas_to_dexxx_planet:-2", -1, novas_to_dexxx_planet(-2))) n++;
  if(check("novas_to_dexxx_planet:-1", -1, novas_to_dexxx_planet(-1))) n++;
  if(check("novas_to_dexxx_planet:hi", -1, novas_to_dexxx_planet(NOVAS_PLANETS))) n++;

  return n;
}

static int test_naif_to_novas_planet() {
  int n = 0;

  if(check("naif_to_novas_planet:-2", -1, naif_to_novas_planet(-1))) n++;
  if(check("naif_to_novas_planet:-1", -1, naif_to_novas_planet(-1))) n++;
  if(check("naif_to_novas_planet:", -1, naif_to_novas_planet(-1))) n++;
  if(check("naif_to_novas_planet:500", -1, naif_to_novas_planet(500))) n++;
  if(check("naif_to_novas_planet:501", -1, naif_to_novas_planet(501))) n++;
  if(check("naif_to_novas_planet:598", -1, naif_to_novas_planet(598))) n++;
  if(check("naif_to_novas_planet:1000", -1, naif_to_novas_planet(1000))) n++;

  return n;
}

static int test_planet_for_name() {
  int n = 0;

  if(check("planet_for_name:NULL", -1, novas_planet_for_name(NULL))) n++;
  if(check("planet_for_name:blah", -1, novas_planet_for_name(""))) n++;
  if(check("planet_for_name:blah", -1, novas_planet_for_name("blah"))) n++;
  if(check("planet_for_name:blah", -1, novas_planet_for_name("solar"))) n++;
  if(check("planet_for_name:blah", -1, novas_planet_for_name("Solar flare"))) n++;
  if(check("planet_for_name:blah", -1, novas_planet_for_name("Solar system"))) n++;
  if(check("planet_for_name:blah", -1, novas_planet_for_name("Solar system size"))) n++;

  return n;
}

static int test_make_orbital_object() {
  int n = 0;
  novas_orbital orbit = {};
  object body = {};

  if(check("make_orbital_object:orbit", -1, make_orbital_object("blah", -1, NULL, &body))) n++;
  if(check("make_orbital_object:body", -1, make_orbital_object("blah", -1, &orbit, NULL))) n++;
  if(check("make_orbital_object:orbit+body", -1, make_orbital_object("blah", -1, NULL, NULL))) n++;

  return n;
}

static int test_orbit_posvel() {
  int n = 0;
  double pos[3] = {}, vel[3] = {};
  int saved = novas_inv_max_iter;
  novas_orbital orbit = NOVAS_ORBIT_INIT;

  orbit.a = 1.0;

  if(check("set_obsys_pole:orbit", -1, novas_orbit_posvel(0.0, NULL, NOVAS_REDUCED_ACCURACY, pos, vel))) n++;
  if(check("set_obsys_pole:pos=vel", -1, novas_orbit_posvel(0.0, &orbit, NOVAS_REDUCED_ACCURACY, pos, pos))) n++;
  if(check("set_obsys_pole:pos=vel:NULL", -1, novas_orbit_posvel(0.0, &orbit, NOVAS_REDUCED_ACCURACY, NULL, NULL))) n++;
  if(check("set_obsys_pole:accuracy:-1", -1, novas_orbit_posvel(0.0, &orbit, -1, pos, vel))) n++;
  if(check("set_obsys_pole:accuracy:2", -1, novas_orbit_posvel(0.0, &orbit, 2, pos, vel))) n++;

  if(check("set_obsys_pole:orbit:converge", 0, novas_orbit_posvel(0.0, &orbit, NOVAS_REDUCED_ACCURACY, pos, vel))) n++;

  novas_inv_max_iter = 0;
  if(check("set_obsys_pole:orbit:converge", -1, novas_orbit_posvel(0.0, &orbit, NOVAS_REDUCED_ACCURACY, pos, vel))) n++;
  else if(check("set_obsys_pole:orbit:converge:errno", ECANCELED, errno)) n++;
  novas_inv_max_iter = saved;

  orbit.system.type = -1;
  if(check("set_obsys_pole:orbit:type:-1", -1, novas_orbit_posvel(0.0, &orbit, NOVAS_REDUCED_ACCURACY, pos, vel))) n++;

  orbit.system.type = NOVAS_REFERENCE_SYSTEMS;
  if(check("set_obsys_pole:orbit:type:hi", -1, novas_orbit_posvel(0.0, &orbit, NOVAS_REDUCED_ACCURACY, pos, vel))) n++;

  orbit.system.plane = NOVAS_EQUATORIAL_PLANE;
  orbit.system.type = NOVAS_REFERENCE_SYSTEMS;
  if(check("set_obsys_pole:orbit:type:-1:eq", -1, novas_orbit_posvel(0.0, &orbit, NOVAS_REDUCED_ACCURACY, pos, vel))) n++;

  orbit.system.type = NOVAS_GCRS;
  orbit.system.plane = -1;
  if(check("set_obsys_pole:orbit:plane:-1", -1, novas_orbit_posvel(0.0, &orbit, NOVAS_REDUCED_ACCURACY, pos, vel))) n++;

  return n;
}

static int test_set_obsys_pole() {
  int n = 0;

  if(check("set_obsys_pole:orbit", -1, novas_set_orbsys_pole(NOVAS_GCRS, 0.0, 0.0, NULL))) n++;

  return n;
}

int main() {
  int n = 0;

  if(test_v2z()) n++;
  if(test_z2v()) n++;

  if(test_make_on_surface()) n++;
  if(test_make_in_space()) n++;
  if(test_make_observer()) n++;
  if(test_make_airborne_observer()) n++;

  if(test_make_object()) n++;
  if(test_make_cat_object()) n++;
  if(test_make_redshifted_object()) n++;
  if(test_make_ephem_object()) n++;
  if(test_make_planet()) n++;
  if(test_make_cat_entry()) n++;
  if(test_transform_cat()) n++;
  if(test_transform_hip()) n++;

  if(test_refract()) n++;
  if(test_refract_astro()) n++;
  if(test_inv_refract()) n++;
  if(test_limb_angle()) n++;

  if(test_ephemeris()) n++;

  if(test_j2000_to_tod()) n++;
  if(test_tod_to_j2000()) n++;
  if(test_gcrs_to_cirs()) n++;
  if(test_cirs_to_gcrs()) n++;
  if(test_cirs_to_app_ra()) n++;
  if(test_app_to_cirs_ra()) n++;

  if(test_set_planet_provider()) n++;
  if(test_set_planet_provider_hp()) n++;

  if(test_place()) n++;
  if(test_place_star()) n++;
  if(test_radec_planet()) n++;
  if(test_mean_star()) n++;

  if(test_equ2gal()) n++;
  if(test_gal2equ()) n++;

  if(test_equ2ecl_vec()) n++;
  if(test_ecl2equ_vec()) n++;
  if(test_equ2ecl()) n++;
  if(test_ecl2equ()) n++;

  if(test_itrs_to_hor()) n++;
  if(test_hor_to_itrs()) n++;

  if(test_equ2hor()) n++;
  if(test_gcrs2equ()) n++;

  if(test_sidereal_time()) n++;
  if(test_ter2cel()) n++;
  if(test_cel2ter()) n++;

  if(test_spin()) n++;
  if(test_wobble()) n++;
  if(test_terra()) n++;
  if(test_e_tilt()) n++;
  if(test_cel_pole()) n++;
  if(test_frame_tie()) n++;

  if(test_proper_motion()) n++;
  if(test_bary2obs()) n++;
  if(test_geo_posvel()) n++;

  if(test_light_time2()) n++;
  if(test_d_light()) n++;

  if(test_cio_array()) n++;
  if(test_cio_basis()) n++;
  if(test_cio_location()) n++;
  if(test_cio_ra()) n++;

  if(test_starvectors()) n++;
  if(test_radec2vector()) n++;
  if(test_vector2radec()) n++;

  if(test_planet_lon()) n++;
  if(test_fund_args()) n++;
  if(test_nutation_angles()) n++;
  if(test_set_nutation_lp_provider()) n++;
  if(test_nutation()) n++;
  if(test_precession()) n++;
  if(test_rad_vel()) n++;
  if(test_aberration()) n++;
  if(test_grav_vec()) n++;
  if(test_grav_def()) n++;
  if(test_grav_undef()) n++;
  if(test_grav_init_planets()) n++;
  if(test_grav_planets()) n++;
  if(test_grav_undo_planets()) n++;

  if(test_earth_sun_calc()) n++;
  if(test_earth_sun_calc_hp()) n++;
  if(test_sun_eph()) n++;

  if(test_obs_posvel()) n++;
  if(test_time()) n++;
  if(test_refraction()) n++;

  if(test_make_frame()) n++;
  if(test_change_observer()) n++;
  if(test_make_transform()) n++;
  if(test_geom_posvel()) n++;
  if(test_geom_to_app()) n++;
  if(test_app_to_geom()) n++;
  if(test_app_to_hor()) n++;
  if(test_hor_to_app()) n++;
  if(test_sky_pos()) n++;
  if(test_transform_vector()) n++;
  if(test_transform_sky_pos()) n++;
  if(test_inv_transform()) n++;

  if(test_redshift_vrad()) n++;
  if(test_unredshift_vrad()) n++;
  if(test_z_add()) n++;
  if(test_z_inv()) n++;

  if(test_novas_to_naif_planet()) n++;
  if(test_novas_to_dexxx_planet()) n++;
  if(test_naif_to_novas_planet()) n++;

  if(test_planet_for_name()) n++;
  if(test_make_orbital_object()) n++;
  if(test_set_obsys_pole()) n++;
  if(test_orbit_posvel()) n++;

  if(test_gcrs_to_tod()) n++;
  if(test_tod_to_gcrs()) n++;
  if(test_gcrs_to_mod()) n++;
  if(test_mod_to_gcrs()) n++;

  if(n) fprintf(stderr, " -- FAILED %d tests\n", n);
  else fprintf(stderr, " -- OK\n");

  return n;
}
