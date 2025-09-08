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
#define OUTPATH_STRING    "apidoc"

#if defined _WIN32 || defined __CYGWIN__
#  define PATH_SEP  "\\"
#else
#  define PATH_SEP  "/"
#endif

static const char *inpath = ".";
static const char *outpath = NULL;

static FILE *openfile(const char *path, const char *name, const char *mode) {
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

  in = openfile(inpath, "Doxyfile", "r");
  if(!in) return -1;

  out = openfile(outpath ? outpath : inpath, "Doxyfile.local", "w");
  if(!out) {
    fclose(in);
    return -1;
  }

  while(fgets(line, sizeof(line) - 1, in) != NULL) {
    char *match = strstr(line, DOXYGEN_STRING);

    if(match) {
      memset(match, ' ', sizeof(DOXYGEN_STRING) - 1);
    }

    else if(outpath) {
      match = strstr(line, OUTPATH_STRING);
      if(match) {
        char *rem = &match[sizeof(OUTPATH_STRING) - 1];
        memmove(&match[strlen(outpath)], rem, strlen(rem));
        memcpy(match, outpath, strlen(outpath));
      }
    }

    fwrite(line, strlen(line), 1, out);
  }

  fclose(out);
  fclose(in);

  return 0;
}

// Syntax: docedit [input-path] [output-path]
int main(int argc, const char *argv[]) {
  int nerr = 0;

  if(argc > 1)
    inpath = argv[1];

  if(argc > 2)
    outpath = argv[2];

  if(make_local_doxyfile() != 0) nerr++;

  return nerr;
}
