/**
 * @date Created  on Feb 18, 2024
 * @author Attila Kovacs
 */


// We'll use gcc major version as a proxy for the glibc library to decide which feature macro to use.
// gcc 5.1 was released 2015-04-22...
#if defined(__GNUC__) && (__GNUC__ < 5)
#  define _BSD_SOURCE            ///< strcasecmp() feature macro for glibc <= 2.19
#else
#  define _DEFAULT_SOURCE        ///< strcasecmp() feature macro starting glibc 2.20 (2014-09-08)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <libgen.h>

#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"

#define J2000   NOVAS_JD_J2000


static char *workPath;

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
  (void) jd_tdb;
  (void) origin;
  memset(position, 0, 3 * sizeof(double));
  memset(velocity, 0, 3 * sizeof(double));
  position[0] = body % 10;
  velocity[1] = 0.01 * (body % 10);
  return 0;
}

static short dummy_planet(double jd_tdb, enum novas_planet body, enum novas_origin origin, double *position, double *velocity) {
  double tdb2[2] = { tdb };
  (void) jd_tdb;
  return dummy_planet_hp(tdb2, body, origin, position, velocity);
}

static int dummy_ephem(const char *name, long id, double jd_tdb_high, double jd_tdb_low, enum novas_origin *origin, double *pos, double *vel) {
  (void) name;
  (void) jd_tdb_high;
  (void) jd_tdb_low;
  *origin = ephem_origin;
  memset(pos, 0, 3 * sizeof(double));
  memset(vel, 0, 3 * sizeof(double));
  pos[0] = id % 100;
  vel[1] = 0.01 * (id % 100);
  return 0;
}

static int check_equal_pos(const double *posa, const double *posb, double tol) {
  int i;

  tol = fabs(tol);
  if(tol < 1e-30) tol = 1e-30;

  for(i = 0; i < 3; i++) {
    if(fabs(posa[i] - posb[i]) <= tol) continue;
    if(isnan(posa[i]) && isnan(posb[i])) continue;

    fprintf(stderr, "  A[%d] = %.9g vs B[%d] = %.9g (delta=%.1g)\n", i, posa[i], i, posb[i], posa[i] - posb[i]);
    return i + 1;
  }

  return 0;
}

static int is_ok(const char *func, int error) {
  if(error) fprintf(stderr, "ERROR %d! %s (source = %s, from = %d)\n", error, func, source.name, obs.where);
  return !error;
}

static int is_equal(const char *func, double v1, double v2, double prec) {
  if(fabs(v1 - v2) < prec) return 1;

  fprintf(stderr, "ERROR! %s (%.12g != %.12g)\n", func, v1, v2);
  return 0;
}

static double vlen(double *pos) {
  return sqrt(pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2]);
}

static int test_gcrs_j2000_gcrs() {
  double pos1[3];

  if(!is_ok("gcrs_to_j2000", gcrs_to_j2000(pos0, pos1))) return 1;
  if(!is_ok("j2000_to_gcrs", j2000_to_gcrs(pos1, pos1))) return 1;
  if(!is_ok("gcrs_j2000_gcrs", check_equal_pos(pos0, pos1, 1e-12 * vlen(pos0)))) return 1;
  return 0;
}

static int test_j2000_tod_j2000() {
  double pos1[3];

  if(!is_ok("j2000_to_tod", j2000_to_tod(tdb, 0, pos0, pos1))) return 1;
  if(!is_ok("tod_to_j2000", tod_to_j2000(tdb, 0, pos1, pos1))) return 1;
  if(!is_ok("j2000_tod_j2000", check_equal_pos(pos0, pos1, 1e-12 * vlen(pos0)))) return 1;
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
  if(!is_ok("gcrs_cirs_gcrs", check_equal_pos(pos0, pos1, 1e-12 * vlen(pos0)))) return 1;
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
    if(!is_ok("itrs_hor_itrs", check_equal_pos(p, pos1, 1e-12))) return 1;

    if(!is_ok("itrs_to_hor:az:null", itrs_to_hor(&obs.on_surf, p, NULL, &za))) return 1;
    if(!is_ok("itrs_to_hor:za:null", itrs_to_hor(&obs.on_surf, p, &az, NULL))) return 1;
  }
  return 0;
}

static int test_cel2ter2cel() {
  double l = vlen(pos0), p0[3], p[3];
  int i;

  for(i=0; i < 3; i++) p0[i] = pos0[i] / l;

  if(obs.where != NOVAS_OBSERVER_ON_EARTH) return 0;

  if(!is_ok("cel2ter2cel:cel2ter:gst", cel2ter(tdb, 0.0, 0.0, EROT_GST, NOVAS_FULL_ACCURACY, NOVAS_REFERENCE_CLASS, 0.0, 0.0, p0, p))) return 1;
  if(!is_ok("cel2ter2cel:ter2cel:gst", ter2cel(tdb, 0.0, 0.0, EROT_GST, NOVAS_FULL_ACCURACY, NOVAS_REFERENCE_CLASS, 0.0, 0.0, p, p))) return 1;
  if(!is_ok("cel2ter2cel:gst:check", check_equal_pos(p, p0, 1e-12))) return 1;

  if(!is_ok("cel2ter2cel:cel2ter:gst:dyn", cel2ter(tdb, 0.0, 0.0, EROT_GST, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p0, p))) return 1;
  if(!is_ok("cel2ter2cel:ter2cel:gst:dyn", ter2cel(tdb, 0.0, 0.0, EROT_GST, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p, p))) return 1;
  if(!is_ok("cel2ter2cel:gst:dyn:check", check_equal_pos(p, p0, 1e-12))) return 1;

  if(!is_ok("cel2ter2cel:cel2ter:era", cel2ter(tdb, 0.0, 0.0, EROT_ERA, NOVAS_FULL_ACCURACY, NOVAS_REFERENCE_CLASS, 0.0, 0.0, p0, p))) return 1;
  if(!is_ok("cel2ter2cel:ter2cel:era", ter2cel(tdb, 0.0, 0.0, EROT_ERA, NOVAS_FULL_ACCURACY, NOVAS_REFERENCE_CLASS, 0.0, 0.0, p, p))) return 1;
  if(!is_ok("cel2ter2cel:era:check", check_equal_pos(p, p0, 1e-12))) return 1;

  if(!is_ok("cel2ter2cel:cel2ter:era:dyn", cel2ter(tdb, 0.0, 0.0, EROT_ERA, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p0, p))) return 1;
  if(!is_ok("cel2ter2cel:ter2cel:era:dyn", ter2cel(tdb, 0.0, 0.0, EROT_ERA, NOVAS_FULL_ACCURACY, NOVAS_DYNAMICAL_CLASS, 0.0, 0.0, p, p))) return 1;
  if(!is_ok("cel2ter2cel:era:dyn:check", check_equal_pos(p, p0, 1e-12))) return 1;

  if(!is_ok("cel2ter2cel:cel2ter:gst:dx", cel2ter(tdb, 0.0, 0.0, EROT_GST, NOVAS_FULL_ACCURACY, NOVAS_REFERENCE_CLASS, 1.0, 0.0, p0, p))) return 1;
  if(!is_ok("cel2ter2cel:ter2cel:gst:dx", ter2cel(tdb, 0.0, 0.0, EROT_GST, NOVAS_FULL_ACCURACY, NOVAS_REFERENCE_CLASS, 1.0, 0.0, p, p))) return 1;
  if(!is_ok("cel2ter2cel:gst:dx:check", check_equal_pos(p, p0, 1e-12))) return 1;

  if(!is_ok("cel2ter2cel:cel2ter:gst:dy", cel2ter(tdb, 0.0, 0.0, EROT_GST, NOVAS_FULL_ACCURACY, NOVAS_REFERENCE_CLASS, 0.0, 1.0, p0, p))) return 1;
  if(!is_ok("cel2ter2cel:ter2cel:gst:dy", ter2cel(tdb, 0.0, 0.0, EROT_GST, NOVAS_FULL_ACCURACY, NOVAS_REFERENCE_CLASS, 0.0, 1.0, p, p))) return 1;
  if(!is_ok("cel2ter2cel:gst:dy:check", check_equal_pos(p, p0, 1e-12))) return 1;

  if(!is_ok("cel2ter2cel:cel2ter:era:dx", cel2ter(tdb, 0.0, 0.0, EROT_ERA, NOVAS_FULL_ACCURACY, NOVAS_REFERENCE_CLASS, 1.0, 0.0, p0, p))) return 1;
  if(!is_ok("cel2ter2cel:ter2cel:era:dx", ter2cel(tdb, 0.0, 0.0, EROT_ERA, NOVAS_FULL_ACCURACY, NOVAS_REFERENCE_CLASS, 1.0, 0.0, p, p))) return 1;
  if(!is_ok("cel2ter2cel:era:dx:check", check_equal_pos(p, p0, 1e-12))) return 1;

  if(!is_ok("cel2ter2cel:cel2ter:era:dy", cel2ter(tdb, 0.0, 0.0, EROT_ERA, NOVAS_FULL_ACCURACY, NOVAS_REFERENCE_CLASS, 0.0, 1.0, p0, p))) return 1;
  if(!is_ok("cel2ter2cel:ter2cel:era:dy", ter2cel(tdb, 0.0, 0.0, EROT_ERA, NOVAS_FULL_ACCURACY, NOVAS_REFERENCE_CLASS, 0.0, 1.0, p, p))) return 1;
  if(!is_ok("cel2ter2cel:era:dy:check", check_equal_pos(p, p0, 1e-12))) return 1;

  return 0;
}


static int test_equ2hor() {
  int a;

  if(obs.where != NOVAS_OBSERVER_ON_EARTH) return 0;

  for(a = 0; a < 24.0; a += 2) {
    int d;
    for(d = -90; d <= 90; d += 30) {
      double ra = a, dec = d, az, za, rar, decr;

      if(!is_ok("equ2hor:rar:null", equ2hor(tdb, 0.0, NOVAS_REDUCED_ACCURACY, 0.0, 0.0, &obs.on_surf, ra, dec, NOVAS_STANDARD_ATMOSPHERE, &az, &za, NULL, &decr))) return 1;
      if(!is_ok("equ2hor:decr:null", equ2hor(tdb, 0.0, NOVAS_REDUCED_ACCURACY, 0.0, 0.0, &obs.on_surf, ra, dec, NOVAS_STANDARD_ATMOSPHERE, &az, &za, &rar, NULL))) return 1;
    }
  }

  return 0;
}

static int test_aberration() {
  double p[3], v[3] = {}, out[3];

  //if(source.type != NOVAS_PLANET) return 0;

  memcpy(p, pos0, sizeof(p));

  if(!is_ok("aberration:corner:diff", aberration(p, v, 0.0, out))) return 1;
  if(!is_ok("aberration:corner:same", aberration(p, v, 0.0, p))) return 1;
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

  for(a = 0; a < 24; a += 2) {
    int d;
    for(d = -90; d <= 90; d += 30) {
      double ra0 = a, dec0 = d, elon, elat, ra, dec;

      if(!is_ok("equ2ecl", equ2ecl(tdb, NOVAS_GCRS_EQUATOR, NOVAS_FULL_ACCURACY, ra0, dec0, &elon, &elat))) return 1;
      if(!is_ok("ecl2equ", ecl2equ(tdb, NOVAS_GCRS_EQUATOR, NOVAS_FULL_ACCURACY, elon, elat, &ra, &dec))) return 1;
      if(!is_ok("equ_ecl_equ:ra", fabs(remainder((ra - ra0), 24.0) * cos(dec0 * DEG2RAD)) > 1e-8)) return 1;
      if(!is_ok("equ_ecl_equ:dec", fabs(dec - dec0) > 1e-7)) return 1;
    }
  }

  return 0;
}


static int test_equ_gal() {
  int a;

  for(a = 0; a < 24; a += 2) {
    int d;
    for(d = -90; d <= 90; d += 30) {
      double ra0 = a, dec0 = d, glon, glat, ra, dec;

      if(!is_ok("equ2gal", equ2gal(ra0, dec0, &glon, &glat))) return 1;
      if(!is_ok("gal2equ", gal2equ(glon, glat, &ra, &dec))) return 1;
      if(!is_ok("equ_gal_equ:ra", fabs(remainder((ra - ra0), 24.0) * cos(dec0 * DEG2RAD)) > 1e-8)) return 1;
      if(!is_ok("equ_gal_equ:dec", fabs(dec - dec0) > 1e-7)) return 1;
    }
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

static int test_place_mod() {
  int i;

  if(obs.where != NOVAS_OBSERVER_AT_GEOCENTER) return 0;

  for(i = 0; i < 4; i++) {
    sky_pos posa = {}, posb = {};
    if(!is_ok("place_mod", place_mod(tdb, &source, 1, &posa))) return 1;
    if(!is_ok("place_mod:control", place(tdb, &source, &obs, ut12tt, NOVAS_MOD, 1, &posb))) return 1;
    if(!is_ok("place_mod:check", check_equal_pos(posa.r_hat, posb.r_hat, 1e-9))) return 1;
  }

  return 0;
}

static int test_place_j2000() {
  int i;

  if(obs.where != NOVAS_OBSERVER_AT_GEOCENTER) return 0;

  for(i = 0; i < 4; i++) {
    sky_pos posa = {}, posb = {};
    if(!is_ok("place_j2000", place_j2000(tdb, &source, 1, &posa))) return 1;
    if(!is_ok("place_j2000:control", place(tdb, &source, &obs, ut12tt, NOVAS_J2000, 1, &posb))) return 1;
    if(!is_ok("place_j2000:check", check_equal_pos(posa.r_hat, posb.r_hat, 1e-9))) return 1;
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
    if(!is_ok("radec_star:check_pos", check_equal_pos(posa.r_hat, posb.r_hat, 1e-9))) return 1;
    if(!is_ok("radec_star:check_rv", fabs(rv - posb.rv) > 1e-6)) return 1;

    if(!is_ok("radec_star:ra:null", radec_star(tdb, &source.star, &obs, ut12tt, i, 1, NULL, &dec, &rv))) return 1;
    if(!is_ok("radec_star:dec:null", radec_star(tdb, &source.star, &obs, ut12tt, i, 1, &ra, NULL, &rv))) return 1;
    if(!is_ok("radec_star:rv:null", radec_star(tdb, &source.star, &obs, ut12tt, i, 1, &ra, &dec, NULL))) return 1;
  }

  return 0;
}


static int test_app_hor(enum novas_reference_system sys) {
  char label[50];
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {};

  double ra = source.star.ra, dec = source.star.dec, az, el, ra1, dec1, x, y;

  sprintf(label, "app_hor:sys=%d:set_time", sys);
  if(!is_ok(label, novas_set_time(NOVAS_TT, tdb, 32, 0.0, &ts))) return 1;

  sprintf(label, "app_hor:sys=%d:make_observer", sys);
  if(!is_ok(label, make_observer_on_surface(1.0, 2.0, 3.0, 4.0, 1001.0, &obs))) return 1;

  sprintf(label, "app_hor:sys=%d:make_frame", sys);
  if(!is_ok(label, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame))) return 1;

  sprintf(label, "app_hor:sys=%d:app_to_hor", sys);
  if(!is_ok(label, novas_app_to_hor(&frame, sys, ra, dec, NULL, &az, &el))) return 1;

  sprintf(label, "app_hor:sys=%d:app_to_hor:air", sys);
  frame.observer.where = NOVAS_AIRBORNE_OBSERVER;
  if(!is_ok(label, novas_app_to_hor(&frame, sys, ra, dec, NULL, &x, &y))) return 1;
  if(!is_equal(label, az, x, 1e-6)) return 1;
  if(!is_equal(label, el, y, 1e-6)) return 1;
  frame.observer.where = NOVAS_OBSERVER_ON_EARTH;

  sprintf(label, "app_hor:sys=%d:app_to_hor:no_az", sys);
  if(!is_ok(label, novas_app_to_hor(&frame, sys, ra, dec, NULL, NULL, &x))) return 1;
  if(!is_equal(label, x, el, 1e-9)) return 1;

  sprintf(label, "app_hor:sys=%d:app_to_hor:no_el", sys);
  if(!is_ok(label, novas_app_to_hor(&frame, sys, ra, dec, NULL, &x, NULL))) return 1;
  if(!is_equal(label, x, az, 1e-9)) return 1;

  sprintf(label, "app_hor:sys=%d:hor_to_app", sys);
  if(!is_ok(label, novas_hor_to_app(&frame, az, el, NULL, sys, &ra1, &dec1))) return 1;

  sprintf(label, "app_hor:sys=%d:hor_to_app:air", sys);
  frame.observer.where = NOVAS_AIRBORNE_OBSERVER;
  if(!is_ok(label, novas_hor_to_app(&frame, az, el, NULL, sys, &x, &y))) return 1;
  if(!is_equal(label, ra1, x, 1e-6)) return 1;
  if(!is_equal(label, dec1, y, 1e-6)) return 1;
  frame.observer.where = NOVAS_OBSERVER_ON_EARTH;

  sprintf(label, "app_hor:sys=%d:hor_to_app:no_ra", sys);
  if(!is_ok(label, novas_hor_to_app(&frame, az, el, NULL, sys, NULL, &x))) return 1;
  if(!is_equal(label, x, dec1, 1e-9)) return 1;

  sprintf(label, "app_hor:sys=%d:hor_to_app:no_dec", sys);
  if(!is_ok(label, novas_hor_to_app(&frame, az, el, NULL, sys, &x, NULL))) return 1;
  if(!is_equal(label, x, ra1, 1e-9)) return 1;

  sprintf(label, "app_hor:sys=%d:trip:ra", sys);
  if(!is_equal(label, ra1, ra, 1e-7)) return 1;

  sprintf(label, "app_hor:sys=%d:trip:dec", sys);
  if(!is_equal(label, dec1, dec, 1e-6)) return 1;

  sprintf(label, "app_hor:sys=%d:app_to_hor:refract", sys);
  if(!is_ok(label, novas_app_to_hor(&frame, sys, ra, dec, novas_standard_refraction, &az, &el))) return 1;

  sprintf(label, "app_hor:sys=%d:hor_to_app:refract", sys);
  if(!is_ok(label, novas_hor_to_app(&frame, az, el, novas_standard_refraction, sys, &ra1, &dec1))) return 1;

  // TODO check against cel2ter...
  sprintf(label, "app_hor:sys=%d:refract:ra", sys);
  if(!is_equal(label, ra1, ra, 1e-6)) return 1;

  sprintf(label, "app_hor:sys=%d:refract:dec", sys);
  if(!is_equal(label, dec1, dec, 1e-5)) return 1;


  return 0;
}


static int test_app_to_hor_compat() {
  char label[50];
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {};
  double pos1[3] = {};

  double ra, dec, az, el, az1, za1;

  sprintf(label, "app_to_hor_compat:set_time");
  if(!is_ok(label, novas_set_time(NOVAS_TT, tdb, 37, 0.0, &ts))) return 1;

  sprintf(label, "app_to_hor_compat:make_observer");
  if(!is_ok(label, make_observer_on_surface(1.0, 2.0, 3.0, 4.0, 1001.0, &obs))) return 1;

  sprintf(label, "app_to_hor_compat:make_frame");
  if(!is_ok(label, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame))) return 1;

  vector2radec(pos0, &ra, &dec);

  if(!is_ok("app_to_hor_comat:app_to_hor:cirs",
          novas_app_to_hor(&frame, NOVAS_CIRS, ra, dec, NULL, &az, &el))) return 1;
  if(!is_ok("app_to_hor_compat:cirs_to_itrs",
          cirs_to_itrs(tdb, 0.0, 69.184, NOVAS_REDUCED_ACCURACY, 0.0, 0.0, pos0, pos1))) return 1;
  if(!is_ok("app_to_hor_compat:itrs_to_hor:cirs", itrs_to_hor(&obs.on_surf, pos1, &az1, &za1))) return 1;
  if(!is_equal("app_to_hor_compat:check:az", az, az1, 1e-6)) return 1;
  if(!is_equal("app_to_hor_compat:check:el", el, 90.0 - za1, 1e-6)) return 1;

  if(!is_ok("app_to_hor_comat:app_to_hor:tod",
          novas_app_to_hor(&frame, NOVAS_TOD, ra, dec, NULL, &az, &el))) return 1;
  if(!is_ok("app_to_hor_compat:tod_to_itrs",
          tod_to_itrs(tdb, 0.0, 69.184, NOVAS_REDUCED_ACCURACY, 0.0, 0.0, pos0, pos1))) return 1;
  if(!is_ok("app_to_hor_compat:itrs_to_hor:tod", itrs_to_hor(&obs.on_surf, pos1, &az1, &za1))) return 1;
  if(!is_equal("app_to_hor_compat:check:az", az, az1, 1e-6)) return 1;
  if(!is_equal("app_to_hor_compat:check:el", el, 90.0 - za1, 1e-6)) return 1;

  return 0;
}

static int test_app_geom(enum novas_reference_system sys) {
  char label[50];
  novas_timespec ts = {};
  int i;

  for(i = 0; i < NOVAS_OBSERVER_PLACES; i++) {
    observer obs = {};
    novas_frame frame = {};
    double sc_pos[3] = {};
    double sc_vel[3] = {};
    double pos1[3] = {};
    sky_pos app = {};

    switch(i) {
      case NOVAS_OBSERVER_AT_GEOCENTER: make_observer_at_geocenter(&obs); break;
      case NOVAS_OBSERVER_ON_EARTH: make_observer_on_surface(1.0, 2.0, 3.0, 4.0, 1001.0, &obs); break;
      case NOVAS_OBSERVER_IN_EARTH_ORBIT: make_observer_in_space(sc_pos, sc_vel, &obs); break;
      case NOVAS_AIRBORNE_OBSERVER: {
        on_surface loc;
        make_on_surface(1.0, 2.0, 3.0, 4.0, 1001.0, &loc);
        make_airborne_observer(&loc, sc_vel, &obs);
        break;
      }
      case NOVAS_SOLAR_SYSTEM_OBSERVER: make_solar_system_observer(sc_pos, sc_vel, &obs); break;
      default: return -1;
    }

    sprintf(label, "app_hor:sys=%d:obs=%d:set_time", sys, i);
    if(!is_ok(label, novas_set_time(NOVAS_TT, tdb, 32, 0.0, &ts))) return 1;

    sprintf(label, "app_hor:sys=%d:obs=%d:make_frame", sys, i);
    if(!is_ok(label, novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame))) return 1;

    sprintf(label, "app_hor:sys=%d:obs=%d:geom_to_app", sys, i);
    if(!is_ok(label, novas_geom_to_app(&frame, pos0, sys, &app))) return 1;

    sprintf(label, "app_hor:sys=%d:obs=%d:app_to_geom", sys, i);
    if(!is_ok(label, novas_app_to_geom(&frame, sys, app.ra, app.dec, vlen(pos0), pos1))) return 1;

    sprintf(label, "app_hor:sys=%d:obs=%d:check", sys, i);
    if(!is_ok(label, check_equal_pos(pos1, pos0, 1e-8 * vlen(pos0)))) return 1;

  }
  return 0;
}

static int test_transform_icrs_cirs() {
  novas_transform T = {};
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {};
  double pos1[3] = {1}, pos2[3] = {2};


  if(!is_ok("transform:icrs_cirs:set_time", novas_set_time(NOVAS_TDB, tdb, 32, 0.0, &ts))) return 1;
  if(!is_ok("transform:icrs_cirs:make_observer", make_observer_at_geocenter(&obs))) return 1;
  if(!is_ok("transform:icrs_cirs:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame))) return 1;


  novas_make_transform(&frame, NOVAS_ICRS, NOVAS_CIRS, &T);
  novas_transform_vector(pos0, &T, pos1);
  novas_make_transform(&frame, NOVAS_GCRS, NOVAS_CIRS, &T);
  novas_transform_vector(pos0, &T, pos2);
  if(!is_ok("transform:icrs_cirs:gcrs", check_equal_pos(pos1, pos2, 1e-12 * vlen(pos0)))) return 1;

  gcrs_to_cirs(tdb, NOVAS_REDUCED_ACCURACY, pos0, pos2);
  if(!is_ok("transform:icrs_cirs:check", check_equal_pos(pos1, pos2, 1e-12 * vlen(pos0)))) return 1;

  return 0;
}

static int test_transform_icrs_j2000() {
  novas_transform T = {};
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {};
  double pos1[3] = {1}, pos2[3] = {2};


  if(!is_ok("transform:icrs_j2000:set_time", novas_set_time(NOVAS_TDB, tdb, 32, 0.0, &ts))) return 1;
  if(!is_ok("transform:icrs_j2000:make_observer", make_observer_at_geocenter(&obs))) return 1;
  if(!is_ok("transform:icrs_j2000:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame))) return 1;


  novas_make_transform(&frame, NOVAS_ICRS, NOVAS_J2000, &T);
  novas_transform_vector(pos0, &T, pos1);
  novas_make_transform(&frame, NOVAS_GCRS, NOVAS_J2000, &T);
  novas_transform_vector(pos0, &T, pos2);
  if(!is_ok("transform:icrs_j2000:gcrs", check_equal_pos(pos1, pos2, 1e-12 * vlen(pos0)))) return 1;

  gcrs_to_j2000(pos0, pos2);
  if(!is_ok("transform:icrs_j2000:check", check_equal_pos(pos1, pos2, 1e-12 * vlen(pos0)))) return 1;

  return 0;
}

static int test_transform_j2000_mod() {
  novas_transform T = {};
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {};
  double pos1[3] = {1}, pos2[3] = {2};

  if(!is_ok("transform:j2000_mod:set_time", novas_set_time(NOVAS_TDB, tdb, 32, 0.0, &ts))) return 1;
  if(!is_ok("transform:j2000_mod:make_observer", make_observer_at_geocenter(&obs))) return 1;
  if(!is_ok("transform:j2000_mod:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame))) return 1;

  novas_make_transform(&frame, NOVAS_J2000, NOVAS_MOD, &T);
  novas_transform_vector(pos0, &T, pos1);

  precession(NOVAS_JD_J2000, pos0, tdb, pos2);
  if(!is_ok("transform:j2000_mod:check", check_equal_pos(pos1, pos2, 1e-12 * vlen(pos0)))) return 1;

  return 0;
}

static int test_transform_mod_tod() {
  novas_transform T = {};
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {};
  double pos1[3] = {1}, pos2[3] = {2};

  if(!is_ok("transform:mod_tod:set_time", novas_set_time(NOVAS_TDB, tdb, 32, 0.0, &ts))) return 1;
  if(!is_ok("transform:mod_tod:make_observer", make_observer_at_geocenter(&obs))) return 1;
  if(!is_ok("transform:mod_tod:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame))) return 1;

  novas_make_transform(&frame, NOVAS_MOD, NOVAS_TOD, &T);
  novas_transform_vector(pos0, &T, pos1);

  nutation(tdb, NUTATE_MEAN_TO_TRUE, NOVAS_REDUCED_ACCURACY, pos0, pos2);
  if(!is_ok("transform:mod_tod:check", check_equal_pos(pos1, pos2, 1e-12 * vlen(pos0)))) return 1;

  return 0;
}

static int test_transform_inv() {
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {};
  enum novas_reference_system from;

  if(!is_ok("transform_rev:set_time", novas_set_time(NOVAS_TDB, tdb, 32, 0.0, &ts))) return 1;
  if(!is_ok("transform_rev:make_observer", make_observer_at_geocenter(&obs))) return 1;
  if(!is_ok("transform_rev:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame))) return 1;

  for(from = 0; from < NOVAS_REFERENCE_SYSTEMS; from++) {
    char label[50];
    enum novas_reference_system to;

    for(to = from; to < NOVAS_REFERENCE_SYSTEMS; to++) {
      novas_transform T = {}, I = {};
      double pos1[3] = {1}, pos2[3] = {2};

      novas_make_transform(&frame, from, to, &T);
      novas_make_transform(&frame, to, from, &I);

      novas_transform_vector(pos0, &T, pos1);
      novas_transform_vector(pos1, &I, pos2);

      sprintf(label, "transform_rev:from=%d:to=%d", from, to);
      if(!is_ok(label, check_equal_pos(pos0, pos2, 1e-12 * vlen(pos0)))) return 1;
    }
  }

  return 0;
}

static int test_gcrs_to_tod() {
  double pos1[3] = {}, pos2[3] = {}, d;
  int n = 0;

  d = novas_vlen(pos0);

  if(!is_ok("gcrs_to_tod", gcrs_to_tod(tdb, NOVAS_FULL_ACCURACY, pos0, pos1))) n++;

  gcrs_to_j2000(pos0, pos2);
  j2000_to_tod(tdb, NOVAS_FULL_ACCURACY, pos2, pos2);

  if(!is_ok("gcrs_to_tod:check", check_equal_pos(pos1, pos2, 1e-9 * d))) n++;

  if(!is_ok("gcrs_to_tod:tod_to_gcrs", tod_to_gcrs(tdb, NOVAS_FULL_ACCURACY, pos1, pos2))) n++;
  if(!is_ok("gcrs_to_tod:tod_to_gcrs:check", check_equal_pos(pos2, pos0, 1e-9 * d))) n++;

  return n;
}

static int test_gcrs_to_mod() {
  double pos1[3] = {}, pos2[3] = {}, d;
  int n = 0;

  d = novas_vlen(pos0);

  if(!is_ok("gcrs_to_mod", gcrs_to_mod(tdb, pos0, pos1))) n++;

  gcrs_to_j2000(pos0, pos2);
  precession(NOVAS_JD_J2000, pos2, tdb, pos2);

  if(!is_ok("gcrs_to_mod:check", check_equal_pos(pos1, pos2, 1e-9 * d))) n++;

  if(!is_ok("gcrs_to_mod:mod_to_gcrs", mod_to_gcrs(tdb, pos1, pos2))) n++;
  if(!is_ok("gcrs_to_mod:mod_to_gcrs:check", check_equal_pos(pos2, pos0, 1e-9 * d))) n++;

  return n;
}

static int test_source() {
  int k, n = 0;

  starvectors(&source.star, pos0, NULL);

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
  if(test_place_mod()) n++;
  if(test_place_j2000()) n++;

  if(test_radec_star()) n++;

  if(test_equ2hor()) n++;
  if(test_aberration()) n++;
  if(test_starvectors()) n++;

  if(test_geo_posvel()) n++;

  if(test_transform_icrs_cirs()) n++;
  if(test_transform_icrs_j2000()) n++;
  if(test_transform_j2000_mod()) n++;
  if(test_transform_mod_tod()) n++;
  if(test_transform_inv()) n++;

  if(test_gcrs_to_tod()) n++;
  if(test_gcrs_to_mod()) n++;

  for(k = 0; k < NOVAS_REFERENCE_SYSTEMS; k++)  if(test_app_hor(k)) n++;
  for(k = 0; k < NOVAS_REFERENCE_SYSTEMS; k++)  if(test_app_geom(k)) n++;

  return n;
}


static int test_make_planet() {
  object mars;

  if(!is_ok("make_panet", make_planet(NOVAS_MARS, &mars))) return 1;

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


static int test_cirs_tod() {
  double pos1[3] = {}, pos2[3] = {};
  double ra0, dec0, ra1, dec1;

  if(vector2radec(pos0, &ra0, &dec0) != 0) return 0;

  if(!is_ok("cirs_tod:cirs_to_tod", cirs_to_tod(tdb, NOVAS_FULL_ACCURACY, pos0, pos1))) return 1;

  vector2radec(pos1, &ra1, &dec1);
  if(!is_equal("cirs_tod:cirs_to_tod:check", cirs_to_app_ra(tdb, NOVAS_FULL_ACCURACY, ra0), ra1, 1e-10)) return 1;

  if(!is_ok("cirs_tod:tod_to_cirs", tod_to_cirs(tdb, NOVAS_FULL_ACCURACY, pos1, pos2))) return 1;
  if(!is_ok("cirs_tod:tod_to_cirs:check", check_equal_pos(pos2, pos0, 1e-13 * vlen(pos0)))) return 1;

  return 0;
}

static int test_observers() {
  double ps[3] = { 100.0, 30.0, 10.0 }, vs[3] = { 10.0 };
  int n = 0;

  if(test_precession()) n++;
  if(test_radec_planet()) n++;

  if(test_equ_ecl()) n++;
  if(test_equ_gal()) n++;

  if(test_cirs_tod()) n++;

  make_observer_at_geocenter(&obs);
  n += test_source();

  make_observer_on_surface(20.0, -15.0, 0.0, 0.0, 1000.0, &obs);
  n += test_source();
  if(test_itrs_hor_itrs()) n++;
  if(test_terra()) n++;
  if(test_bary2obs()) n++;
  if(test_cel2ter2cel()) n++;
  if(test_app_to_hor_compat()) n++;

  make_observer_in_space(ps, vs, &obs);
  n += test_source();

  return n;
}



static int test_sources() {
  cat_entry star;
  int n = 0;

  make_cat_entry("22+20", "TST", 1001, 22.0, 20.0, 3.0, -2.0, 5.0, 10.0, &star);
  if(make_cat_object(&star, &source) != 0) return -1;
  n += test_observers();

  make_cat_entry("22-40", "TST", 1001, 22.0, -40.0, 3.0, -2.0, 5.0, 10.0, &star);
  if(make_cat_object(&star, &source) != 0) return -1;
  n += test_observers();

  make_cat_entry("16-20", "TST", 1001, 16.0, -20.0, 3.0, -2.0, 5.0, 10.0, &star);
  if(make_cat_object(&star, &source) != 0) return -1;
  n += test_observers();

  make_cat_entry("16+77", "TST", 1001, 16.0, 77.0, -3.0, 2.0, -5.0, -10.0, &star);
  if(make_cat_object(&star, &source) != 0) return -1;
  n += test_observers();

  make_cat_entry("08+03", "TST", 1001, 8.0, 3.0, -3.0, 2.0, -5.0, -10.0, &star);
  if(make_cat_object(&star, &source) != 0) return -1;
  n += test_observers();

  make_cat_entry("08-66", "TST", 1001, 8.0, -66.0, -3.0, 2.0, -5.0, -10.0, &star);
  if(make_cat_object(&star, &source) != 0) return -1;
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

static int test_cirs_app_ra() {
  double cio_dra, ra0;

  if(!is_ok("cirs_app_ra:cio_ra", cio_ra(tdb, NOVAS_FULL_ACCURACY, &cio_dra))) return 1;

  for (ra0 = -12.0; ra0 < 36.0; ra0 += 2.0) {
    double ra1 = cirs_to_app_ra(tdb, NOVAS_FULL_ACCURACY, ra0);
    if (!is_ok("cirs_app_ra:cirs_to_app", fabs(remainder(ra1 - ra0 - cio_dra, 24.0)) > 1e-12)) return 1;
    ra1 = app_to_cirs_ra(tdb, NOVAS_FULL_ACCURACY, ra1);
    if (!is_ok("cirs_app_ra:app_to_cirs", fabs(remainder(ra1 - ra0, 24.0)) > 1e-12)) return 1;
  }
  return 0;
}



static int test_set_time() {
  novas_timespec tt, tt1, tai, gps, TDB, tcb, tcg, utc, ut1;
  int leap = 32;
  double dut1 = 0.1;
  long ijd = (long) tdb;
  double fjd = 0.25;
  double dt;

  const double CT0 = 2443144.5003725;
  const double LB = 1.550519768e-8;
  const double TDB0 = 6.55e-5;
  const double LG = 6.969291e-10;

  tdb2tt(ijd, NULL, &dt);

  if(!is_ok("set_time:set:tt", novas_set_split_time(NOVAS_TT, ijd, fjd, leap, dut1, &tt))) return 1;
  if(!is_ok("set_time:check:tt:int", tt.ijd_tt != ijd)) return 1;
  if(!is_ok("set_time:check:tt:frac", fabs(tt.fjd_tt - fjd) > 1e-9)) return 1;

  if(!is_ok("set_time:set:tt1", novas_set_time(NOVAS_TT, ijd + fjd, leap, dut1, &tt1))) return 1;
  dt = novas_get_time(&tt, NOVAS_TT) - novas_get_time(&tt1, NOVAS_TT);
  if(!is_ok("set_time:check:nosplit", fabs(dt * DAY) > 1e-5)) {
    printf("!!! Delta split: %.9f\n", dt * DAY);
    return 1;
  }

  if(!is_ok("set_time:set:tdb", novas_set_split_time(NOVAS_TDB, ijd, fjd, leap, dut1, &TDB))) return 1;
  if(!is_ok("set_time:set:tcb", novas_set_split_time(NOVAS_TCB, ijd, fjd, leap, dut1, &tcb))) return 1;
  if(!is_ok("set_time:set:tcg", novas_set_split_time(NOVAS_TCG, ijd, fjd, leap, dut1, &tcg))) return 1;
  if(!is_ok("set_time:set:tai", novas_set_split_time(NOVAS_TAI, ijd, fjd, leap, dut1, &tai))) return 1;
  if(!is_ok("set_time:set:gps", novas_set_split_time(NOVAS_GPS, ijd, fjd, leap, dut1, &gps))) return 1;
  if(!is_ok("set_time:set:utc", novas_set_split_time(NOVAS_UTC, ijd, fjd, leap, dut1, &utc))) return 1;
  if(!is_ok("set_time:set:ut1", novas_set_split_time(NOVAS_UT1, ijd, fjd, leap, dut1, &ut1))) return 1;

  dt = remainder(novas_get_split_time(&TDB, NOVAS_TT, NULL) - novas_get_split_time(&tt, NOVAS_TT, NULL), 1.0);
  if(!is_equal("set_time:check:tdb-tt", dt * DAY, -tt2tdb(novas_get_time(&tt, NOVAS_TT)), 1e-9)) {
    printf("!!! TT-TDB: %.9f (expected %.9f)\n", dt * DAY, -tt2tdb(ijd + fjd));
    return 1;
  }

  dt = novas_get_split_time(&tcb, NOVAS_TT, NULL) - novas_get_split_time(&TDB, NOVAS_TT, NULL);
  dt += LB * (novas_get_time(&TDB, NOVAS_TDB) - CT0) - TDB0 / DAY;
  if(!is_equal("set_time:check:tcb-tdb", dt * DAY, 0.0, 1e-9)) return 1;

  dt = novas_get_split_time(&tcg, NOVAS_TT, NULL) - novas_get_split_time(&tt, NOVAS_TT, NULL);
  dt += LG * (novas_get_time(&tt, NOVAS_TT) - CT0);
  if(!is_equal("set_time:check:tcg-tt", dt * DAY, 0.0, 1e-9)) return 1;

  dt = novas_get_split_time(&tt, NOVAS_TT, NULL) - novas_get_split_time(&tai, NOVAS_TT, NULL);
  if(!is_equal("set_time:check:tt-tai", dt * DAY, -32.184, 1e-9)) return 1;

  dt = novas_get_split_time(&tai, NOVAS_TT, NULL) - novas_get_split_time(&gps, NOVAS_TT, NULL);
  if(!is_equal("set_time:check:gps-tai", dt * DAY, -19.0, 1e-9)) return 1;

  dt = novas_get_split_time(&tai, NOVAS_TT, NULL) - novas_get_split_time(&utc, NOVAS_TT, NULL);
  if(!is_equal("set_time:check:tai-utc", dt * DAY, -leap, 1e-9)) return 1;

  dt = novas_get_split_time(&ut1, NOVAS_TT, NULL) - novas_get_split_time(&utc, NOVAS_TT, NULL);
  if(!is_equal("set_time:check:ut1-utc", dt * DAY, -dut1, 1e-9)) return 1;

  return 0;
}

static int test_get_time() {
  novas_timespec tt;
  int leap = 32;
  double dut1 = 0.1;
  double dt;
  long ijd;

  const double CT0 = 2443144.5003725;
  const double LB = 1.550519768e-8;
  const double TDB0 = 6.55e-5;
  const double LG = 6.969291e-10;

  if(!is_ok("get_time:set:tt", novas_set_time(NOVAS_TT, tdb + 0.25, leap, dut1, &tt))) return 1;

  dt = novas_get_time(&tt, NOVAS_TT) - (tt.ijd_tt + tt.fjd_tt);
  if(!is_equal("get_time:check:nosplit", dt * DAY, 0.0, 1e-5)) return 1;

  dt = remainder(novas_get_split_time(&tt, NOVAS_TDB, NULL) - novas_get_split_time(&tt, NOVAS_TT, NULL), 1.0);
  if(!is_equal("get_time:check:tdb-tt", dt * DAY, tt2tdb(novas_get_time(&tt, NOVAS_TT)), 1e-9)) return 1;

  dt = novas_get_split_time(&tt, NOVAS_TCB, NULL) - novas_get_split_time(&tt, NOVAS_TDB, NULL);
  dt -= LB * (novas_get_time(&tt, NOVAS_TDB) - CT0) - TDB0 / DAY;
  if(!is_equal("get_time:check:tcb-tdb", dt * DAY, 0.0, 1e-9)) return 1;

  dt = novas_get_split_time(&tt, NOVAS_TT, NULL) - novas_get_split_time(&tt, NOVAS_TAI, NULL);
  if(!is_equal("get_time:check:tt-tai", dt * DAY, 32.184, 1e-9)) return 1;

  dt = novas_get_split_time(&tt, NOVAS_TCG, NULL) - novas_get_split_time(&tt, NOVAS_TT, NULL);
  dt -= LG * (novas_get_time(&tt, NOVAS_TT) - CT0);
  if(!is_equal("get_time:check:tcg-tt", dt * DAY, 0.0, 1e-9)) return 1;

  dt = novas_get_split_time(&tt, NOVAS_TAI, NULL) - novas_get_split_time(&tt, NOVAS_GPS, NULL);
  if(!is_equal("get_time:check:gps-tai", dt * DAY, 19.0, 1e-9)) return 1;

  dt = novas_get_split_time(&tt, NOVAS_TAI, NULL) - novas_get_split_time(&tt, NOVAS_UTC, NULL);
  if(!is_equal("get_time:check:tai-utc", dt * DAY, leap, 1e-9)) return 1;

  dt = novas_get_split_time(&tt, NOVAS_UT1, NULL) - novas_get_split_time(&tt, NOVAS_UTC, NULL);
  if(!is_equal("get_time:check:ut1-utc", dt * DAY, dut1, 1e-9)) return 1;

  tt.fjd_tt = 0.0;
  dt = novas_get_split_time(&tt, NOVAS_TAI, &ijd) - (1.0 - 32.184 / DAY);
  if(!is_equal("get_time:wrap:lo:check:fjd", dt * DAY, 0.0, 1e-9)) return 1;

  if(!is_ok("get_time:wrap:lo:check:ijd", (ijd + 1) != tt.ijd_tt)) {
    printf("!!! ijd: %ld (expected %ld)\n", ijd, tt.ijd_tt - 1);
    return 1;
  }

  // Same with NULL ijd...
  dt = novas_get_split_time(&tt, NOVAS_TAI, NULL) - (1.0 - 32.184 / DAY);
  if(!is_equal("get_time:wrap:lo:check:fjd", dt * DAY, 0.0, 1e-9)) return 1;


  tt.fjd_tt = 1.0 - 1e-9 / DAY;
  tt.tt2tdb = 1e-3;

  dt = novas_get_split_time(&tt, NOVAS_TDB, &ijd);
  if(!is_ok("get_time:wrap:hi:check:fjd", dt * DAY >= 1e-3)) {
    printf("!!! delta: %.9f\n", dt * DAY);
    return 1;
  }
  if(!is_ok("get_time:wrap:hi:check:ijd", (ijd - 1) != tt.ijd_tt)) {
    printf("!!! ijd: %ld (expected %ld)\n", ijd, tt.ijd_tt + 1);
    return 1;
  }

  // Same with NULL ijd
  dt = novas_get_split_time(&tt, NOVAS_TDB, NULL);
  if(!is_ok("get_time:wrap:hi:check:fjd", dt * DAY >= 1e-3)) {
    printf("!!! delta: %.9f\n", dt * DAY);
    return 1;
  }

  return 0;
}

static int test_sky_pos(enum novas_reference_system sys) {
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {};
  cat_entry c = {};
  object source[2] = {{}};
  int i;

  if(!is_ok("sky_pos:set_time", novas_set_time(NOVAS_TT, tdb, 32, 0.0, &ts))) return 1;
  if(!is_ok("sky_pos:make_observer", make_observer_at_geocenter(&obs))) return 1;
  if(!is_ok("sky_pos:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame))) return 1;

  make_cat_entry("test", "TST", 1, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, &c);

  make_cat_object(&c, &source[0]);
  make_planet(NOVAS_SUN, &source[1]);

  cel_pole(tdb, POLE_OFFSETS_X_Y, 0.0, 0.0);


  for(i = 0; i < 2; i++) {
    char label[50];
    sky_pos p = {}, pc = {};

    // place does not apply deflection / aberration for ICRS
    place(tdb, &source[i], &obs, ts.ut1_to_tt, (sys == NOVAS_ICRS ? NOVAS_GCRS : sys), NOVAS_REDUCED_ACCURACY, &pc);

    sprintf(label, "sky_pos:sys=%d:source=%d", sys, i);
    if(!is_ok(label, novas_sky_pos(&source[i], &frame, sys, &p))) return 1;

    sprintf(label, "sky_pos:sys=%d:source=%d:check:ra", sys, i);
    if(!is_equal(label, p.ra, pc.ra, 1e-10)) return 1;

    sprintf(label, "sky_pos:sys=%d:source=%d:check:dec", sys, i);
    if(!is_equal(label, p.dec, pc.dec, 1e-9)) return 1;

    sprintf(label, "sky_pos:sys=%d:source=%d:check:rv", sys, i);
    if(!is_equal(label, p.rv, pc.rv, 1e-9)) return 1;

    sprintf(label, "sky_pos:sys=%d:source=%d:check:r_hat", sys, i);
    if(!is_ok(label, check_equal_pos(p.r_hat, pc.r_hat, 1e-12))) return 1;
  }

  return 0;
}

static int test_geom_posvel() {
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {}, acc = {};
  object source = {};
  double pos0[3] = {}, vel0[3] = {}, pos[3] = {1.0}, vel[3] = {1.0};

  enable_earth_sun_hp(1);

  if(!is_ok("sky_pos:set_time", novas_set_time(NOVAS_TDB, tdb, 32, 0.0, &ts))) return 1;
  if(!is_ok("sky_pos:make_observer", make_observer_at_geocenter(&obs))) return 1;
  if(!is_ok("sky_pos:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame))) return 1;
  if(!is_ok("sky_pos:make_frame:acc", novas_make_frame(NOVAS_FULL_ACCURACY, &obs, &ts, 0.0, 0.0, &acc))) return 1;

  make_planet(NOVAS_SUN, &source);

  if(!is_ok("geom_posvel", novas_geom_posvel(&source, &frame, NOVAS_ICRS, pos0, vel0))) return 1;
  if(!is_ok("geom_posvel", novas_geom_posvel(&source, &acc, NOVAS_ICRS, pos, vel))) return 1;
  if(!is_ok("geom_posvel:pos:acc", check_equal_pos(pos, pos0, 1e-9 * vlen(pos0)))) return 1;
  if(!is_ok("geom_posvel:vel:acc", check_equal_pos(vel, vel0, 1e-8))) return 1;

  if(!is_ok("geom_posvel:pos:null", novas_geom_posvel(&source, &frame, NOVAS_ICRS, NULL, vel))) return 1;
  if(!is_ok("geom_posvel:pos:null:check", check_equal_pos(vel, vel0, 1e-8))) return 1;

  if(!is_ok("geom_posvel:vel:null", novas_geom_posvel(&source, &frame, NOVAS_ICRS, pos, NULL))) return 1;
  if(!is_ok("geom_posvel:vel:null:check", check_equal_pos(pos, pos0, 1e-9 * vlen(pos0)))) return 1;

  return 0;
}


static int test_dates() {
  double offsets[] = {-10000.0, 0.0, 10000.0, 10000.0, 10000.01 };
  int i, n = 0;

  if(test_get_ut1_to_tt()) n++;
  if(test_get_utc_to_tt()) n++;
  if(test_nutation_lp_provider()) n++;
  if(test_cal_date()) n++;
  if(test_cirs_app_ra()) n++;

  for(i = 0; i < 5; i++) {
    int k;

    printf(" Testing date %.3f\n", offsets[i]);

    tdb = J2000 + offsets[i];

    if(test_set_time()) n++;
    if(test_get_time()) n++;
    if(test_geom_posvel()) n++;

    for(k =0; k < NOVAS_REFERENCE_SYSTEMS; k++) if(test_sky_pos(k)) n++;

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

static int test_v2z() {
  int v;

  for(v = 0.0; v < NOVAS_C; v += 10000000) {
    char label[40];
    double zexp = sqrt((1.0 + v / NOVAS_C) / (1.0 - v / NOVAS_C)) - 1.0;

    sprintf(label, "v2z:v:%d", v);
    if(!is_equal(label, novas_v2z(v / 1000.0), zexp, 1e-6)) return 1;

    sprintf(label, "v2z:z2v:v:%d", v);
    if(!is_equal(label, novas_z2v(zexp), v / 1000.0, 1e-6)) return 1;
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
  cat_entry c = {};

  if(!is_ok("make_object:name:null", make_object(NOVAS_CATALOG_OBJECT, 1, NULL, &c, &o))) return 1;

  return 0;
}

static int test_make_redshifted_object() {
  object gal;

  if(!is_ok("make_redshifted_object", make_redshifted_object("test", 1.0, 2.0, 3.0, &gal))) return 1;

  if(!is_ok("make_redshifted_object:type", gal.type != NOVAS_CATALOG_OBJECT)) return 1;
  if(!is_equal("make_redshifted_object:ra", gal.star.ra, 1.0, 1e-12)) return 1;
  if(!is_equal("make_redshifted_object:dec", gal.star.dec, 2.0, 1e-12)) return 1;
  if(!is_equal("make_redshifted_object:rv", novas_v2z(gal.star.radialvelocity), 3.0, 1e-12)) return 1;
  if(!is_ok("make_redshifted_object:ra", gal.star.promora != 0.0)) return 1;
  if(!is_ok("make_redshifted_object:ra", gal.star.promodec != 0.0)) return 1;
  if(!is_ok("make_redshifted_object:ra", gal.star.parallax != 0.0)) return 1;

  return 0;
}

static int test_transform_cat() {
  cat_entry in = {}, out;

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
  if(!is_ok("planet_provider:get_planet_provider", get_planet_provider() != dummy_planet)) goto cleanup; // @suppress("Goto statement used")

  if(!is_ok("planet_provider:set_planet_provider_hp", set_planet_provider_hp(dummy_planet_hp))) goto cleanup; // @suppress("Goto statement used")
  if(!is_ok("planet_provider:get_planet_provider_hp", get_planet_provider_hp() != dummy_planet_hp)) goto cleanup; // @suppress("Goto statement used")

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
      fprintf(stderr, ">>> Expecting diffent A/B, twice:\n");
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

static int test_tt2tdb() {
  double d;

  if(!is_ok("tdb2tt:tt:null", tdb2tt(tdb, NULL, &d))) return 1;
  if(!is_ok("tt2tdb:check", fabs(tt2tdb(tdb) - d) >= 1e-9)) return 1;

  return 0;
}

static int test_grav_vec() {
  double pz[3] = {}, p1[] = {1.0, 0.0, 0.0}, pm[] = {0.5, 0.0, 0.0}, pn[] = {0.0, 1.0, 0.0}, out[3];

  // Observing null vector pos...

  // 1. gravitating body is observed object (observer not aligned)
  if(!is_ok("grav_vec:pos:obj", grav_vec(pz, pn, pz, 1000.0, out))) return 1;
  if(!is_ok("grav_vec:check_obj", check_equal_pos(pz, out, 1e-9))) return 1;

  // 2. gravitating body is observer (target not aligned)
  if(!is_ok("grav_vec:pos:obs", grav_vec(pz, pn, pn, 1000.0, out))) return 1;
  if(!is_ok("grav_vec:check_obs", check_equal_pos(pz, out, 1e-9))) return 1;

  // 3. gravitating body is aligned
  if(!is_ok("grav_vec:pos:align", grav_vec(pz, p1, pm, 1000.0, out))) return 1;
  if(!is_ok("grav_vec:check_align", check_equal_pos(pz, out, 1e-9))) return 1;

  return 0;
}

static int test_grav_undef() {
  double pos_src[3], pos_obs[3], pos_app[3] = {}, pos0[3] = {}, v[3];
  double tdb2[2] = { tdb };
  object earth = {};
  int i;

  if(!is_ok("grav_invdef:make_planet", make_planet(NOVAS_EARTH, &earth))) return 1;
  if(!is_ok("grav_invdef:ephemeris", ephemeris(tdb2, &earth, NOVAS_HELIOCENTER, NOVAS_REDUCED_ACCURACY, pos_obs, v))) return 1;

  for(i = 0; i < 3; i++) pos_src[i] = -(2.001 * pos_obs[i]);

  if(!is_ok("grav_invdef:def", grav_def(tdb, NOVAS_OBSERVER_AT_GEOCENTER, NOVAS_REDUCED_ACCURACY, pos_src, pos_obs, pos_app))) return 1;
  if(!is_ok("grav_invdef:undef", grav_undef(tdb, NOVAS_REDUCED_ACCURACY, pos_app, pos_obs, pos0))) return 1;

  if(!is_ok("grav_invdef:check", check_equal_pos(pos_src, pos0, 1e-9))) return 1;

  memset(pos_app, 0, sizeof(pos_app));
  if(!is_ok("grav_invdef:undef:zero", grav_undef(tdb, NOVAS_REDUCED_ACCURACY, pos_app, pos_obs, pos0))) return 1;
  if(!is_ok("grav_invdef:check:zero", check_equal_pos(pos0, pos_app, 1e-9))) return 1;

  memset(pos_app, 0, sizeof(pos_app));
  if(!is_ok("grav_invdef:undef:zero", grav_undef(tdb, NOVAS_REDUCED_ACCURACY, pos_app, pos_obs, pos_app))) return 1;
  if(!is_ok("grav_invdef:check:zero", check_equal_pos(pos0, pos_app, 1e-9))) return 1;

  return 0;
}

static int test_vector2radec() {
  double pos[3] = {1.0};
  double x;

  if(!is_ok("vector2radec:ra:null", vector2radec(pos, NULL, &x))) return 1;
  if(!is_ok("vector2radec:dec:null", vector2radec(pos, &x, NULL))) return 1;

  return 0;
}

static int test_make_cat_object() {
  cat_entry star = {};
  object source = {};

  make_cat_entry("test", "FK4", 123, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, &star);

  if(!is_ok("make_cat_object", make_cat_object(&star, &source))) return 1;
  if(!is_ok("make_cat_object:check", memcmp(&source.star, &star, sizeof(star)))) return 1;
  return 0;
}

static int test_airborne_observer() {
  on_surface loc = {};
  observer obs = {}, gc = {};
  double vel[3] = { 10.0, 11.0, 12.0 };
  double epos[3], evel[3], gpos[3], gvel[3], opos[3], ovel[3];
  int i;

  if(!is_ok("airborne_observer:make_on_surface", make_on_surface(1.0, 2.0, 3.0, 4.0, 5.0, &loc))) return 1;

  if(!is_ok("airborne_observer:make", make_airborne_observer(&loc, vel, &obs))) return 1;
  if(!is_ok("airborne_observer:check:on_surf", memcmp(&obs.on_surf, &loc, sizeof(loc)))) return 1;
  if(!is_ok("airborne_observer:check:vel", memcmp(&obs.near_earth.sc_vel, &vel, sizeof(vel)))) return 1;

  if(!is_ok("airborne_observer:make_observer_at_geocenter", make_observer_at_geocenter(&gc))) return 1;
  if(!is_ok("airborne_observer:geo_posvel:gc", geo_posvel(tdb, ut12tt, NOVAS_REDUCED_ACCURACY, &gc, epos, evel))) return 1;
  if(!is_ok("airborne_observer:geo_posvel:obs", geo_posvel(tdb, ut12tt, NOVAS_REDUCED_ACCURACY, &obs, gpos, gvel))) return 1;
  if(!is_ok("airborne_observer:obs_posvel", obs_posvel(tdb, ut12tt, NOVAS_REDUCED_ACCURACY, &obs, epos, evel, opos, ovel))) return 1;

  for(i = 0; i < 3; i++) {
    gpos[i] += epos[i];
    gvel[i] += evel[i];
  }

  if(!is_ok("airborne_observer:check:result:pos", check_equal_pos(gpos, opos, 1e-9))) return 1;
  if(!is_ok("airborne_observer:check:result:vel", check_equal_pos(gvel, ovel, 1e-9))) return 1;

  return 0;
}

static int test_solar_system_observer() {
  observer obs = {}, gc = {};
  object earth = NOVAS_EARTH_INIT;
  double pos[3] = {1.0, 2.0, 3.0}, vel[3] = { 10.0, 11.0, 12.0 };
  double epos[3], evel[3], gpos[3], gvel[3], opos[3], ovel[3];
  double tdb2[2] = { tdb, 0.0 };
  int i;

  if(!is_ok("solar_system_observer:make", make_solar_system_observer(pos, vel, &obs))) return 1;
  if(!is_ok("solar_system_observer:check:pos", memcmp(&obs.near_earth.sc_pos, &pos, sizeof(pos)))) return 1;
  if(!is_ok("solar_system_observer:check:vel", memcmp(&obs.near_earth.sc_vel, &vel, sizeof(vel)))) return 1;

  if(!is_ok("solar_system_observer:make_observer_at_geocenter", make_observer_at_geocenter(&gc))) return 1;
  if(!is_ok("solar_system_observer:obs_posvel", obs_posvel(tdb, ut12tt, NOVAS_REDUCED_ACCURACY, &obs, NULL, NULL, opos, ovel))) return 1;
  if(!is_ok("solar_system_observer:geo_posvel:obs", geo_posvel(tdb - tt2tdb(tdb) / 86400.0, ut12tt, NOVAS_REDUCED_ACCURACY, &obs, gpos, gvel))) return 1;
  if(!is_ok("solar_system_observer:ephemeris:earth", ephemeris(tdb2, &earth, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, epos, evel))) return 1;

  for(i = 0; i < 3; i++) {
    gpos[i] += epos[i];
    // Relativistic addition of velocities.
    gvel[i] = (gvel[i] + evel[i]) / (1.0 + gvel[i] * evel[i] / (NOVAS_C_AU_PER_DAY * NOVAS_C_AU_PER_DAY));
  }

  if(!is_ok("solar_system_observer:check:result:pos:1", check_equal_pos(opos, pos, 1e-9))) return 1;
  if(!is_ok("solar_system_observer:check:result:vel:1", check_equal_pos(ovel, vel, 1e-9))) return 1;
  if(!is_ok("solar_system_observer:check:result:pos:2", check_equal_pos(gpos, pos, 1e-9))) return 1;
  if(!is_ok("solar_system_observer:check:result:vel:2", check_equal_pos(gvel, vel, 1e-9))) return 1; // TODO check rel.

  if(!is_ok("solar_system_observer:obs_posvel:pos:null", obs_posvel(tdb, ut12tt, NOVAS_REDUCED_ACCURACY, &obs, NULL, NULL, NULL, ovel))) return 1;
  if(!is_ok("solar_system_observer:obs_posvel:vel:null", obs_posvel(tdb, ut12tt, NOVAS_REDUCED_ACCURACY, &obs, NULL, NULL, opos, NULL))) return 1;

  if(!is_ok("solar_system_observer:geo_posvel:pos:null", geo_posvel(tdb, ut12tt, NOVAS_REDUCED_ACCURACY, &obs, NULL, ovel))) return 1;
  if(!is_ok("solar_system_observer:geo_posvel:vel:null", geo_posvel(tdb, ut12tt, NOVAS_REDUCED_ACCURACY, &obs, opos, NULL))) return 1;


  return 0;
}

static int test_obs_posvel() {
  double epos[3] = {}, evel[3] = {}, x[3];
  observer obs;
  object earth = NOVAS_EARTH_INIT;
  double tdb2[2] = { tdb, 0.0 };
  double sc_pos[3] = {1.0, 2.0, 3.0}, sc_vel[3] = {4.0, 5.0, 6.0};
  double gpos[3], gvel[3];

  if(!is_ok("obs_posvel:ephemeris:earth", ephemeris(tdb2, &earth, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, epos, evel))) return 1;

  make_observer_at_geocenter(&obs);

  if(!is_ok("obs_posvel:pos:null", obs_posvel(tdb, ut12tt, NOVAS_REDUCED_ACCURACY, &obs, epos, evel, NULL, x))) return 1;
  if(!is_ok("obs_posvel:check:vel:1", check_equal_pos(evel, x, 1e-9))) return 1;

  if(!is_ok("obs_posvel:vel:null", obs_posvel(tdb, ut12tt, NOVAS_REDUCED_ACCURACY, &obs, epos, evel, x, NULL))) return 1;
  if(!is_ok("obs_posvel:check:pos:1", check_equal_pos(epos, x, 1e-9))) return 1;

  if(!is_ok("obs_posvel:no_epos:pos:null", obs_posvel(tdb, ut12tt, NOVAS_REDUCED_ACCURACY, &obs, epos, NULL, NULL, x))) return 1;
  if(!is_ok("obs_posvel:check:vel:2", check_equal_pos(evel, x, 1e-9))) return 1;

  if(!is_ok("obs_posvel:no_evel:vel:null", obs_posvel(tdb, ut12tt, NOVAS_REDUCED_ACCURACY, &obs, NULL, evel, x, NULL))) return 1;
  if(!is_ok("obs_posvel:check:pos:2", check_equal_pos(epos, x, 1e-9))) return 1;

  if(!is_ok("obs_posvel:no_earth:pos:null", obs_posvel(tdb, ut12tt, NOVAS_REDUCED_ACCURACY, &obs, NULL, NULL, NULL, x))) return 1;
  if(!is_ok("obs_posvel:check:vel:3", check_equal_pos(evel, x, 1e-9))) return 1;

  if(!is_ok("obs_posvel:no_earth:vel:null", obs_posvel(tdb, ut12tt, NOVAS_REDUCED_ACCURACY, &obs, NULL, NULL, x, NULL))) return 1;
  if(!is_ok("obs_posvel:check:pos:3", check_equal_pos(epos, x, 1e-9))) return 1;

  // Observer in orbit...
  make_observer_in_space(sc_pos, sc_vel, &obs);

  geo_posvel(tdb, ut12tt, NOVAS_REDUCED_ACCURACY, &obs, gpos, gvel);

  if(!is_ok("obs_posvel:eorb:pos:null", obs_posvel(tdb, ut12tt, NOVAS_REDUCED_ACCURACY, &obs, epos, evel, NULL, x))) return 1;
  if(!is_ok("obs_posvel:eorb:vel:null", obs_posvel(tdb, ut12tt, NOVAS_REDUCED_ACCURACY, &obs, epos, evel, x, NULL))) return 1;

  return 0;
}


static int test_dxdy_to_dpsideps() {
  double x;

  if(!is_ok("dxdy_to_dpsideps:dpsi:null", polar_dxdy_to_dpsideps(NOVAS_JD_J2000, 1.0, 2.0, NULL, &x))) return 1;
  if(!is_ok("dxdy_to_dpsideps:deps:null", polar_dxdy_to_dpsideps(NOVAS_JD_J2000, 1.0, 2.0, &x, NULL))) return 1;

  return 0;
}

static int test_cio_location() {
  double loc;
  short type;

  cio_location(NOVAS_JD_J2000, NOVAS_FULL_ACCURACY, &loc, &type);

  return 0;
}

static int test_cio_array() {
  char path[256];
  ra_of_cio data[10] = {};

  sprintf(path, "%s/../data/CIO_RA.TXT", workPath);

  if(!is_ok("cio_array:ascii:set_cio_locator_file", set_cio_locator_file(path))) return 1;
  if(!is_ok("cio_array:ascii", cio_array(NOVAS_JD_J2000, 10, data))) return 1;
  if(!is_ok("cio_array:ascii:check:date", fabs(data[0].jd_tdb - NOVAS_JD_J2000) > 6.01)) return 1;
  if(!is_ok("cio_array:ascii:check:first", data[0].ra_cio == 0.0)) return 1;
  if(!is_ok("cio_array:ascii:check:last", data[9].ra_cio == 0.0)) return 1;

  sprintf(path, "%s/../cio_ra.bin", workPath);

  if(!is_ok("cio_array:bin:set_cio_locator_file", set_cio_locator_file(path))) return 1;
  if(!is_ok("cio_array:bin", cio_array(NOVAS_JD_J2000, 10, data))) return 1;
  if(!is_ok("cio_array:bin:check:date", fabs(data[0].jd_tdb - NOVAS_JD_J2000) > 6.01)) return 1;
  if(!is_ok("cio_array:bin:check:first", data[0].ra_cio == 0.0)) return 1;
  if(!is_ok("cio_array:bin:check:last", data[9].ra_cio == 0.0)) return 1;

  return 0;
}

static int test_novas_debug() {
  int n = 0;

  novas_debug(NOVAS_DEBUG_OFF);
  if(!is_ok("novas_debug:off", novas_get_debug_mode() != NOVAS_DEBUG_OFF)) n++;

  novas_debug(NOVAS_DEBUG_ON);
  if(!is_ok("novas_debug:on", novas_get_debug_mode() != NOVAS_DEBUG_ON)) n++;

  novas_debug(NOVAS_DEBUG_EXTRA);
  if(!is_ok("novas_debug:full", novas_get_debug_mode() != NOVAS_DEBUG_EXTRA)) n++;

  novas_debug(3);
  if(!is_ok("novas_debug:3", novas_get_debug_mode() != NOVAS_DEBUG_EXTRA)) n++;

  novas_debug(NOVAS_DEBUG_ON);

  return n;
}

static int test_unix_time() {
  time_t sec = time(NULL);
  long nanos = 1;
  novas_timespec t;
  long nsec = -1;

  if(!is_ok("unix_time:set", novas_set_unix_time(sec, nanos, 37, 0.11, &t))) return 1;
  if(!is_ok("unix_time:check:sec", novas_get_unix_time(&t, &nsec) != sec)) {
    printf("!!! sec: %ld  %ld\n", (long) novas_get_unix_time(&t, &nsec), sec);
    return 1;
  }
  if(!is_ok("sunix_time:check:nsec", labs(nsec - nanos) > 0)) {
    printf("!!! nsec %ld  %ld\n", nsec, nanos);
    return 1;
  }

  if(!is_ok("unix_time:check2:sec", novas_get_unix_time(&t, NULL) != sec)) {
    printf("!!! sec: %ld  %ld\n", (long) novas_get_unix_time(&t, NULL), (long) sec);
    return 1;
  }

  // Offset by half a second (to test rounding other way)
  nanos += 500000000;
  if(!is_ok("unix_time:incr", novas_set_unix_time(sec, nanos, 37, 0.11, &t))) return 1;
  if(!is_ok("unix_time:offset:check:incr:sec", novas_get_unix_time(&t, &nsec) != sec)) {
    printf("!!! sec: %ld  %ld\n", (long) novas_get_unix_time(&t, &nsec), (long) sec);
    return 1;
  }
  if(!is_ok("unix_time:offset:check:incr:nsec", labs(nsec - nanos) > 0)) {
    printf("!!! nsec %ld  %ld\n", nsec, nanos);
    return 1;
  }

  sec = -86400;
  if(!is_ok("unix_time:neg", novas_set_unix_time(sec, nanos, 0, 0.11, &t))) return 1;
  if(!is_ok("unix_time:neg:check:sec", novas_get_unix_time(&t, &nsec) != sec)) {
    printf("!!! sec: %ld  %ld\n", (long) novas_get_unix_time(&t, &nsec), (long) sec);
    return 1;
  }
  if(!is_ok("unix_time:neg:check:nsec", labs(nsec - nanos) > 0)) {
    printf("!!! nsec %ld  %ld\n", nsec, nanos);
    return 1;
  }

  // Check rounding up to next second.
  if(!is_ok("unix_time:wrap", novas_set_unix_time(sec, 999999999L, 0, 0.11, &t))) return 1;
  t.fjd_tt += 6e-10 / DAY;
  novas_get_unix_time(&t, &nsec);
  if(!is_ok("unix_time:wrap:check:nsec", nsec > 0)) {
    printf("!!! nsec %ld\n", nsec);
    return 1;
  }


  return 0;
}

static int test_diff_time() {
  novas_timespec t, t1;
  time_t sec = time(NULL);
  double dt;

  const double LB = 1.550519768e-8;
  const double LG = 6.969291e-10;

  if(!is_ok("diff_time:set", novas_set_unix_time(sec, 1, 37, 0.11, &t))) return 1;
  if(!is_ok("diff_time:incr", novas_offset_time(&t, 0.5, &t1))) return 1;

  if(!is_equal("diff_time:check", novas_diff_time(&t1, &t), 0.5, 1e-9)) return 1;
  if(!is_equal("diff_time:check:rev", novas_diff_time(&t, &t1), -0.5, 1e-9)) return 1;

  dt = novas_diff_tcb(&t, &t1) - (1.0 + LB) * novas_diff_time(&t, &t1);
  if(!is_ok("diff_time:check:tcb", fabs(dt) >= 1e-9)) {
    printf("!!! missed TCB by %.9f\n", dt);
    return 1;
  }

  dt = novas_diff_tcg(&t, &t1) - (1.0 + LG) * novas_diff_time(&t, &t1);
  if(!is_ok("diff_time:check:tcg", fabs(dt) >= 1e-9)) {
    printf("!!! missed TCG by %.9f\n", dt);
    return 1;
  }

  if(!is_ok("diff_time:decr", novas_offset_time(&t, -0.5, &t1))) return 1;
  if(!is_equal("diff_time:check:decr", novas_diff_time(&t1, &t), -0.5, 1e-9)) return 1;

  if(!is_ok("diff_time:incr:same", novas_offset_time(&t, -0.5, &t))) return 1;
  if(!is_equal("diff_time:incr:check:same", novas_diff_time(&t1, &t), 0.0, 1e-9)) return 1;

  if(!is_ok("diff_time:incr:overflow", novas_offset_time(&t, 86400.0, &t))) return 1;
  if(!is_equal("diff_time:incr:check:overflow", novas_diff_time(&t, &t1), 86400.0, 1e-9)) return 1;

  return 0;
}

static int test_standard_refraction() {
  on_surface obs = {};
  int el;

  for(el = 1; el < 90.0; el += 5) {
    char label[50];

    sprintf(label, "standard_refraction:observed:%d", el);
    if(!is_equal(label, novas_standard_refraction(NOVAS_J2000, &obs, NOVAS_REFRACT_OBSERVED, el), refract(&obs, NOVAS_STANDARD_ATMOSPHERE, 90 - el), 1e-3)) return 1;

    sprintf(label, "standard_refraction:astro:%d", el);
    if(!is_equal(label, novas_standard_refraction(NOVAS_J2000, &obs, NOVAS_REFRACT_ASTROMETRIC, el), refract_astro(&obs, NOVAS_STANDARD_ATMOSPHERE, 90 - el), 1e-3)) return 1;
  }

  return 0;
}

static int test_optical_refraction() {
  on_surface obs = {};
  int el;

  obs.temperature = 10.0;
  obs.pressure = 1000.0;
  obs.humidity = 40.0;

  for(el = 1; el < 90.0; el += 5) {
    char label[50];

    sprintf(label, "optical_refraction:observed:%d", el);
    if(!is_equal(label, novas_optical_refraction(NOVAS_J2000, &obs, NOVAS_REFRACT_OBSERVED, el), refract(&obs, NOVAS_WEATHER_AT_LOCATION, 90 - el), 1e-3)) return 1;

    sprintf(label, "optical_refraction:observed:%d", el);
    if(!is_equal(label, novas_optical_refraction(NOVAS_J2000, &obs, NOVAS_REFRACT_ASTROMETRIC, el), refract_astro(&obs, NOVAS_WEATHER_AT_LOCATION, 90 - el), 1e-3)) return 1;
  }

  return 0;
}

static int test_radio_refraction() {
  const double exp[] = { 1365.48, 512.67, 294.20, 206.08, 156.43, 122.56, 98.08, 80.39, 67.44,
          57.34, 48.54, 40.21, 32.32, 25.33, 19.50, 14.42, 9.01, 3.11};
  on_surface obs = {};
  int i, el;

  obs.temperature = 10.0;
  obs.pressure = 1000.0;
  obs.humidity = 40.0;

  for(i = 0, el = 1; el < 90.0; i++, el += 5) {
    char label[50];
    double del, del1;

    sprintf(label, "radio_refraction:%d:astro", el);
    del = novas_radio_refraction(NOVAS_J2000, &obs, NOVAS_REFRACT_ASTROMETRIC, el);

    if(!is_equal(label, del, exp[i] / 3600.0, 1e-3)) return -1;
    del1 = novas_radio_refraction(NOVAS_J2000, &obs, NOVAS_REFRACT_OBSERVED, el + del);

    sprintf(label, "radio_refraction:%d:trip", el);
    if(!is_equal(label, del, del1, 1e-4)) return 1;
  }

  printf("\n");

  return 0;
}


static int test_inv_refract() {
  on_surface obs = {};
  int el;

  obs.temperature = 10.0;
  obs.pressure = 1000.0;
  obs.humidity = 40.0;

  for(el = 1; el < 90.0; el += 5) {
    char label[50];

    sprintf(label, "inv_refract:observed:%d", el);
    if(!is_equal(label,
            novas_inv_refract(novas_optical_refraction, NOVAS_J2000, &obs, NOVAS_REFRACT_OBSERVED, el),
            refract_astro(&obs, NOVAS_WEATHER_AT_LOCATION, 90 - el),
            1e-4))
      return 1;
  }

  return 0;
}

static int test_make_frame() {
  novas_timespec ts = {};
  novas_frame frame = {};
  observer obs = {};

  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);
  make_observer_at_geocenter(&obs);

  if(!is_ok("make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 1.0, 2.0, &frame))) return 1;

  if(!is_ok("make_frame:time", memcmp(&frame.time, &ts, sizeof(ts)))) return 1;
  if(!is_ok("make_frame:obs", memcmp(&frame.observer, &obs, sizeof(obs)))) return 1;
  if(!is_ok("make_frame:dx", frame.dx != 1.0)) return 1;
  if(!is_ok("make_frame:dy", frame.dy != 2.0)) return 1;

  return 0;
}

static int test_change_observer() {
  novas_timespec ts = {};
  novas_frame frame = {}, out = {};
  observer obs = {};

  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &ts);
  make_observer_at_geocenter(&obs);

  if(!is_ok("change_observer:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 1.0, 2.0, &frame))) return 1;

  make_observer_on_surface(1.0, 2.0, 3.0, 4.0, 1001.0, &obs);
  if(!is_ok("change_observer", novas_change_observer(&frame, &obs, &out))) return 1;
  if(!is_ok("change_observer:check", memcmp(&out.observer, &obs, sizeof(obs)))) return 1;

  if(!is_ok("change_observer:same", novas_change_observer(&frame, &obs, &frame))) return 1;
  if(!is_ok("change_observer:same:check", memcmp(&frame.observer, &obs, sizeof(obs)))) return 1;

  return 0;
}

static int test_transform() {
  novas_timespec ts = {};
  novas_frame frame = {};
  observer obs = {};
  novas_transform T = {}, I = {};

  double pos0[3] = {1.0, 2.0, 3.0}, pos1[3] = {1.0, 2.0, 3.0};
  sky_pos p0 = {}, p1 = {};

  p0.r_hat[1] = 1.0;
  p1.r_hat[1] = 1.0;
  vector2radec(p0.r_hat, &p0.ra, &p0.dec);

  novas_set_time(NOVAS_TT, NOVAS_JD_J2000 + 10000.0, 32, 0.0, &ts);
  make_observer_at_geocenter(&obs);

  if(!is_ok("transform:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 1.0, 2.0, &frame))) return 1;
  if(!is_ok("transform:make", novas_make_transform(&frame, NOVAS_ICRS, NOVAS_TOD, &T))) return 1;
  if(!is_ok("transform:invert", novas_invert_transform(&T, &I))) return 1;

  novas_transform_vector(pos0, &T, pos1);
  if(!is_ok("transform:vec", !check_equal_pos(pos0, pos1, 1e-9))) return 1;

  novas_transform_vector(pos1, &I, pos1);
  if(!is_ok("transform:inv:vec", check_equal_pos(pos0, pos1, 1e-9))) return 1;

  novas_transform_sky_pos(&p0, &T, &p1);
  if(!is_ok("transform:sky", !check_equal_pos(p0.r_hat, p1.r_hat, 1e-9))) return 1;

  novas_transform_sky_pos(&p1, &I, &p1);
  if(!is_ok("transform:inv:sky", check_equal_pos(p0.r_hat, p1.r_hat, 1e-9))) return 1;

  if(!is_equal("transform:inv:sky:ra", p0.ra, p1.ra, 1e-9)) return 1;
  if(!is_equal("transform:inv:sky:dec", p0.dec, p1.dec, 1e-9)) return 1;

  return 0;
}


static int test_app_hor2() {
  novas_timespec ts = {};
  observer obs = {};
  novas_frame frame = {};
  int i;

  if(!is_ok("app_hor2:sys=%d:set_time", novas_set_time(NOVAS_TT, tdb, 32, 0.0, &ts))) return 1;
  if(!is_ok("app_hor2:sys=%d:make_observer", make_observer_on_surface(1.0, 2.0, 3.0, 4.0, 1001.0, &obs))) return 1;
  if(!is_ok("app_hor2:sys=%d:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame))) return 1;

  for(i = -85; i <= 85; i += 10) {
    int j;

    for(j = 0; j <= 24.0; j++) {
      char label[50];
      double x, y;

      sprintf(label, "app_hor2:ra=%d:dec=%d", j, i);

      if(!is_ok(label, novas_app_to_hor(&frame, NOVAS_ICRS, j, i, NULL, &x, &y))) return 1;
      if(!is_ok(label, novas_hor_to_app(&frame, x, y, NULL, NOVAS_ICRS, &x, &y))) return 1;

      if(!is_equal(label, remainder(x - j, 24.0), 0.0, 1e-8)) return 1;
      if(!is_equal(label, y, i, 1e-9)) return 1;
    }
  }

  return 0;
}

static int test_rad_vel2() {
  object planet = {};
  double pos[3] = {1.0}, pos_obs[3] = {1.0}, v[3] = {};
  double rv0, rv1, rv2;
  int n = 0;

  make_planet(NOVAS_SUN, &planet);

  // d_src_sun 0 vs -1 -- different for surface gravity
  rv0 = rad_vel2(&planet, pos, v, pos_obs, v, 0.0, 0.0, 0.0);

  rv1 = rad_vel2(&planet, pos, v, pos_obs, v, 0.0, 0.0, -1.0);
  if(!is_ok("rad_vel:src_sun:-1", rv0 == rv1)) n++;

  // no surface gravity for SSB
  planet.number = 0;
  rv2 = rad_vel2(&planet, pos, v, pos_obs, v, 0.0, 0.0, 0.0);
  if(!is_equal("rad_vel:ssb", rv2, rv1, 1e-9)) n++;

  // no surface gravity for illegal planet number
  planet.number = NOVAS_PLANETS;
  rv2 = rad_vel2(&planet, pos, v, pos_obs, v, 0.0, 0.0, 0.0);
  if(!is_equal("rad_vel:hi", rv2, rv1, 1e-9)) n++;

  return n;
}


static int test_grav_redshift() {
  static const double G = 6.6743e-11; // G in SI units.
  static const double c2 = C * C;

  double M = 2e30;  // [kg]
  double r = NOVAS_SOLAR_RADIUS;

  double rs = 2 * G * M / c2;
  double zp1 = 1.0 / sqrt(1.0 - rs / r);

  int n = 0;

  if(!is_equal("grav_redshift", 1.0 + grav_redshift(M, r), zp1, 1e-12)) n++;

  return n;
}

static int test_redshift_vrad() {
  double v0 = 100.0;
  double z;

  int n = 0;

  for(z = -0.5; z < 3.0; z += 0.1) {
    char label[80];
    double v = redshift_vrad(v0, z);

    sprintf(label, "redshift_vrad:z=%.1f:inv", z);
    if(!is_equal(label, unredshift_vrad(v, z), v0, 1e-6)) n++;

    sprintf(label, "redshift_vrad:z=%.1f:check", z);
    if(!is_equal(label, 1.0 + novas_v2z(v), (1.0 + novas_v2z(v0)) * (1.0 + z), 1e-6)) n++;
  }

  return n;
}

static int test_z_add() {
  int n = 0;
  double z1;

  for(z1 = -0.5; z1 < 5.0; z1 += 0.5) {
    double z2;
    for(z2 = -0.1; z2 < 1.0; z2 += 0.1) {
      double zexp;

      zexp = (1.0 + z1) * (1.0 + z2) - 1.0;
      if(!is_equal("z_add", novas_z_add(z1, z2), zexp, 1e-12)) n++;
    }
  }
  return n;
}

static int test_z_inv() {
  int n = 0;
  double z;

  for(z = -0.5; z < 5.0; z += 0.5) {
    char label[80];
    double zi = novas_z_inv(z);

    sprintf(label, "z_inv:z=%.1f", z);
    if(!is_equal(label, 1.0, (1.0 + z) * (1.0 + zi), 1e-6)) n++;
  }

  return n;
}

static int test_novas_to_naif_planet() {
  int n = 0;

  if(!is_ok("novas_to_naif_planet:ssb", novas_to_naif_planet(NOVAS_SSB) != NAIF_SSB)) n++;
  if(!is_ok("novas_to_naif_planet:sun", novas_to_naif_planet(NOVAS_SUN) != NAIF_SUN)) n++;
  if(!is_ok("novas_to_naif_planet:moon", novas_to_naif_planet(NOVAS_MOON) != NAIF_MOON)) n++;
  if(!is_ok("novas_to_naif_planet:earth", novas_to_naif_planet(NOVAS_EARTH) != NAIF_EARTH)) n++;
  if(!is_ok("novas_to_naif_planet:earth", novas_to_naif_planet(NOVAS_EMB) != NAIF_EMB)) n++;
  if(!is_ok("novas_to_naif_planet:mercury", novas_to_naif_planet(NOVAS_MERCURY) != 199)) n++;
  if(!is_ok("novas_to_naif_planet:venus", novas_to_naif_planet(NOVAS_VENUS) != 299)) n++;
  if(!is_ok("novas_to_naif_planet:mars", novas_to_naif_planet(NOVAS_MARS) != 499)) n++;
  if(!is_ok("novas_to_naif_planet:jupiter", novas_to_naif_planet(NOVAS_JUPITER) != 599)) n++;
  if(!is_ok("novas_to_naif_planet:saturn", novas_to_naif_planet(NOVAS_SATURN) != 699)) n++;
  if(!is_ok("novas_to_naif_planet:uranus", novas_to_naif_planet(NOVAS_URANUS) != 799)) n++;
  if(!is_ok("novas_to_naif_planet:neptune", novas_to_naif_planet(NOVAS_NEPTUNE) != 899)) n++;
  if(!is_ok("novas_to_naif_planet:pluto", novas_to_naif_planet(NOVAS_PLUTO) != 999)) n++;
  if(!is_ok("novas_to_dexxx_planet:pluto", novas_to_naif_planet(NOVAS_PLUTO_BARYCENTER) != 9)) n++;

  return n;
}

static int test_novas_to_dexxx_planet() {
  int n = 0;

  if(!is_ok("novas_to_dexxx_planet:ssb", novas_to_dexxx_planet(NOVAS_SSB) != NAIF_SSB)) n++;
  if(!is_ok("novas_to_dexxx_planet:sun", novas_to_dexxx_planet(NOVAS_SUN) != NAIF_SUN)) n++;
  if(!is_ok("novas_to_dexxx_planet:moon", novas_to_dexxx_planet(NOVAS_MOON) != NAIF_MOON)) n++;
  if(!is_ok("novas_to_dexxx_planet:earth", novas_to_dexxx_planet(NOVAS_EARTH) != NAIF_EARTH)) n++;
  if(!is_ok("novas_to_dexxx_planet:earth", novas_to_dexxx_planet(NOVAS_EMB) != NAIF_EMB)) n++;
  if(!is_ok("novas_to_dexxx_planet:mercury", novas_to_dexxx_planet(NOVAS_MERCURY) != 1)) n++;
  if(!is_ok("novas_to_dexxx_planet:venus", novas_to_dexxx_planet(NOVAS_VENUS) != 2)) n++;
  if(!is_ok("novas_to_dexxx_planet:mars", novas_to_dexxx_planet(NOVAS_MARS) != 4)) n++;
  if(!is_ok("novas_to_dexxx_planet:jupiter", novas_to_dexxx_planet(NOVAS_JUPITER) != 5)) n++;
  if(!is_ok("novas_to_dexxx_planet:saturn", novas_to_dexxx_planet(NOVAS_SATURN) != 6)) n++;
  if(!is_ok("novas_to_dexxx_planet:uranus", novas_to_dexxx_planet(NOVAS_URANUS) != 7)) n++;
  if(!is_ok("novas_to_dexxx_planet:neptune", novas_to_dexxx_planet(NOVAS_NEPTUNE) != 8)) n++;
  if(!is_ok("novas_to_dexxx_planet:pluto", novas_to_dexxx_planet(NOVAS_PLUTO) != 9)) n++;
  if(!is_ok("novas_to_dexxx_planet:pluto", novas_to_dexxx_planet(NOVAS_PLUTO_BARYCENTER) != 9)) n++;

  return n;
}

static int test_naif_to_novas_planet() {
  int n = 0;

  if(!is_ok("naif_to_novas_planet:ssb", naif_to_novas_planet(NAIF_SSB) != NOVAS_SSB)) n++;
  if(!is_ok("naif_to_novas_planet:sun", naif_to_novas_planet(NAIF_SUN) != NOVAS_SUN)) n++;
  if(!is_ok("naif_to_novas_planet:moon", naif_to_novas_planet(NAIF_MOON) != NOVAS_MOON)) n++;
  if(!is_ok("naif_to_novas_planet:earth", naif_to_novas_planet(NAIF_EARTH) != NOVAS_EARTH)) n++;
  if(!is_ok("naif_to_novas_planet:earth", naif_to_novas_planet(NAIF_EMB) != NOVAS_EMB)) n++;
  if(!is_ok("naif_to_novas_planet:mercury", naif_to_novas_planet(199) != NOVAS_MERCURY)) n++;
  if(!is_ok("naif_to_novas_planet:venus", naif_to_novas_planet(299) != NOVAS_VENUS)) n++;
  if(!is_ok("naif_to_novas_planet:mars", naif_to_novas_planet(499) != NOVAS_MARS)) n++;
  if(!is_ok("naif_to_novas_planet:jupiter", naif_to_novas_planet(599) != NOVAS_JUPITER)) n++;
  if(!is_ok("naif_to_novas_planet:saturn", naif_to_novas_planet(699) != NOVAS_SATURN)) n++;
  if(!is_ok("naif_to_novas_planet:uranus", naif_to_novas_planet(799) != NOVAS_URANUS)) n++;
  if(!is_ok("naif_to_novas_planet:neptune", naif_to_novas_planet(899) != NOVAS_NEPTUNE)) n++;
  if(!is_ok("naif_to_novas_planet:pluto", naif_to_novas_planet(999) != NOVAS_PLUTO)) n++;
  if(!is_ok("naif_to_novas_planet:mercury", naif_to_novas_planet(1) != NOVAS_MERCURY)) n++;
  if(!is_ok("naif_to_novas_planet:venus", naif_to_novas_planet(2) != NOVAS_VENUS)) n++;
  if(!is_ok("naif_to_novas_planet:mars", naif_to_novas_planet(4) != NOVAS_MARS)) n++;
  if(!is_ok("naif_to_novas_planet:jupiter", naif_to_novas_planet(5) != NOVAS_JUPITER)) n++;
  if(!is_ok("naif_to_novas_planet:saturn", naif_to_novas_planet(6) != NOVAS_SATURN)) n++;
  if(!is_ok("naif_to_novas_planet:uranus", naif_to_novas_planet(7) != NOVAS_URANUS)) n++;
  if(!is_ok("naif_to_novas_planet:neptune", naif_to_novas_planet(8) != NOVAS_NEPTUNE)) n++;
  if(!is_ok("naif_to_novas_planet:pluto", naif_to_novas_planet(9) != NOVAS_PLUTO_BARYCENTER)) n++;
  if(!is_ok("naif_to_novas_planet:pluto", naif_to_novas_planet(999) != NOVAS_PLUTO)) n++;

  return n;
}

static int test_planet_for_name() {
  int n = 0;

  if(!is_ok("planet_for_name:mercury", novas_planet_for_name("mercury") != NOVAS_MERCURY)) n++;
  if(!is_ok("planet_for_name:pluto", novas_planet_for_name("PLUTO") != NOVAS_PLUTO)) n++;
  if(!is_ok("planet_for_name:sun", novas_planet_for_name("Sun") != NOVAS_SUN)) n++;
  if(!is_ok("planet_for_name:moon", novas_planet_for_name("MooN") != NOVAS_MOON)) n++;
  if(!is_ok("planet_for_name:ssb", novas_planet_for_name("SSB") != NOVAS_SSB)) n++;
  if(!is_ok("planet_for_name:ssb1", novas_planet_for_name("Solar-system barycenter") != NOVAS_SSB)) n++;

  return n;
}


static int test_orbit_place() {
  object ceres = {};
  novas_orbital orbit = NOVAS_ORBIT_INIT;
  observer obs = {};
  sky_pos pos = {};
  double p0[3] = {}, p1[3] = {};

  // Nov 14 0 UTC, geocentric from JPL Horizons.
  double tjd = 2460628.50079861;      // 0 UT as TT.
  double RA0 = 19.684415;
  double DEC0 = -28.62084;
  double rv0 = 21.4255198;            // km/s
  double r = 3.32557776285144;        // AU
  int n = 0;

  // Orbital Parameters for JD 2460600.5 from MPC
  orbit.jd_tdb = 2460600.5;
  orbit.a = 2.7666197;
  orbit.e = 0.079184;
  orbit.i = 10.5879;
  orbit.omega = 73.28579;
  orbit.Omega = 80.25414;
  orbit.M0 = 145.84905;
  orbit.n = 0.21418047;

  make_observer_at_geocenter(&obs);
  make_orbital_object("Ceres", -1, &orbit, &ceres);

  if(!is_ok("orbit_place", place(tjd, &ceres, &obs, ut12tt, NOVAS_TOD, NOVAS_REDUCED_ACCURACY, &pos))) return 1;

  if(!is_equal("orbit_place:ra", pos.ra, RA0, 1e-5 / cos(DEC0 * DEGREE))) n++;
  if(!is_equal("orbit_place:dec", pos.dec, DEC0, 1e-4)) n++;
  if(!is_equal("orbit_place:dist", pos.dis, r, 1e-4)) n++;
  if(!is_equal("orbit_place:vrad", pos.rv, rv0, 1e-2)) n++;

  if(!is_ok("orbit_place", novas_orbit_posvel(tjd, &orbit, NOVAS_FULL_ACCURACY, p0, NULL))) return 1;
  equ2ecl_vec(tjd, NOVAS_GCRS_EQUATOR, NOVAS_FULL_ACCURACY, p0, p0);

  orbit.system.type = NOVAS_ICRS;
  if(!is_ok("orbit_place:icrs", novas_orbit_posvel(tjd, &orbit, NOVAS_FULL_ACCURACY, p1, NULL))) n++;
  else {
    equ2ecl_vec(tjd, NOVAS_GCRS_EQUATOR, NOVAS_FULL_ACCURACY, p1, p1);
    if(!is_ok("orbit_place:icrs:check", check_equal_pos(p1, p0, 1e-9))) n++;
  }

  orbit.system.type = NOVAS_CIRS;
  if(!is_ok("orbit_place:cirs", novas_orbit_posvel(tjd, &orbit, NOVAS_FULL_ACCURACY, p1, NULL))) n++;
  else {
    gcrs_to_cirs(tjd, NOVAS_REDUCED_ACCURACY, p1, p1);
    equ2ecl_vec(tjd, NOVAS_TRUE_EQUATOR, NOVAS_FULL_ACCURACY, p1, p1);
    if(!is_ok("orbit_place:cirs:check", check_equal_pos(p1, p0, 1e-9))) n++;
  }

  orbit.system.type = NOVAS_J2000;
  if(!is_ok("orbit_place:j2000", novas_orbit_posvel(tjd, &orbit, NOVAS_FULL_ACCURACY, p1, NULL))) n++;
  else {
    gcrs_to_j2000(p1, p1);
    equ2ecl_vec(NOVAS_JD_J2000, NOVAS_TRUE_EQUATOR, NOVAS_FULL_ACCURACY, p1, p1);
    if(!is_ok("orbit_place:j2000:check", check_equal_pos(p1, p0, 1e-9))) n++;
  }

  orbit.system.type = NOVAS_MOD;
  if(!is_ok("orbit_place:mod", novas_orbit_posvel(tjd, &orbit, NOVAS_FULL_ACCURACY, p1, NULL))) n++;
  else {
    gcrs_to_mod(tjd, p1, p1);
    equ2ecl_vec(tjd, NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, p1, p1);
    if(!is_ok("orbit_place:mod:check", check_equal_pos(p1, p0, 1e-9))) n++;
  }

  orbit.system.type = NOVAS_TOD;
  if(!is_ok("orbit_place:tod", novas_orbit_posvel(tjd, &orbit, NOVAS_FULL_ACCURACY, p1, NULL))) n++;
  else {
    gcrs_to_tod(tjd, NOVAS_FULL_ACCURACY, p1, p1);
    equ2ecl_vec(tjd, NOVAS_TRUE_EQUATOR, NOVAS_FULL_ACCURACY, p1, p1);
    if(!is_ok("orbit_place:tod:check", check_equal_pos(p1, p0, 1e-9))) n++;
  }

  return n;
}


static int test_orbit_posvel_callisto() {
  novas_orbital orbit = NOVAS_ORBIT_INIT;
  novas_orbital_system *sys = &orbit.system;
  double pos0[3] = {}, pos[3] = {}, vel[3] = {}, pos1[3] = {}, vel1[3] = {}, ra, dec, dra, ddec;
  int i;

  // 2000-01-01 12 UT, geocentric from JPL Horizons.

  double dist = 4.62117513332102;
  double lt = 0.00577551831217194 * dist;                 // day
  double tjd = 2451545.00079861 - lt;   // 0 UT as TT, corrected or light time

  double RA0 = 23.86983 * DEGREE;
  double DEC0 = 8.59590 * DEGREE;

  double dRA = (23.98606 * DEGREE - RA0) / cos(DEC0);
  double dDEC = (8.64868 * DEGREE - DEC0);
  int n = 0;

  // Planet pos;
  radec2vector(RA0 / HOURANGLE, DEC0 / DEGREE, dist, pos1);

  // Callisto's parameters from JPL Horizons
  // https://ssd.jpl.nasa.gov/sats/elem/sep.html
  // 1882700. 0.007 43.8  87.4  0.3 309.1 16.690440 277.921 577.264 268.7 64.8
  sys->center = NOVAS_JUPITER;
  novas_set_orbsys_pole(NOVAS_GCRS, 268.7 / 15.0, 64.8, sys);

  orbit.jd_tdb = NOVAS_JD_J2000;
  orbit.a = 1882700.0 * 1e3 / AU;
  orbit.e = 0.007;
  orbit.omega = 43.8;
  orbit.M0 = 87.4;
  orbit.i = 0.3;
  orbit.Omega = 309.1;
  orbit.n = TWOPI / 16.690440;
  orbit.apsis_period = 277.921 * 365.25;
  orbit.node_period = 577.264 * 365.25;

  if(!is_ok("orbit_posvel_callisto", novas_orbit_posvel(tjd, &orbit, NOVAS_FULL_ACCURACY, pos, vel))) return 1;
  memcpy(pos0, pos, sizeof(pos));

  for(i = 3 ;--i >= 0; ) pos[i] += pos1[i];
  vector2radec(pos, &ra, &dec);

  ra *= HOURANGLE;
  dec *= DEGREE;

  dra = (ra - RA0) * cos(DEC0);
  ddec = (dec - DEC0);

  if(!is_equal("orbit_posvel_callisto:dist", hypot(dra, ddec) / ARCSEC, hypot(dRA, dDEC) / ARCSEC, 15.0)) n++;
  if(!is_equal("orbit_posvel_callisto:ra", dra / ARCSEC, dRA / ARCSEC, 15.0)) n++;
  if(!is_equal("orbit_posvel_callisto:dec", ddec / ARCSEC, dDEC / ARCSEC, 15.0)) n++;

  if(!is_ok("orbit_posvel_callisto:vel:null", novas_orbit_posvel(tjd, &orbit, NOVAS_FULL_ACCURACY, pos1, NULL))) n++;
  if(!is_ok("orbit_posvel_callisto:vel:null:check", check_equal_pos(pos1, pos0, 1e-8))) n++;

  if(!is_ok("orbit_posvel_callisto:pos:null", novas_orbit_posvel(tjd, &orbit, NOVAS_FULL_ACCURACY, NULL, vel1))) n++;
  if(!is_ok("orbit_posvel_callisto:pos:null:check", check_equal_pos(vel1, vel, 1e-8))) n++;

  sys->type = NOVAS_MOD;
  if(!is_ok("orbit_posvel_callisto:mod", novas_orbit_posvel(tjd, &orbit, NOVAS_FULL_ACCURACY, pos1, NULL))) n++;
  precession(tjd, pos0, NOVAS_JD_J2000, pos);
  j2000_to_gcrs(pos, pos);
  if(!is_ok("orbit_posvel_callisto:mod:check", check_equal_pos(pos1, pos, 1e-8))) n++;

  sys->type = NOVAS_TOD;
  if(!is_ok("orbit_posvel_callisto:mod", novas_orbit_posvel(tjd, &orbit, NOVAS_FULL_ACCURACY, pos1, NULL))) n++;
  tod_to_j2000(tjd, NOVAS_FULL_ACCURACY, pos0, pos);
  j2000_to_gcrs(pos, pos);
  if(!is_ok("orbit_posvel_callisto:mod:check", check_equal_pos(pos1, pos, 1e-8))) n++;

  sys->type = NOVAS_CIRS;
  if(!is_ok("orbit_posvel_callisto:cirs", novas_orbit_posvel(tjd, &orbit, NOVAS_FULL_ACCURACY, pos1, NULL))) n++;
  cirs_to_gcrs(tjd, NOVAS_FULL_ACCURACY, pos0, pos);
  if(!is_ok("orbit_posvel_callisto:cirs:check", check_equal_pos(pos1, pos, 1e-8))) n++;

  sys->type = NOVAS_J2000;
  if(!is_ok("orbit_posvel_callisto:j2000", novas_orbit_posvel(tjd, &orbit, NOVAS_FULL_ACCURACY, pos1, NULL))) n++;
  j2000_to_gcrs(pos0, pos);
  if(!is_ok("orbit_posvel_callisto:j2000:check", check_equal_pos(pos1, pos, 1e-8))) n++;

  return n;
}

int main(int argc, char *argv[]) {
  int n = 0;

  (void) argc;
  workPath = dirname(argv[0]);

  novas_debug(NOVAS_DEBUG_ON);
  enable_earth_sun_hp(1);

  make_object(NOVAS_CATALOG_OBJECT, 0, "None", NULL, &source);

  if(test_novas_debug()) n++;
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
  if(test_tt2tdb()) n++;
  if(test_grav_vec()) n++;
  if(test_grav_undef()) n++;
  if(test_vector2radec()) n++;
  if(test_make_cat_object()) n++;
  if(test_airborne_observer()) n++;
  if(test_solar_system_observer()) n++;
  if(test_obs_posvel()) n++;
  if(test_dxdy_to_dpsideps()) n++;
  if(test_cio_location()) n++;
  if(test_cio_array()) n++;

  // v1.1
  if(test_unix_time()) n++;
  if(test_diff_time()) n++;
  if(test_standard_refraction()) n++;
  if(test_optical_refraction()) n++;
  if(test_inv_refract()) n++;
  if(test_radio_refraction()) n++;
  if(test_make_frame()) n++;
  if(test_change_observer()) n++;
  if(test_transform()) n++;
  if(test_app_hor2()) n++;
  if(test_rad_vel2()) n++;

  // v1.2
  if(test_v2z()) n++;
  if(test_make_redshifted_object()) n++;
  if(test_z_add()) n++;
  if(test_z_inv()) n++;
  if(test_redshift_vrad()) n++;
  if(test_grav_redshift()) n++;

  if(test_novas_to_naif_planet()) n++;
  if(test_novas_to_dexxx_planet()) n++;
  if(test_naif_to_novas_planet()) n++;

  if(test_planet_for_name()) n++;

  if(test_orbit_place()) n++;
  if(test_orbit_posvel_callisto()) n++;

  n += test_dates();

  return n;
}
