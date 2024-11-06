/**
 * @file
 *
 * @date Created  on Nov 6, 2024
 * @author Attila Kovacs
 *
 *  Solar system ID mappings between NOVAS and NASA's Navigation and Ancillary Information Facility (NAIF), which
 *  is used by the JPL ephemeris systems. The differences affect only the major planets, the Sun, Moon, Solar-System
 *  Barycenter (SSB) and the Earth-Moon Barycenter (EMB). NOVAS does not have predefined IDs beyond this set (and no
 *  defined ID for EMB), thus for all other objects we'll assume and use NOVAS IDs that match NAIF.
 *
 * @since 1.2
 */

#include <errno.h>

#include "novas.h"
#include "naif.h"

/**
 * Converts a NAIF Solar-system body ID to a NOVAS Solar-system body ID. NOVAS and NAIF use slightly different IDs for
 * major planets, the Moon, and the Solar-system Barycenter (SSB). In NOVAS, major planets are have IDs ranging from
 * 1 through 9, but for NAIF 1--9 are the planetary barycenters and the planet centers have numbers in the hundreds
 * ending with 99 (e.g. the center of Earth is NAIF 399; 3 is the NOVAS ID for Earth and the NAIF IF for the Earth-Moon
 * Barycenter [EMB]). The Sun and Moon also have distinct IDs in NAIF vs NOVAS.
 *
 *
 * @param id      The NAIF ID of the Solar-system object of interest
 * @return        the NOVAS ID for the same object (which may or may not be different from the input), or -1 if the
 *                NAIF ID was that of the EMB.
 *
 * @sa novas_to_naif_id()
 *
 * @since 1.2
 */
long naif_to_novas_id(long id) {
  switch (id) {
    case NAIF_SUN: return NOVAS_SUN;
    case NAIF_MOON: return NOVAS_MOON;
    case NAIF_EMB:
      errno = ENOSYS;
      return -1;
    case NAIF_SSB: return NOVAS_SSB;
  }

  // Major planets
  if(id > 100 && id < 1000) if(id % 100 == 99) return (id - 99)/100;

  return id;
}

/**
 * Converts a NOVAS Solar-system body ID to a NAIF Solar-system body ID. NOVAS and NAIF use slightly different IDs for
 * major planets, the Moon, and the Solar-system Barycenter (SSB). In NOVAS, major planets are have IDs ranging from
 * 1 through 9, but for NAIF 1--9 are the planetary barycenters and the planet centers have numbers in the hundreds
 * ending with 99 (e.g. the center of Earth is NAIF 399; 3 is the NOVAS ID for Earth and the NAIF IF for the Earth-Moon
 * Barycenter [EMB]). The Sun and Moon also have distinct IDs in NAIF vs NOVAS.
 *
 *
 * @param id      The NOVAS ID of the Solar-system object of interest
 * @return        the NAIF ID for the same object (which may or may not be different from the input)
 *
 * @sa naif_to_novas_id()
 *
 * @since 1.2
 */
long novas_to_naif_id(long id) {
  if(id >= NOVAS_MERCURY && id <= NOVAS_PLUTO) return 100 * id + 99;     // 1-9: Major planets

  switch (id) {
    case NOVAS_SUN: return NAIF_SUN;
    case NOVAS_MOON: return NAIF_MOON;
    case NOVAS_SSB: return NAIF_SSB;
    default:
  }

  return id;
}


