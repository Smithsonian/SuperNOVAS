/**
 * @file
 *
 * @date Created  on Feb 19, 2024
 * @author Attila Kovacs
 */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <string.h>

#include "novas.h"

static int check(const char *func, int exp, int error) {
  if(error != exp) {
    fprintf(stderr, "ERROR! %s: expected %d, got %d\n", func, exp, error);
    return 1;
  }
  return 0;
}

static int test_make_on_surface() {
  on_surface loc;
  if(check("make_on_surface", -1, make_on_surface(0.0, 0.0, 0.0, 0.0, 0.0, NULL))) return 1;
  return 0;
}

static int test_make_in_space() {
  double p[3], v[3];
  in_space sp;
  int n = 0;

  if(check("make_in_space", -1, make_in_space(p, v, NULL))) n++;
  if(check("make_in_space:p", 0, make_in_space(NULL, v, &sp))) n++;
  if(check("make_in_space:v", 0, make_in_space(p, NULL, &sp))) n++;

  return n;
}

static int test_make_observer() {
  in_space sp;
  on_surface on;
  observer obs;
  int n = 0;

  if(check("make_observer:where", 1, make_observer(-1, &on, &sp, &obs))) n++;
  if(check("make_observer", -1, make_observer(NOVAS_OBSERVER_AT_GEOCENTER, &on, &sp, NULL))) n++;
  if(check("make_observer:on", -1, make_observer(NOVAS_OBSERVER_ON_EARTH, NULL, &sp, &obs))) n++;
  if(check("make_observer:sp", -1, make_observer(NOVAS_OBSERVER_IN_EARTH_ORBIT, &on, NULL, &obs))) n++;

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
  cat_entry s;
  object o;
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

static int test_refract() {
  on_surface o;
  int n = 0;

  errno = 0;
  double r = refract(NULL, NOVAS_STANDARD_ATMOSPHERE, 30.0);
  if(check("refract_loc", 1, r == 0.0 && errno == EINVAL)) n++;

  errno = 0;
  r = refract(&o, -1, 30.0);
  if(check("refract_loc", 1, r == 0.0 && errno == EINVAL)) n++;

  errno = 0;
  r = refract(&o, NOVAS_STANDARD_ATMOSPHERE, 90.11);
  if(check("refract_loc", 1, r == 0.0 && errno == EINVAL)) n++;

  return n;
}

static int test_limb_angle() {
  double pos[3], a, b;
  int n = 0;

  if(check("limb_angle:pos_obj", -1, limb_angle(NULL, pos, &a, &b))) n++;
  if(check("limb_angle:pos_obs", -1, limb_angle(pos, NULL, &a, &b))) n++;

  return n;
}


static int test_transform_cat() {
  cat_entry c, c1;
  int n = 0;

  char longname[SIZE_OF_OBJ_NAME + 1];
  memset(longname, 'A', SIZE_OF_OBJ_NAME);

  if(check("transform_cat:in", -1, transform_cat(PRECESSION, NOVAS_JD_B1950, NULL, NOVAS_JD_J2000, "FK5", &c))) n++;
  if(check("transform_cat:out", -1, transform_cat(PRECESSION, NOVAS_JD_B1950, &c, NOVAS_JD_J2000, "FK5", NULL))) n++;
  if(check("transform_cat:option", -1, transform_cat(-1, NOVAS_JD_B1950, &c, NOVAS_JD_J2000, "FK5", &c1))) n++;
  if(check("transform_cat:name", 2, transform_cat(PRECESSION, NOVAS_JD_B1950, &c, NOVAS_JD_J2000, longname, &c))) n++;

  return n;
}

static int test_transform_hip() {
  cat_entry c;
  int n = 0;

  if(check("transform_hip:in", -1, transform_hip(NULL, &c))) n++;
  if(check("transform_hip:in", -1, transform_hip(&c, NULL))) n++;

  return n;
}

static int test_ephemeris() {
  double p[3], v[3];
  double tdb[2] = { NOVAS_JD_J2000 };
  object mars;
  int n = 0;

  if(check("ephemeris:body", -1, ephemeris(tdb, NULL, NOVAS_BARYCENTER, NOVAS_FULL_ACCURACY, p, v))) n++;
  if(check("ephemeris:jd", -1, ephemeris(NULL, &mars, NOVAS_BARYCENTER, NOVAS_FULL_ACCURACY, p, v))) n++;
  if(check("ephemeris:origin", 1, ephemeris(tdb, &mars, -1, NOVAS_FULL_ACCURACY, p, v))) n++;

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
  double p[3];
  int n = 0;

  if(check("tod_to_j2000:in", -1, tod_to_j2000(0.0, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("tod_to_j2000:out", -1, tod_to_j2000(0.0, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("tod_to_j2000:accuracy", -1, tod_to_j2000(0.0, -1, p, p))) n++;

  return n;
}

static int test_gcrs_to_cirs() {
  double p[3];
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

static int test_set_planet_provider() {
  if(check("set_planet_provider", -1, set_planet_provider(NULL))) return 1;
  return 0;
}

static int test_set_planet_provider_hp() {
  if(check("set_planet_provider_hp", -1, set_planet_provider_hp(NULL))) return 1;
  return 0;
}

static int test_place_star() {
  cat_entry c;
  observer loc;
  sky_pos pos;
  int n = 0;

  if(check("place_star:in", -1, place_star(0.0, NULL, &loc, 0.0, NOVAS_GCRS, NOVAS_FULL_ACCURACY, &pos))) n++;
  if(check("place_star:out", -1, place_star(0.0, &c, &loc, 0.0, NOVAS_GCRS, NOVAS_FULL_ACCURACY, NULL))) n++;

  return n;
}

static int test_place() {
  object o;
  observer loc;
  sky_pos pos;
  int n = 0;

  if(check("place:object", -1, place(0.0, NULL, &loc, 0.0, NOVAS_GCRS, NOVAS_FULL_ACCURACY, &pos))) n++;
  if(check("place:sys:lo", 1, place(0.0, &o, &loc, 0.0, -1, NOVAS_FULL_ACCURACY, &pos))) n++;
  if(check("place:sys:hi", 1, place(0.0, &o, &loc, 0.0, NOVAS_REFERENCE_SYSTEMS, NOVAS_FULL_ACCURACY, &pos))) n++;
  if(check("place:accuracy", 2, place(0.0, &o, &loc, 0.0, NOVAS_GCRS, -1, &pos))) n++;

  return n;
}

static int test_radec_planet() {
  object o;
  observer loc;
  double ra, dec, dis, rv;

  o.type = NOVAS_CATALOG_OBJECT;

  if(check("radec_planet", -1, radec_planet(0.0, &o, &loc, 0.0, NOVAS_GCRS, NOVAS_FULL_ACCURACY, &ra, &dec, &dis, &rv))) return 1;
  return 0;
}

static int test_mean_star() {
  double x;
  int n = 0;

  if(check("mean_star:ira", -1, mean_star(0.0, 0.0, 0.0, NOVAS_FULL_ACCURACY, NULL, &x))) n++;
  if(check("mean_star:idec", -1, mean_star(0.0, 0.0, 0.0, NOVAS_FULL_ACCURACY, &x, NULL))) n++;

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
  double p[3];
  int n = 0;

  if(check("equ2ecl_vec:in", -1, equ2ecl_vec(0.0, NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("equ2ecl_vec:out", -1, equ2ecl_vec(0.0, NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("equ2ecl_vec:accuracy", -1, equ2ecl_vec(0.0, NOVAS_MEAN_EQUATOR, -1, p, p))) n++;
  if(check("equ2ecl_vec:equator", 1, equ2ecl_vec(0.0, -1, NOVAS_FULL_ACCURACY, p, p))) n++;

  return n;
}

static int test_ecl2equ_vec() {
  double p[3];
  int n = 0;

  if(check("ecl2equ_vec:in", -1, ecl2equ_vec(0.0, NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("ecl2equ_vec:out", -1, ecl2equ_vec(0.0, NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, p, NULL))) n++;
  if(check("ecl2equ_vec:accuracy", -1, ecl2equ_vec(0.0, NOVAS_MEAN_EQUATOR, -1, p, p))) n++;
  if(check("ecl2equ_vec:equator", 1, ecl2equ_vec(0.0, -1, NOVAS_FULL_ACCURACY, p, p))) n++;

  return n;
}


static int test_itrs_to_hor() {
  on_surface loc;
  double p[3], az, za;
  int n = 0;

  if(check("itrs_to_hor:loc", -1, itrs_to_hor(NULL, p, &az, &za))) n++;
  if(check("itrs_to_hor:in", -1, itrs_to_hor(&loc, NULL, &az, &za))) n++;

  return n;
}

static int test_hor_to_itrs() {
  on_surface loc;
  double p[3];
  int n = 0;

  if(check("hor_to_itrs:loc", -1, hor_to_itrs(NULL, 0.0, 0.0, p))) n++;
  if(check("hor_to_itrs:in", -1, hor_to_itrs(&loc, 0.0, 0.0, NULL))) n++;

  return n;
}

static int test_equ2hor() {
  on_surface loc;
  double p[3], az, za, rar, decr;
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
  double p[3];
  int n = 0;

  if(check("ter2cel:in", -1, ter2cel(0.0, 0.0, 0.0, EROT_GST, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, NULL, p))) n++;
  if(check("ter2cel:out", -1, ter2cel(0.0, 0.0, 0.0, EROT_GST, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p, NULL))) n++;
  if(check("ter2cel:accuracy", 1, ter2cel(0.0, 0.0, 0.0, EROT_GST, -1, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p, p))) n++;
  if(check("ter2cel:erot", 2, ter2cel(0.0, 0.0, 0.0, -1, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p, p))) n++;

  return n;
}

static int test_cel2ter() {
  double p[3];
  int n = 0;

  if(check("cel2ter:in", -1, cel2ter(0.0, 0.0, 0.0, EROT_GST, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, NULL, p))) n++;
  if(check("cel2ter:out", -1, cel2ter(0.0, 0.0, 0.0, EROT_GST, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p, NULL))) n++;
  if(check("cel2ter:accuracy", 1, cel2ter(0.0, 0.0, 0.0, EROT_GST, -1, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p, p))) n++;
  if(check("cel2ter:erot", 2, cel2ter(0.0, 0.0, 0.0, -1, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p, p))) n++;

  return n;
}


static int test_spin() {
  double p[3];
  int n = 0;

  if(check("spin:in", -1, spin(0.0, NULL, p))) n++;
  if(check("spin:out", -1, spin(0.0, p, NULL))) n++;

  return n;
}

static int test_wobble() {
  double p[3];
  int n = 0;

  if(check("wobble:in", -1, wobble(0.0, WOBBLE_ITRS_TO_PEF, 0.0, 0.0, NULL, p))) n++;
  if(check("wobble:out", -1, wobble(0.0, WOBBLE_ITRS_TO_PEF, 0.0, 0.0, p, NULL))) n++;

  return n;
}

static int test_terra() {
  on_surface loc;
  double p[3], v[3];
  int n = 0;

  if(check("terra:loc", -1, terra(NULL, 0.0, p, v))) n++;
  if(check("terra:same", -1, terra(&loc, 0.0, p, p))) n++;

  return n;
}

static int test_e_tilt() {
  on_surface loc;
  double p[3], v[3];
  int n = 0;

  if(check("e_tilt:accuracy", -1, e_tilt(0.0, -1, NULL, NULL, NULL, NULL, NULL))) n++;

  return n;
}



static int test_cel_pole() {
  on_surface loc;
  double p[3], v[3];
  int n = 0;

  if(check("cel_pole:type", 1, cel_pole(0.0, -1, 0.0, 0.0))) n++;

  return n;
}

static int test_frame_tie() {
  double p[3];
  int n = 0;

  if(check("frame_tie:in", -1, frame_tie(NULL, 0, p))) n++;
  if(check("frame_tie:out", -1, frame_tie(p, 0, NULL))) n++;

  return n;
}

static int test_proper_motion() {
  double p[3], v[3];
  int n = 0;

  if(check("frame_tie:p", -1, proper_motion(0.0, NULL, v, 1.0, p))) n++;
  if(check("frame_tie:v", -1, proper_motion(0.0, p, NULL, 1.0, p))) n++;
  if(check("frame_tie:out", -1, proper_motion(0.0, p, v, 1.0, NULL))) n++;

  return n;
}

static int test_bary2obs() {
  double p[3], po[3], out[3], lt;
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
  object o;
  double pos[3], p[3], v[3], t;
  int n = 0;

  if(check("light_time2:tout", -1, light_time2(0.0, &o, pos, 0.0, NOVAS_FULL_ACCURACY, p, v, NULL))) n++;
  if(check("light_time2:object", -1, light_time2(0.0, NULL, pos, 0.0, NOVAS_FULL_ACCURACY, p, v, &t))) n++;
  if(check("light_time2:pos", -1, light_time2(0.0, &o, NULL, 0.0, NOVAS_FULL_ACCURACY, p, v, &t))) n++;
  if(check("light_time2:same1", -1, light_time2(0.0, &o, pos, 0.0, NOVAS_FULL_ACCURACY, pos, v, &t))) n++;
  if(check("light_time2:same2", -1, light_time2(0.0, &o, pos, 0.0, NOVAS_FULL_ACCURACY, p, pos, &t))) n++;
  if(check("light_time2:same3", -1, light_time2(0.0, &o, pos, 0.0, NOVAS_FULL_ACCURACY, p, p, &t))) n++;

  return n;
}

static int test_d_light() {
  double p[3];
  int n = 0;

  if(check("d_light:1", 1, isnan(d_light(NULL, p)))) n++;
  if(check("d_light:2", 1, isnan(d_light(p, NULL)))) n++;

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
  if(check("cio_array:beg", 2, cio_array(0.0, 5, x))) n++;
  if(check("cio_array:end", 2, cio_array(1e20, 5, x))) n++;

  if(check("cio_array:corner:lo", 6, cio_array(2341952.6, 5, x))) n++;
  if(check("cio_array:corner:hi", 6, cio_array(2561137.4, 5, x))) n++;

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
  if(check("starvectors:same", -1, starvectors(&star, p, p))) n++;

  return n;
}

static int test_radec2vector() {
  int n = 0;

  if(check("radec2vector", -1, radec2vector(0.0, 0.0, 1.0, NULL))) n++;

  return n;
}

static int test_vector2radec() {
  double p[3] = { }, ra, dec;
  int n = 0;

  if(check("vector2radec:vec", -1, vector2radec(NULL, &ra, &dec))) n++;

  if(check("vector2radec:vec", 1, vector2radec(p, &ra, &dec))) n++;

  p[2] = 1.0;
  if(check("vector2radec:vec", 2, vector2radec(p, &ra, &dec))) n++;

  return n;
}

static int test_planet_lon() {
  int n = 0;

  if(check("planet_lon", 1, isnan(planet_lon(0.0, -1)))) n++;

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
  double p[3];
  int n = 0;

  if(check("nutation:in", -1, nutation(0.0, NUTATE_MEAN_TO_TRUE, NOVAS_FULL_ACCURACY, NULL, p))) n++;
  if(check("nutation:out", -1, nutation(0.0, NUTATE_MEAN_TO_TRUE, NOVAS_FULL_ACCURACY, p, NULL))) n++;

  return n;
}

static int test_precession() {
  double p[3];
  int n = 0;

  if(check("precesion:in", -1, precession(0.0, NULL, 1.0, p))) n++;
  if(check("precesion:out", -1, precession(0.0, p, 1.0, NULL))) n++;

  return n;
}

static int test_rad_vel() {
  object o;
  double p[3], v[3], vo[3], rv;
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
  double p[3], v[3] = {};
  int n = 0;

  if(check("aberration:pos", -1, aberration(NULL, v, 0.0, p))) n++;
  if(check("aberration:vel", -1, aberration(p, NULL, 0.0, p))) n++;
  if(check("aberration:out", -1, aberration(p, v, 0.0, NULL))) n++;

  return n;
}

static int test_grav_vec() {
  double p[3], po[3], pb[3];
  int n = 0;

  if(check("grav_vec:pos", -1, grav_vec(NULL, po, pb, 1.0, p))) n++;
  if(check("grav_vec:po", -1, grav_vec(p, NULL, pb, 1.0, p))) n++;
  if(check("grav_vec:pb", -1, grav_vec(p, po, NULL, 1.0, p))) n++;
  if(check("grav_vec:out", -1, grav_vec(p, po, pb, 1.0, NULL))) n++;
  if(check("grav_vec:same", -1, grav_vec(p, po, pb, 1.0, po))) n++;

  return n;
}

static int test_grav_def() {
  double p[3], po[3], pb[3];
  int n = 0;

  if(check("grav_def:pos", -1, grav_def(0.0, NOVAS_OBSERVER_AT_GEOCENTER, NOVAS_FULL_ACCURACY, NULL, po, p))) n++;
  if(check("grav_def:po", -1, grav_def(0.0, NOVAS_OBSERVER_AT_GEOCENTER, NOVAS_FULL_ACCURACY, p, NULL, p))) n++;
  if(check("grav_def:out", -1, grav_def(0.0, NOVAS_OBSERVER_AT_GEOCENTER, NOVAS_FULL_ACCURACY, p, po, NULL))) n++;
  if(check("grav_def:same", -1, grav_def(0.0, NOVAS_OBSERVER_AT_GEOCENTER, NOVAS_FULL_ACCURACY, p, po, po))) n++;

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

static int test_sun_eph() {
  extern int sun_eph(double jd, double *ra, double *dec, double *dis);

  double ra, dec, dis;
  int n = 0;

  if(check("sun_eph", -1, sun_eph(NOVAS_JD_J2000, NULL, &dec, &dis))) n++;
  if(check("sun_eph", -1, sun_eph(NOVAS_JD_J2000, &ra, NULL, &dis))) n++;
  if(check("sun_eph", -1, sun_eph(NOVAS_JD_J2000, &ra, &dec, NULL))) n++;

  return n;
}

int main() {
  int n = 0;

  if(test_make_on_surface()) n++;
  if(test_make_in_space()) n++;
  if(test_make_observer()) n++;

  if(test_make_object()) n++;
  if(test_make_ephem_object()) n++;
  if(test_make_planet()) n++;
  if(test_make_cat_entry()) n++;
  if(test_transform_cat()) n++;
  if(test_transform_hip()) n++;

  if(test_refract()) n++;
  if(test_limb_angle()) n++;

  if(test_ephemeris()) n++;

  if(test_j2000_to_tod()) n++;
  if(test_tod_to_j2000()) n++;
  if(test_gcrs_to_cirs()) n++;
  if(test_cirs_to_gcrs()) n++;

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

  if(test_earth_sun_calc()) n++;
  if(test_sun_eph()) n++;

  if(n) fprintf(stderr, " -- FAILED %d tests\n", n);
  else fprintf(stderr, " -- OK\n");

  return n;
}
