/**
 * @date Created  on Sep 5, 2025
 * @author Attila Kovacs
 *
 *   Creates headless Doxyfile.local and README-orig.md variants
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define DOXYGEN_STRING    "resources/header.html"

#if defined _WIN32 || defined __CYGWIN__
#  define PATH_SEP  "\\"
#else
#  define PATH_SEP  "/"
#endif

static const char *path = ".";

static FILE *openfile(const char *name, const char *mode) {
  char filename[1024] = {'\0'};
  FILE *fp;

  snprintf(filename, sizeof(filename), "%s" PATH_SEP "%s", path, name);
  fp = fopen(filename, mode);
  if(!fp) fprintf(stderr, "ERROR! opening %s: %s", filename, strerror(errno));
  return fp;
}

static int make_local_doxyfile() {
  FILE *in, *out;
  char line[16384] = {'\0'};

  in = openfile("Doxyfile", "r");
  if(!in) return -1;

  out = openfile("Doxyfile.local", "w");
  if(!out) {
    fclose(in);
    return -1;
  }

  while(fgets(line, sizeof(line) - 1, in) != NULL) {
    char *match = strstr(line, DOXYGEN_STRING);
    if(match) {
      memset(match, ' ', sizeof(DOXYGEN_STRING) - 1);
    }

    fwrite(line, strlen(line), 1, out);
  }

  fclose(out);
  fclose(in);

  return 0;
}

static int make_headless_readme() {
  FILE *in, *out;
  char line[16384] = {'\0'};
  int head = 1;

  in = openfile("README.md", "r");
  if(!in) return -1;

  out = openfile("README-orig.md", "w");
  if(!out) {
    fclose(in);
    return -1;
  }

  while(fgets(line, sizeof(line) - 1, in) != NULL) {
    if(head) {
      if(line[0] != '#') continue;
      head = 0;
    }
    fwrite(line, strlen(line), 1, out);
  }

  fclose(out);
  fclose(in);

  return 0;
}

// Syntax: docedit [path]
int main(int argc, const char *argv[]) {
  int nerr = 0;

  if(argc > 1)
    path = argv[1];

  if(make_local_doxyfile() != 0) nerr++;
  if(make_headless_readme() != 0) nerr++;

  return nerr;
}
