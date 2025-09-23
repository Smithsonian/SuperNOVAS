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
    [ "User's guide", "index.html#autotoc_md46", [
      [ "Table of Contents", "index.html#autotoc_md47", null ],
      [ "Introduction", "index.html#autotoc_md49", [
        [ "Related links", "index.html#autotoc_md50", null ]
      ] ],
      [ "Fixed NOVAS C 3.1 issues", "index.html#autotoc_md52", null ],
      [ "Compatibility with NOVAS C 3.1", "index.html#autotoc_md54", null ],
      [ "Building and installation", "index.html#autotoc_md56", [
        [ "Build SuperNOVAS using GNU make", "index.html#autotoc_md57", null ],
        [ "Build SuperNOVAS using CMake", "index.html#autotoc_md58", null ]
      ] ],
      [ "Building your application with SuperNOVAS", "index.html#autotoc_md60", [
        [ "Using a GNU Makefile", "index.html#autotoc_md61", null ],
        [ "Using CMake", "index.html#autotoc_md62", null ],
        [ "Deprecated API", "index.html#autotoc_md63", null ],
        [ "Legacy linking solarsystem() and readeph() modules", "index.html#autotoc_md64", null ],
        [ "Legacy modules: a better way", "index.html#autotoc_md65", null ]
      ] ],
      [ "Celestial coordinate systems (old vs. new)", "index.html#autotoc_md67", null ],
      [ "Example usage", "index.html#autotoc_md69", [
        [ "Calculating positions for a sidereal source", "index.html#autotoc_md70", [
          [ "Specify the object of interest", "index.html#autotoc_md71", null ],
          [ "Specify the observer location", "index.html#autotoc_md72", null ],
          [ "Specify the time of observation", "index.html#autotoc_md73", null ],
          [ "Set up the observing frame", "index.html#autotoc_md74", null ],
          [ "Calculate an apparent place on sky", "index.html#autotoc_md75", null ],
          [ "Calculate azimuth and elevation angles at the observing location", "index.html#autotoc_md76", null ],
          [ "Going in reverse...", "index.html#autotoc_md77", null ],
          [ "Calculate rise, set, and transit times", "index.html#autotoc_md78", null ]
        ] ],
        [ "Calculating positions for a Solar-system source", "index.html#autotoc_md79", [
          [ "Planets and/or ephemeris type objects", "index.html#autotoc_md80", null ],
          [ "Solar-system objects with Keplerian orbital parameters", "index.html#autotoc_md81", null ],
          [ "Approximate planet and Moon orbitals", "index.html#autotoc_md82", null ]
        ] ],
        [ "Coordinate and velocity transforms (change of coordinate system)", "index.html#autotoc_md83", null ]
      ] ],
      [ "Incorporating Solar-system ephemeris data or services", "index.html#autotoc_md85", [
        [ "CALCEPH integration", "index.html#autotoc_md86", null ],
        [ "NAIF CSPICE toolkit integration", "index.html#autotoc_md87", null ],
        [ "Universal ephemeris data / service integration", "index.html#autotoc_md88", null ],
        [ "Legacy support for (older) JPL major planet ephemerides", "index.html#autotoc_md89", null ],
        [ "Legacy linking of custom ephemeris functions", "index.html#autotoc_md90", null ]
      ] ],
      [ "Notes on precision", "index.html#autotoc_md92", [
        [ "Prerequisites to precise results", "index.html#autotoc_md93", null ]
      ] ],
      [ "Tips and tricks", "index.html#autotoc_md95", [
        [ "SuperNOVAS and C++", "index.html#autotoc_md96", null ],
        [ "Reduced accuracy shortcuts", "index.html#autotoc_md97", null ],
        [ "Multi-threaded calculations", "index.html#autotoc_md98", null ],
        [ "Physical units", "index.html#autotoc_md99", null ],
        [ "String times and angles", "index.html#autotoc_md100", null ],
        [ "String dates", "index.html#autotoc_md101", null ]
      ] ],
      [ "Runtime debug support", "index.html#autotoc_md103", null ],
      [ "Representative benchmarks", "index.html#autotoc_md105", null ],
      [ "SuperNOVAS added features", "index.html#autotoc_md107", [
        [ "New functionality highlights", "index.html#autotoc_md108", null ],
        [ "Refinements to the NOVAS C API", "index.html#autotoc_md109", null ]
      ] ],
      [ "Release schedule", "index.html#autotoc_md111", null ]
    ] ],
    [ "Changelog", "md__2github_2workspace_2CHANGELOG.html", [
      [ "[Unreleased]", "md__2github_2workspace_2CHANGELOG.html#autotoc_md1", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md2", null ],
        [ "Added", "md__2github_2workspace_2CHANGELOG.html#autotoc_md3", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md4", null ],
        [ "Deprecated", "md__2github_2workspace_2CHANGELOG.html#autotoc_md5", null ]
      ] ],
      [ "[1.4.2] - 2025-08-25", "md__2github_2workspace_2CHANGELOG.html#autotoc_md6", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md7", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md8", null ]
      ] ],
      [ "[1.4.1] - 2025-07-21", "md__2github_2workspace_2CHANGELOG.html#autotoc_md9", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md10", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md11", null ]
      ] ],
      [ "[1.4.0] - 2025-06-02", "md__2github_2workspace_2CHANGELOG.html#autotoc_md12", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md13", null ],
        [ "Added", "md__2github_2workspace_2CHANGELOG.html#autotoc_md14", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md15", null ],
        [ "Deprecated", "md__2github_2workspace_2CHANGELOG.html#autotoc_md16", null ]
      ] ],
      [ "[1.3.1] - 2025-05-07", "md__2github_2workspace_2CHANGELOG.html#autotoc_md17", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md18", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md19", null ],
        [ "Deprecated", "md__2github_2workspace_2CHANGELOG.html#autotoc_md20", null ]
      ] ],
      [ "[1.3.0] - 2025-04-15", "md__2github_2workspace_2CHANGELOG.html#autotoc_md21", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md22", null ],
        [ "Added", "md__2github_2workspace_2CHANGELOG.html#autotoc_md23", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md24", null ]
      ] ],
      [ "[1.2.0] - 2025-01-15", "md__2github_2workspace_2CHANGELOG.html#autotoc_md25", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md26", null ],
        [ "Added", "md__2github_2workspace_2CHANGELOG.html#autotoc_md27", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md28", null ]
      ] ],
      [ "[1.1.1] - 2024-10-28", "md__2github_2workspace_2CHANGELOG.html#autotoc_md29", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md30", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md31", null ]
      ] ],
      [ "[1.1.0] - 2024-08-04", "md__2github_2workspace_2CHANGELOG.html#autotoc_md32", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md33", null ],
        [ "Added", "md__2github_2workspace_2CHANGELOG.html#autotoc_md34", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md35", null ]
      ] ],
      [ "[1.0.1] - 2024-05-13", "md__2github_2workspace_2CHANGELOG.html#autotoc_md36", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md37", null ],
        [ "Added", "md__2github_2workspace_2CHANGELOG.html#autotoc_md38", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md39", null ]
      ] ],
      [ "[1.0.0] - 2024-03-01", "md__2github_2workspace_2CHANGELOG.html#autotoc_md40", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md41", null ],
        [ "Added", "md__2github_2workspace_2CHANGELOG.html#autotoc_md42", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md43", null ],
        [ "Deprecated", "md__2github_2workspace_2CHANGELOG.html#autotoc_md44", null ]
      ] ]
    ] ],
    [ "Contributing to SuperNOVAS", "md__2github_2workspace_2CONTRIBUTING.html", null ],
    [ "SuperNOVAS vs. astropy", "md_SuperNOVAS__vs__astropy.html", null ],
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
"novas_8h.html",
"novas_8h.html#a9178358732325399f3a5e3582080ced3",
"observer_8c.html#a1d58fd371e7604f75df315365672941e",
"structnovas__timespec.html"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';