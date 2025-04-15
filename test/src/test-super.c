/**
 * @date Created  on Feb 18, 2024
 * @author Attila Kovacs
 */

#define _GNU_SOURCE               ///< for strcasecmp()

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
  if(pos) {
    memset(pos, 0, 3 * sizeof(double));
    pos[0] = id % 100;
  }
  if(vel) {
    memset(vel, 0, 3 * sizeof(double));
    vel[1] = 0.01 * (id % 100);
  }
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
    double az = 0.0, za = 0.0, p[3] = {0.0}, pos1[3];

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
  double p[3], v[3] = {0.0}, out[3];

  //if(source.type != NOVAS_PLANET) return 0;

  memcpy(p, pos0, sizeof(p));

  if(!is_ok("aberration:corner:diff", aberration(p, v, 0.0, out))) return 1;
  if(!is_ok("aberration:corner:same", aberration(p, v, 0.0, p))) return 1;
  return 0;
}

static int test_starvectors() {
  double p[3], v[3] = {0.0};

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
  double p[3] = {0.0}, v[3] = {0.0}, p1[3] = {0.0}, v1[3] = {0.0};

  if(!is_ok("geo_posvel:pos:null", geo_posvel(tdb, 0.0, NOVAS_FULL_ACCURACY, &obs, NULL, v))) return 1;
  if(!is_ok("geo_posvel:vel:null", geo_posvel(tdb, 0.0, NOVAS_FULL_ACCURACY, &obs, p, NULL))) return 1;

  if(!is_ok("geo_posvel:p+v", geo_posvel(tdb, 0.0, NOVAS_FULL_ACCURACY, &obs, p, v))) return 1;

  if(!is_ok("geo_posvel:acc", geo_posvel(tdb, 0.0, NOVAS_REDUCED_ACCURACY, &obs, p1, v1))) return 1;
  if(!is_ok("geo_posvel:acc:check:pos", check_equal_pos(p, p1, 1e-9 * vlen(p)))) return 1;
  if(!is_ok("geo_posvel:acc:check:vel", check_equal_pos(v, v1, 1e-6 * vlen(v)))) return 1;

  if(!is_ok("geo_posvel:tdb", geo_posvel(tdb + 0.01, 0.0, NOVAS_FULL_ACCURACY, &obs, p1, v1))) return 1;
  if(!is_ok("geo_posvel:tdb:check:pos", check_equal_pos(p, p1, 1e-5))) return 1;
  if(!is_ok("geo_posvel:tdb:check:vel", check_equal_pos(v, v1, 1e-4))) return 1;

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
      if(!is_ok("equ2ecl:repeat", equ2ecl(tdb, NOVAS_GCRS_EQUATOR, NOVAS_FULL_ACCURACY, ra0, dec0, &elon, &elat))) return 1;
      if(!is_ok("ecl2equ", ecl2equ(tdb, NOVAS_GCRS_EQUATOR, NOVAS_FULL_ACCURACY, elon, elat, &ra, &dec))) return 1;
      if(!is_ok("ecl2equ:repeat", ecl2equ(tdb, NOVAS_GCRS_EQUATOR, NOVAS_FULL_ACCURACY, elon, elat, &ra, &dec))) return 1;

      if(!is_ok("equ_ecl:ra", fabs(remainder((ra - ra0), 24.0) * cos(dec0 * DEG2RAD)) > 1e-8)) return 1;
      if(!is_ok("equ_ecl:dec", fabs(dec - dec0) > 1e-7)) return 1;

      if(!is_ok("equ2ecl:true:", equ2ecl(tdb, NOVAS_TRUE_EQUATOR, NOVAS_FULL_ACCURACY, ra0, dec0, &elon, &elat))) return 1;
      if(!is_ok("equ2ecl:true:repeat", equ2ecl(tdb, NOVAS_TRUE_EQUATOR, NOVAS_FULL_ACCURACY, ra0, dec0, &elon, &elat))) return 1;
      if(!is_ok("ecl2equ:true", ecl2equ(tdb, NOVAS_TRUE_EQUATOR, NOVAS_FULL_ACCURACY, elon, elat, &ra, &dec))) return 1;
      if(!is_ok("ecl2equ:trUe:repeat", ecl2equ(tdb, NOVAS_TRUE_EQUATOR, NOVAS_FULL_ACCURACY, elon, elat, &ra, &dec))) return 1;

      if(!is_ok("equ_ecl:true:ra", fabs(remainder((ra - ra0), 24.0) * cos(dec0 * DEG2RAD)) > 1e-8)) return 1;
      if(!is_ok("equ_ecl:true:dec", fabs(dec - dec0) > 1e-7)) return 1;

      if(!is_ok("equ2ecl:tdb", equ2ecl(tdb + 0.1, NOVAS_TRUE_EQUATOR, NOVAS_FULL_ACCURACY, ra0, dec0, &elon, &elat))) return 1;
      if(!is_ok("ecl2equ:tdb", ecl2equ(tdb + 0.1, NOVAS_TRUE_EQUATOR, NOVAS_FULL_ACCURACY, elon, elat, &ra, &dec))) return 1;

      if(!is_ok("equ_ecl:tdb:ra", fabs(remainder((ra - ra0), 24.0) * cos(dec0 * DEG2RAD)) > 1e-8)) return 1;
      if(!is_ok("equ_ecl:tdb:dec", fabs(dec - dec0) > 1e-7)) return 1;

      if(!is_ok("equ2ecl:acc", equ2ecl(tdb + 0.1, NOVAS_TRUE_EQUATOR, NOVAS_REDUCED_ACCURACY, ra0, dec0, &elon, &elat))) return 1;
      if(!is_ok("ecl2equ:acc", ecl2equ(tdb + 0.1, NOVAS_TRUE_EQUATOR, NOVAS_REDUCED_ACCURACY, elon, elat, &ra, &dec))) return 1;

      if(!is_ok("equ_ecl:acc:ra", fabs(remainder((ra - ra0), 24.0) * cos(dec0 * DEG2RAD)) > 1e-8)) return 1;
      if(!is_ok("equ_ecl:acc:dec", fabs(dec - dec0) > 1e-7)) return 1;
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
    sky_pos posa = SKY_POS_INIT, posb = SKY_POS_INIT;
    if(!is_ok("place_star", place_star(tdb, &source.star, &obs, ut12tt, i, 1, &posa))) return 1;
    if(!is_ok("place_star:control", place(tdb, &source, &obs, ut12tt, i, 1, &posb))) return 1;
    if(!is_ok("place_star:check", check_equal_pos(posa.r_hat, posb.r_hat, 1e-9))) return 1;
  }

  return 0;
}


static int test_place_icrs() {
  sky_pos posa = SKY_POS_INIT, posb = SKY_POS_INIT;

  if(obs.where != NOVAS_OBSERVER_AT_GEOCENTER) return 0;

  if(!is_ok("place_icrs", place_icrs(tdb, &source, NOVAS_REDUCED_ACCURACY, &posa))) return 1;
  if(!is_ok("place_icrs:repeat", place_icrs(tdb, &source, NOVAS_REDUCED_ACCURACY, &posa))) return 1;
  if(!is_ok("place_icrs:control", place(tdb, &source, &obs, ut12tt, NOVAS_ICRS, 1, &posb))) return 1;
  if(!is_ok("place_icrs:check", check_equal_pos(posa.r_hat, posb.r_hat, 1e-9))) return 1;

  if(!is_ok("place_icrs:acc", place_icrs(tdb, &source, NOVAS_REDUCED_ACCURACY, &posb))) return 1;
  if(!is_ok("place_icrs:acc:check", check_equal_pos(posa.r_hat, posb.r_hat, 1e-8))) return 1;

  if(!is_ok("place_icrs:tdb", place_icrs(tdb + 0.1, &source, NOVAS_REDUCED_ACCURACY, &posb))) return 1;
  if(!is_ok("place_icrs:tdb:check", check_equal_pos(posa.r_hat, posb.r_hat, 1e-8))) return 1;

  if(!is_ok("place_icrs:diff", place_icrs(tdb, &source, NOVAS_FULL_ACCURACY, &posb))) return 1;
  if(!is_ok("place_icrs:diff:check", check_equal_pos(posa.r_hat, posb.r_hat, 1e-8))) return 1;

  return 0;
}

static int test_place_gcrs() {
  sky_pos posa = SKY_POS_INIT, posb = SKY_POS_INIT;

  if(obs.where != NOVAS_OBSERVER_AT_GEOCENTER) return 0;

  if(!is_ok("place_gcrs", place_gcrs(tdb, &source, NOVAS_REDUCED_ACCURACY, &posa))) return 1;
  if(!is_ok("place_gcrs:control", place(tdb, &source, &obs, ut12tt, NOVAS_GCRS, 1, &posb))) return 1;
  if(!is_ok("place_gcrs:check", check_equal_pos(posa.r_hat, posb.r_hat, 1e-9))) return 1;

  return 0;
}

static int test_place_cirs() {
  sky_pos posa = SKY_POS_INIT, posb = SKY_POS_INIT;

  if(obs.where != NOVAS_OBSERVER_AT_GEOCENTER) return 0;

  if(!is_ok("place_cirs", place_cirs(tdb, &source, NOVAS_REDUCED_ACCURACY, &posa))) return 1;
  if(!is_ok("place_cirs:control", place(tdb, &source, &obs, ut12tt, NOVAS_CIRS, 1, &posb))) return 1;
  if(!is_ok("place_cirs:check", check_equal_pos(posa.r_hat, posb.r_hat, 1e-9))) return 1;

  return 0;
}

static int test_place_tod() {
  sky_pos posa = SKY_POS_INIT, posb = SKY_POS_INIT;

  if(obs.where != NOVAS_OBSERVER_AT_GEOCENTER) return 0;

  if(!is_ok("place_tod", place_tod(tdb, &source, NOVAS_REDUCED_ACCURACY, &posa))) return 1;
  if(!is_ok("place_tod:control", place(tdb, &source, &obs, ut12tt, NOVAS_TOD, 1, &posb))) return 1;
  if(!is_ok("place_tod:check", check_equal_pos(posa.r_hat, posb.r_hat, 1e-9))) return 1;

  return 0;
}

static int test_place_mod() {
  sky_pos posa = SKY_POS_INIT, posb = SKY_POS_INIT;

  if(obs.where != NOVAS_OBSERVER_AT_GEOCENTER) return 0;

  if(!is_ok("place_mod", place_mod(tdb, &source, 1, &posa))) return 1;
  if(!is_ok("place_mod:control", place(tdb, &source, &obs, ut12tt, NOVAS_MOD, 1, &posb))) return 1;
  if(!is_ok("place_mod:check", check_equal_pos(posa.r_hat, posb.r_hat, 1e-9))) return 1;

  return 0;
}

static int test_place_j2000() {
  sky_pos posa = SKY_POS_INIT, posb = SKY_POS_INIT;

  if(obs.where != NOVAS_OBSERVER_AT_GEOCENTER) return 0;

  if(!is_ok("place_j2000", place_j2000(tdb, &source, NOVAS_REDUCED_ACCURACY, &posa))) return 1;
  if(!is_ok("place_j2000:control", place(tdb, &source, &obs, ut12tt, NOVAS_J2000, 1, &posb))) return 1;
  if(!is_ok("place_j2000:check", check_equal_pos(posa.r_hat, posb.r_hat, 1e-9))) return 1;

  return 0;
}


static int test_radec_star() {
  int i;

  for(i = 0; i < 4; i++) {
    sky_pos posa = SKY_POS_INIT, posb = SKY_POS_INIT;
    double ra, dec, rv;
    if(!is_ok("radec_star", radec_star(tdb, &source.star, &obs, ut12tt, i, NOVAS_REDUCED_ACCURACY, &ra, &dec, &rv))) return 1;
    radec2vector(ra, dec, 1.0, posa.r_hat);

    if(!is_ok("radec_star:control", place(tdb, &source, &obs, ut12tt, i, NOVAS_REDUCED_ACCURACY, &posb))) return 1;
    if(!is_ok("radec_star:check_pos", check_equal_pos(posa.r_hat, posb.r_hat, 1e-9))) return 1;
    if(!is_ok("radec_star:check_rv", fabs(rv - posb.rv) > 1e-6)) return 1;

    if(!is_ok("radec_star:ra:null", radec_star(tdb, &source.star, &obs, ut12tt, i, NOVAS_REDUCED_ACCURACY, NULL, &dec, &rv))) return 1;
    if(!is_ok("radec_star:dec:null", radec_star(tdb, &source.star, &obs, ut12tt, i, NOVAS_REDUCED_ACCURACY, &ra, NULL, &rv))) return 1;
    if(!is_ok("radec_star:rv:null", radec_star(tdb, &source.star, &obs, ut12tt, i, NOVAS_REDUCED_ACCURACY, &ra, &dec, NULL))) return 1;
  }

  return 0;
}


static int test_app_hor(enum novas_reference_system sys) {
  char label[50];
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;

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
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  double pos1[3] = {0.0};

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
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  int i;

  for(i = 0; i < NOVAS_OBSERVER_PLACES; i++) {
    observer obs = OBSERVER_INIT;
    novas_frame frame = NOVAS_FRAME_INIT;
    double sc_pos[3] = {0.0};
    double sc_vel[3] = {0.0};
    double pos1[3] = {0.0};
    sky_pos app = SKY_POS_INIT;

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
  novas_transform T = NOVAS_TRANSFORM_INIT;
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  double pos1[3] = {1.0}, pos2[3] = {2.0};


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
  novas_transform T = NOVAS_TRANSFORM_INIT;
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  double pos1[3] = {1.0}, pos2[3] = {2.0};


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
  novas_transform T = NOVAS_TRANSFORM_INIT;
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  double pos1[3] = {1.0}, pos2[3] = {2.0};

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
  novas_transform T = NOVAS_TRANSFORM_INIT;
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  double pos1[3] = {1.0}, pos2[3] = {2.0};

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
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  enum novas_reference_system from;

  if(!is_ok("transform_rev:set_time", novas_set_time(NOVAS_TDB, tdb, 32, 0.0, &ts))) return 1;
  if(!is_ok("transform_rev:make_observer", make_observer_at_geocenter(&obs))) return 1;
  if(!is_ok("transform_rev:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &ts, 0.0, 0.0, &frame))) return 1;

  for(from = 0; from < NOVAS_REFERENCE_SYSTEMS; from++) {
    char label[50];
    enum novas_reference_system to;

    for(to = from; to < NOVAS_REFERENCE_SYSTEMS; to++) {
      novas_transform T = NOVAS_TRANSFORM_INIT, I = NOVAS_TRANSFORM_INIT;
      double pos1[3] = {1.0}, pos2[3] = {2.0};

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
  double pos1[3] = {0.0}, pos2[3] = {0.0}, d;
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
  double pos1[3] = {0.0}, pos2[3] = {0.0}, d;
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

static int test_make_cat_object_sys() {
  int n = 0;

  cat_entry icrs = CAT_ENTRY_INIT;
  object obj;

  if(!is_ok("make_cat_object_sys:icrs", make_cat_object_sys(&source.star, "ICRS", &obj))) n++;
  if(!is_equal("make_cat_object_sys:icrs:check:ra", obj.star.ra, source.star.ra, 1e-9)) n++;
  if(!is_equal("make_cat_object_sys:icrs:check:dec", obj.star.dec, source.star.dec, 1e-9)) n++;

  if(!is_ok("make_cat_object_sys:j2000", make_cat_object_sys(&source.star, "J2000", &obj))) n++;
  transform_cat(CHANGE_J2000_TO_ICRS, 0.0, &source.star, 0.0, NOVAS_SYSTEM_ICRS, &icrs);
  if(!is_equal("make_cat_object_sys:j2000:check:ra", obj.star.ra, icrs.ra, 1e-9)) n++;
  if(!is_equal("make_cat_object_sys:j2000:check:dec", obj.star.dec, icrs.dec, 1e-9)) n++;

  if(!is_ok("make_cat_object_sys:b1950", make_cat_object_sys(&source.star, "B1950", &obj))) n++;
  transform_cat(CHANGE_EPOCH, NOVAS_JD_B1950, &source.star, NOVAS_JD_J2000, NOVAS_SYSTEM_FK5, &icrs);
  transform_cat(CHANGE_J2000_TO_ICRS, 0.0, &icrs, 0.0, NOVAS_SYSTEM_ICRS, &icrs);
  if(!is_equal("make_cat_object_sys:b19500:check:ra", obj.star.ra, icrs.ra, 1e-9)) n++;
  if(!is_equal("make_cat_object_sys:b19500:check:dec", obj.star.dec, icrs.dec, 1e-9)) n++;

  return n;
}

static int test_make_redshifted_object_sys() {
  int n = 0;

  object obj;

  if(!is_ok("make_redshifted_object_sys:icrs", make_redshifted_object_sys("test", source.star.ra, source.star.dec, "ICRS", 0.0, &obj))) n++;
  if(!is_equal("make_redshifted_object_sys:icrs:check:ra", obj.star.ra, source.star.ra, 1e-9)) n++;
  if(!is_equal("make_redshifted_object_sys:icrs:check:dec", obj.star.dec, source.star.dec, 1e-9)) n++;

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

  if(test_make_cat_object_sys()) n++;
  if(test_make_redshifted_object_sys()) n++;

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
    sky_pos posa = SKY_POS_INIT, posb = SKY_POS_INIT;
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
  double pos1[3] = {0.0}, pos2[3] = {0.0};
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
  int n = 0;

  if(!is_ok("cal_date:y:null", cal_date(tdb, NULL, &m, &d, &h))) n++;
  if(!is_ok("cal_date:m:null", cal_date(tdb, &y, NULL, &d, &h))) n++;
  if(!is_ok("cal_date:d:null", cal_date(tdb, &y, &m, NULL, &h))) n++;
  if(!is_ok("cal_date:h:null", cal_date(tdb, &y, &m, &d, NULL))) n++;

  return n;
}


static int test_jd_to_date() {
  int n = 0;
  int y, m, d;
  double h;
  double tdb = NOVAS_JD_J2000;

  if(!is_ok("jd_to_date:J2000", novas_jd_to_date(NOVAS_JD_J2000, NOVAS_ASTRONOMICAL_CALENDAR, &y, &m, &d, NULL))) n++;
  if(!is_equal("jd_to_date:J2000:year", y, 2000, 1e-6)) n++;
  if(!is_equal("jd_to_date:J2000:month", m, 1, 1e-6)) n++;
  if(!is_equal("jd_to_date:J2000:day", d, 1, 1e-6)) n++;

  if(!is_ok("jd_to_date:1AD", novas_jd_to_date(1721424.0, NOVAS_ASTRONOMICAL_CALENDAR, &y, &m, &d, NULL))) n++;
  if(!is_equal("jd_to_date:1AD:check", y, 1, 1e-6)) n++;

  if(!is_ok("jd_to_date:1BC", novas_jd_to_date(1721423.0, NOVAS_ASTRONOMICAL_CALENDAR, &y, &m, &d, NULL))) n++;
  if(!is_equal("jd_to_date:1BC:check", y, 0, 1e-6)) n++;

  if(!is_ok("jd_to_date:astronomical:reform", novas_jd_to_date(NOVAS_JD_START_GREGORIAN, NOVAS_ASTRONOMICAL_CALENDAR, &y, &m, &d, NULL))) n++;
  if(!is_equal("jd_to_date:astronomical:reform:year", y, 1582, 1e-6)) n++;
  if(!is_equal("jd_to_date:astronomical:reform:month", m, 10, 1e-6)) n++;
  if(!is_equal("jd_to_date:astronomical:reform:day", d, 15, 1e-6)) n++;

  if(!is_ok("jd_to_date:gregorian", novas_jd_to_date(NOVAS_JD_START_GREGORIAN, NOVAS_GREGORIAN_CALENDAR, &y, &m, &d, NULL))) n++;
  if(!is_equal("jd_to_date:gregorian:year", y, 1582, 1e-6)) n++;
  if(!is_equal("jd_to_date:gregorian:month", m, 10, 1e-6)) n++;
  if(!is_equal("jd_to_date:gregorian:day", d, 15, 1e-6)) n++;

  if(!is_ok("jd_to_date:roman", novas_jd_to_date(NOVAS_JD_START_GREGORIAN - 0.5, NOVAS_ROMAN_CALENDAR, &y, &m, &d, NULL))) n++;
  if(!is_equal("jd_to_date:roman:year", y, 1582, 1e-6)) n++;
  if(!is_equal("jd_to_date:roman:month", m, 10, 1e-6)) n++;
  if(!is_equal("jd_to_date:romna:day", d, 4, 1e-6)) n++;

  if(!is_ok("jd_to_date:y:null", novas_jd_to_date(tdb, NOVAS_ASTRONOMICAL_CALENDAR, NULL, &m, &d, &h))) n++;
  if(!is_ok("jd_to_date:m:null", novas_jd_to_date(tdb, NOVAS_ASTRONOMICAL_CALENDAR, &y, NULL, &d, &h))) n++;
  if(!is_ok("jd_to_date:d:null", novas_jd_to_date(tdb, NOVAS_ASTRONOMICAL_CALENDAR, &y, &m, NULL, &h))) n++;
  if(!is_ok("jd_to_date:h:null", novas_jd_to_date(tdb, NOVAS_ASTRONOMICAL_CALENDAR, &y, &m, &d, NULL))) n++;

  return n;
}

static int test_julian_date() {
  int n = 0;

  if(!is_equal("julian_date:J2000", julian_date(2000, 1, 1, 12.0), NOVAS_JD_J2000, 1e-6)) n++;
  if(!is_equal("julian_date:AD-BC", julian_date(1, 1, 1, 0.0), julian_date(0, 12, 31, 0.0) + 1, 1e-6)) n++;

  return n;
}

static int test_jd_from_date() {
  int n = 0;

  if(!is_equal("calendar_to_jd:J2000", novas_jd_from_date(NOVAS_ASTRONOMICAL_CALENDAR, 2000, 1, 1, 12.0), NOVAS_JD_J2000, 1e-6)) n++;

  if(!is_equal("jd_from_date:astronomical",
          novas_jd_from_date(NOVAS_ASTRONOMICAL_CALENDAR, 1582, 10, 15, 0.0),
          novas_jd_from_date(NOVAS_ASTRONOMICAL_CALENDAR, 1582, 10, 4, 0.0) + 1, 1e-6)) n++;

  if(!is_equal("jd_from_date:gregorian",
          novas_jd_from_date(NOVAS_ASTRONOMICAL_CALENDAR, 1582, 10, 15, 0.0),
          novas_jd_from_date(NOVAS_GREGORIAN_CALENDAR, 1582, 10, 15, 0.0), 1e-6)) n++;

  if(!is_equal("jd_to_date:roman",
          novas_jd_from_date(NOVAS_ROMAN_CALENDAR, 1582, 10, 14, 0.0),
          novas_jd_from_date(NOVAS_ASTRONOMICAL_CALENDAR, 1582, 10, 14, 0.0), 1e-6)) n++;

  return n;
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
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  cat_entry c = CAT_ENTRY_INIT;
  object source[2] = { NOVAS_OBJECT_INIT, NOVAS_OBJECT_INIT };
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
    sky_pos p = SKY_POS_INIT, pc = SKY_POS_INIT;

    // place does not apply deflection / aberration for ICRS
    place(tdb, &source[i], &obs, ts.ut1_to_tt, (sys == NOVAS_ICRS ? NOVAS_GCRS : sys), NOVAS_REDUCED_ACCURACY, &pc);

    sprintf(label, "sky_pos:sys=%d:source=%d", sys, i);
    if(!is_ok(label, novas_sky_pos(&source[i], &frame, sys, &p))) return 1;

    sprintf(label, "sky_pos:sys=%d:source=%d:check:ra", sys, i);
    if(!is_equal(label, p.ra, pc.ra, 1e-10)) return 1;

    sprintf(label, "sky_pos:sys=%d:source=%d:check:dec", sys, i);
    if(!is_equal(label, p.dec, pc.dec, 1e-9)) return 1;

    sprintf(label, "sky_pos:sys=%d:source=%d:check:rv", sys, i);
    if(!is_equal(label, p.rv, pc.rv, 1e-6)) return 1;

    sprintf(label, "sky_pos:sys=%d:source=%d:check:r_hat", sys, i);
    if(!is_ok(label, check_equal_pos(p.r_hat, pc.r_hat, 1e-12))) return 1;
  }

  return 0;
}

static int test_geom_posvel() {
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT, acc = NOVAS_FRAME_INIT;
  object source = NOVAS_OBJECT_INIT;
  double pos0[3] = {0.0}, vel0[3] = {0.0}, pos[3] = {1.0}, vel[3] = {1.0};

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
  cat_entry c = CAT_ENTRY_INIT;

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
  cat_entry in = CAT_ENTRY_INIT, out;

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
    if(!is_ok("ephem_provider:ephemeris", ephemeris(tdb2, &body, NOVAS_BARYCENTER, NOVAS_FULL_ACCURACY, p, v))) goto cleanup; // @suppress("Goto statement used")
    if(!is_ok("ephem_provider:control", dummy_ephem(body.name, body.number, tdb, 0.0, &o, p0, v0))) goto cleanup; // @suppress("Goto statement used")
    if(o == NOVAS_BARYCENTER) {
      if(!is_ok("ephem_provider:check_pos", check_equal_pos(p, p0, 1e-9 * vlen(p0)))) goto cleanup; // @suppress("Goto statement used")
      if(!is_ok("ephem_provider:check_vel", check_equal_pos(v, v0, 1e-9 * vlen(v0)))) goto cleanup; // @suppress("Goto statement used")
    }

    if(!is_ok("ephem_provider:ephemeris", ephemeris(tdb2, &body, NOVAS_HELIOCENTER, NOVAS_FULL_ACCURACY, p, v))) goto cleanup; // @suppress("Goto statement used")
    if(o == NOVAS_BARYCENTER) {
      fprintf(stderr, ">>> Expecting diffent A/B, twice:\n");
      if(!is_ok("ephem_provider:check_pos", !check_equal_pos(p, p0, 1e-9 * vlen(p0)))) goto cleanup; // @suppress("Goto statement used")
      if(!is_ok("ephem_provider:check_vel", !check_equal_pos(v, v0, 1e-9 * vlen(v0)))) goto cleanup; // @suppress("Goto statement used")
      fprintf(stderr, " OK.\n");
    }

    if(!is_ok("ephem_provider:bary", planet_ephem_provider(tdb, NOVAS_SUN, NOVAS_BARYCENTER, p, v))) goto cleanup; // @suppress("Goto statement used")
    if(!is_ok("ephem_provider:helio", planet_ephem_provider(tdb, NOVAS_SUN, NOVAS_HELIOCENTER, p, v))) goto cleanup; // @suppress("Goto statement used")
    if(!is_ok("ephem_provider:pos:null", planet_ephem_provider(tdb, NOVAS_SUN, NOVAS_BARYCENTER, NULL, v))) goto cleanup; // @suppress("Goto statement used")
    if(!is_ok("ephem_provider:vel:null", planet_ephem_provider(tdb, NOVAS_SUN, NOVAS_BARYCENTER, p, NULL))) goto cleanup; // @suppress("Goto statement used")
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

  if(!is_equal("ira_equinox", e1, e2, 1e-11)) return 1;

  e2 = ira_equinox(tdb, NOVAS_MEAN_EQUINOX, NOVAS_REDUCED_ACCURACY);
  if(!is_equal("ira_equinox:acc", e1, e2, 1e-8)) return 1;

  e2 = ira_equinox(tdb, NOVAS_TRUE_EQUINOX, NOVAS_REDUCED_ACCURACY);
  if(!is_equal("ira_equinox:type", e1, e2, 5e-3)) return 1;

  e1 = ira_equinox(tdb + 1.0, NOVAS_TRUE_EQUINOX, NOVAS_REDUCED_ACCURACY);
  if(!is_equal("ira_equinox:tjd", e2, e1, 1e-5)) return 1;

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
  double pz[3] = {0.0}, p1[] = {1.0, 0.0, 0.0}, pm[] = {0.5, 0.0, 0.0}, pn[] = {0.0, 1.0, 0.0}, out[3];

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
  double pos_src[3], pos_obs[3], pos_app[3] = {0.0}, pos0[3] = {0.0}, v[3];
  double tdb2[2] = { tdb };
  object earth = NOVAS_OBJECT_INIT;
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

static int test_grav_planets() {
  int n = 0;

  double psrc[3] = {0.0}, pout[3] = {0.0};
  double pose[3] = {0.0}, vele[3] = {0.0};
  double tdb2[2] = { NOVAS_JD_J2000 };
  int pl_mask = 1 << NOVAS_EARTH;
  object earth = NOVAS_EARTH_INIT;
  novas_planet_bundle pl = NOVAS_PLANET_BUNDLE_INIT;

  ephemeris(tdb2, &earth, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pose, vele);
  obs_planets(NOVAS_JD_J2000, NOVAS_REDUCED_ACCURACY, pose, pl_mask, &pl);

  if(!is_ok("grav_planets", grav_planets(psrc, pose, &pl, pout))) return 1;
  if(!is_ok("grav_planets:skip", check_equal_pos(psrc, pout, 1e-12))) return 1;

  return n;
}

static int test_vector2radec() {
  double pos[3] = {1.0};
  double x;

  if(!is_ok("vector2radec:ra:null", vector2radec(pos, NULL, &x))) return 1;
  if(!is_ok("vector2radec:dec:null", vector2radec(pos, &x, NULL))) return 1;

  return 0;
}

static int test_make_cat_object() {
  cat_entry star = CAT_ENTRY_INIT;
  object source = NOVAS_OBJECT_INIT;

  make_cat_entry("test", "FK4", 123, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, &star);

  if(!is_ok("make_cat_object", make_cat_object(&star, &source))) return 1;
  if(!is_ok("make_cat_object:check", memcmp(&source.star, &star, sizeof(star)))) return 1;
  return 0;
}

static int test_airborne_observer() {
  on_surface loc = ON_SURFACE_INIT;
  observer obs = OBSERVER_INIT, gc = OBSERVER_INIT;
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
  observer obs = OBSERVER_INIT, gc = OBSERVER_INIT;
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
  double epos[3] = {0.0}, evel[3] = {0.0}, x[3];
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
  double loc, loc1;
  short type;

  if(!is_ok("cio_location", cio_location(NOVAS_JD_J2000, NOVAS_FULL_ACCURACY, &loc, &type))) return 1;
  if(!is_ok("cio_location:repeat", cio_location(NOVAS_JD_J2000, NOVAS_FULL_ACCURACY, &loc1, &type))) return 1;
  if(!is_equal("cio_location:repeat:check", loc, loc1, 1e-12)) return 1;

  if(!is_ok("cio_location:acc", cio_location(NOVAS_JD_J2000, NOVAS_REDUCED_ACCURACY, &loc1, &type))) return 1;
  if(!is_equal("cio_location:acc:check", loc, loc1, 1e-8)) return 1;

  if(!is_ok("cio_location:tdb", cio_location(NOVAS_JD_J2000 + 0.1, NOVAS_REDUCED_ACCURACY, &loc1, &type))) return 1;
  if(!is_equal("cio_location:tdb:check", loc, loc1, 1e-8)) return 1;

  if(!is_ok("cio_location:diff", cio_location(NOVAS_JD_J2000, NOVAS_FULL_ACCURACY, &loc1, &type))) return 1;
  if(!is_equal("cio_location:diff:check", loc, loc1, 1e-8)) return 1;

  return 0;
}

static int test_cio_array() {
  char path[256];
  ra_of_cio data[10];

  memset(data, 0, sizeof(data));

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
  on_surface obs = ON_SURFACE_INIT;
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
  on_surface obs = ON_SURFACE_INIT;
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
  on_surface obs = ON_SURFACE_INIT;
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
  on_surface obs = ON_SURFACE_INIT;
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
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  observer obs = OBSERVER_INIT;

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
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  novas_frame frame = NOVAS_FRAME_INIT, out = NOVAS_FRAME_INIT;
  observer obs = OBSERVER_INIT;

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
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  observer obs = OBSERVER_INIT;
  novas_transform T = NOVAS_TRANSFORM_INIT, I = NOVAS_TRANSFORM_INIT;

  double pos0[3] = {1.0, 2.0, 3.0}, pos1[3] = {1.0, 2.0, 3.0};
  sky_pos p0 = SKY_POS_INIT, p1 = SKY_POS_INIT;

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
  novas_timespec ts = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
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
  object planet = NOVAS_OBJECT_INIT;
  double pos[3] = {1.0}, pos_obs[3] = {1.0}, v[3] = {0.0};
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
  object ceres = NOVAS_OBJECT_INIT;
  novas_orbital orbit = NOVAS_ORBIT_INIT;
  observer obs = OBSERVER_INIT;
  sky_pos pos = SKY_POS_INIT;
  double p0[3] = {0.0}, p1[3] = {0.0};

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
    if(!is_ok("orbit_place:icrs:check", check_equal_pos(p1, p0, 1e-8))) n++;
  }

  orbit.system.type = NOVAS_CIRS;
  if(!is_ok("orbit_place:cirs", novas_orbit_posvel(tjd, &orbit, NOVAS_FULL_ACCURACY, p1, NULL))) n++;
  else {
    gcrs_to_cirs(tjd, NOVAS_REDUCED_ACCURACY, p1, p1);
    equ2ecl_vec(tjd, NOVAS_TRUE_EQUATOR, NOVAS_FULL_ACCURACY, p1, p1);
    if(!is_ok("orbit_place:cirs:check", check_equal_pos(p1, p0, 1e-8))) n++;
  }

  orbit.system.type = NOVAS_J2000;
  if(!is_ok("orbit_place:j2000", novas_orbit_posvel(tjd, &orbit, NOVAS_FULL_ACCURACY, p1, NULL))) n++;
  else {
    gcrs_to_j2000(p1, p1);
    equ2ecl_vec(NOVAS_JD_J2000, NOVAS_TRUE_EQUATOR, NOVAS_FULL_ACCURACY, p1, p1);
    if(!is_ok("orbit_place:j2000:check", check_equal_pos(p1, p0, 1e-8))) n++;
  }

  orbit.system.type = NOVAS_MOD;
  if(!is_ok("orbit_place:mod", novas_orbit_posvel(tjd, &orbit, NOVAS_FULL_ACCURACY, p1, NULL))) n++;
  else {
    gcrs_to_mod(tjd, p1, p1);
    equ2ecl_vec(tjd, NOVAS_MEAN_EQUATOR, NOVAS_FULL_ACCURACY, p1, p1);
    if(!is_ok("orbit_place:mod:check", check_equal_pos(p1, p0, 1e-8))) n++;
  }

  orbit.system.type = NOVAS_TOD;
  if(!is_ok("orbit_place:tod", novas_orbit_posvel(tjd, &orbit, NOVAS_FULL_ACCURACY, p1, NULL))) n++;
  else {
    gcrs_to_tod(tjd, NOVAS_FULL_ACCURACY, p1, p1);
    equ2ecl_vec(tjd, NOVAS_TRUE_EQUATOR, NOVAS_FULL_ACCURACY, p1, p1);
    if(!is_ok("orbit_place:tod:check", check_equal_pos(p1, p0, 1e-8))) n++;
  }

  return n;
}

static int test_orbit_posvel_callisto() {
  novas_orbital orbit = NOVAS_ORBIT_INIT;
  novas_orbital_system *sys = &orbit.system;
  double pos0[3] = {0.0}, pos[3] = {0.0}, vel[3] = {0.0}, pos1[3] = {0.0}, vel1[3] = {0.0};
  double ra, dec, dra, ddec;
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

static int test_hms_hours() {
  int n = 0;
  double hours = 23.0 + 59.0/60.0 + 59.999/3600.0;

  if(!is_equal("hms_hours:colons", novas_hms_hours("23:59:59.999"), hours, 1e-10)) n++;
  if(!is_equal("hms_hours:spaces", novas_hms_hours("23 59 59.999"), hours, 1e-10)) n++;
  if(!is_equal("hms_hours:hm", novas_hms_hours("23h59m59.999s"), hours, 1e-10)) n++;
  if(!is_equal("hms_hours:HM", novas_hms_hours("23H59M59.999S"), hours, 1e-10)) n++;
  if(!is_equal("hms_hours:HM", novas_hms_hours("23 59’59.999”"), hours, 1e-10)) n++;
  if(!is_equal("hms_hours:hprime", novas_hms_hours("23h59'59.999"), hours, 1e-10)) n++;
  if(!is_equal("hms_hours:combo", novas_hms_hours("23h 59' 59.999"), hours, 1e-10)) n++;
  if(!is_equal("hms_hours:combo:errno=0", errno, 0, 1e-6)) n++;

  if(!is_equal("hms_hours:few", novas_hms_hours("23 59"), hours, 0.02)) n++;
  if(!is_equal("hms_hours:few:errno=0", errno, 0, 1e-6)) n++;

  if(!is_equal("hms_hours:extra", novas_hms_hours("23 59 blah"), hours, 0.02)) n++;
  if(!is_equal("hms_hours:extra:errno=EINVAL", errno, EINVAL, 1e-6)) n++;

  return n;
}

static int test_dms_degrees() {
  int n = 0;
  double degs = 179.0 + 59.0 / 60.0 + 59.999 / 3600.0;

  if(!is_equal("dms_degrees:colons", novas_dms_degrees("179:59:59.999"), degs, 1e-9)) n++;
  if(!is_equal("dms_degrees:spaces", novas_dms_degrees("179 59 59.999"), degs, 1e-9)) n++;
  if(!is_equal("dsm_degrees:dms", novas_dms_degrees("179d59m59.999s"), degs, 1e-9)) n++;
  if(!is_equal("dms_degrees:dprime", novas_dms_degrees("179d59'59.999"), degs, 1e-9)) n++;
  if(!is_equal("dms_degrees:combo", novas_dms_degrees("179d 59' 59.999"), degs, 1e-9)) n++;
  if(!is_equal("dms_degrees:signed", novas_dms_degrees("+179 59 59.999"), degs, 1e-9)) n++;
  if(!is_equal("dms_degrees:signed:errno=0", errno, 0, 1e-6)) n++;

  if(!is_equal("dms_degrees:few", novas_dms_degrees("179 59"), degs, 0.02)) n++;
  if(!is_equal("dms_degrees:few:errno=0", errno, 0, 1e-6)) n++;

  if(!is_equal("dms_degrees:extra", novas_dms_degrees("179 59 blah"), degs, 0.02)) n++;
  if(!is_equal("dms_degrees:extra:errno=EINVAL", errno, EINVAL, 1e-6)) n++;

  if(!is_equal("dms_degrees:combo:N", novas_dms_degrees("179d 59' 59.999N"), degs, 1e-9)) n++;
  if(!is_equal("dms_degrees:combo:S", novas_dms_degrees("-179d 59' 59.999S"), degs, 1e-9)) n++;
  if(!is_equal("dms_degrees:combo:E", novas_dms_degrees("179d 59' 59.999E"), degs, 1e-9)) n++;
  if(!is_equal("dms_degrees:combo:W", novas_dms_degrees("-179d 59' 59.999W"), degs, 1e-9)) n++;

  if(!is_equal("dms_degrees:combo:+N", novas_dms_degrees("179d 59' 59.999 N"), degs, 1e-9)) n++;
  if(!is_equal("dms_degrees:combo:+S", novas_dms_degrees("-179d 59' 59.999 S"), degs, 1e-9)) n++;
  if(!is_equal("dms_degrees:combo:+E", novas_dms_degrees("179d 59' 59.999 E"), degs, 1e-9)) n++;
  if(!is_equal("dms_degrees:combo:+W", novas_dms_degrees("-179d 59' 59.999 W"), degs, 1e-9)) n++;

  if(!is_equal("dms_degrees:neg:colons", novas_dms_degrees("-179:59:59.999"), -degs, 1e-9)) n++;
  if(!is_equal("dms_degrees:neg:spaces", novas_dms_degrees("-179 59 59.999"), -degs, 1e-9)) n++;
  if(!is_equal("dsm_degrees:neg:dms", novas_dms_degrees("-179d59m59.999s"), -degs, 1e-9)) n++;
  if(!is_equal("dms_degrees:neg:dprime", novas_dms_degrees("-179d59'59.999"), -degs, 1e-9)) n++;
  if(!is_equal("dms_degrees:neg:combo", novas_dms_degrees("-179d 59' 59.999"), -degs, 1e-9)) n++;

  if(!is_equal("dms_degrees:neg:combo:N", novas_dms_degrees("-179d 59' 59.999N"), -degs, 1e-9)) n++;
  if(!is_equal("dms_degrees:neg:combo:S", novas_dms_degrees("179d 59' 59.999S"), -degs, 1e-9)) n++;
  if(!is_equal("dms_degrees:neg:combo:E", novas_dms_degrees("-179d 59' 59.999E"), -degs, 1e-9)) n++;
  if(!is_equal("dms_degrees:neg:combo:W", novas_dms_degrees("179d 59' 59.999W"), -degs, 1e-9)) n++;

  if(!is_equal("dms_degrees:neg:combo:W+", novas_parse_dms("179d 59' 59.999W ", NULL), -degs, 1e-9)) n++;
  if(!is_equal("dms_degrees:neg:combo:_W_", novas_parse_dms("179_59_59.999W_", NULL), -degs, 1e-9)) n++;
  if(!is_equal("dms_degrees:neg:combo:_W!", novas_parse_dms("179_59_59.999W!", NULL), -degs, 1e-9)) n++;

  if(!is_equal("dms_degrees:neg:combo:Whatever", novas_dms_degrees("179d 59' 59.999 Whatever"), degs, 1e-9)) n++;
  if(!is_equal("dms_degrees:neg:combo:_Whatever", novas_dms_degrees("179_59_59.999_Whatever"), degs, 1e-9)) n++;
  return n;
}

static int test_parse_degrees() {
  int n = 0;
  double degs = 179.0 + 59.0 / 60.0 + 59.999 / 3600.0;
  char *tail = NULL;

  if(!is_equal("parse_degrees:dms", novas_parse_degrees("179:59:59.999", &tail), degs, 1e-9)) n++;
  if(!is_equal("parse_degrees:dms:notail", novas_parse_degrees("179:59:59.999", NULL), degs, 1e-9)) n++;

  if(!is_equal("parse_degrees:dms:+E", novas_parse_degrees("179:59:59.999E", NULL), degs, 1e-9)) n++;
  if(!is_equal("parse_degrees:dms:+East", novas_parse_degrees("179:59:59.999 East", NULL), degs, 1e-9)) n++;
  if(!is_equal("parse_degrees:dms:^E", novas_parse_degrees("E 179:59:59.999", NULL), degs, 1e-9)) n++;
  if(!is_equal("parse_degrees:dms:^East", novas_parse_degrees("East 179:59:59.999", NULL), degs, 1e-9)) n++;

  if(!is_equal("parse_degrees:dms:+W", novas_parse_degrees("179:59:59.999W", NULL), -degs, 1e-9)) n++;
  if(!is_equal("parse_degrees:dms:+West", novas_parse_degrees("179:59:59.999 West", NULL), -degs, 1e-9)) n++;
  if(!is_equal("parse_degrees:dms:^W", novas_parse_degrees("W 179:59:59.999", NULL), -degs, 1e-9)) n++;
  if(!is_equal("parse_degrees:dms:^West", novas_parse_degrees("West 179:59:59.999", NULL), -degs, 1e-9)) n++;

  if(!is_equal("parse_degrees:dms:^W+E", novas_parse_degrees("W 179:59:59.999E", &tail), -degs, 1e-9)) n++;
  if(!is_equal("parse_degrees:dms:^W+E:tail", *tail, 'E', 1e-6)) n++;

  if(!is_equal("parse_degrees:decimal", novas_parse_degrees("-179.9999999", &tail), -degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:decimal:notail", novas_parse_degrees("-179.9999999", NULL), -degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:decimal:d", novas_parse_degrees("-179.9999999d", &tail), -degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:decimal:d:tail", *tail, 0, 1e-6)) n++;

  if(!is_equal("parse_degrees:N", novas_parse_degrees("179.9999999N", &tail), degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:E", novas_parse_degrees("179.9999999E", &tail), degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:W", novas_parse_degrees("179.9999999W", &tail), -degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:W:tail", *tail, 0, 1e-6)) n++;

  if(!is_equal("parse_degrees:North", novas_parse_degrees("179.9999999North", &tail), degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:East", novas_parse_degrees("179.9999999East", &tail), degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:East:tail", *tail, 0, 1e-6)) n++;

  if(!is_equal("parse_degrees:^North", novas_parse_degrees("North 179.9999999", &tail), degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:^West", novas_parse_degrees("West 179.9999999", &tail), -degs, 1e-6)) n++;

  if(!is_equal("parse_degrees:+S", novas_parse_degrees("179.9999999 S", &tail), -degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:+S:tail", *tail, 0, 1e-6)) n++;

  if(!is_equal("parse_degrees:+South", novas_parse_degrees("179.9999999 South", &tail), -degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:+East", novas_parse_degrees("179.9999999 East", &tail), degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:+East:tail", *tail, 0, 1e-6)) n++;

  if(!is_equal("parse_degrees:^S", novas_parse_degrees("S 179.9999999", &tail), -degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:^W", novas_parse_degrees("W 179.9999999", &tail), -degs, 1e-6)) n++;

  if(!is_equal("parse_degrees:+Whatever", novas_parse_degrees("179.9999999 Whatever", &tail), degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:+Whatever:tail", *tail, ' ', 1e-6)) n++;

  if(!is_equal("parse_degrees:^N+E:tail", *tail, ' ', 1e-6)) n++;
  if(!is_equal("parse_degrees:^N+S", novas_parse_degrees("N 179.9999999 S", &tail), degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:^N+S:tail", *tail, ' ', 1e-6)) n++;

  if(!is_equal("parse_degrees:d+S", novas_parse_degrees("179.9999999d_S", &tail), -degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:d+S:tail", *tail, 0, 1e-6)) n++;

  if(!is_equal("parse_degrees:deg+S", novas_parse_degrees("179.9999999_deg S", &tail), -degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:deg+S:tail", *tail, 0, 1e-6)) n++;

  if(!is_equal("parse_degrees:degree+S", novas_parse_degrees("179.9999999 degree S", &tail), -degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:degree+S:tail", *tail, 0, 1e-6)) n++;

  if(!is_equal("parse_degrees:^W+degree+S", novas_parse_degrees("W 179.9999999 degree S", &tail), -degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:^W+degree+S:tail", *tail, ' ', 1e-6)) n++;

  if(!is_equal("parse_degrees:W+", novas_parse_degrees("179.9999999W ", &tail), -degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:W_", novas_parse_degrees("179.9999999W_", &tail), -degs, 1e-6)) n++;
  if(!is_equal("parse_degrees:W,", novas_parse_degrees("179.9999999W,", &tail), -degs, 1e-6)) n++;

  return n;
}

static int test_parse_hours() {
  int n = 0;
  double h = 23.0 + 59.0 / 60.0 + 59.999 / 3600.0;
  char *tail = NULL;

  if(!is_equal("parse_hours:hms", novas_parse_hours("23:59:59.999", &tail), h, 1e-9)) n++;
  if(!is_equal("parse_hours:hms:notail", novas_parse_hours("23:59:59.999", NULL), h, 1e-9)) n++;
  if(!is_equal("parse_hours:decimal", novas_parse_hours("23.9999999", &tail), h, 1e-6)) n++;
  if(!is_equal("parse_hours:decimal:h", novas_parse_hours("23.9999999h", &tail), h, 1e-6)) n++;
  if(!is_equal("parse_hours:decimal:notail", novas_parse_hours("23.9999999", NULL), h, 1e-6)) n++;
  if(!is_equal("parse_hours:decimal:h_", novas_parse_hours("23.9999999h_", &tail), h, 1e-6)) n++;
  if(!is_equal("parse_hours:decimal:h_:tail", *tail, '_', 1e-6)) n++;

  if(!is_equal("parse_hours:decimal:_h!", novas_parse_hours("23.9999999_h!", &tail), h, 1e-6)) n++;
  if(!is_equal("parse_hours:decimal:_h!:tail", *tail, '!', 1e-6)) n++;

  if(!is_equal("parse_hours:decimal:hours!", novas_parse_hours("23.9999999 hour!", &tail), h, 1e-6)) n++;
  if(!is_equal("parse_hours:decimal:hours!:tail", *tail, '!', 1e-6)) n++;

  return n;
}

static int test_str_hours() {
  int n = 0;
  double h = 23.0 + 59.0 / 60.0 + 59.999 / 3600.0;

  if(!is_equal("str_hours:hms", novas_str_hours("23:59:59.999"), h, 1e-9)) n++;
  if(!is_equal("str_hours:decimal", novas_str_hours("23.9999999"), h, 1e-6)) n++;
  if(!is_equal("str_hours:decimal:h", novas_str_hours("23.9999999h"), h, 1e-6)) n++;
  if(!is_equal("str_hours:decimal:h:errno=0", errno, 0, 1e-6)) n++;

  if(!is_equal("str_hours:decimal:space", novas_str_hours("23.9999999 "), h, 1e-6)) n++;
  if(!is_equal("str_hours:decimal:space:errno=0", errno, 0, 1e-6)) n++;

  if(!is_equal("str_hours:decimal:punct", novas_str_hours("23.9999999,"), h, 1e-6)) n++;
  if(!is_equal("str_hours:decimal:punct:errno=0", errno, 0, 1e-6)) n++;

  if(!is_equal("str_hours:decimal:+z", novas_str_hours("23.9999999z"), h, 1e-6)) n++;
  if(!is_equal("str_hours:decimal:+z:errno=EINVAL", errno, EINVAL, 1e-6)) n++;

  return n;
}

static int test_str_degrees() {
  int n = 0;
  double d = 179.0 + 59.0 / 60.0 + 59.999 / 3600.0;

  if(!is_equal("str_degrees:hms", novas_str_degrees("179:59:59.999"), d, 1e-9)) n++;
  if(!is_equal("str_degrees:decimal", novas_str_degrees("179.9999999"), d, 1e-6)) n++;
  if(!is_equal("str_degrees:decimal:d", novas_str_degrees("179.9999999d"), d, 1e-6)) n++;
  if(!is_equal("str_degrees:decimal:d:errno=0", errno, 0, 1e-6)) n++;

  if(!is_equal("str_degrees:decimal:W", novas_str_degrees("179.9999999W"), -d, 1e-6)) n++;
  if(!is_equal("str_degrees:decimal:W:errno=0", errno, 0, 1e-6)) n++;

  if(!is_equal("str_degrees:decimal:d+W", novas_str_degrees("179.9999999d W"), -d, 1e-6)) n++;
  if(!is_equal("str_degrees:decimal:d+W:errno=0", errno, 0, 1e-6)) n++;

  if(!is_equal("str_degrees:decimal:space", novas_str_degrees("179.9999999 "), d, 1e-6)) n++;
  if(!is_equal("str_degrees:decimal:space:errno=0", errno, 0, 1e-6)) n++;

  if(!is_equal("str_degrees:decimal:punct", novas_str_degrees("179.9999999,"), d, 1e-6)) n++;
  if(!is_equal("str_degrees:decimal:punct:errno=0", errno, 0, 1e-6)) n++;

  if(!is_equal("str_degrees:decimal:+z", novas_str_degrees("179.9999999z"), d, 1e-6)) n++;
  if(!is_equal("str_degrees:decimal:+z:errno=EINVAL", errno, EINVAL, 1e-6)) n++;

  return n;
}

static int test_hpa() {
  int n = 0;

  if(!is_equal("hpa:S", novas_hpa(180, 60, 45), 0.0, 1e-9)) n++;
  if(!is_equal("hpa:E", novas_hpa(90, 60, 0), -90.0, 1e-9)) n++;
  if(!is_equal("hpa:W", novas_hpa(-90, 60, 0), 90.0, 1e-9)) n++;
  if(!is_equal("hpa:N1", remainder(novas_hpa(0, 60, 45) - 180.0, 360.0), 0.0, 1e-9)) n++;
  if(!is_equal("hpa:N2", novas_hpa(0, 30, 45), 0.0, 1e-9)) n++;

  return n;
}

static int test_epa() {
  int n = 0;

  if(!is_equal("epa:ra=0:transit:S", novas_epa(0, 30, 45), 0.0, 1e-9)) n++;
  if(!is_equal("epa:ra=0:transit:N", remainder(novas_epa(0, 60, 45) - 180.0, 360.0), 0.0, 1e-9)) n++;
  if(!is_equal("epa:ra=0:rise", novas_epa(-6, 30, 0), -90.0, 1e-9)) n++;
  if(!is_equal("epa:ra=0:set", novas_epa(6, 30, 0), 90.0, 1e-9)) n++;

  return n;
}

static int test_helio_dist() {
  int n = 0;
  object earth = NOVAS_EARTH_INIT;
  object sun = NOVAS_SUN_INIT;
  double rate;

  if(!is_equal("helio_dist:earth", novas_helio_dist(NOVAS_JD_J2000, &earth, &rate), 1.0, 0.03)) n++;
  if(!is_equal("helio_dist:earth:rate", rate, 0.0, 0.03)) n++;
  if(!is_equal("helio_dist:earth:rate:NULL", novas_helio_dist(NOVAS_JD_J2000, &earth, NULL), 1.0, 0.03)) n++;

  if(!is_equal("helio_dist:sun", novas_helio_dist(NOVAS_JD_J2000, &sun, &rate), 0.0, 1e-9)) n++;
  if(!is_equal("helio_dist:sun:rate", rate, 0.0, 1e-9)) n++;
  if(!is_equal("helio_dist:sun:rate:NULL", novas_helio_dist(NOVAS_JD_J2000, &sun, NULL), 0.0, 1e-9)) n++;

  return n;
}

static int test_solar_power() {
  int n = 0;
  object earth = NOVAS_EARTH_INIT;

  if(!is_equal("solar_power:earth", novas_solar_power(NOVAS_JD_J2000, &earth), 1360.8, 130.0)) n++;

  return n;
}

static int test_solar_illum() {
  int n = 0;
  object cat, earth = NOVAS_EARTH_INIT;
  novas_timespec time = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  double pos[3] = {0.0}, vel[3] = {0.0};
  int i;

  make_redshifted_object("test", 0.0, 0.0, 0.0, &cat);
  make_solar_system_observer(pos, vel, &obs);
  novas_set_time(NOVAS_TDB, NOVAS_JD_J2000, 32.0, 0.0, &time);

  if(!is_ok("solar_illum:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame))) n++;
  if(!is_equal("solar_illum:source:sidereal", 1.0, novas_solar_illum(&cat, &frame), 1e-12)) n++;
  if(!is_equal("solar_illum:source:earth:ssb", 1.0, novas_solar_illum(&earth, &frame), 1e-3)) n++;

  for(i = 3; --i >= 0; ) obs.near_earth.sc_pos[i] = 1.1 * frame.earth_pos[i];
  if(!is_ok("solar_illum:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame))) n++;
  if(!is_equal("solar_illum:source:earth:beyond", 0.0, novas_solar_illum(&earth, &frame), 1e-3)) n++;

  for(i = 3; --i >= 0; ) obs.near_earth.sc_pos[i] = frame.earth_pos[i] + frame.earth_vel[i];
  if(!is_ok("solar_illum:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame))) n++;
  if(!is_equal("solar_illum:source:earth:beyond", 0.5, novas_solar_illum(&earth, &frame), 1e-3)) n++;

  return n;
}

static int test_equ_sep() {
  int n = 0;

  if(!is_equal("equ_sep:dec=0:ra+1", novas_equ_sep(5.5, 0.0, 6.5, 0.0), 15.0, 1e-9)) n++;
  if(!is_equal("equ_sep:dec=0:ra-1", novas_equ_sep(5.5, 0.0, 6.5, 0.0), 15.0, 1e-9)) n++;
  if(!is_equal("equ_sep:dec=60:ra+0.01", novas_equ_sep(5.5, 60.0, 5.51, 60.0), 0.075, 1e-5)) n++;
  if(!is_equal("equ_sep:dec+1", novas_equ_sep(5.5, 15.3, 5.5, 16.3), 1.0, 1e-9)) n++;
  if(!is_equal("equ_sep:poles", novas_equ_sep(1.0, -90.0, 3.0, 90.0), 180.0, 1e-9)) n++;
  if(!is_equal("equ_sep:pole:equ", novas_equ_sep(1.0, -90.0, 3.0, 0.0), 90.0, 1e-9)) n++;
  return n;
}

static int test_h2e_offset() {
  int pa, n = 0;

  for(pa = -180; pa <= 180; pa += 15) {
    double s = sin(pa * DEGREE);
    double c = cos(pa * DEGREE);
    int daz;

    for(daz = -100; daz < 100; daz += 10) {
      int del;
      for(del = -100; del <= 100; del += 10) {
        char label[80];
        double dra, ddec, dAZ, dEL;

        sprintf(label, "h2e_offset:PA=%d:az=%d:el=%d:dra", pa, daz, del);
        novas_h2e_offset(daz, del, pa, &dra, NULL);
        if(!is_equal(label, dra, -c * daz + s * del, 1e-9)) n++;

        sprintf(label, "h2e_offset:PA=%d:az=%d:el=%d:ddec", pa, daz, del);
        novas_h2e_offset(daz, del, pa, NULL, &ddec);
        if(!is_equal(label, ddec, s * daz + c * del, 1e-9)) n++;

        sprintf(label, "h2e_offset:PA=%d:az=%d:el=%d:daz", pa, daz, del);
        novas_e2h_offset(dra, ddec, pa, &dAZ, NULL);
        if(!is_equal(label, dAZ, daz, 1e-9)) n++;

        sprintf(label, "h2e_offset:PA=%d:az=%d:el=%d:del", pa, daz, del);
        novas_e2h_offset(dra, ddec, pa, NULL, &dEL);
        if(!is_equal(label, dEL, del, 1e-9)) n++;
      }
    }
  }

  return n;
}

static int test_object_sep() {
  int n = 0;
  object a = NOVAS_OBJECT_INIT, b = NOVAS_OBJECT_INIT;
  novas_timespec time = NOVAS_TIMESPEC_INIT;
  observer obs = OBSERVER_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;

  novas_set_time(NOVAS_TDB, NOVAS_JD_J2000, 32.0, 0.0, &time);
  make_observer_at_geocenter(&obs);
  if(!is_ok("object_sep:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame))) n++;

  make_redshifted_object("a", 0.0, 60.0, 0.0, &a);
  make_redshifted_object("b", 0.01, 60.0, 0.0, &b);

  if(!is_equal("object_sep:same", novas_object_sep(&a, &a, &frame), 0.0, 1e-12)) n++;

  if(!is_equal("object_sep:dra=0.01:dec=60", novas_object_sep(&a, &b, &frame), 0.075, 1e-5)) n++;

  b.star.ra = 0.0;
  b.star.dec = 61.0;
  if(!is_equal("object_sep:ddec=1:ra=1", novas_object_sep(&a, &b, &frame), 1.0, 1e-4)) n++;

  b.star.ra = 0.02/15.0;
  b.star.dec = 60.01;
  if(!is_equal("object_sep:ddra=ddec=0.01", novas_object_sep(&a, &b, &frame), 0.01 * sqrt(2.0), 1e-4)) n++;


  return n;
}

static int test_frame_lst() {
  int n = 0;
  observer obs = OBSERVER_INIT;
  novas_timespec time = NOVAS_TIMESPEC_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  on_surface loc = ON_SURFACE_INIT;
  double vel[3] = {.0};

  novas_set_time(NOVAS_TDB, NOVAS_JD_J2000, 32.0, 0.0, &time);
  make_observer_on_surface(33.0, 15.0, 0.0, 0.0, 0.0, &obs);
  if(!is_ok("frame_lst:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame))) n++;
  if(!is_equal("frame_lst:lst", novas_frame_lst(&frame), frame.gst + 1.0, 1e-9)) n++;

  make_observer_on_surface(33.0, 90.0, 0.0, 0.0, 0.0, &obs);
  if(!is_ok("frame_lst:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame))) n++;
  if(!is_equal("frame_lst:lst", novas_frame_lst(&frame), frame.gst - 18.0, 1e-9)) n++;

  loc = obs.on_surf;
  make_airborne_observer(&loc, vel, &obs);
  if(!is_ok("frame_lst:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame))) n++;
  if(!is_equal("frame_lst:lst", novas_frame_lst(&frame), frame.gst - 18.0, 1e-9)) n++;

  return n;
}

static int test_rise_set() {
  int n = 0;
  observer obs = OBSERVER_INIT;
  novas_timespec time = NOVAS_TIMESPEC_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  object sun = NOVAS_SUN_INIT;
  object cat;
  double refr;

  // noon (near transit)
  novas_set_time(NOVAS_TDB, NOVAS_JD_J2000, 32.0, 0.0, &time);
  make_observer_on_surface(0.0, 0.0, 0.0, 0.0, 0.0, &obs);
  if(!is_ok("rise_set:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame))) n++;

  // 6am next day...
  if(!is_equal("rise_set:rise", novas_rises_above(0.0, &sun, &frame, NULL), NOVAS_JD_J2000 + 0.75, 0.01)) n++;

  // 6pm same day...
  if(!is_equal("rise_set:set", novas_sets_below(0.0, &sun, &frame, NULL), NOVAS_JD_J2000 + 0.25, 0.01)) n++;

  refr = refract_astro(&obs.on_surf, NOVAS_STANDARD_ATMOSPHERE, 90.0);

  // 6am next day...
  if(!is_equal("rise_set:rise", novas_rises_above(refr, &sun, &frame, novas_standard_refraction), NOVAS_JD_J2000 + 0.75, 0.01)) n++;

  // 6pm same day...
  if(!is_equal("rise_set:set", novas_sets_below(refr, &sun, &frame, novas_standard_refraction), NOVAS_JD_J2000 + 0.25, 0.01)) n++;

  make_redshifted_object("test", frame.gst, 20.0, 0.0, &cat);
  if(!is_equal("rise_set:rise:ra=gst", novas_rises_above(0.0, &cat, &frame, NULL), NOVAS_JD_J2000 + 0.75, 0.01)) n++;
  if(!is_equal("rise_set:set:ra=gst", novas_sets_below(0.0, &cat, &frame, NULL), NOVAS_JD_J2000 + 0.25, 0.01)) n++;

  if(!is_ok("rise_set:fixed_time", novas_set_time(NOVAS_UTC, novas_date("2025-08-20"), 37, 0.0, &time))) n++;
  make_observer_on_surface(40.73, -73.92, 0.0, 0.0, 0.0, &obs);
  if(!is_ok("rise_set:fixed_time:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame))) n++;

  // 10:11 UTC from USNO
  if(!is_equal("rise_set:fixed_time:rise", novas_rises_above(-0.26, &sun, &frame, novas_standard_refraction), 2460907.5 + 0.4243, 1e-3)) n++;

  // 23:46 UTC from USNO
  if(!is_equal("rise_set:fixed_time:set", novas_sets_below(-0.26, &sun, &frame, novas_standard_refraction), 2460907.5 + 0.9903, 1e-3)) n++;

  return n;
}

static int test_transit_time() {
  int n = 0;

  observer obs = OBSERVER_INIT;
  novas_timespec time = NOVAS_TIMESPEC_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  object sun = NOVAS_SUN_INIT;

  // midnight (near transit)
  novas_set_time(NOVAS_TDB, NOVAS_JD_J2000 - 0.5, 32.0, 0.0, &time);
  make_observer_on_surface(0.0, 0.0, 0.0, 0.0, 0.0, &obs);
  if(!is_ok("transit_time:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame))) n++;

  if(!is_equal("transit_time:lon=0", novas_transit_time(&sun, &frame), NOVAS_JD_J2000, 0.01)) n++;

  make_observer_on_surface(0.0, -90.0, 0.0, 0.0, 0.0, &obs);
  novas_change_observer(&frame, &obs, &frame);
  if(!is_equal("transit_time:lon=-90", novas_transit_time(&sun, &frame), NOVAS_JD_J2000 + 0.25, 0.01)) n++;

  return n;
}

static int test_equ_track() {
  int n = 0;
  observer obs = OBSERVER_INIT;
  novas_timespec time = NOVAS_TIMESPEC_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  object sun = NOVAS_SUN_INIT;
  novas_track track = NOVAS_TRACK_INIT;
  sky_pos pos = SKY_POS_INIT;
  double x = 0.0;

  // noon (near transit)
  novas_set_time(NOVAS_TDB, NOVAS_JD_J2000, 32.0, 0.0, &time);
  make_observer_on_surface(0.0, 0.0, 0.0, 0.0, 0.0, &obs);
  if(!is_ok("equ_track:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame))) n++;

  if(!is_ok("equ_track:sky_pos", novas_sky_pos(&sun, &frame, NOVAS_TOD, &pos))) n++;
  if(!is_ok("equ_track", novas_equ_track(&sun, &frame, 3600.0, &track))) n++;

  if(!is_equal("equ_track:ra", track.pos.lon / 15.0, pos.ra, 1e-9)) n++;
  if(!is_equal("equ_track:dec", track.pos.lat, pos.dec, 1e-9)) n++;
  if(!is_equal("equ_track:dis", track.pos.dist, pos.dis, 1e-9)) n++;
  if(!is_equal("equ_track:z", track.pos.z, novas_v2z(pos.rv), 1e-9)) n++;

  if(!is_equal("equ_track:rate", hypot(track.rate.lon, track.rate.lat), (360.0 / 365.25) / DAY, 0.2 / DAY)) n++;
  if(!is_equal("equ_track:rate:z", track.rate.z, 0.0, 1e-9)) n++;
  if(!is_equal("equ_track:rate:dist", track.rate.dist, 0.0, 1e-9)) n++;

  if(!is_equal("equ_track:accel", hypot(track.accel.lon, track.accel.lat), 0.0, 0.03 / (DAY * DAY))) n++;
  if(!is_equal("equ_track:accel:z", track.accel.z, 0.0, 1e-16)) n++;
  if(!is_equal("equ_track:accel:dist", track.accel.dist, 0.0, 1e-12)) n++;

  time.fjd_tt += 0.01;
  if(!is_ok("equ_track:make_frame:shifted", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame))) n++;
  if(!is_ok("equ_track:sky_pos", novas_sky_pos(&sun, &frame, NOVAS_TOD, &pos))) n++;

  if(!is_ok("equ_track:track_pos:lon", novas_track_pos(&track, &time, &x, NULL, NULL, NULL))) n++;
  if(!is_equal("equ_track:track_pos:lon:check", x, remainder(15.0 * pos.ra, 360.0), 1e-5)) n++;

  if(!is_ok("equ_track:track_pos:lat", novas_track_pos(&track, &time, NULL, &x, NULL, NULL))) n++;
  if(!is_equal("equ_track:track_pos:lat:check", x, pos.dec, 1e-5)) n++;

  if(!is_ok("equ_track:track_pos:dist", novas_track_pos(&track, &time, NULL, NULL, &x, NULL))) n++;
  if(!is_equal("equ_track:track_pos:dist:check", x, pos.dis, 1e-9)) n++;

  if(!is_ok("equ_track:track_pos:z", novas_track_pos(&track, &time, NULL, NULL, NULL, &x))) n++;
  if(!is_equal("equ_track:track_pos:dist:z", x, novas_v2z(pos.rv), 1e-9)) n++;

  return n;
}

static int test_hor_track() {
  int n = 0;
  observer obs = OBSERVER_INIT;
  novas_timespec time = NOVAS_TIMESPEC_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  object source = NOVAS_OBJECT_INIT;
  novas_track track = NOVAS_TRACK_INIT;
  sky_pos pos = SKY_POS_INIT;
  double az0 = 0.0, el0 = 0.0, x = 0.0;

  // noon (near transit)
  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32.0, 0.0, &time);
  make_observer_on_surface(0.0, 0.0, 0.0, 0.0, 0.0, &obs);
  if(!is_ok("hor_track:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame))) n++;

  // A source that transits at 60 deg elevation
  make_redshifted_object("Test", frame.gst, -60.0, 0.0, &source);

  if(!is_ok("hor_track:sky_pos", novas_sky_pos(&source, &frame, NOVAS_TOD, &pos))) n++;
  if(!is_ok("hor_track:app_to_hor", novas_app_to_hor(&frame, NOVAS_TOD, pos.ra, pos.dec, NULL, &az0, &el0))) n++;
  if(!is_ok("hor_track", novas_hor_track(&source, &frame, NULL, &track))) n++;

  if(!is_equal("hor_track:az", track.pos.lon, az0, 1e-9)) n++;
  if(!is_equal("hor_track:el", track.pos.lat, el0, 1e-9)) n++;
  if(!is_equal("hor_track:dis", track.pos.dist, pos.dis, 1e-12 * pos.dis)) n++;
  if(!is_equal("hor_track:z", track.pos.z, novas_v2z(pos.rv), 1e-9)) n++;

  if(!is_equal("hor_track:rate:lat", track.rate.lat, 0.0, 1e-5)) n++;
  if(!is_equal("hor_track:rate:z", track.rate.z, 0.0, 1e-9)) n++;
  if(!is_equal("hor_track:rate:dist", track.rate.dist, 0.0, 1e-2)) n++;

  if(!is_equal("hor_track:accel:lon", track.accel.lon, 0.0, 1e-9)) n++;
  if(!is_equal("hor_track:rate:lat", track.rate.lat, 0.0, 1e-3)) n++;
  if(!is_equal("hor_track:accel:z", track.accel.z, 0.0, 1e-16)) n++;
  if(!is_equal("hor_track:accel:dist", track.accel.dist, 0.0, 1.0)) n++;

  if(!is_ok("hor_track:app_to_hor:ref", novas_app_to_hor(&frame, NOVAS_TOD, pos.ra, pos.dec, novas_standard_refraction, &az0, &el0))) n++;
  if(!is_ok("hor_track:ref", novas_hor_track(&source, &frame, novas_standard_refraction, &track))) n++;

  if(!is_equal("hor_track:az:ref", track.pos.lon, az0, 1e-9)) n++;
  if(!is_equal("hor_track:el:ref", track.pos.lat, el0, 1e-9)) n++;
  if(!is_equal("hor_track:dis:ref", track.pos.dist, pos.dis, 1e-12 * pos.dis)) n++;
  if(!is_equal("hor_track:z:ref", track.pos.z, novas_v2z(pos.rv), 1e-9)) n++;

  time.fjd_tt += 10.0 / DAY;
  if(!is_ok("hor_track:make_frame:shifted", novas_make_frame(NOVAS_REDUCED_ACCURACY, &obs, &time, 0.0, 0.0, &frame))) n++;
  if(!is_ok("hor_track:sky_pos", novas_sky_pos(&source, &frame, NOVAS_TOD, &pos))) n++;
  if(!is_ok("hor_track:app_to_hor", novas_app_to_hor(&frame, NOVAS_TOD, pos.ra, pos.dec, novas_standard_refraction, &az0, &el0))) n++;

  if(!is_ok("hor_track:track_pos:lon", novas_track_pos(&track, &time, &x, NULL, NULL, NULL))) n++;
  if(!is_equal("hor_track:track_pos:lon:check", x, remainder(az0, 360.0), 1e-3)) n++;

  if(!is_ok("hor_track:track_pos:lat", novas_track_pos(&track, &time, NULL, &x, NULL, NULL))) n++;
  if(!is_equal("hor_track:track_pos:lat:check", x, el0, 1e-3)) n++;

  if(!is_ok("hor_track:track_pos:dist", novas_track_pos(&track, &time, NULL, NULL, &x, NULL))) n++;
  if(!is_equal("hor_track:track_pos:dist:check", x, pos.dis, 1e-12 * pos.dis)) n++;

  if(!is_ok("hor_track:track_pos:z", novas_track_pos(&track, &time, NULL, NULL, NULL, &x))) n++;
  if(!is_equal("hor_track:track_pos:dist:z", x, novas_v2z(pos.rv), 1e-9)) n++;

  return n;
}

static int test_xyz_to_uvw() {
  int n = 0;
  double xyz[3] = {0.0}, uvw[3] = {0.0};

  xyz[0] = 1.0;
  novas_xyz_to_uvw(xyz, 0.0, 0.0, uvw);
  if(!is_equal("xyz_to_uvw:x:u", uvw[0], 0.0, 1e-12)) n++;
  if(!is_equal("xyz_to_uvw:x:v", uvw[1], 0.0, 1e-12)) n++;
  if(!is_equal("xyz_to_uvw:x:w", uvw[2], 1.0, 1e-12)) n++;

  novas_uvw_to_xyz(uvw, 0.0, 0.0, xyz);
  if(!is_equal("xyz_to_uvw:x:x", xyz[0], 1.0, 1e-12)) n++;
  if(!is_equal("xyz_to_uvw:x:y", xyz[1], 0.0, 1e-12)) n++;
  if(!is_equal("xyz_to_uvw:x:z", xyz[2], 0.0, 1e-12)) n++;

  xyz[0] = 0.0;
  xyz[1] = 1.0;
  xyz[2] = 0.0;
  novas_xyz_to_uvw(xyz, 0.0, 0.0, uvw);
  if(!is_equal("xyz_to_uvw:y:u", uvw[0], 1.0, 1e-12)) n++;
  if(!is_equal("xyz_to_uvw:y:v", uvw[1], 0.0, 1e-12)) n++;
  if(!is_equal("xyz_to_uvw:y:w", uvw[2], 0.0, 1e-12)) n++;

  novas_uvw_to_xyz(uvw, 0.0, 0.0, xyz);
  if(!is_equal("xyz_to_uvw:y:x", xyz[0], 0.0, 1e-12)) n++;
  if(!is_equal("xyz_to_uvw:y:y", xyz[1], 1.0, 1e-12)) n++;
  if(!is_equal("xyz_to_uvw:y:z", xyz[2], 0.0, 1e-12)) n++;

  xyz[0] = 0.0;
  xyz[1] = 0.0;
  xyz[2] = 1.0;
  novas_xyz_to_uvw(xyz, 0.0, 0.0, uvw);
  if(!is_equal("xyz_to_uvw:z:u", uvw[0], 0.0, 1e-12)) n++;
  if(!is_equal("xyz_to_uvw:z:v", uvw[1], 1.0, 1e-12)) n++;
  if(!is_equal("xyz_to_uvw:z:w", uvw[2], 0.0, 1e-12)) n++;

  novas_uvw_to_xyz(uvw, 0.0, 0.0, xyz);
  if(!is_equal("xyz_to_uvw:z:x", xyz[0], 0.0, 1e-12)) n++;
  if(!is_equal("xyz_to_uvw:z:y", xyz[1], 0.0, 1e-12)) n++;
  if(!is_equal("xyz_to_uvw:z:z", xyz[2], 1.0, 1e-12)) n++;

  return n;
}

static int test_sun_moon_angle() {
  int n = 0;
  object sun = NOVAS_SUN_INIT;
  object moon = NOVAS_MOON_INIT;
  object earth = NOVAS_EARTH_INIT;
  observer obs = OBSERVER_INIT, gc = OBSERVER_INIT;
  novas_timespec time = NOVAS_TIMESPEC_INIT;
  novas_frame frame = NOVAS_FRAME_INIT;
  novas_planet_provider pl;
  double pos[3] = {0.0}, vel[3] = {0.0}, op[3] = {0.0}, ov[3] = {0.0};
  int i;

  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32.0, 0.0, &time);
  make_observer_at_geocenter(&gc);

  if(!is_ok("sun_angle:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &gc, &time, 0.0, 0.0, &frame))) n++;
  if(!is_ok("sun_angle:geom_posvel:sun", novas_geom_posvel(&sun, &frame, NOVAS_TOD, pos, vel))) n++;

  for(i = 3; --i >= 0; ) op[i] = frame.earth_pos[i] + 0.1 * pos[i];
  make_solar_system_observer(op, ov, &obs);
  novas_change_observer(&frame, &obs, &frame);
  if(!is_equal("sun_angle:oppose", novas_sun_angle(&earth, &frame), 180.0, 0.1)) n++;

  for(i = 3; --i >= 0; ) op[i] = frame.earth_pos[i] - 0.1 * pos[i];
  make_solar_system_observer(op, ov, &obs);
  novas_change_observer(&frame, &obs, &frame);
  if(!is_equal("sun_angle:align", novas_sun_angle(&earth, &frame), 0.0, 0.1)) n++;

  //------------------------------------------------------------------------------------------
  pl = get_planet_provider();
  set_planet_provider(dummy_planet);
  if(!is_ok("sun_angle:make_frame", novas_make_frame(NOVAS_REDUCED_ACCURACY, &gc, &time, 0.0, 0.0, &frame))) n++;
  if(!is_ok("moon_angle:geom_posvel:moon", novas_geom_posvel(&moon, &frame, NOVAS_TOD, pos, vel))) n++;

  for(i = 0; i < 3; i++) op[i] = frame.earth_pos[i] - 0.1 * pos[i];
  make_solar_system_observer(op, ov, &obs);
  novas_change_observer(&frame, &obs, &frame);
  if(!is_equal("moon_angle:align", novas_moon_angle(&earth, &frame), 0.0, 0.1)) n++;

  set_planet_provider(pl);

  return n;
}

static int test_unwrap_angles() {
  extern double novas_unwrap_angles(double *a, double *b, double *c);

  int n = 0;
  double a, b, c;

  a = 270.0, b = 0.0, c = 89.9;
  novas_unwrap_angles(&a, &b, &c);
  if(!is_equal("unwrap_angles:1:a", a, 270.0, 1e-12)) n++;
  if(!is_equal("unwrap_angles:1:b", b, 360.0, 1e-12)) n++;
  if(!is_equal("unwrap_angles:1:c", c, 449.9, 1e-12)) n++;

  a = 89.9, b = 270.0, c = 0.0;
  novas_unwrap_angles(&a, &b, &c);
  if(!is_equal("unwrap_angles:2:a", a, 449.9, 1e-12)) n++;
  if(!is_equal("unwrap_angles:2:b", b, 270.0, 1e-12)) n++;
  if(!is_equal("unwrap_angles:2:c", c, 360.0, 1e-12)) n++;

  a = 0.0, b = 89.9, c = 270.0;
  novas_unwrap_angles(&a, &b, &c);
  if(!is_equal("unwrap_angles:3:a", a, 360.0, 1e-12)) n++;
  if(!is_equal("unwrap_angles:3:b", b, 449.9, 1e-12)) n++;
  if(!is_equal("unwrap_angles:3:c", c, 270.0, 1e-12)) n++;

  return n;
}

static int test_lsr_vel() {
  int n = 0;

  double vSSB[3] = { 11.1, 12.24, 7.25 };      // km/s
  double p[3] = {0.0};
  int i;

  for(i = 0; i < 3; i++) {
    double ra, dec, v;

    memset(p, 0, sizeof(p));
    p[i] = 1.0;

    vector2radec(p, &ra, &dec);

    v = novas_ssb_to_lsr_vel(2000.0, ra, dec, 0.0);
    if(!is_equal("lsr_vel:lsr", v, vSSB[i], 1e-12)) n++;

    v = novas_lsr_to_ssb_vel(2000.0, ra, dec, v);
    if(!is_equal("lsr_vel:ssb", v, 0.0, 1e-12)) n++;
  }

  return n;
}

static int test_parse_date() {
  int n = 0;
  double jd;
  char *tail = NULL;

  //  2025-01-26
  //  2025 January 26
  //  2025_Jan_26
  //  2025-01-26T19:33:08Z
  //  2025.01.26T19:33:08
  //  2025 1 26 19h33m28.113
  //  2025/1/26 19:33:28+02
  //  2025-01-26T19:33:28-0600
  //  2025 Jan 26 19:33:28+0530


  jd = julian_date(2025, 1, 26, 0.0);


  //  2025-01-26
  if(!is_equal("parse_date:1", novas_parse_date("2025-01-26", &tail), jd, 1e-6)) n++;
  if(!is_ok("parse_date:1:tail", tail == NULL)) n++;
  if(!is_equal("parse_date:1:tail", *tail, 0, 1e-6)) n++;

  //  2025 January 26
  if(!is_equal("parse_date:2", novas_parse_date("2025 January 26", &tail), jd, 1e-6)) n++;
  if(!is_equal("parse_date:2:tail", *tail, 0, 1e-6)) n++;

  //  2025_Jan_26
  if(!is_equal("parse_date:3", novas_parse_date("2025_Jan_26", &tail), jd, 1e-6)) n++;
  if(!is_equal("parse_date:3:tail", *tail, 0, 1e-6)) n++;

  if(!is_equal("parse_date:4", novas_parse_date("2025/1/26", &tail), jd, 1e-6)) n++;
  if(!is_ok("parse_date:4:tail", tail == NULL)) n++;
  if(!is_equal("parse_date:4:tail", *tail, 0, 1e-6)) n++;

  if(!is_equal("parse_date:5", novas_parse_date("2025/1/26 _", &tail), jd, 1e-6)) n++;
  if(!is_ok("parse_date:5:tail", tail == NULL)) n++;
  if(!is_equal("parse_date:5:tail", *tail, ' ', 1e-6)) n++;

  if(!is_equal("parse_date:6", novas_parse_date("2025 1 26 blah", &tail), jd, 1e-6)) n++;
  if(!is_equal("parse_date:6:tail", *tail, 'b', 1e-6)) n++;

  jd += (19.0 + 33.0/60.0 + 8.0/3600.0) / 24.0;

  //  2025-01-26T19:33:08Z
  if(!is_equal("parse_date:7", novas_parse_date("2025-01-26T19:33:08Z", &tail), jd, 1e-6)) n++;
  if(!is_equal("parse_date:7:tail", *tail, 0, 1e-6)) n++;

  //  2025.01.26T19:33:08
  if(!is_equal("parse_date:8", novas_parse_date("2025.01.26T19:33:08", &tail), jd, 1e-6)) n++;
  if(!is_equal("parse_date:8:tail", *tail, 0, 1e-6)) n++;

  //  2025/1/26 19:33:8+02
  if(!is_equal("parse_date:9", novas_parse_date("2025/1/26 19:33:8+02", &tail), jd - (2.0/24.0), 1e-6)) n++;
  if(!is_equal("parse_date:9:tail", *tail, 0, 1e-6)) n++;

  //  2025-01-26T19:33:08-0600
  if(!is_equal("parse_date:10", novas_parse_date("2025-01-26t19:33:08-0600", &tail), jd + (6.0/24.0), 1e-6)) n++;
  if(!is_equal("parse_date:10:tail", *tail, 0, 1e-6)) n++;

  //  2025 Jan 26 19:33:08+05:30
  if(!is_equal("parse_date:11", novas_parse_date("2025 Jan 26 19:33:08+05:30", &tail), jd - (5.5/24.0), 1e-6)) n++;
  if(!is_equal("parse_date:11:tail", *tail, 0, 1e-6)) n++;

  //  2025 Jan 26 19:33:08+05:
  if(!is_equal("parse_date:12", novas_parse_date("2025 Jan 26 19:33:08+05:", &tail), jd - (5.0/24.0), 1e-6)) n++;
  if(!is_equal("parse_date:12:tail", *tail, ':', 1e-6)) n++;

  //  2025 1 26 19h33m28.113
  jd += 0.113 / DAY;
  if(!is_equal("parse_date:13", novas_parse_date("2025 1 26 19h33m08.113", &tail), jd, 1e-6)) n++;
  if(!is_equal("parse_date:13:tail", *tail, 0, 1e-6)) n++;

  if(!is_equal("parse_date:14", novas_parse_date("2025 1 26 19h33m08.113 _", &tail), jd, 1e-6)) n++;
  if(!is_equal("parse_date:14:tail", *tail, ' ', 1e-6)) n++;

  if(!is_equal("parse_date:15", novas_parse_date("2025 1 26 19h33m08.113z_ ", &tail), jd, 1e-6)) n++;
  if(!is_equal("parse_date:15:tail", *tail, '_', 1e-6)) n++;

  return n;
}

static int test_parse_iso_date() {
  int n = 0;

  char *date = "2000-01-01T12:00:00.000Z";
  char *tail = NULL;
  double jd = novas_parse_iso_date(date, &tail);

  if(!is_equal("parse_iso_date:J2000:jd", jd, NOVAS_JD_J2000, 1e-6)) n++;
  if(!is_equal("parse_iso_date:J2000:astro", jd, novas_parse_date(date, NULL), 1e-6)) n++;

  // The Roman/Julian date the day before the Gregorian calendar reform, or 10 days
  // before it in the proleptic Gregorian calendar used by the ISO timestamps.
  date = "1582-10-04";
  jd = novas_parse_iso_date(date, &tail);
  if(!is_equal("parse_iso_date:pre-reform:jd", jd, NOVAS_JD_START_GREGORIAN - 11, 1e-6)) n++;
  if(!is_equal("parse_iso_date:pre-reform:astro", jd, novas_parse_date(date, NULL) - 10, 1e-6)) n++;

  return n;
}

static int test_parse_date_format() {
  int n = 0;
  double jd;
  char *tail = NULL;

  jd = julian_date(2025, 1, 26, 0.0);

  //  2025-01-26
  if(!is_equal("parse_date_format:YMD", novas_parse_date_format(0, NOVAS_YMD, "2025-01-26", &tail), jd, 1e-6)) n++;
  if(!is_ok("parse_date_format:YMD:tail", tail == NULL)) n++;
  if(!is_equal("parse_date_format:YMD:tail", *tail, 0, 1e-6)) n++;

  //  2025-01-26
  if(!is_equal("parse_date_format:DMY", novas_parse_date_format(0, NOVAS_DMY, "26.01.2025", &tail), jd, 1e-6)) n++;
  if(!is_ok("parse_date_format:DMY:tail", tail == NULL)) n++;
  if(!is_equal("parse_date_format:DMY:tail", *tail, 0, 1e-6)) n++;

  //  2025-01-26
  if(!is_equal("parse_date_format:MDY", novas_parse_date_format(0, NOVAS_MDY, "1/26/2025", &tail), jd, 1e-6)) n++;
  if(!is_ok("parse_date_format:MDY:tail", tail == NULL)) n++;
  if(!is_equal("parse_date_format:MDY:tail", *tail, 0, 1e-6)) n++;

  return n;
}

static int test_date() {
  int n = 0;

  double jd = julian_date(2025, 3, 01, 0.0);

  if(!is_equal("parse_date_format:YMD", novas_date("2025-03-01"), jd, 1e-6)) n++;

  return n;
}

static int test_date_scale() {
  int n = 0;

  enum novas_timescale scale;
  double jd = julian_date(2025, 3, 01, 0.0);

  if(!is_equal("date_scale:check:jd", novas_date_scale("2025-03-01", &scale), jd, 1e-6)) n++;
  if(!is_equal("date_scale:check:scale:default", scale, NOVAS_UTC, 1e-6)) n++;

  if(!is_equal("date_scale:tai:check:jd", novas_date_scale("2025-03-01 TAI", &scale), jd, 1e-6)) n++;
  if(!is_equal("date_scale:tai:check:scale:default", scale, NOVAS_TAI, 1e-6)) n++;

  return n;
}

static int test_iso_timestamp() {
  int n = 0;
  int i;
  novas_timespec time = NOVAS_TIMESPEC_INIT;
  char str[30] = {'\0'};

  if(!is_ok("iso_timestamp:set_time", novas_set_time(NOVAS_UTC, NOVAS_JD_J2000, 32, 0.0, &time))) n++;
  if(!is_ok("iso_timestamp:J2000", novas_iso_timestamp(&time, str, sizeof(str) - 1) <= 0)) n++;
  if(!is_ok("iso_timestamp:J2000:check", strcmp(str, "2000-01-01T12:00:00.000Z"))) n++;
  if(!is_equal("iso_timestamp:truncate", novas_iso_timestamp(&time, str, 10), 9, 1e-6)) n++;

  for(i = 0; i < 100.0; i++) {
    double jd = NOVAS_JD_J2000 + M_PI * i;
    char label[40];

    sprintf(label, "iso_timestamp:set_time:%d", i);
    if(!is_ok(label, novas_set_time(NOVAS_UTC, jd, 32, 0.0, &time))) n++;

    sprintf(label, "iso_timestamp:round:%d", i);
    if(!is_ok(label, novas_iso_timestamp(&time, str, sizeof(str)) < 0)) n++;

    sprintf(label, "iso_timestamp:check:%d", i);
    if(!is_equal(label, novas_parse_date(str, NULL), jd, 1e-6)) {
      printf("### %s\n", str);
      n++;
    }
  }

  return n;
}

static int test_timescale_for_string() {
  int n = 0;

  if(!is_equal("timescale_for_string:UTC", novas_timescale_for_string("UTC"), NOVAS_UTC, 1e-6)) n++;
  if(!is_equal("timescale_for_string:UT", novas_timescale_for_string("UT"), NOVAS_UTC, 1e-6)) n++;
  if(!is_equal("timescale_for_string:UT0", novas_timescale_for_string("UT0"), NOVAS_UTC, 1e-6)) n++;
  if(!is_equal("timescale_for_string:GMT", novas_timescale_for_string("GMT"), NOVAS_UTC, 1e-6)) n++;
  if(!is_equal("timescale_for_string:UT1", novas_timescale_for_string("UT1"), NOVAS_UT1, 1e-6)) n++;
  if(!is_equal("timescale_for_string:GPS", novas_timescale_for_string("GPS"), NOVAS_GPS, 1e-6)) n++;
  if(!is_equal("timescale_for_string:TAI", novas_timescale_for_string("TAI"), NOVAS_TAI, 1e-6)) n++;
  if(!is_equal("timescale_for_string:TT", novas_timescale_for_string("TT"), NOVAS_TT, 1e-6)) n++;
  if(!is_equal("timescale_for_string:ET", novas_timescale_for_string("ET"), NOVAS_TT, 1e-6)) n++;
  if(!is_equal("timescale_for_string:TCG", novas_timescale_for_string("TCG"), NOVAS_TCG, 1e-6)) n++;
  if(!is_equal("timescale_for_string:TCB", novas_timescale_for_string("TCB"), NOVAS_TCB, 1e-6)) n++;
  if(!is_equal("timescale_for_string:TDB", novas_timescale_for_string("TDB"), NOVAS_TDB, 1e-6)) n++;

  if(!is_equal("timescale_for_string:utc", novas_timescale_for_string("utc"), NOVAS_UTC, 1e-6)) n++;
  if(!is_equal("timescale_for_string:ut", novas_timescale_for_string("ut"), NOVAS_UTC, 1e-6)) n++;
  if(!is_equal("timescale_for_string:ut0", novas_timescale_for_string("ut0"), NOVAS_UTC, 1e-6)) n++;
  if(!is_equal("timescale_for_string:gmt", novas_timescale_for_string("gmt"), NOVAS_UTC, 1e-6)) n++;
  if(!is_equal("timescale_for_string:ut1", novas_timescale_for_string("ut1"), NOVAS_UT1, 1e-6)) n++;
  if(!is_equal("timescale_for_string:gps", novas_timescale_for_string("gps"), NOVAS_GPS, 1e-6)) n++;
  if(!is_equal("timescale_for_string:tai", novas_timescale_for_string("tai"), NOVAS_TAI, 1e-6)) n++;
  if(!is_equal("timescale_for_string:tt", novas_timescale_for_string("tt"), NOVAS_TT, 1e-6)) n++;
  if(!is_equal("timescale_for_string:et", novas_timescale_for_string("et"), NOVAS_TT, 1e-6)) n++;
  if(!is_equal("timescale_for_string:tcg", novas_timescale_for_string("tcg"), NOVAS_TCG, 1e-6)) n++;
  if(!is_equal("timescale_for_string:tcb", novas_timescale_for_string("tcb"), NOVAS_TCB, 1e-6)) n++;
  if(!is_equal("timescale_for_string:tdb", novas_timescale_for_string("tdb"), NOVAS_TDB, 1e-6)) n++;

  return n;
}

static int test_parse_timescale() {
  int n = 0;
  char *str = "UTC";
  char *tail = NULL;

  if(!is_equal("parse_timescale:UTC", novas_parse_timescale(str, &tail), NOVAS_UTC, 1e-6)) n++;
  if(!is_equal("parse_timescale:UTC:tail", (double) (tail - str), 3, 1e-6)) n++;
  if(!is_equal("parse_timescale:UTC:notail", novas_parse_timescale("UTC", NULL), NOVAS_UTC, 1e-6)) n++;
  if(!is_equal("parse_timescale:UTC:leading", novas_parse_timescale(" UTC", &tail), NOVAS_UTC, 1e-6)) n++;

  return n;
}

static int test_timestamp() {
  int n = 0;

  int i;
  novas_timespec time = NOVAS_TIMESPEC_INIT;
  char ts[40] = {'\0'};

  novas_set_time(NOVAS_TT, NOVAS_JD_J2000, 32, 0.0, &time);

  for(i = 0; i < NOVAS_TIMESCALES; i++) {
    novas_timespec time1 = NOVAS_TIMESPEC_INIT;
    char label[40];
    double jd;


    sprintf(label, "timestamp:%d", i);
    if(!is_ok(label, novas_timestamp(&time, i, ts, sizeof(ts)) < 0)) n++;

    jd = novas_parse_date(ts, NULL);
    novas_set_time(i, jd, 32, 0.0, &time1);

    sprintf(label, "timestamp:%d:check", i);
    if(!is_equal(label, novas_diff_time(&time1, &time), 0.0, 1e-3)) n++;

    sprintf(label, "timestamp:%d:truncate", i);
    if(!is_equal(label, novas_timestamp(&time, i, ts, 10), 9, 1e-6)) n++;
  }

  // Rounding a fraction of a ms before midnight....
  novas_set_split_time(NOVAS_TT, NOVAS_JD_J2000, 0.5 - 1e-4 / DAY, 32, 0.0, &time);
  if(!is_ok("timestamp:round", novas_timestamp(&time, NOVAS_TT, ts, sizeof(ts)) < 0)) n++;

  if(!is_ok("timestamp:round:check", strncmp("2000-01-02T", ts, 11))) {
    printf(" >>> got: %s', expected '2000-01-02'\n", ts);
    n++;
  }

  return n;
}

static int test_epoch() {
  int n = 0;

  if(!is_equal("epoch:icrs", novas_epoch("ICRS"), NOVAS_JD_J2000, 1e-8)) n++;
  if(!is_equal("epoch:icrs", novas_epoch("FK5"), NOVAS_JD_J2000, 1e-8)) n++;
  if(!is_equal("epoch:icrs", novas_epoch("FK4"), NOVAS_JD_B1950, 1e-8)) n++;
  if(!is_equal("epoch:icrs", novas_epoch("HIP"), NOVAS_JD_HIP, 1e-8)) n++;
  if(!is_equal("epoch:icrs", novas_epoch("J2000"), NOVAS_JD_J2000, 1e-8)) n++;
  if(!is_equal("epoch:icrs", novas_epoch("J2000.0"), NOVAS_JD_J2000, 1e-8)) n++;
  if(!is_equal("epoch:icrs", novas_epoch("2000"), NOVAS_JD_J2000, 1e-8)) n++;
  if(!is_equal("epoch:icrs", novas_epoch("B1950"), NOVAS_JD_B1950, 1e-8)) n++;
  if(!is_equal("epoch:icrs", novas_epoch("B1950.0"), NOVAS_JD_B1950, 1e-8)) n++;
  if(!is_equal("epoch:icrs", novas_epoch("1950"), NOVAS_JD_B1950, 1e-8)) n++;

  return n;
}

static int test_print_hms() {
  int n = 0;
  char snan[40] = {'\0'};
  char buf[40] = {'\0'};

  double h = novas_parse_hms("12:34:56.999999", NULL);

  sprintf(snan, "%f", (0.0 / 0.0));

  if(!is_ok("print_hms:nan", novas_print_hms(0.0 / 0.0, NOVAS_SEP_COLONS, 3, buf, sizeof(buf)) < 0)) n++;
  if(!is_ok("print_hms:nan:check", strcmp(buf, snan))) n++;


  if(!is_ok("print_hms:colons", novas_print_hms(h, NOVAS_SEP_COLONS, 3, buf, sizeof(buf)) < 0)) n++;
  if(!is_ok("print_hms:colons:check", strcmp(buf, "12:34:57.000"))) n++;

  if(!is_ok("print_hms:spaces", novas_print_hms(h, NOVAS_SEP_SPACES, 3, buf, sizeof(buf)) < 0)) n++;
  if(!is_ok("print_hms:spaces:check", strcmp(buf, "12 34 57.000"))) n++;

  if(!is_ok("print_hms:units", novas_print_hms(h, NOVAS_SEP_UNITS, 3, buf, sizeof(buf)) < 0)) n++;
  if(!is_ok("print_hms:units:check", strcmp(buf, "12h34m57.000s"))) n++;

  if(!is_ok("print_hms:units+spaces", novas_print_hms(h, NOVAS_SEP_UNITS_AND_SPACES, 3, buf, sizeof(buf)) < 0)) n++;
  if(!is_ok("print_hms:units+spaces:check", strcmp(buf, "12h 34m 57.000s"))) n++;

  if(!is_ok("print_hms:sep:-1", novas_print_hms(h, -1, 3, buf, sizeof(buf)) < 0)) n++;
  if(!is_ok("print_hms:sep:-1:check", strcmp(buf, "12:34:57.000"))) n++;

  if(!is_ok("print_hms:decimals:6", novas_print_hms(h, NOVAS_SEP_COLONS, 6, buf, sizeof(buf)) < 0)) n++;
  if(!is_ok("print_hms:decimals:6:check", strcmp(buf, "12:34:56.999999"))) n++;

  if(!is_ok("print_hms:decimals:10", novas_print_hms(h, NOVAS_SEP_COLONS, 10, buf, sizeof(buf)) < 0)) n++;
  if(!is_ok("print_hms:decimals:10:check", strcmp(buf, "12:34:56.999999000"))) n++;

  if(!is_ok("print_hms:decimals:0", novas_print_hms(h, NOVAS_SEP_COLONS, 0, buf, sizeof(buf)) < 0)) n++;
  if(!is_ok("print_hms:decimals:0:check", strcmp(buf, "12:34:57"))) n++;

  if(!is_ok("print_hms:decimals:-1", novas_print_hms(h, NOVAS_SEP_COLONS, -1, buf, sizeof(buf)) < 0)) n++;
  if(!is_ok("print_hms:decimals:-1:check", strcmp(buf, "12:34:57"))) n++;

  return n;
}


static int test_print_dms() {
  int n = 0;
  char snan[40] = {'\0'};
  char buf[40] = {'\0'};

  double deg = novas_parse_dms("120:34:56.999999", NULL);

  sprintf(snan, "%f", (0.0 / 0.0));

  if(!is_ok("print_dms:nan", novas_print_dms(0.0 / 0.0, NOVAS_SEP_COLONS, 3, buf, sizeof(buf)) < 0)) n++;
  if(!is_ok("print_dms:nan:check", strcmp(buf, snan))) n++;

  if(!is_ok("print_dms:colons", novas_print_dms(deg, NOVAS_SEP_COLONS, 3, buf, sizeof(buf)) < 0)) n++;
  if(!is_ok("print_dms:colons:check", strcmp(buf, " 120:34:57.000"))) n++;

  if(!is_ok("print_dms:spaces", novas_print_dms(deg, NOVAS_SEP_SPACES, 3, buf, sizeof(buf)) < 0)) n++;
  if(!is_ok("print_dms:spaces:check", strcmp(buf, " 120 34 57.000"))) n++;

  if(!is_ok("print_dms:units", novas_print_dms(deg, NOVAS_SEP_UNITS, 3, buf, sizeof(buf)) < 0)) n++;
  if(!is_ok("print_dms:units:check", strcmp(buf, " 120d34m57.000s"))) n++;

  if(!is_ok("print_dms:units+spaces", novas_print_dms(deg, NOVAS_SEP_UNITS_AND_SPACES, 3, buf, sizeof(buf)) < 0)) n++;
  if(!is_ok("print_dms:units+spaces:check", strcmp(buf, " 120d 34m 57.000s"))) n++;

  if(!is_ok("print_dms:sep:-1", novas_print_dms(deg, -1, 3, buf, sizeof(buf)) < 0)) n++;
  if(!is_ok("print_dms:sep:-1:check", strcmp(buf, " 120:34:57.000"))) n++;

  if(!is_ok("print_dms:decimals:6", novas_print_dms(deg, NOVAS_SEP_COLONS, 6, buf, sizeof(buf)) < 0)) n++;
  if(!is_ok("print_dms:decimals:6:check", strcmp(buf, " 120:34:56.999999"))) n++;

  if(!is_ok("print_dms:decimals:10", novas_print_dms(deg, NOVAS_SEP_COLONS, 10, buf, sizeof(buf)) < 0)) n++;
  if(!is_ok("print_dms:decimals:10:check", strcmp(buf, " 120:34:56.999999000"))) n++;

  if(!is_ok("print_dms:decimals:0", novas_print_dms(deg, NOVAS_SEP_COLONS, 0, buf, sizeof(buf)) < 0)) n++;
  if(!is_ok("print_dms:decimals:0:check", strcmp(buf, " 120:34:57"))) n++;

  if(!is_ok("print_dms:decimals:-1", novas_print_dms(deg, NOVAS_SEP_COLONS, -1, buf, sizeof(buf)) < 0)) n++;
  if(!is_ok("print_dms:decimals:-1:check", strcmp(buf, " 120:34:57"))) n++;

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
  if(test_grav_planets()) n++;
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

  if(test_hms_hours()) n++;
  if(test_dms_degrees()) n++;
  if(test_parse_degrees()) n++;
  if(test_parse_hours()) n++;
  if(test_str_degrees()) n++;
  if(test_str_hours()) n++;

  if(test_hpa()) n++;
  if(test_epa()) n++;
  if(test_helio_dist()) n++;
  if(test_solar_power()) n++;
  if(test_solar_illum()) n++;
  if(test_equ_sep()) n++;
  if(test_object_sep()) n++;
  if(test_h2e_offset()) n++;

  if(test_frame_lst()) n++;
  if(test_rise_set()) n++;
  if(test_transit_time()) n++;
  if(test_equ_track()) n++;
  if(test_hor_track()) n++;
  if(test_xyz_to_uvw()) n++;
  if(test_sun_moon_angle()) n++;
  if(test_unwrap_angles()) n++;
  if(test_lsr_vel()) n++;
  if(test_parse_date()) n++;
  if(test_parse_iso_date()) n++;
  if(test_parse_date_format()) n++;
  if(test_date()) n++;
  if(test_date_scale()) n++;
  if(test_iso_timestamp()) n++;
  if(test_timestamp()) n++;
  if(test_timescale_for_string()) n++;
  if(test_parse_timescale()) n++;
  if(test_julian_date()) n++;
  if(test_jd_to_date()) n++;
  if(test_jd_from_date()) n++;

  if(test_epoch()) n++;

  if(test_print_hms()) n++;
  if(test_print_dms()) n++;

  n += test_dates();

  return n;
}
