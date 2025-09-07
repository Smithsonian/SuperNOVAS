/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "SuperNOVAS", "index.html", [
    [ "Table of Contents", "index.html#autotoc_md56", null ],
    [ "Introduction", "index.html#autotoc_md58", [
      [ "Related links", "index.html#autotoc_md59", null ]
    ] ],
    [ "Fixed NOVAS C 3.1 issues", "index.html#autotoc_md61", null ],
    [ "Compatibility with NOVAS C 3.1", "index.html#autotoc_md63", null ],
    [ "Building and installation", "index.html#autotoc_md65", [
      [ "Build SuperNOVAS using GNU make", "index.html#autotoc_md66", null ],
      [ "Build SuperNOVAS using CMake", "index.html#autotoc_md67", null ]
    ] ],
    [ "Building your application with SuperNOVAS", "index.html#autotoc_md69", [
      [ "Using a GNU <tt>Makefile</tt>", "index.html#autotoc_md70", null ],
      [ "Using CMake", "index.html#autotoc_md71", null ],
      [ "Legacy linking <tt>solarsystem()</tt> and <tt>readeph()</tt> modules", "index.html#autotoc_md72", null ],
      [ "Legacy modules: a better way", "index.html#autotoc_md73", null ]
    ] ],
    [ "Celestial coordinate systems (old vs. new)", "index.html#autotoc_md75", null ],
    [ "Example usage", "index.html#autotoc_md77", [
      [ "Calculating positions for a sidereal source", "index.html#autotoc_md78", [
        [ "Specify the object of interest", "index.html#autotoc_md79", null ],
        [ "Specify the observer location", "index.html#autotoc_md80", null ],
        [ "Specify the time of observation", "index.html#autotoc_md81", null ],
        [ "Set up the observing frame", "index.html#autotoc_md82", null ],
        [ "Calculate an apparent place on sky", "index.html#autotoc_md83", null ],
        [ "Calculate azimuth and elevation angles at the observing location", "index.html#autotoc_md84", null ],
        [ "Calculate rise, set, and transit times", "index.html#autotoc_md85", null ]
      ] ],
      [ "Calculating positions for a Solar-system source", "index.html#autotoc_md86", null ],
      [ "Coordinate and velocity transforms (change of coordinate system)", "index.html#autotoc_md87", null ]
    ] ],
    [ "Incorporating Solar-system ephemeris data or services", "index.html#autotoc_md89", [
      [ "CALCEPH integration", "index.html#autotoc_md90", null ],
      [ "NAIF CSPICE toolkit integration", "index.html#autotoc_md91", null ],
      [ "Universal ephemeris data / service integration", "index.html#autotoc_md92", null ],
      [ "Legacy support for (older) JPL major planet ephemerides", "index.html#autotoc_md93", [
        [ "Planets via <tt>eph_manager</tt>", "index.html#autotoc_md94", null ],
        [ "Planets via JPL's <tt>pleph</tt> FORTRAN interface", "index.html#autotoc_md95", null ]
      ] ],
      [ "Legacy linking of custom ephemeris functions", "index.html#autotoc_md96", null ]
    ] ],
    [ "Notes on precision", "index.html#autotoc_md98", [
      [ "Prerequisites to precise results", "index.html#autotoc_md99", null ]
    ] ],
    [ "Tips and tricks", "index.html#autotoc_md101", [
      [ "SuperNOVAS and C++", "index.html#autotoc_md102", null ],
      [ "Reduced accuracy shortcuts", "index.html#autotoc_md103", null ],
      [ "Multi-threaded calculations", "index.html#autotoc_md104", null ],
      [ "Physical units", "index.html#autotoc_md105", null ],
      [ "String times and angles", "index.html#autotoc_md106", null ],
      [ "String dates", "index.html#autotoc_md107", null ]
    ] ],
    [ "Runtime debug support", "index.html#autotoc_md109", null ],
    [ "Representative benchmarks", "index.html#autotoc_md111", null ],
    [ "SuperNOVAS added features", "index.html#autotoc_md113", [
      [ "New functionality highlights", "index.html#autotoc_md114", [
        [ "New in v1.0", "index.html#autotoc_md115", null ],
        [ "New in v1.1", "index.html#autotoc_md116", null ],
        [ "New in v1.2", "index.html#autotoc_md117", null ],
        [ "New in v1.3", "index.html#autotoc_md118", null ],
        [ "New in v1.4", "index.html#autotoc_md119", null ],
        [ "New in 1.5", "index.html#autotoc_md120", null ]
      ] ],
      [ "Refinements to the NOVAS C API", "index.html#autotoc_md121", null ]
    ] ],
    [ "Release schedule", "index.html#autotoc_md123", null ],
    [ "Changelog", "md_CHANGELOG.html", [
      [ "[Unreleased]", "md_CHANGELOG.html#autotoc_md1", [
        [ "Added", "md_CHANGELOG.html#autotoc_md2", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md3", null ],
        [ "Deprecated", "md_CHANGELOG.html#autotoc_md4", null ]
      ] ],
      [ "[1.4.2] - 2025-08-25", "md_CHANGELOG.html#autotoc_md5", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md6", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md7", null ]
      ] ],
      [ "[1.4.1] - 2025-07-21", "md_CHANGELOG.html#autotoc_md8", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md9", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md10", null ]
      ] ],
      [ "[1.4.0] - 2025-06-02", "md_CHANGELOG.html#autotoc_md11", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md12", null ],
        [ "Added", "md_CHANGELOG.html#autotoc_md13", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md14", null ],
        [ "Deprecated", "md_CHANGELOG.html#autotoc_md15", null ]
      ] ],
      [ "[1.3.1] - 2025-05-07", "md_CHANGELOG.html#autotoc_md16", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md17", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md18", null ],
        [ "Deprecated", "md_CHANGELOG.html#autotoc_md19", null ]
      ] ],
      [ "[1.3.0] - 2025-04-15", "md_CHANGELOG.html#autotoc_md20", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md21", null ],
        [ "Added", "md_CHANGELOG.html#autotoc_md22", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md23", null ]
      ] ],
      [ "[1.2.0] - 2025-01-15", "md_CHANGELOG.html#autotoc_md24", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md25", null ],
        [ "Added", "md_CHANGELOG.html#autotoc_md26", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md27", null ]
      ] ],
      [ "[1.1.1] - 2024-10-28", "md_CHANGELOG.html#autotoc_md28", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md29", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md30", null ]
      ] ],
      [ "[1.1.0] - 2024-08-04", "md_CHANGELOG.html#autotoc_md31", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md32", null ],
        [ "Added", "md_CHANGELOG.html#autotoc_md33", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md34", null ]
      ] ],
      [ "[1.0.1] - 2024-05-13", "md_CHANGELOG.html#autotoc_md35", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md36", null ],
        [ "Added", "md_CHANGELOG.html#autotoc_md37", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md38", null ]
      ] ],
      [ "[1.0.0] - 2024-03-01", "md_CHANGELOG.html#autotoc_md39", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md40", null ],
        [ "Added", "md_CHANGELOG.html#autotoc_md41", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md42", null ],
        [ "Deprecated", "md_CHANGELOG.html#autotoc_md43", null ]
      ] ]
    ] ],
    [ "Contributing to SuperNOVAS", "md_CONTRIBUTING.html", null ],
    [ "Astrometric Positions the Old Way", "md_LEGACY.html", [
      [ "Calculating positions for a sidereal source", "md_LEGACY.html#autotoc_md46", [
        [ "Specify the object of interest", "md_LEGACY.html#autotoc_md47", null ],
        [ "Spefify the observer location", "md_LEGACY.html#autotoc_md48", null ],
        [ "Specify the time of observation", "md_LEGACY.html#autotoc_md49", null ],
        [ "Specify Earth orientation parameters", "md_LEGACY.html#autotoc_md50", null ],
        [ "Calculate apparent positions on sky", "md_LEGACY.html#autotoc_md51", [
          [ "A. True apparent R.A. and declination", "md_LEGACY.html#autotoc_md52", null ],
          [ "B. Azimuth and elevation angles at the observing location", "md_LEGACY.html#autotoc_md53", null ]
        ] ]
      ] ],
      [ "Calculating positions for a Solar-system source", "md_LEGACY.html#autotoc_md54", null ]
    ] ],
    [ "Deprecated List", "deprecated.html", null ],
    [ "Data Structures", "annotated.html", [
      [ "Data Structures", "annotated.html", "annotated_dup" ],
      [ "Data Structure Index", "classes.html", null ],
      [ "Data Fields", "functions.html", [
        [ "All", "functions.html", null ],
        [ "Variables", "functions_vars.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "Globals", "globals.html", [
        [ "All", "globals.html", "globals_dup" ],
        [ "Functions", "globals_func.html", "globals_func" ],
        [ "Variables", "globals_vars.html", null ],
        [ "Typedefs", "globals_type.html", null ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Enumerator", "globals_eval.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"annotated.html",
"md_CHANGELOG.html#autotoc_md4",
"novas_8h.html#a7c474aa996c5ccc0c03b1006ea5b30e1",
"nutation_8h.html",
"structnovas__orbital__system.html#ac3165061a779934bfe04da14ad5555d0"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';