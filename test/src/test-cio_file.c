/**
 * @date Created  on Feb 16, 2024
 * @author Attila Kovacs
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <string.h>

#include "novas.h"

#define J2000   2451545.0

static double tdb = J2000;

static FILE *fp;

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
}

static int is_ok(int error) {
  if(error) fprintf(fp, "ERROR %d ", error);
  return !error;
}

static void test_cio_location() {
  double h = 0.0;
  short sys = -1;

  openfile("cio_location.file");

  if(is_ok(cio_location(tdb, 0, &h, &sys)))
    fprintf(fp, "%12.3f: %d %12.9f ", (tdb - J2000), sys, h);

  // repeat.
  if(is_ok(cio_location(tdb, 0, &h, &sys)))
    fprintf(fp, "%12.3f: %d %12.9f ", (tdb - J2000), sys, h);

}

int main() {
  int i;

  for(i = 0; i < 3; i++) {
    tdb = J2000 + (i-1) * 10000.0;
    test_cio_location();
  }

  return 0;
}
