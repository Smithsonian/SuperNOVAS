/**
 * @file
 *
 * @date Created  on Nov 12, 2024
 * @author Attila Kovacs
 *
 *   SuperNOVAS functions interfacing with the NAIF CSPICE Toolkit
 */

#ifndef NOVAS_CSPICE_H_
#define NOVAS_CSPICE_H_

int novas_use_cspice();

int novas_use_cspice_ephem();

int novas_use_cspice_planets();

int cspice_add_kernel(const char *filename);

int cspice_remove_kernel(const char *filename);

#endif /* NOVAS_CSPICE_H_ */
