# SuperNOVAS: A fork of the Naval Observatory NOVAS C library

## Introduction

SuperNOVAS is a fork of the The Naval Observatory Vector Astrometry Software ([NOVAS](https://aa.usno.navy.mil/software/novas_info)).
NOVAS is a light-weight but capable C library for precise positional astronomy calculations, such as one might need for an observatory, 
where apparent coordinates must be calculated to sub-milliarcsec precisions. 

The primary goals of SuperNOVAS is to improve on the stock NOVAS C library by:

 - Fixing outstanding issues
 - Providing a Makefile to build static and shared libraries from sources easily
 - Improving the ease of use by using enums instead of integer constants
 - Improving API documentation with doxygen to provide browsable cross-referenced API docs. 
 - Streamlining calculations where possible
 - Adding new API to facilitate more accessible use
 
At the same time, SuperNOVAS aims to be fully backward compatible with the stick NOVAS C library, such that it can be used as a drop-in
link-time replacement for NOVAS in your application without having to change your exsiting code.
 
SuperNOVAS is currently based on vwersion 3.1 of NOVAS C. We plan to rebase SuperNOVAS to the latest upstream release of the NOVAS C 
package, if new releases become available.
 
SuperNOVAS is maintained by Attila Kovacs at the Center for Astrophysics | Harvard and Smithsonian, and it is available through
the [Smithsonian/SuperNOVAS](https://github.com/Smithsonian/SuperNOVAS) repo on Github.


