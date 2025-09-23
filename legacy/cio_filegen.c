/*
 * @date Created  on Sep 23, 2025
 * @author Attila Kovacs
 *
 *  This program can generate CIO_RA.TXT, a slightly obsoleted version of which was included in
 *  the NOVAS C 3.1 distribution. SuperNOVAS no longer needs or uses CIO locator data files,
 *  hence, this program is provided only as a way to generate the lookup table if needed for
 *  legacy applications.
 *
 *  Link with:
 *
 *    -lsupernovas -lm
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "novas.h"

#define DEFAULT_FILENAME          "CIO_RA.TXT"
#define GENERATE_FROM_JD          2341951.4   ///< [day] TDB-based Julian day of start
#define GENERATE_TO_JD            2561139.0   ///< [day] TDB-based Julian day of start
#define GENERATE_STEP         1.2         ///< [day]

static int usage(int retval) {
  printf("cio_filegen -- Generates CIO vs GCRS locator data for (Super)NOVAS.\n\n");
  printf("Syntax: cio_filegen [--from <JD>] [--to <JD>] [--step <days>] <filename>\n\n");
  printf("   <filename>       Output file name/path, e.g. 'CIO_RA.TXT'.\n\n");
  printf("Options:\n\n");
  printf("   --from <JD>      Starting Julian date for table (%.1f)\n", GENERATE_FROM_JD);
  printf("   --to <JD>        Ending (exclusive) Julian date for table (%.1f)\n", GENERATE_TO_JD);
  printf("   --step <days>    Starting Julian date for table (%.1f)\n", GENERATE_STEP);
  printf("\n");

  return retval;
}

static double parse_double_arg(int argc, const char *argv[], int i) {
  double val = NAN;

  if(i++ >= argc) {
    fprintf(stderr, "ERROR! Missing option argument.\n");
    return NAN;
  }
  errno = 0;
  val = strtod(argv[i], NULL);
  if(errno) {
    fprintf(stderr, "ERROR! %s: '%s'\n", strerror(errno), argv[i]);
    return NAN;
  }

  return val;
}

int main(int argc, const char *argv[]) {
  const char *filename = DEFAULT_FILENAME;
  double from_jd = GENERATE_FROM_JD;
  double to_jd = GENERATE_TO_JD;
  double step = GENERATE_STEP;

  FILE *fp;
  long i = 1, n;

  if(argc < 2) return usage(1);

  for(i = 1; i < argc; i++) {
    if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "help") == 0) return usage(0);
    if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-?") == 0) return usage(0);

    if(strcmp(argv[i], "--from") == 0) {
      from_jd = parse_double_arg(argc, argv, ++i);
      if(isnan(from_jd)) return 1;
    }
    else if(strcmp(argv[i], "--to") == 0) {
      to_jd = parse_double_arg(argc, argv, ++i);
      if(isnan(to_jd)) return 1;
    }
    else if(strcmp(argv[i], "--step") == 0) {
      step = parse_double_arg(argc, argv, ++i);
      if(isnan(step)) return 1;
    }
    else if(argv[i][0] == '-') {
      fprintf(stderr, "ERROR! Invalid option argument: %s\n", argv[i]);
      return 2;
    }
    else {
      filename = argv[i];
    }
  }

  filename = argv[1];

  n = (long) ceil((to_jd - from_jd) / step);
  printf("Generating %s. It may take a while...\n", filename);

  fp = fopen(filename, "w");
  if(!fp) {
    fprintf(stderr, "ERROR! opening %s: %s\n", filename, strerror(errno));
    return 1;
  }

  fprintf(fp, "CIO RA P03  @ %.3fd\n", step);

  for(i = 0; ; i++) {
    double jd = from_jd + (i * step);
    double pos[3] = {0.0};
    double RA;

    if(jd > to_jd) break;

    // CIO's R.A. in TOD.
    RA = -ira_equinox(jd, NOVAS_TRUE_EQUINOX, NOVAS_FULL_ACCURACY);
    radec2vector(RA, 0.0, 1.0, pos);

    // TOD -> GCRS
    tod_to_gcrs(jd, NOVAS_FULL_ACCURACY, pos, pos);

    // Get the GCRS R.A. of the CIO
    vector2radec(pos, &RA, NULL);

    fprintf(fp, "%16.6f  %22.14f\n", jd, remainder(RA, 24.0) * NOVAS_HOURANGLE / NOVAS_ARCSEC);

    if(i % 100 == 0) {
      printf(" [%3d%%]\r", (int) floor((100.0 * i) / n));
      fflush(stdout);
    }
  }

  printf(" [100%%]\n");

  fclose(fp);
  return 0;
}
