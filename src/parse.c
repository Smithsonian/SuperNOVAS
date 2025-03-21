/**
 * @file
 *
 * @date Created  on Mar 2, 2025
 * @author Attila Kovacs
 *
 *  Various functions to parse string values for SuperNOVAS.
 */

#include <math.h>
#include <errno.h>
#include <string.h>
#include <strings.h>              // strcasecmp() / strncasecmp()
#include <ctype.h>

/// \cond PRIVATE
#define __NOVAS_INTERNAL_API__    ///< Use definitions meant for internal use by SuperNOVAS only
#include "novas.h"
/// \endcond

/**
 * Returns the Julian day corresponding to an astronomical coordinate epoch.
 *
 * @param system        Coordinate system, e.g. "ICRS", "B1950.0", "J2000.0", "FK4", "FK5",
 *                      "1950", "2000", or "HIP". In general, any Besselian or Julian year epoch
 *                      can be used by year (e.g. "B1933.193" or "J2022.033"), or else the fixed
 *                      values listed. If 'B' or 'J' is ommitted in front of the epoch year, then
 *                      Besselian epochs are assumed prior to 1984.0.
 * @return              [day] The Julian day corresponding to the given coordinate epoch, or else
 *                      NAN if the input string is NULL or the input is not recognised as a
 *                      coordinate epoch specification (errno will be set to EINVAL).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa make_cat_object_sys()
 * @sa make_redshifted_object_sys()
 * @sa transform_cat()
 * @sa precession()
 * @sa NOVAS_SYSTEM_ICRS
 * @sa NOVAS_SYSTEM_B1950
 * @sa NOVAS_SYSTEM_J2000
 * @sa NOVAS_SYSTEM_HIP
 */
double novas_epoch(const char *restrict system) {
  static const char *fn = "novas_epoch";

  double year;
  char type = 0, *tail = NULL;

  if(!system) {
    novas_error(0, EINVAL, fn, "epoch is NULL");
    return NAN;
  }

  if(!system[0]) {
    novas_error(0, EINVAL, fn, "epoch is empty");
    return NAN;
  }

  if(strcasecmp(system, NOVAS_SYSTEM_ICRS) == 0)
    return NOVAS_JD_J2000;

  if(strcasecmp(system, NOVAS_SYSTEM_FK5) == 0)
    return NOVAS_JD_J2000;

  if(strcasecmp(system, NOVAS_SYSTEM_FK4) == 0)
    return NOVAS_JD_B1950;

  if(strcasecmp(system, NOVAS_SYSTEM_HIP) == 0)
    return NOVAS_JD_HIP;

  if(toupper(system[0]) == 'B') {
    type = 'B';
    system++;
  }
  else if(toupper(system[0]) == 'J') {
    type = 'J';
    system++;
  }

  errno = 0;

  year = strtod(system, &tail);
  if(tail <= system) {
    novas_error(0, EINVAL, fn, "Invalid epoch: %s", system);
    return NAN;
  }

  if(!type)
    type = year < 1984.0 ? 'B' : 'J';

  if(type == 'J')
    return NOVAS_JD_J2000 + (year - 2000.0) * JULIAN_YEAR_DAYS;

  return NOVAS_JD_B1950 + (year - 1950.0) * BESSELIAN_YEAR_DAYS;
}

/**
 * Parses the decimal hours for a HMS string specification. The hour, minute, and second
 * components may be separated by spaces, tabs, colons `:`, underscore `_`, or a combination
 * thereof. Additionally, the hours and minutes may be separated by the letter `h`, and the
 * minutes and seconds may be separated by `m` or a single quote `'`. The seconds may be followed
 * by 's' or double quote `"`.
 *
 * There is no enforcement on the range of hours that can be represented in this way. Any
 * finite angle is parseable, even if it is outside its conventional range of 0-24h.
 *
 * For example, all of the lines below are valid specifications:
 *
 * <pre>
 *  23:59:59.999
 *  23h 59m 59.999
 *  23h59'59.999
 *  23 59 59.999
 *  23 59
 * </pre>
 *
 * At least the leading two components (hours and minutes) are required. If the seconds are
 * ommitted, they will be assumed zero, i.e. `23:59` is the same as `23:59:00.000`.
 *
 * @param hms         String specifying hours, minutes, and seconds, which correspond to
 *                    a time between 0 and 24 h. Time in any range is permitted, but the minutes and
 *                    seconds must be &gt;=0 and &lt;60.
 * @param[out] tail   (optional) If not NULL it will be set to the next character in the string after
 *                    the parsed time.
 * @return        [hours] Corresponding decimal time value, or else NAN if there was an
 *                error parsing the string (errno will be set to EINVAL).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_hms_hours()
 * @sa novas_parse_hours()
 * @sa novas_parse_dms()
 */
double novas_parse_hms(const char *restrict hms, char **restrict tail) {
  static const char *fn = "novas_hms_hours";

  int h = 0, m = 0, n = 0, k = 0;
  double s = 0.0;
  char next;

  if(tail)
    *tail = (char *) hms;

  if(!hms) {
    novas_error(0, EINVAL, fn, "input string is NULL");
    return NAN;
  }
  if(!hms[0]) {
    novas_error(0, EINVAL, fn, "input string is empty");
    return NAN;
  }

  if(sscanf(hms, "%d%*[:hH _\t]%d%n%*[:mM'’ _\t]%lf%n", &h, &m, &n, &s, &n) < 2) {
    novas_error(0, EINVAL, fn, "not in HMS format: '%s'", hms);
    return NAN;
  }

  if(m < 0 || m >= 60) {
    novas_error(0, EINVAL, fn, "invalid minutes: got %d, expected 0-59", m);
    return NAN;
  }

  if(s < 0.0 || s >= 60.0) {
    novas_error(0, EINVAL, fn, "invalid seconds: got %f, expected [0.0:60.0)", s);
    return NAN;
  }

  // Trailing seconds marker (if any)
  sscanf(&hms[n], "%*[ _\t]%*[s\"”]%n", &k);

  // The trailing markers must be standalone (end of string or followed by white space)
  next = hms[n + k];
  if(next == '\0' || next == '_' || isspace(next) || ispunct(next))
    n += k;

  if(tail)
    *tail += n;

  s = abs(h) + (m / 60.0) + (s / 3600.0);
  return h < 0 ? -s : s;
}

/**
 * Returns the decimal hours for a HMS string specification. The hour, minute, and second
 * components may be separated by spaces, tabs, colons `:`, or a combination thereof.
 * Additionally, the hours and minutes may be separated by the letter `h`, and the minutes
 * and seconds may be separated by `m` or a single quote `'`. The seconds may be followed by 's'
 * or double quote `"`.
 *
 * There is no enforcement on the range of hours that can be represented in this way. Any
 * finite angle is parseable, even if it is outside its conventional range of 0-24h.
 *
 * For example, all of the lines below specify the same time:
 *
 * <pre>
 *  23:59:59.999
 *  23h 59m 59.999s
 *  23h59'59.999
 *  23 59 59.999
 *  23 59
 *  23h
 * </pre>
 *
 * At least the leading two components (hours and minutes) are required. If the seconds are
 * ommitted, they will be assumed zero, i.e. `23:59` is the same as `23:59:00.000`.
 *
 * NOTES:
 * <ol>
 * <li> To see if the string was fully parsed when returning a valid (non-NAN) value, you can check
 * `errno`: it should be zero (0) if all non-whitespace characters have been parsed from the input
 * string, or else `EINVAL` if the parsed value used only the leading part of the string.</li>
 * </ol>
 *
 * @param hms     String specifying hours, minutes, and seconds, which correspond to
 *                a time between 0 and 24 h. Time in any range is permitted, but the minutes and
 *                seconds must be &gt;=0 and &lt;60.
 * @return        [hours] Corresponding decimal time value, or else NAN if there was an
 *                error parsing the string (errno will be set to EINVAL).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_str_hours()
 * @sa novas_parse_hms()
 * @sa novas_dms_degrees()
 */
double novas_hms_hours(const char *restrict hms) {
  char *tail = (char *) hms;
  double h = novas_parse_hms(hms, &tail);
  if(isnan(h))
    return novas_trace_nan("novas_hms_hours");

  errno = 0;

  // Skip trailing white spaces / punctuation
  while(*tail && (isspace(*tail) || ispunct(*tail))) tail++;
  if(*tail)
    errno = EINVAL;

  return h;
}

static int parse_compass(const char *restrict str, int *n) {
  char compass[7] = {};
  int from = 0;

  *n = 0;

  // Skip underscores and white spaces
  while(str[from] && (str[from] == '_' || isspace(str[from]) || ispunct(str[from]))) from++;

  // Compass direction (if any)
  if(sscanf(&str[from], "%6s", compass) > 0) {
    int i;

    for(i = 0; compass[i]; i++)
      if(compass[i] == '_' || ispunct(compass[i])) {
        compass[i] = '\0';
        break;
      }

    if(strcmp(compass, "N") == 0 || strcmp(compass, "E") == 0 ||
            strcasecmp(compass, "north") == 0 || strcasecmp(compass, "east") == 0) {
      *n = from + i;
      return 1;
    }
    else if (strcmp(compass, "S") == 0 || strcmp(compass, "W") == 0 ||
            strcasecmp(compass, "south") == 0 || strcasecmp(compass, "west") == 0) {
      *n = from + i;
      return -1;
    }
  }

  return 0;
}

/**
 * Parses the decimal degrees for a DMS string specification. The degree, (arc)minute, and
 * (arc)second components may be separated by spaces, tabs, colons `:`, underscore `_`, or a
 * combination thereof. Additionally, the degree and minutes may be separated by the letter `d`,
 * and the minutes and seconds may be separated by `m` or a single quote `'`. The seconds may be
 * followed by `s` or a double quote `"`. Finally, the leading or trailing component may
 * additionally be a standalone upper-case letter 'N', 'E', 'S', or 'W' or the
 * words 'North', 'East', 'South', or 'West' (case insensitive), signifying a compass
 * direction.
 *
 * There is no enforcement on the range of angles that can be represented in this way. Any
 * finite angle is parseable, even if it is outside its conventional range, such as +- 90
 * degrees N/S.
 *
 * For example, all of the lines below are valid specifications:
 *
 * <pre>
 *  -179:59:59.999
 *  -179d 59m 59.999s
 *  -179 59' 59.999
 *  179:59:59.999S
 *  179:59:59.999 W
 *  179:59:59.999 West
 *  179_59_59.999__S
 *  179 59 S
 *  W 179 59 59
 *  North 179d 59m
 * </pre>
 *
 * At least the leading two components (degrees and arcminutes) are required. If the arcseconds
 * are ommitted, they will be assumed zero, i.e. `179:59` is the same as `179:59:00.000`.
 *
 * @param dms         String specifying degrees, minutes, and seconds, which correspond to
 *                    an angle. Angles in any range are permitted, but the minutes and
 *                    seconds must be &gt;=0 and &lt;60.
 * @param[out] tail   (optional) If not NULL it will be set to the next character in the string after
 *                    the parsed time.
 * @return            [deg] Corresponding decimal angle value, or else NAN if there was
 *                    an error parsing the string (errno will be set to EINVAL).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 *
 * @sa novas_dms_degrees()
 * @sa novas_parse_degrees()
 * @sa novas_parse_hms()
 */
double novas_parse_dms(const char *restrict dms, char **restrict tail) {
  static const char *fn = "novas_dms_degrees";

  int sign = 0, d = 0, m = 0, nv = 0, nu = 0, nc = 0;
  double s = 0.0;
  char *str;

  if(tail)
    *tail = (char *) dms;

  if(!dms) {
    novas_error(0, EINVAL, fn, "input string is NULL");
    return NAN;
  }
  if(!dms[0]) {
    novas_error(0, EINVAL, fn, "input string is empty");
    return NAN;
  }

  sign = parse_compass(dms, &nc);
  str = (char *) dms + nc;

  if(sscanf(str, "%d%*[:d _\t]%d%n%*[:m' _\t]%lf%n", &d, &m, &nv, &s, &nv) < 2) {
    novas_error(0, EINVAL, fn, "not in DMS format: '%s'", dms);
    return NAN;
  }

  if(m < 0 || m >= 60) {
    novas_error(0, EINVAL, fn, "invalid minutes: got %d, expected 0-59", m);
    return NAN;
  }

  if(s < 0.0 || s >= 60.0) {
    novas_error(0, EINVAL, fn, "invalid seconds: got %f, expected [0.0:60.0)", s);
    return NAN;
  }

  if(toupper(str[nv-1]) == 'E')
    nv--; /// don't treat trailing 'E' as part of the number

  s = abs(d) + (m / 60.0) + (s / 3600.0);

  if (d < 0)
    s = -s;

  if(sign < 0) {
    s = -s;
  }

  // Trailing seconds marker (if any)
  sscanf(&str[nv], "%*[ _\t]%*[s\"]%n", &nu);

  // Compass direction (if any)
  if(nc == 0 && parse_compass(&str[nv + nu], &nc) < 0)
    s = -s;

  if(tail)
    *tail = (char *) dms + nv + nu + nc;

  return s;
}

/**
 * Returns the decimal degrees for a DMS string specification. The degree, (arc)minute, and
 * (arc)second components may be separated by spaces, tabs, colons `:`, or a combination thereof.
 * Additionally, the degree and minutes may be separated by the letter `d`, and the minutes and
 * seconds may be separated by `m` or a single quote `'`. The seconds may be followed by 's' or
 * double quote `"`. Finally, the leading or trailing component may additionally be a
 * standalone upper-case letter 'N', 'E', 'S', or 'W' or the words 'North', 'East', 'South', or
 * 'West' (case insensitive), signifying a compass direction.
 *
 * There is no enforcement on the range of angles that can be represented in this way. Any
 * finite angle is parseable, even if it is outside its conventional range, such as +- 90
 * degrees N/S.
 *
 * For example, all of the lines below are valid specifications:
 *
 * <pre>
 *  -179:59:59.999
 *  -179d 59m 59.999s
 *  -179 59' 59.999
 *  179:59:59.999S
 *  179 59 59.999 W
 *  179 59 59.999 west
 *  179_59_59.999__S
 *  W 179 59 59
 *  North 179d 59m
 * </pre>
 *
 * At least the leading two components (degrees and arcminutes) are required. If the arcseconds
 * are ommitted, they will be assumed zero, i.e. `179:59` is the same as `179:59:00.000`.
 *
 * NOTES:
 * <ol>
 * <li> To see if the string was fully parsed when returning a valid (non-NAN) value, you can check
 * `errno`: it should be zero (0) if all non-whitespace characters have been parsed from the input
 * string, or else `EINVAL` if the parsed value used only the leading part of the string.</li>
 * </ol>
 *
 * @param dms     String specifying degrees, minutes, and seconds, which correspond to
 *                an angle. Angles in any range are permitted, but the minutes and
 *                seconds must be &gt;=0 and &lt;60.
 * @return        [deg] Corresponding decimal angle value, or else NAN if there was
 *                an error parsing the string (errno will be set to EINVAL).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_str_degrees()
 * @sa novas_parse_dms()
 * @sa novas_hms_hours()
 */
double novas_dms_degrees(const char *restrict dms) {
  char *tail = (char *) dms;
  double deg = novas_parse_dms(dms, &tail);
  if(isnan(deg))
    return novas_trace_nan("novas_dms_degrees");

  errno = 0;

  // Skip trailing white spaces / punctuation
  while(*tail && (isspace(*tail) || ispunct(*tail))) tail++;
  if(*tail)
    errno = EINVAL;

  return deg;
}

/**
 * Parses an angle in degrees from a string that contains either a decimal degrees or else a
 * broken-down DMS representation.
 *
 * The decimal representation may be followed by a unit designator: "d", "dg", "deg", "degree",
 * or "degrees", which will be parsed case-insensitively also, if present.
 *
 * Both DMS and decimal values may start or end with a compass direction: such as an upper-case
 * letter `N`, `E`, `S`, or `W`, or else the case-insensitive words 'North', 'East', 'South' or
 * 'West'.
 *
 * There is no enforcement on the range of angles that can be represented in this way. Any
 * finite angle is parseable, even if it is outside its conventional range, such as +- 90
 * degrees N/S.
 *
 * A few examples of angles that may be parsed:
 *
 * <pre>
 *  -179:59:59.999
 *  -179d 59m 59.999s
 *  179 59' 59.999" S
 *  179 59 S
 *  -179.99999d
 *  -179.99999
 *  179.99999W
 *  179.99999 West
 *  179.99999 deg S
 *  W 179.99999d
 *  North 179d 59m
 *  east 179.99 degrees
 * </pre>
 *
 *
 * @param str         The input string that specified an angle either as decimal degrees
 *                    or as a broken down DMS speficication. The decimal value may be
 *                    followed by the letter `d` immediately. And both the decimal and DMS
 *                    representation may be ended with a compass direction marker,
 *                    `N`, `E`, `S`, or `W`. See more in `novas_parse_dms()` on acceptable DMS
 *                    specifications.
 * @param[out] tail   (optional) If not NULL it will be set to the next character in the string after
 *                    the parsed angle.
 * @return            [deg] The angle represented by the string, or else NAN if the
 *                    string could not be parsed into an angle value (errno will indicate
 *                    the type of error).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_str_degrees()
 * @sa novas_parse_dms()
 * @sa novas_parse_hours()
 */
double novas_parse_degrees(const char *restrict str, char **restrict tail) {
  static const char *fn = "novas_parse_degrees";

  double deg;
  enum novas_debug_mode debug = novas_get_debug_mode();
  int sign, n = 0, nc = 0;
  char *next;

  if(tail)
    *tail = (char *) str;

  if(!str) {
    novas_error(0, EINVAL, fn, "input string is NULL");
    return NAN;
  }

  novas_debug(NOVAS_DEBUG_OFF);
  deg = novas_parse_dms(str, tail);
  novas_debug(debug);

  if(!isnan(deg))
    return deg;

  sign = parse_compass(str, &nc);
  next = (char *) str + nc;

  if(sscanf(next, "%lf%n", &deg, &n) > 0) {
    char unit[9] = {};
    int n1, nu = 0;

    if(toupper(next[n-1]) == 'E')       /// don't treat trailing 'E' as part of the number
      n--;

    // Skip underscores and white spaces
    for(n1 = n; next[n1] && (next[n1] == '_' || isspace(next[n1]));) n1++;

    // Skip over unit specification
    if(sscanf(&next[n1], "%8s%n", unit, &nu) > 0) {
      static const char *units[] = { "d", "dg", "deg", "degree", "degrees" , NULL};
      int i;

      // Terminate unit at punctuation
      for(i = 0; unit[i]; i++) if(unit[i] == '_' || ispunct(unit[i])) {
        unit[i] = '\0';
        nu = i;
        break;
      }

      // Check for match against recognised units.
      for(i = 0; units[i]; i++) if(strcasecmp(units[i], unit) == 0) {
        n = n1 + nu;
        break;
      }
    }

    if(nc == 0) {
      sign = parse_compass(&next[n], &nc);
      n += nc;
    }

    if(sign < 0)
      deg = -deg;

    if(tail)
      *tail = next + n;
  }
  else {
    novas_error(0, EINVAL, fn, "invalid angle specification: '%s'", str);
    return NAN;
  }

  return deg;
}

/**
 * Parses a time or time-like angle from a string that contains either a decimal hours or else a
 * broken-down HMS representation.
 *
 * The decimal representation may be followed by a unit designator: "h", "hr", "hrs", "hour", or
 * "hours", which will be parsed case-insensitively also, if present.
 *
 * There is no enforcement on the range of hours that can be represented in this way. Any
 * finite angle is parseable, even if it is outside its conventional range of 0-24h.
 *
 * A few examples of angles that may be parsed:
 *
 * <pre>
 *  23:59:59.999
 *  23h 59m 59.999s
 *  23h59'59.999
 *  23 59 59.999
 *  23.999999h
 *  23.999999 hours
 *  23.999999
 * </pre>
 *
 *
 * @param str         The input string that specified an angle either as decimal hours
 *                    or as a broken down HMS speficication. The decimal value may be immediately
 *                    followed by a letter 'h'. See more in `novas_parse_hms()` on acceptable HMS
 *                    input specifications.
 * @param[out] tail   (optional) If not NULL it will be set to the next character in the string after
 *                    the parsed angle.
 * @return            [h] The time-like value represented by the string, or else NAN if the
 *                    string could not be parsed into a time-like value (errno will indicate
 *                    the type of error).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_str_hours()
 * @sa novas_parse_hms()
 * @sa novas_parse_degrees()
 */
double novas_parse_hours(const char *restrict str, char **restrict tail) {
  static const char *fn = "novas_parse_hours";

  double h;
  enum novas_debug_mode debug = novas_get_debug_mode();
  int n = 0;

  if(tail)
    *tail = (char *) str;

  if(!str) {
    novas_error(0, EINVAL, fn, "input string is NULL");
    return NAN;
  }

  novas_debug(NOVAS_DEBUG_OFF);
  h = novas_parse_hms(str, tail);
  novas_debug(debug);

  if(!isnan(h))
    return h;

  if(sscanf(str, "%lf%n", &h, &n) > 0) {
    char unit[7] = {};
    int n1, n2 = 0;

    // Skip underscores and white spaces
    for(n1 = n; str[n1] && (str[n1] == '_' || isspace(str[n1])); n1++);

    // Skip over unit specification
    if(sscanf(&str[n1], "%6s%n", unit, &n2) > 0) {
      static const char *units[] = { "h", "hr", "hrs", "hour", "hours" , NULL};
      int i;

      // Terminate unit at punctuation
      for(i = 0; unit[i]; i++) if(unit[i] == '_' || ispunct(unit[i])) {
        unit[i] = '\0';
        n2 = i;
        break;
      }

      // Check for match against recognised units.
      for(i = 0; units[i]; i++) if(strcasecmp(units[i], unit) == 0) {
        n = n1 + n2;
        break;
      }
    }

    if(tail)
      *tail += n;
  }
  else {
    novas_error(0, EINVAL, fn, "invalid time specification: '%s'", str);
    return NAN;
  }


  return h;
}

/**
 * Returns an angle parsed from a string that contains either a decimal degrees or else a broken-down
 * DMS representation. See `novas_parse_degrees()` to see what string representations may be used.
 *
 * To see if the string was fully parsed when returning a valid (non-NAN) value, you can check
 * `errno`: it should be zero (0) if all non-whitespace and punctuation characters have been parsed
 * from the input string, or else `EINVAL` if the parsed value used only the leading part of the
 * string.
 *
 * @param str     The input string that specified an angle either as decimal degrees
 *                or as a broken down DMS speficication. The decimal value may be immediately
 *                followed by a letter 'd'. See more in `novas_parse_degrees()` on acceptable
 *                input specifications.
 * @return        [deg] The angle represented by the string, or else NAN if the
 *                string could not be parsed into an angle value (errno will indicate
 *                the type of error).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_parse_degrees()
 * @sa novas_parse_dms()
 * @sa novas_str_hours()
 */
double novas_str_degrees(const char *restrict str) {
  char *tail = (char *) str;
  double deg = novas_parse_degrees(str, &tail);
  if(isnan(deg))
    return novas_trace_nan("novas_str_degrees");

  errno = 0;

  // Skip trailing white spaces / punctuation
  while(*tail && (isspace(*tail) || ispunct(*tail))) tail++;
  if(*tail)
    errno = EINVAL;

  return deg;
}

/**
 * Returns a time or time-like angleparsed  from a string that contains either a decimal hours
 * or else a broken-down HMS representation. See `novas_parse_hours()` to see what string
 * representations may be used.
 *
 * To check if the string was fully parsed when returning a valid (non-NAN) value you can check
 * `errno`: it should be zero (0) if all non-whitespace and punctuation characters have been parsed
 * from the input string, or else `EINVAL` if the parsed value used only the leading part of the
 * string.
 *
 * @param str     The input string that specified an angle either as decimal hours
 *                or as a broken down HMS speficication. The decimal value may be
 *                immediately followed by a letter 'h'. See more in `novas_parse_hours()`
 *                on acceptable input specifications.
 * @return        [h] The time-like value represented by the string, or else NAN if the
 *                string could not be parsed into a time-like value (errno will indicate
 *                the type of error).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_parse_hours()
 * @sa novas_parse_hms()
 * @sa novas_str_degrees()
 */
double novas_str_hours(const char *restrict str) {
  char *tail = (char *) str;
  double h = novas_parse_hours(str, &tail);
  if(isnan(h))
    return novas_trace_nan("novas_str_hours");

  errno = 0;

  // Skip trailing white spaces / punctuation
  while(*tail && (isspace(*tail) || ispunct(*tail))) tail++;
  if(*tail)
    errno = EINVAL;

  return h;
}


