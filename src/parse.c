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
 * Parses the decimal hours for a HMS string specification. The hour, minute, and second
 * components may be separated by spaces, tabs, colons `:`, underscore `_`, or a combination
 * thereof. Additionally, the hours and minutes may be separated by the letter `h`, and the
 * minutes and seconds may be separated by `m` or a single quote `'`. The seconds may be followed
 * by 's' or double quote `"`.
 *
 * Additionally, the hour and minutes may be separated by the letter `h`, and the minutes and
 * seconds may be separated by `m`, or a single quote `'`. For example, all of the lines below
 * specify the same time:
 *
 * <pre>
 *  23:59:59.999
 *  23h 59m 59.999
 *  23h59'59.999
 *  23 59 59.999
 * </pre>
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
 * @sa novas_parse_dms()
 */
double novas_parse_hms(const char *restrict hms, char **restrict tail) {
  static const char *fn = "novas_hms_hours";

  int h = 0, m = 0, n = 0, k = 0;
  double s = NAN;
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

  if(sscanf(hms, "%d%*[:hH _\t]%d%*[:mM'’ _\t]%lf%n", &h, &m, &s, &n) < 3) {
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
  if(next == '\0' || next == '_' || isspace(next))
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
 * For example, all of the lines below specify the same time:
 *
 * <pre>
 *  23:59:59.999
 *  23h 59m 59.999s
 *  23h59'59.999
 *  23 59 59.999
 * </pre>
 *
 * @param hms         String specifying hours, minutes, and seconds, which correspond to
 *                    a time between 0 and 24 h. Time in any range is permitted, but the minutes and
 *                    seconds must be &gt;=0 and &lt;60.
 * @return        [hours] Corresponding decimal time value, or else NAN if there was an
 *                error parsing the string (errno will be set to EINVAL).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_dms_degrees()
 */
double novas_hms_hours(const char *restrict hms) {
  return novas_parse_hms(hms, NULL);
}

/**
 * Parses the decimal degrees for a DMS string specification. The degree, (arc)minute, and
 * (arc)second components may be separated by spaces, tabs, colons `:`, underscore `_`, or a
 * combination thereof. Additionally, the degree and minutes may be separated by the letter `d`,
 * and the minutes and seconds may be separated by `m` or a single quote `'`. The seconds may be
 * followed by 's' or a double quote `"`. Finally, the last component may additionally
 * be followed by a standalone upper-case letter 'N', 'E', 'S', or 'W' signifying a compass
 * direction.
 *
 * For example, all of the lines below specify the same angle:
 *
 * <pre>
 *  -179:59:59.999
 *  -179d 59m 59.999s
 *  -179 59' 59.999
 *  179:59:59.999S
 *  179:59:59.999 W
 *  179_59_59.999__S
 * </pre>
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
 * @sa novas_dms_degrees()
 * @sa novas_parse_hms()
 */
double novas_parse_dms(const char *restrict dms, char **restrict tail) {
  static const char *fn = "novas_dms_degrees";

  int d = 0, m = 0, n = 0, k = 0, l = 0;
  double s = NAN;
  char next, compass[3] = {};

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

  if(sscanf(dms, "%d%*[:d _\t]%d%*[:m' _\t]%lf%n", &d, &m, &s, &n) < 3) {
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

  s = abs(d) + (m / 60.0) + (s / 3600.0);
  if (d < 0)
    s = -s;

  if(dms[n-1] == 'E') {
    // An 'E' immediately after the last numerical value, is parsed as part of the number
    // but we should interpret it as a compass direction below.
    n--;
  }

  // Trailing seconds marker (if any)
  sscanf(&dms[n], "%*[ _\t]%*[s\"]%n", &k);

  // Compass direction (if any)
  if(sscanf(&dms[n + k], "%2s%n", compass, &l) > 0) {
    if(compass[1] == '_')
      compass[1] = '\0';

    if(strcmp(compass, "N") == 0 || strcmp(compass, "E") == 0) {
      k += l;
    }
    else if (strcmp(compass, "S") == 0 || strcmp(compass, "W") == 0) {
      s = -s;
      k += l;
    }
  }

  // The trailing markers must be standalone (end of string or followed by white space)
  next = dms[n + k];
  if(next == '\0' || next == '_' || isspace(next))
    n += k;

  if(tail)
    *tail += n;

  return s;
}

/**
 * Returns the decimal degrees for a DMS string specification. The degree, (arc)minute, and
 * (arc)second components may be separated by spaces, tabs, colons `:`, or a combination thereof.
 * Additionally, the degree and minutes may be separated by the letter `d`, and the minutes and
 * seconds may be separated by `m` or a single quote `'`. The seconds may be followed by 's' or
 * double quote `"`. Finally, the last component may additionally be followed by a standalone
 * upper-case letter 'N', 'E', 'S', or 'W' signifying a compass direction.
 *
 * For example, all of the lines below specify the same angle:
 *
 * <pre>
 *  -179:59:59.999
 *  -179d 59m 59.999s
 *  -179 59' 59.999
 *  179:59:59.999S
 *  179 59 59.999 W
 *  179_59_59.999__S
 * </pre>
 *
 * @param dms         String specifying degrees, minutes, and seconds, which correspond to
 *                    an angle. Angles in any range are permitted, but the minutes and
 *                    seconds must be &gt;=0 and &lt;60.
 * @return            [deg] Corresponding decimal angle value, or else NAN if there was
 *                    an error parsing the string (errno will be set to EINVAL).
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_hms_hours()
 */
double novas_dms_degrees(const char *restrict dms) {
  return novas_parse_dms(dms, NULL);
}

/**
 * Parses an angle in degrees from a string that contains either a decimal degrees
 * or else a broken-down DMS representation. Like in the case of `novas_dms_degrees()`
 * and `novas_parse_dms()`, the decimal value may also be followed by a compass
 * direction: `N`, `E`, `S`, or `W`. A few examples of angles that may be parsed:
 *
 * <pre>
 *  -179:59:59.999
 *  -179d 59m 59.999s
 *  179 59 59.999 S
 *  -179.99999d
 *  -179.99999
 *  179.99999W
 *  179.99999d S
 * </pre>
 *
 *
 * @param str         The input string that specified an angle either as decimal degrees
 *                    or as a broken down DMS speficication. The decimal value may be
 *                    followed by the letter d or the degree symbol (`0xB0`) immediately. And both the
 *                    decimal and DMS representation may be ended with a compass direction marker,
 *                    `N`, `E`, `S`, or `W`. See more in `novas_parse_dms()` on acceptable DSM
 *                    specifications.
 * @param[out] tail   (optional) If not NULL it will be set to the next character in the string after
 *                    the parsed angle.
 * @return            [deg] The angle represented by the string
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_parse_hours()
 * @sa novas_parse_dms()
 */
double novas_parse_degrees(const char *restrict str, char **restrict tail) {
  static const char *fn = "novas_parse_degrees";

  double deg;
  enum novas_debug_mode debug = novas_get_debug_mode();

  if(tail)
    *tail = (char *) str;

  if(!str) {
    novas_error(0, EINVAL, fn, "input string is NULL");
    return NAN;
  }

  novas_debug(NOVAS_DEBUG_OFF);
  deg = novas_parse_dms(str, tail);
  novas_debug(debug);

  if(isnan(deg)) {
    int n = 0;

    if(sscanf(str, "%lf%n", &deg, &n) > 0) {
      char compass[3] = {};
      int n2 = 0;

      if(str[n-1] == 'E')       /// trailing E compass, handled below
        n--;
      else if(str[n] == 'd')    /// Skip single 'd' immediately after decimal
        n++;

      if(sscanf(&str[n], "%2s%n", compass, &n2) > 0) {
        if(strcmp(compass, "N") == 0 || strcmp(compass, "E") == 0) {
          n += n2;
        }
        else if (strcmp(compass, "S") == 0 || strcmp(compass, "W") == 0) {
          deg = -deg;
          n += n2;
        }
      }

      if(tail)
        *tail += n;
    }
    else {
      novas_error(0, EINVAL, fn, "invalid angle specification: %s", str);
      return NAN;
    }
  }

  return deg;
}

/**
 * Parses a time or time-like angle from a string that contains either a decimal hours
 * or else a broken-down HMS representation. Like in the case of `novas_hms_hours()`
 * and `novas_parse_hms()`.
 * A few examples of angles that may be parsed:
 *
 * <pre>
 *  23:59:59.999
 *  23h 59m 59.999s
 *  23h59'59.999
 *  23 59 59.999
 *  23.999999h
 *  23.999999
 * </pre>
 *
 *
 * @param str         The input string that specified an angle either as decimal degrees
 *                    or as a broken down DMS speficication. The decimal value may be immediately
 *                    followed by a letter 'h'. See more in
 *                    `novas_parse_dms()` on acceptable DSM specifications.
 * @param[out] tail   (optional) If not NULL it will be set to the next character in the string after
 *                    the parsed angle.
 * @return            [h] The time-like value represented by the string
 *
 * @since 1.3
 * @author Attila Kovacs
 *
 * @sa novas_parse_hours()
 * @sa novas_parse_dms()
 */
double novas_parse_hours(const char *restrict str, char **restrict tail) {
  static const char *fn = "novas_parse_hours";

  double h;
  enum novas_debug_mode debug = novas_get_debug_mode();

  if(tail)
    *tail = (char *) str;

  if(!str) {
    novas_error(0, EINVAL, fn, "input string is NULL");
    return NAN;
  }

  novas_debug(NOVAS_DEBUG_OFF);
  h = novas_parse_hms(str, tail);
  novas_debug(debug);

  if(isnan(h)) {
    int n = 0;
    if(sscanf(str, "%lf%n", &h, &n) > 0) {
      if(str[n] == 'h')
        n++;

      if(tail)
        *tail += n;
    }
    else {
      novas_error(0, EINVAL, fn, "invalid time specification: %s", str);
      return NAN;
    }
  }

  return h;
}
