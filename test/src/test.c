/**
 * @file
 *
 * @date Created  on Jan 30, 2024
 * @author Attila Kovacs
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <string.h>

#include "novas.h"

#define DEGREE  (M_PI / 180.0)
#define ARCSEC  (DEGREE / 3600.0)
#define HOURANGLE (M_PI / 12.0)
#define AU      NOVAS_AU
#define J2000   2451545.0

static observer obs;
static object source;
static double tdb = J2000;
static short accuracy = 1;
static double ut12tt = 69.0;

// Initialized quantities.
static double lst, pos0[3], vel0[3], epos[3], evel[3], pobs[3], vobs[3];

static FILE *fp;
static int idx = -1;


static void newline() {
  fprintf(fp, "\n%8.1f %-10s S%d O%d A%d: ", (tdb - J2000), source.name, source.type, obs.where, accuracy);
}

static void openfile(const char *name) {
  char filename[100] = {};

  //if(idx >= 0) sprintf(filename, "data/%02d-%s.out", idx++, name);
  sprintf(filename, "data/%s.out", name);

  if(fp) {
    fprintf(fp, "\n");
    fclose(fp);
  }
  fp = fopen(filename, "a");
  if(!fp) {
    perror("opening output file");
    exit(errno);
  }

  if(idx >= 0) newline();
}


static void printvector(double *v) {
  if(!v) fprintf(fp, "null ");
  else fprintf(fp, "%12.6f %12.6f %12.6f ", v[0], v[1], v[2]);
}

static int is_ok(int error) {
  if(error) fprintf(fp, "ERROR %d", error);
  return !error;
}


// ======================================================================================
// Basic functions
// ======================================================================================


static void test_make_cat_entry() {
  cat_entry star;

  openfile("make_cat_entry");
  if(is_ok(make_cat_entry("Test", "TST", 1001, 1.1, -2.2, 3.3, -4.4, 5.5, -6.6, &star))) {
    fprintf(fp, "%s %s %ld %.3f %.3f %.3f %.3f %.3f %.3f ", star.starname, star.catalog, star.starnumber,
            star.ra, star.dec, star.promodec, star.promodec, star.parallax, star.radialvelocity);
  }

}


static void test_make_object() {
  object object;
  cat_entry star;

  openfile("make_object");

  if(!is_ok(make_object(0, 3, "Earth", NULL, &object))) return;
  fprintf(fp, "%-10s %d %ld\n", object.name, object.type, object.number);

  if(!is_ok(make_object(1, 501, "Io", NULL, &object))) return;
  fprintf(fp, "%-10s %d %ld\n", object.name, object.type, object.number);

  if(!is_ok(make_cat_entry("Test", "TST", 1001, 1.1, -2.2, 3.3, -4.4, 5.5, -6.6, &star))) return;
  if(!is_ok(make_object(2, 1234567890, star.starname, &star, &object))) return;
  fprintf(fp, "%-10s %d %ld\n", object.name, object.type, object.number);
}


static void test_basics() {
  idx = -1;
  test_make_cat_entry();
  test_make_object();
}


// ======================================================================================
// Time specific
// ======================================================================================

static void test_ephemeris() {
  double pos1[3], vel1[3], tdb2[2] = {tdb};
  object body[2];
  int i, j;

  if(!is_ok(make_object(0, 10, "Sun", NULL, &body[0]))) return;
  if(!is_ok(make_object(0, 3, "Earth", NULL, &body[1]))) return;

  openfile("ephemeris");

  for(i = 0; i < 2; i++) for(j = 0; j < 2; j++) {
    if(is_ok(ephemeris(tdb2, &body[i], j, accuracy, pos1, vel1))) {
      fprintf(fp, "%-10s %d ", body[i].name, j);
      printvector(pos1);
      printvector(vel1);
      fprintf(fp, "\n");
    }
  }
}


static void test_era() {
  openfile("era");
  fprintf(fp, "%12.6f", era(tdb, 0.0));
}

static void test_ee_ct() {
  openfile("ee_ct");


  fprintf(fp, "%12.6f", ee_ct(tdb, 0.0, accuracy) / ARCSEC);
}

static void test_iau2000a() {
  double dpsi = 0.0, deps = 0.0;
  openfile("iau2000a");

  iau2000a(tdb, 0.0, &dpsi, &deps);
  fprintf(fp, "%12.6f %12.6f ", dpsi, deps);

  iau2000a(tdb + 100000, 0.0, &dpsi, &deps);
  fprintf(fp, "%12.6f %12.6f ", dpsi, deps);
}

static void test_iau2000b() {
  double dpsi = 0.0, deps = 0.0;
  openfile("iau2000b");

  iau2000b(tdb, 0.0, &dpsi, &deps);
  fprintf(fp, "%12.6f %12.6f ", dpsi, deps);

  iau2000b(tdb + 100000, 0.0, &dpsi, &deps);
  fprintf(fp, "%12.6f %12.6f ", dpsi, deps);
}

static void test_nu2000k() {
  double dpsi = 0.0, deps = 0.0;
  openfile("nu2000k");

  nu2000k(tdb, 0.0, &dpsi, &deps);
  fprintf(fp, "%12.6f %12.6f ", dpsi, deps);

  nu2000k(tdb + 100000, 0.0, &dpsi, &deps);
  fprintf(fp, "%12.6f %12.6f ", dpsi, deps);
}

static void test_nutation_angles() {
  double t = (tdb - J2000) / 36525.0;
  double dpsi = 0.0, deps = 0.0;

  openfile("nutation_angles");

  nutation_angles(t, accuracy, &dpsi, &deps);
  fprintf(fp, "%12.6f %12.6f", dpsi, deps);

  nutation_angles(t + 100.0, accuracy, &dpsi, &deps);
  fprintf(fp, "%12.6f %12.6f", dpsi, deps);
}


static void test_e_tilt() {
  double r1=0.0, r2=0.0, r3=0.0, r4=0.0, r5=0.0;
  openfile("e_tilt");
  e_tilt(tdb, accuracy, &r1, &r2, &r3, &r4, &r5);
  fprintf(fp, "%12.6f %12.6f %12.6f %12.6f %12.6f", r1, r2, r3, r4, r5);
}

static void test_nutation() {
  double pos1[3];
  openfile("nutation");
  nutation(tdb, 0, accuracy, pos0, pos1);
  printvector(pos1);
}

static void test_ira_equinox() {
  openfile("ira_equinox");
  fprintf(fp, "%12.9f %12.9f", ira_equinox(tdb, 0, accuracy), ira_equinox(tdb, 1, accuracy));
}

static void test_cio_location() {
  double h = 0.0;
  short sys = -1;
  openfile("cio_location");
  if(is_ok(cio_location(tdb, accuracy, &h, &sys)))
    fprintf(fp, "%d %12.9f", sys, h);
}

static void test_cio_basis() {
  double h = 0.0, x[3], y[3], z[3];
  short sys = -1;

  openfile("cio_basis");

  if(!is_ok(cio_location(tdb, accuracy, &h, &sys))) return;
  if(!is_ok(cio_basis(tdb, h, sys, accuracy, x, y, z))) return;

  printvector(x);
  printvector(y);
  printvector(z);
}

static void test_sidereal_time() {
  double h = 0.0;
  openfile("sidereal_time");
  if(!is_ok(sidereal_time(tdb, 0.0, ut12tt, 0, 0, accuracy, &h))) return;
  fprintf(fp, "%12.6f ", h);

  if(!is_ok(sidereal_time(tdb, 0.0, ut12tt, 0, 1, accuracy, &h))) return;
  fprintf(fp, "%12.6f", h);
}

static void test_geo_posvel() {
  double pos1[3], vel1[3];
  openfile("geo_posvel");
  if(is_ok(geo_posvel(tdb, ut12tt, accuracy, &obs, pos1, vel1))) {
    printvector(pos1);
    printvector(vel1);
  }
}

static void test_time_specific() {
  idx = -1;
  test_ephemeris();
  test_era();
  test_ee_ct();
  test_iau2000a();
  test_iau2000b();
  test_nu2000k();
  test_nutation_angles();
  test_e_tilt();
  test_nutation();
  test_ira_equinox();
  test_cio_location();
  test_cio_basis();
  test_sidereal_time();
  test_geo_posvel();
}


// ======================================================================================
// Source/time specific
// ======================================================================================


static int init() {
  object earth;
  double tdb2[2] = { tdb };

  memset(pos0, 0, sizeof(pos0));
  memset(vel0, 0, sizeof(vel0));
  memset(epos, 0, sizeof(epos));
  memset(evel, 0, sizeof(evel));
  memset(pobs, 0, sizeof(pobs));
  memset(vobs, 0, sizeof(vobs));

  lst = 0.0;

  if(source.type == 0) {
    int error = ephemeris(tdb2, &source, 0, accuracy, pos0, vel0);
    if(error) {
      fprintf(stderr, "init: Failed source ephemeris for %s: error %d\n", source.name, error);
      return -1;
    }
  }
  else if(source.type == 2) {
    starvectors(&source.star, pos0, vel0);
  }
  else {
    fprintf(stderr, "init: invalid source type %d\n", source.type);
    return -1;
  }

  fprintf(fp, "SOU ");
  printvector(pos0);
  printvector(vel0);
  newline();

  if(make_object(0, 3, "Earth", NULL, &earth) != 0) {
    fprintf(stderr, "init: Failed make_object(Earth)\n");
    return -1;
  }
  if(ephemeris(tdb2, &earth, 0, accuracy, epos, evel) != 0) {
    fprintf(stderr, "init: Failed Earth ephemeris\n");
    return -1;
  }

  fprintf(fp, "EAR ");
  printvector(epos);
  printvector(evel);
  newline();

  if(sidereal_time(tdb, 0.0, ut12tt, 0, 1, accuracy, &lst) != 0) {
    fprintf(stderr, "init: Failed sidereal_time\n");
    return -1;
  }

  fprintf(fp, "LST ");
  fprintf(fp, "%12.6f", lst);
  newline();

  if(obs.where == 1) {
    int i;

    terra(&obs.on_surf, lst, pobs, vobs);

    for(i = 0; i < 3; i++) {
      pobs[i] += epos[i];
      vobs[i] += evel[i];
    }
  }
  else if (obs.where == 2) {
    int i;
    for(i = 0; i < 3; i++) {
      pobs[i] = epos[i] + obs.near_earth.sc_pos[i];
      vobs[i] = evel[i] + obs.near_earth.sc_vel[i];
    }
  }

  fprintf(fp, "OBS ");
  printvector(pobs);
  printvector(vobs);

  return 0;
}

static void test_frame_tie() {
  double pos1[3];

  openfile("frame_tie");
  frame_tie(pos0, -1, pos1);
  printvector(pos1);
}



static void test_wobble() {
  double pos1[3];

  openfile("wobble");

  wobble(tdb, 0, 2.0, -3.0, pos0, pos1);
  printvector(pos1);
}


static void test_precession() {
  double pos1[3];
  openfile("precession");

  if(is_ok(precession(tdb, pos0, J2000, pos1)))
    printvector(pos1);
}


static void test_light_time() {
  double pos1[3], t = 0.0;

  openfile("light_time");

  if(is_ok(light_time(tdb, &source, pobs, 0.0, accuracy, pos1, &t)))
    fprintf(fp, "%12.6f", t);
}

static void test_grav_def() {
  double pos1[3];
  openfile("grav_def");

  if(is_ok(grav_def(tdb, obs.where, accuracy, pos0, pobs, pos1))) {
    printvector(pos1);
  }
}

static void test_place() {
  int i;

  openfile("place");

  for(i=0; i < 4; i++) {
    sky_pos out;
    if(is_ok(place(tdb, &source, &obs, ut12tt, i, accuracy, &out))) {
      fprintf(fp, "%d %12.6f %12.6f", i, out.dis, out.rv);
      newline();
    }
  }
}


static int test_source() {
  openfile("init");

  idx = 0;

  if(init() != 0) return -1;

  test_frame_tie();
  test_wobble();
  test_precession();
  test_light_time();
  test_grav_def();
  test_place();

  return 0;
}

static int test_observers() {
  double ps[3] = { 100.0, 30.0, 10.0 }, vs[3] = { 10.0 };

  make_observer_at_geocenter(&obs);
  if(test_source() != 0) return -1;

  make_observer_on_surface(20.0, -15.0, 0.0, 0.0, 1000.0, &obs);
  if(test_source() != 0) return -1;

  make_observer_in_space(ps, vs, &obs);
  if(test_source() != 0) return -1;

  return 0;
}

static int test_all() {
  cat_entry star;

  printf(" Testing date %.1f\n", tdb);

  make_cat_entry("22+20", "TST", 1001, 22.0, 20.0, 3.0, -2.0, 5.0, 10.0, &star);
  if(make_object(2, star.starnumber, star.starname, &star, &source) != 0) return -1;
  if(test_observers() != 0) return -1;

  make_cat_entry("16-20", "TST", 1001, 16.0, -20.0, 3.0, -2.0, 5.0, 10.0, &star);
  if(make_object(2, star.starnumber, star.starname, &star, &source) != 0) return -1;
  if(test_observers() != 0) return -1;

  if(make_object(0, 10, "Sun", &star, &source) != 0) return -1;
  if(test_observers() != 0) return -1;

  return 0;
}

static int test_dates() {
  int i;

  for(i = 0; i < 3; i++) {
    tdb = J2000 + (i-1) * 10000.0;

    test_time_specific();

    if(test_all() != 0) {
      printf(" -- FAILED!\n");
      return -1;
    }
  }

  printf(" -- OK\n");
  return 0;
}


int main(int argc, char *argv[]) {
  test_basics();
  test_dates();
  fclose(fp);
}
