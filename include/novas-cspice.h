/**
 * @file
 *
 *  SuperNOVAS functions interfacing with the NAIF CSPICE Toolkit.
 *
 * @date Created  on Nov 12, 2024
 * @author Attila Kovacs
 */

#ifndef NOVAS_CSPICE_H_
#define NOVAS_CSPICE_H_

#if __cplusplus

#  ifdef NOVAS_NAMESPACE
namespace novas {
#  endif
#endif

int novas_use_cspice();

int novas_use_cspice_ephem();

int novas_use_cspice_planets();

int cspice_add_kernel(const char *filename);

int cspice_remove_kernel(const char *filename);

#if __cplusplus
#  ifdef NOVAS_NAMESPACE
} // namespace novas
#  endif
#endif

#endif /* NOVAS_CSPICE_H_ */
