/**
 * @file
 *
 * @date Created  on Sep 4, 2025
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

int main() {
  ra_of_cio x[5];
  int n = 0;

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
