/**
 * @file
 *
 *  SuperNOVAS functions interfacing with the CALCEPH C library.
 *
 * @date Created  on Nov 12, 2024
 * @author Attila Kovacs
 */

#ifndef NOVAS_CALCEPH_H_
#define NOVAS_CALCEPH_H_

#  include <calceph.h>

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
namespace novas {
#  endif
#endif

int novas_use_calceph(t_calcephbin *eph);

int novas_use_calceph_planets(t_calcephbin *eph);

int novas_calceph_use_ids(enum novas_id_type idtype);

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
} // namespace novas
#  endif
#endif

#endif /* NOVAS_CALCEPH_H_ */
