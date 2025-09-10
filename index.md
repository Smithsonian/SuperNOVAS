---
excerpt: SuperNOVAS is a light-weight, high-precision astrometry C/C++ library. It is a fork of NOVAS C 3.1.
---

<img src="/SuperNOVAS/resources/CfA-logo.png" alt="CfA logo" width="400" height="67" align="right"><br clear="all">

<img src="https://img.shields.io/github/v/release/Smithsonian/SuperNOVAS?label=github" class="badge" alt="GitHub release version" align="left">
<img src="https://img.shields.io/fedora/v/supernovas?color=lightblue" class="badge" alt="Fedora package version" align="left">
<img src="https://img.shields.io/debian/v/supernovas?color=lightblue" class="badge" alt="Debian package version" align="left">
<img src="https://img.shields.io/ubuntu/v/supernovas?color=lightblue" class="badge" alt="Ubuntu package version" align="left">
<br clear="all">

__SuperNOVAS__ is a C/C++ astronomy software library, providing high-precision astrometry such as one might need for 
running an observatory, a precise planetarium program, or for analyzing astronomical datasets. It started as a fork of 
the Naval Observatory Vector Astrometry Software ([NOVAS](https://aa.usno.navy.mil/software/novas_info)) C version 
3.1, but since then it has grown into its own, providing bug fixes, tons of new features, and a much improved API 
compared to the original NOVAS.

__SuperNOVAS__ is easy to use and it is very fast, providing 3--5 orders of magnitude faster position calculations than 
[astropy](https://www.astropy.org/) 7.0.0 in a single thread (see the [benchmarks](#benchmarks)), and its performance 
will scale with the number of CPUs when calculations are performed in parallel threads.

__SuperNOVAS__ is entirely free to use without licensing restrictions. Its source code is compatible with the C99 
standard, and hence should be suitable for old and new platforms alike. And, despite it being a light-weight library,
it fully supports the IAU 2000/2006 standards for sub-microarcsecond position calculations. 

__SuperNOVAS__ is maintained by [Attila Kovacs](https://github.com/attipaci) at the Center for Astrophysics \| Harvard 
& Smithsonian, and it available via the [Smithsonian/SuperNOVAS](https://github.com/Smithsonian/SuperNOVAS/) 
repository on GitHub.

This site contains various online resources that support the library:

 
__Downloads__

 - [Releases](https://github.com/Smithsonian/SuperNOVAS/releases) from GitHub


__Documentation__

 - [User's guide](doc/README.md) (`README.md`)
 - [API Documentation](doc/html/files.html)
 - [History of changes](doc/CHANGELOG.md) (`CHANGELOG.md`)
 - [Issues](https://github.com/Smithsonian/SuperNOVAS/issues) affecting SuperNOVAS releases (past and/or present)
 - [Community Forum](https://github.com/Smithsonian/SuperNOVAS/discussions) &ndash; ask a question, provide feedback, or 
   check announcements.


__Linux Packages__


SuperNOVAS is also available in packaged for for both RPM and Debian-based Linux distros. It has the following package
structure, which allows non-bloated installations of just the parts that are needed for the particular use case(s):


 | __Fedora RPM__                        |  __Debian package__                          |
 |---------------------------------------|----------------------------------------------|
 | `supernovas`                          | `libsupernovas1`                             |
 | `supernovas-cio-data`                 | `libsupernovas-cio-data`                     |
 | `supernovas-solsys1`                  | `libsolsys1_1`                               |
 | `supernovas-solsys2`                  | `libsolsys2_1`                               |
 | `supernovas-solsys-calceph`           | `libsolsys-calceph1`                         |
 | `supernovas-devel`                    | `libsupernovas-dev`                          |
 | `supernovas-doc`                      | `libsupernovas-doc`                          |
 

The differences in package naming are due to the different naming policies for RedHat/Fedora vs Debian. Otherwise, the 
RPM and Debian packages provide identical contents and features.
 

 
