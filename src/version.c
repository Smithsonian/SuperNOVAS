/**
 * Utility probgram that simply prints the version to the standard output
 *
 * @author: Attila Kovacs
 * @since 1.0.2
 */

#include <stdio.h>
#include <string.h>

#include "novas.h"

int main(int argc, const char *argv[]) {
  if (argc > 1) {
    const char *arg = argv[1];
    if (strcmp("major", arg) == 0) printf("%d\n", SUPERNOVAS_MAJOR_VERSION);
    else if (strcmp("minor", arg) == 0) printf("%d\n", SUPERNOVAS_MINOR_VERSION);
    else if (strcmp("patch", arg) == 0) printf("%d\n", SUPERNOVAS_PATCHLEVEL);
    else if (strcmp("release", arg) == 0) printf("%s\n", SUPERNOVAS_RELEASE_STRING);
    else return 1;
    return 0;
  }

  printf("%s\n", SUPERNOVAS_VERSION_STRING);
  return 0;
}
