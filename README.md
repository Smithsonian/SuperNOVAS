
![Build Status](https://github.com/Smithsonian/SuperNOVAS/actions/workflows/build.yml/badge.svg)
![Test](https://github.com/Smithsonian/SuperNOVAS/actions/workflows/test.yml/badge.svg)
![Static Analysis](https://github.com/Smithsonian/SuperNOVAS/actions/workflows/test.yml/badge.svg)
![API documentation](https://github.com/Smithsonian/SuperNOVAS/actions/workflows/test.yml/badge.svg)



# SuperNOVAS: the Naval Observatory NOVAS C library, made easier to use.

## Introduction

SuperNOVAS is a fork of the The Naval Observatory Vector Astrometry Software ([NOVAS](https://aa.usno.navy.mil/software/novas_info)).
NOVAS is a light-weight but capable C library for precise positional astronomy calculations, such as one might need for an observatory, 
where apparent coordinates must be calculated to high precision. 

The primary goals of SuperNOVAS is to improve on the stock NOVAS C library by:

 - Fixing outstanding issues
 - Improving the ease of use by using enums instead of integer constants
 - Improving API documentation with doxygen to provide browsable cross-referenced API docs. 
 - Streamlining calculations where possible
 - Adding `const` modifier to prototype arguments where appropriate
 - Checking arguments and setting `errno` as appropriate (and returning -1 if possible) 
 - Adding new API to facilitate more accessible use
 - Providing a Makefile to build static and shared libraries from sources easily
 
At the same time, SuperNOVAS aims to be fully backward compatible with the upstream NOVAS C library, such that it can be used as a 
drop-in link-time replacement for NOVAS in your application without having to change your exsiting code.
 
SuperNOVAS is currently based on NOVAS C version 3.1. We plan to rebase SuperNOVAS to the latest upstream release of the NOVAS C 
package, if new releases become available.
 
SuperNOVAS is maintained by Attila Kovacs at the Center for Astrophysics | Harvard and Smithsonian, and it is available through
the [Smithsonian/SuperNOVAS](https://github.com/Smithsonian/SuperNOVAS) repo on Github.


