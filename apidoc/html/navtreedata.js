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
    [ "Changelog", "md_CHANGELOG.html", [
      [ "Table of Contents", "index.html#autotoc_md33", null ],
      [ "Introduction", "index.html#autotoc_md35", [
        [ "Related links", "index.html#autotoc_md36", null ]
      ] ],
      [ "Fixed NOVAS C 3.1 issues", "index.html#autotoc_md38", null ],
      [ "Compatibility with NOVAS C 3.1", "index.html#autotoc_md40", null ],
      [ "Building and installation", "index.html#autotoc_md42", null ],
      [ "Building your application with SuperNOVAS", "index.html#autotoc_md44", [
        [ "Legacy linking <tt>solarsystem()</tt> and <tt>readeph()</tt> modules", "index.html#autotoc_md45", null ],
        [ "Legacy modules: a better way...", "index.html#autotoc_md46", null ]
      ] ],
      [ "Example usage", "index.html#autotoc_md48", [
        [ "Note on alternative methodologies", "index.html#autotoc_md49", null ],
        [ "Calculating positions for a sidereal source", "index.html#autotoc_md50", [
          [ "Specify the object of interest", "index.html#autotoc_md51", null ],
          [ "Specify the observer location", "index.html#autotoc_md52", null ],
          [ "Specify the time of observation", "index.html#autotoc_md53", null ],
          [ "Set up the observing frame", "index.html#autotoc_md54", null ],
          [ "Calculate an apparent place on sky", "index.html#autotoc_md55", null ],
          [ "Calculate azimuth and elevation angles at the observing location", "index.html#autotoc_md56", null ]
        ] ],
        [ "Calculating positions for a Solar-system source", "index.html#autotoc_md57", null ]
      ] ],
      [ "Tips and tricks", "index.html#autotoc_md59", [
        [ "Reduced accuracy shortcuts", "index.html#autotoc_md60", null ],
        [ "Multi-threaded calculations", "index.html#autotoc_md61", null ],
        [ "Physical units", "index.html#autotoc_md62", null ]
      ] ],
      [ "Notes on precision", "index.html#autotoc_md64", [
        [ "Prerequisites to precise results", "index.html#autotoc_md65", null ]
      ] ],
      [ "SuperNOVAS specific features", "index.html#autotoc_md67", [
        [ "Newly added functionality", "index.html#autotoc_md68", [
          [ "Added in v1.1", "index.html#autotoc_md69", null ],
          [ "Added in v1.2", "index.html#autotoc_md70", null ]
        ] ],
        [ "Refinements to the NOVAS C API", "index.html#autotoc_md71", null ]
      ] ],
      [ "Incorporating Solar-system ephemeris data or services", "index.html#autotoc_md73", [
        [ "Optional CALCEPH integration", "index.html#autotoc_md74", null ],
        [ "Optional NAIF CSPICE toolkit integration", "index.html#autotoc_md75", null ],
        [ "Universal ephemeris data / service integration", "index.html#autotoc_md76", null ],
        [ "Legacy support for (older) JPL major planet ephemerides", "index.html#autotoc_md77", [
          [ "Planets via <tt>eph_manager</tt>", "index.html#autotoc_md78", null ],
          [ "Planets via JPL's <tt>pleph</tt> FORTRAN interface", "index.html#autotoc_md79", null ]
        ] ],
        [ "Legacy linking of custom ephemeris functions", "index.html#autotoc_md80", null ]
      ] ],
      [ "Runtime debug support", "index.html#autotoc_md82", null ],
      [ "Release schedule", "index.html#autotoc_md84", null ],
      [ "[Unreleased]", "md_CHANGELOG.html#autotoc_md1", null ],
      [ "[1.2.0] - 2025-01-15", "md_CHANGELOG.html#autotoc_md2", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md3", null ],
        [ "Added", "md_CHANGELOG.html#autotoc_md4", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md5", null ]
      ] ],
      [ "[1.1.1] - 2024-10-28", "md_CHANGELOG.html#autotoc_md6", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md7", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md8", null ]
      ] ],
      [ "[1.1.0] - 2024-08-04", "md_CHANGELOG.html#autotoc_md9", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md10", null ],
        [ "Added", "md_CHANGELOG.html#autotoc_md11", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md12", null ]
      ] ],
      [ "[1.0.1] - 2024-05-13", "md_CHANGELOG.html#autotoc_md13", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md14", null ],
        [ "Added", "md_CHANGELOG.html#autotoc_md15", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md16", null ]
      ] ],
      [ "[1.0.0] - 2024-03-01", "md_CHANGELOG.html#autotoc_md17", [
        [ "Fixed", "md_CHANGELOG.html#autotoc_md18", null ],
        [ "Added", "md_CHANGELOG.html#autotoc_md19", null ],
        [ "Changed", "md_CHANGELOG.html#autotoc_md20", null ],
        [ "Deprecated", "md_CHANGELOG.html#autotoc_md21", null ]
      ] ]
    ] ],
    [ "Contributing to SuperNOVAS", "md_CONTRIBUTING.html", null ],
    [ "Astrometric Positions the Old Way", "md_LEGACY.html", [
      [ "Calculating positions for a sidereal source", "md_LEGACY.html#autotoc_md24", [
        [ "Specify the object of interest", "md_LEGACY.html#autotoc_md25", null ],
        [ "Spefify the observer location", "md_LEGACY.html#autotoc_md26", null ],
        [ "Specify the time of observation", "md_LEGACY.html#autotoc_md27", null ],
        [ "Specify Earth orientation parameters", "md_LEGACY.html#autotoc_md28", null ],
        [ "Calculate apparent positions on sky", "md_LEGACY.html#autotoc_md29", [
          [ "A. True apparent R.A. and declination", "md_LEGACY.html#autotoc_md30", null ],
          [ "B. Azimuth and elevation angles at the observing location", "md_LEGACY.html#autotoc_md31", null ]
        ] ]
      ] ],
      [ "Calculating positions for a Solar-system source", "md_LEGACY.html#autotoc_md32", null ]
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
"novas_8c.html#a8bd9c6d2127483551c98300e264977f5",
"novas_8h.html#a9e882d9c9055f187d3a8dc9b019f0609",
"structcat__entry.html#a2cc0369c1acae73cf6f3e06f1a8ddab8"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';