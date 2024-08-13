---
excerpt: SuperNOVAS is a light-weight, high-precision astrometry C/C++ library. It is a fork of NOVAS C 3.1.
---

<img src="/SuperNOVAS/resources/CfA-logo.png" alt="CfA logo" width="400" height="67" align="right"><br clear="all">


__SuperNOVAS__ is a C/C++ astronomy software library, providing high-precision astrometry such as one 
might need for running an observatory or a precise planetarium program. It is a fork of the Naval Observatory Vector 
Astrometry Software ([NOVAS](https://aa.usno.navy.mil/software/novas_info)) C version 3.1, providing bug fixes and 
making it safer and easier to use overall.

SuperNOVAS is entirely free to use without licensing restrictions.  Its source code is compatible with the C90 
standard, and hence should be suitable for old and new platforms alike. It is light-weight and easy to use, with full 
support for the IAU 2000/2006 standards for sub-microarcsecond position calculations.

SuperNOVAS is maintained by [Attila Kovacs](https://github.com/attipaci) at the Center for Astrophysics \| Harvard & 
Smithsonian, and it available via the [Smithsonian/SuperNOVAS](https://github.com/Smithsonian/SuperNOVAS/)
repository on GitHub.

This site contains various online resources that support the library:

 
__Downloads__

 - [Releases](https://github.com/Smithsonian/SuperNOVAS/releases) from GitHub
 - Doxygen [tag file](apidoc/novas.tag) (`novas.tag`) for linking 
   your [Doxygen](https://www.doxygen.nl/) documentation to that of SuperNOVAS


__Linux Packages__

SuperNOVAS is also available in packaged for for both RPM and Debian-based Linux distros. It has the following package
structure, which allows non-bloated installations of just the parts that are needed for the particular use case(s):


 | __Fedora RPM__           |  __Debian package__       |
 |--------------------------|---------------------------|
 | `supernovas`             | `libsupernovas1`          |
 | `supernovas-cio-data`    | `libsupernovas-cio-data`  |
 | `supernovas-solsys1`     | `libsolsys1_1`            |
 | `supernovas-solsys2`     | `libsolsys2_1`            |
 | `supernovas-devel`       | `libsupernovas-dev`       |
 | `supernovas-doc`         | `libsupernovas-doc`       |
 

The differences in package naming are due to the different naming policies for RedHat/Fedora vs Debian. Some files
are installed at different locations and/or with different names for RMS vs Debian, also owing to the differences in
packaging policies between the distibutions. Otherwise the RPM and Debian packages provide identical contents and 
features.
 

__Documentation__

 - [User's guide](doc/README.md) (`README.md`)
 - [API Documentation](apidoc/html/files.html)
 - [History of changes](doc/CHANGELOG.md) (`CHANGELOG.md`)
 - [Issues](https://github.com/Smithsonian/SuperNOVAS/issues) affecting SuperNOVAS releases (past and/or present)
 - [Community Forum](https://github.com/Smithsonian/SuperNOVAS/issues) &ndash; ask a question, provide feedback, or 
   check announcements.

 
