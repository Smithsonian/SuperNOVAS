/**
 * @file
 *
 * @date Created  on Feb 18, 2024
 * @author Attila Kovacs
 */

#define _XOPEN_SOURCE 500           /// strdup()


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <string.h>

#define __NOVAS_INTERNAL_API__      ///< Use definitions meant for internal use by SuperNOVAS only
#define USE_CSPICE 1               ///< Use cspice routines
#include "novas.h"
#include "solarsystem.h"

#include "cspice/SpiceUsr.h"
#include "cspice/SpiceZpr.h"        // for reset_c

#define PLANET_EPH                  "de440s-j2000.bsp"
#define MARS_EPH                    "mar097-j2000.bsp"

static const char *prefix;

static int usage() {
  fprintf(stderr, " Syntax: test-cspice <ephem-path>\n\n");
  fprintf(stderr, "   <ephem-path>   Path to de440s.bsp and mar097.bsp containing J2000 data.\n\n");
  exit(1);
}

static int check_equal_pos(const double *posa, const double *posb, double tol) {
  int i;

  tol = fabs(tol);
  if(tol < 1e-30) tol = 1e-30;

  for(i = 0; i < 3; i++) {
    if(fabs(posa[i] - posb[i]) <= tol) continue;
    if(isnan(posa[i]) && isnan(posb[i])) continue;

    fprintf(stderr, "  A[%d] = %.9g vs B[%d] = %.9g\n", i, posa[i], i, posb[i]);
    return i + 1;
  }

  return 0;
}

static int is_ok(const char *func, int error) {
  if(error) fprintf(stderr, "ERROR %d! %s\n", error, func);
  return !error;
}

static int check(const char *func, int exp, int error) {
  if(error != exp) {
    fprintf(stderr, "ERROR! %s: expected %d, got %d\n", func, exp, error);
    return 1;
  }
  return 0;
}


static int test_cspice() {
  double pos[3], vel[3], pos0[3], vel0[3];
  double jd = NOVAS_JD_J2000;
  double jd2[2] = { jd, 0.0 };

  char filename[1024];
  object earth, mars;

  make_planet(NOVAS_EARTH, &earth);
  make_planet(NOVAS_MARS, &mars);

  if(!is_ok("use_cspice", novas_use_cspice() != 0)) return 1;

  if(!is_ok("cspice:earth", ephemeris(jd2, &earth, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pos, vel))) return 1;
  earth_sun_calc(jd, NOVAS_EARTH, NOVAS_BARYCENTER, pos0, vel0);

  if(!is_ok("cspice:earth:pos", check_equal_pos(pos, pos0, 1e-5))) return 1;
  if(!is_ok("cspice:earth:vel", check_equal_pos(vel, vel0, 1e-5))) return 1;

  if(!is_ok("cspice_planet:mars", ephemeris(jd2, &mars, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pos0, vel0))) return 1;



  return 0;
}

static int test_cspice_planet() {
  double pos[3], vel[3], pos0[3] = {}, vel0[3] = {};
  double jd = NOVAS_JD_J2000;
  double jd2[2] = { jd, 0.0 };

  char filename[1024];
  object ssb, sun, earth, moon, mars, phobos;

  make_planet(NOVAS_SSB, &ssb);
  make_planet(NOVAS_SUN, &sun);
  make_planet(NOVAS_EARTH, &earth);
  make_planet(NOVAS_MOON, &moon);
  make_planet(NOVAS_MARS, &mars);
  make_ephem_object("Phobos", 401, &phobos);

  if(!is_ok("use_cspice", novas_use_cspice() != 0)) return 1;

  if(!is_ok("cspice_planet:ssb", ephemeris(jd2, &ssb, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pos, vel))) return 1;
  if(!is_ok("cspice_planet:ssb:pos", check_equal_pos(pos, pos0, 1e-5))) return 1;

  if(!is_ok("cspice_planet:sun_vs_sun", ephemeris(jd2, &sun, NOVAS_HELIOCENTER, NOVAS_REDUCED_ACCURACY, pos, vel))) return 1;
  if(!is_ok("cspice_planet:sun_vs_sun:pos", check_equal_pos(pos, pos0, 1e-5))) return 1;

  if(!is_ok("cspice_planet:sun", ephemeris(jd2, &sun, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pos, vel))) return 1;
  earth_sun_calc(jd, NOVAS_SUN, NOVAS_BARYCENTER, pos0, vel0);
  if(!is_ok("cspice_planet:sun:pos", check_equal_pos(pos, pos0, 1e-5))) return 1;

  if(!is_ok("cspice_planet:earth", ephemeris(jd2, &earth, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pos, vel))) return 1;
  earth_sun_calc(jd, NOVAS_EARTH, NOVAS_BARYCENTER, pos0, vel0);

  if(!is_ok("cspice_planet:earth:pos", check_equal_pos(pos, pos0, 1e-5))) return 1;
  if(!is_ok("cspice_planet:earth:vel", check_equal_pos(vel, vel0, 1e-5))) return 1;

  if(!is_ok("cspice_planet:moon", ephemeris(jd2, &moon, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pos, vel))) return 1;
  earth_sun_calc(jd, NOVAS_MOON, NOVAS_BARYCENTER, pos0, vel0);

  if(!is_ok("cspice_planet:moon:pos", check_equal_pos(pos, pos0, 1e-2))) return 1;
  if(!is_ok("cspice_planet:moon:vel", check_equal_pos(vel, vel0, 1e-3))) return 1;

  if(!is_ok("cspice_planet:mars", ephemeris(jd2, &mars, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pos0, vel0))) return 1;
  if(!is_ok("cspice_planet:phobos", ephemeris(jd2, &phobos, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pos, vel))) return -1;

  if(!is_ok("cspice_planet:mars-phobos:pos", check_equal_pos(pos, pos0, 1e-4))) return 1;

  phobos.number = -1;
  if(!is_ok("cspice_planet:phobos:byname", ephemeris(jd2, &phobos, NOVAS_BARYCENTER, NOVAS_REDUCED_ACCURACY, pos0, vel0))) return -1;
  if(!is_ok("cspice_planet:phobos:match", check_equal_pos(pos, pos0, 1e-6))) return 1;


  return 0;
}


static int test_errors() {
  double pos[3], vel[3];
  double jd = NOVAS_JD_J2000;
  double jd2[2] = { jd, 0.0 };
  int n = 0;

  object earth, phobos;
  novas_planet_provider_hp pl = get_planet_provider_hp();
  novas_ephem_provider eph = get_ephem_provider();

  make_planet(NOVAS_EARTH, &earth);
  make_ephem_object("Phobos", 401, &phobos);

  if(check("errors:tdb", -1, pl(NULL, NOVAS_MARS, NOVAS_BARYCENTER, pos, vel))) n++;
  if(check("errors:planet:number:-1", 1, pl(jd2, -1, NOVAS_BARYCENTER, pos, vel))) n++;
  if(check("errors:planet:number:hi", 1, pl(jd2, NOVAS_PLANETS, NOVAS_BARYCENTER, pos, vel))) n++;
  if(check("errors:planet:origin", 2, pl(jd2, NOVAS_MARS, -1, pos, vel))) n++;
  if(check("errors:body:name:NULL", -1, eph(NULL, -1, jd2[0], jd2[1], NOVAS_BARYCENTER, pos, vel))) n++;
  if(check("errors:body:name:empty", -1, eph("", -1, jd2[0], jd2[1], NOVAS_BARYCENTER, pos, vel))) n++;
  if(check("errors:body:name:nomatch", 1, eph("blah", -1, jd2[0], jd2[1], NOVAS_BARYCENTER, pos, vel))) n++;

  jd2[0] = -999999.0;

  if(check("errors:planet:time", 3, pl(jd2, NOVAS_MARS, NOVAS_BARYCENTER, pos, vel))) n++;
  if(check("errors:body:time", 3, eph("phobos", 401, jd2[0], jd2[1], NOVAS_BARYCENTER, pos, vel))) n++;

  return n;
}



static void load_eph(const char *name) {
  char filename[1024];

  sprintf(filename, "%s/%s", prefix, name);
  reset_c();
  furnsh_c(filename);
  if(return_c()) {
    fprintf(stderr, "ERROR! furnsh_c() failed for %s\n", name);
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  int n = 0;

  if(argc < 2) usage();

  prefix = strdup(argv[1]);

  load_eph(PLANET_EPH);
  load_eph(MARS_EPH);

  enable_earth_sun_hp(1);

  if(test_cspice()) n++;
  if(test_cspice_planet()) n++;

  novas_debug(NOVAS_DEBUG_OFF);
  if(test_errors()) n++;

  return n;
}
