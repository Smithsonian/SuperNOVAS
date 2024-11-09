# Test JPL ephemeris files

This directory contains test JPL ephemeris files and `spkmerge` command files and time kernel to generate them.

The test files contain data just for a 24-hour period centered on J2000 (12 TT 1 Jan 2000). The original ephemeris 
files, on which these are based, can be found at https://naif.jpl.nasa.gov/pub/naif/generic_kernels/spk/.

 - `de440s-j2000` contains data for the major planets extracted from the DE440 ephemeris (`de440s.bsp`).
 
 - `mar097-j2000` contains data for Mars and its satellites extracted from `mar097.bsp`.
 
The test files can be generated from the originals and the included time kernel (`naif0012.tls`), by running
the `spkmerge` tool of the CSPICE toolkit, with the supplied command file names as arguments.
 
