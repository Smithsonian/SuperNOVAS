/**
 * @file
 *
 * @date Created  on Jan 26, 2025
 * @author Attila Kovacs
 */


/**
 * @file
 *
 * @date Created  on Jan 24, 2025
 * @author Attila Kovacs
 */

#define _POSIX_C_SOURCE 199309L   ///< for clock_gettime()

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <novas.h>      ///< SuperNOVAS functions and definitions

#define  LEAP_SECONDS     37        ///< [s] current leap seconds from IERS Bulletin C
#define  DUT1             0.114     ///< [s] current UT1 - UTC time difference from IERS Bulletin A




int main(int argc, const char *argv[]) {              // observer location
  // Other variables we need ----------------------------------------------->
  int i, N = 1000;
  double tjd = 2460683.132905, dx, dy;
  struct timespec unix_time, end;

  // -------------------------------------------------------------------------
  // Start benchmarks...
  fprintf(stderr, "Starting single-thread benchmarks...\n");

  // -------------------------------------------------------------------------
  // Benchmark reduced accuracy, place(), same time
  clock_gettime(CLOCK_REALTIME, &unix_time);
  for(i = 0; i < N; i++) iau2000a(tjd + i * 0.01, 0.0, &dx, &dy);
  clock_gettime(CLOCK_REALTIME, &end);
  printf(" - iau2000a:   %12.1f nutations/sec\n",
          N / (end.tv_sec - unix_time.tv_sec + 1e-9 * (end.tv_nsec - unix_time.tv_nsec)));

  // -------------------------------------------------------------------------
  // Benchmark reduced accuracy, place(), different times()
  clock_gettime(CLOCK_REALTIME, &unix_time);
  for(i = 0; i < N; i++)   for(i = 0; i < N; i++) iau2000b(tjd + i * 0.01, 0.0, &dx, &dy);
  clock_gettime(CLOCK_REALTIME, &end);
  printf(" - iau2000b:   %12.1f positions/sec\n",
          N / (end.tv_sec - unix_time.tv_sec + 1e-9 * (end.tv_nsec - unix_time.tv_nsec)));

  // -------------------------------------------------------------------------
  // Benchmark reduced accuracy, place(), different times()
  clock_gettime(CLOCK_REALTIME, &unix_time);
  for(i = 0; i < N; i++)   for(i = 0; i < N; i++) nu2000k(tjd + i * 0.01, 0.0, &dx, &dy);
  clock_gettime(CLOCK_REALTIME, &end);
  printf(" - nu2000k:    %12.1f positions/sec\n",
          N / (end.tv_sec - unix_time.tv_sec + 1e-9 * (end.tv_nsec - unix_time.tv_nsec)));

  return 0;
}
