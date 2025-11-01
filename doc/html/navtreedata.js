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
    [ "User's guide", "index.html#autotoc_md66", [
      [ "Table of Contents", "index.html#autotoc_md67", null ],
      [ "Introduction", "index.html#autotoc_md69", [
        [ "Related links", "index.html#autotoc_md70", null ]
      ] ],
      [ "Fixed NOVAS C 3.1 issues", "index.html#autotoc_md72", null ],
      [ "Compatibility with NOVAS C 3.1", "index.html#autotoc_md74", null ],
      [ "Building and installation", "index.html#autotoc_md76", [
        [ "Build SuperNOVAS using GNU make", "index.html#autotoc_md77", null ],
        [ "Build SuperNOVAS using CMake", "index.html#autotoc_md78", null ],
        [ "Install SuperNOVAS via vcpkg", "index.html#autotoc_md79", null ],
        [ "Linux packages", "index.html#autotoc_md80", null ],
        [ "Homebrew package", "index.html#autotoc_md81", null ]
      ] ],
      [ "Building your application with SuperNOVAS", "index.html#autotoc_md83", [
        [ "Using a GNU Makefile", "index.html#autotoc_md84", null ],
        [ "Using CMake", "index.html#autotoc_md85", null ],
        [ "Deprecated API", "index.html#autotoc_md86", null ],
        [ "Legacy linking solarsystem() / solarsystem_hp() and readeph() modules", "index.html#autotoc_md87", null ],
        [ "Legacy modules: a better way", "index.html#autotoc_md88", null ]
      ] ],
      [ "Celestial coordinate systems (old vs. new)", "index.html#autotoc_md90", null ],
      [ "Example usage", "index.html#autotoc_md92", [
        [ "Calculating positions for a sidereal source", "index.html#autotoc_md93", [
          [ "Specify the object of interest", "index.html#autotoc_md94", null ],
          [ "Specify the observer location", "index.html#autotoc_md95", null ],
          [ "Specify the time of observation", "index.html#autotoc_md96", null ],
          [ "Set up the observing frame", "index.html#autotoc_md97", null ],
          [ "Calculate an apparent place on sky", "index.html#autotoc_md98", null ],
          [ "Calculate azimuth and elevation angles at the observing location", "index.html#autotoc_md99", null ],
          [ "Going in reverse...", "index.html#autotoc_md100", null ],
          [ "Calculate rise, set, and transit times", "index.html#autotoc_md101", null ]
        ] ],
        [ "Calculating positions for a Solar-system source", "index.html#autotoc_md102", [
          [ "Planets and/or ephemeris type objects", "index.html#autotoc_md103", null ],
          [ "Solar-system objects with Keplerian orbital parameters", "index.html#autotoc_md104", null ],
          [ "Approximate planet and Moon orbitals", "index.html#autotoc_md105", null ]
        ] ],
        [ "Coordinate and velocity transforms (change of coordinate system)", "index.html#autotoc_md106", null ]
      ] ],
      [ "Incorporating Solar-system ephemeris data or services", "index.html#autotoc_md108", [
        [ "CALCEPH integration", "index.html#autotoc_md109", null ],
        [ "NAIF CSPICE toolkit integration", "index.html#autotoc_md110", null ],
        [ "Universal ephemeris data / service integration", "index.html#autotoc_md111", null ]
      ] ],
      [ "Notes on precision", "index.html#autotoc_md112", [
        [ "Prerequisites to precise results", "index.html#autotoc_md113", null ]
      ] ],
      [ "Tips and tricks", "index.html#autotoc_md115", [
        [ "SuperNOVAS and C++", "index.html#autotoc_md116", null ],
        [ "Reduced accuracy shortcuts", "index.html#autotoc_md117", null ],
        [ "Multi-threaded calculations", "index.html#autotoc_md118", null ],
        [ "Physical units", "index.html#autotoc_md119", null ],
        [ "String times and angles", "index.html#autotoc_md120", null ],
        [ "String dates", "index.html#autotoc_md121", null ]
      ] ],
      [ "Runtime debug support", "index.html#autotoc_md123", null ],
      [ "Representative benchmarks", "index.html#autotoc_md125", null ],
      [ "SuperNOVAS added features", "index.html#autotoc_md127", [
        [ "New functionality highlights", "index.html#autotoc_md128", null ],
        [ "Refinements to the NOVAS C API", "index.html#autotoc_md129", null ]
      ] ],
      [ "Release schedule", "index.html#autotoc_md131", null ]
    ] ],
    [ "Changelog", "md__2github_2workspace_2CHANGELOG.html", [
      [ "[Unreleased]", "md__2github_2workspace_2CHANGELOG.html#autotoc_md18", [
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md19", null ]
      ] ],
      [ "[1.5.0] - 2025-10-29", "md__2github_2workspace_2CHANGELOG.html#autotoc_md20", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md21", null ],
        [ "Added", "md__2github_2workspace_2CHANGELOG.html#autotoc_md22", null ],
        [ "Removed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md23", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md24", null ],
        [ "Deprecated", "md__2github_2workspace_2CHANGELOG.html#autotoc_md25", null ]
      ] ],
      [ "[1.4.2] - 2025-08-25", "md__2github_2workspace_2CHANGELOG.html#autotoc_md26", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md27", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md28", null ]
      ] ],
      [ "[1.4.1] - 2025-07-21", "md__2github_2workspace_2CHANGELOG.html#autotoc_md29", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md30", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md31", null ]
      ] ],
      [ "[1.4.0] - 2025-06-02", "md__2github_2workspace_2CHANGELOG.html#autotoc_md32", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md33", null ],
        [ "Added", "md__2github_2workspace_2CHANGELOG.html#autotoc_md34", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md35", null ],
        [ "Deprecated", "md__2github_2workspace_2CHANGELOG.html#autotoc_md36", null ]
      ] ],
      [ "[1.3.1] - 2025-05-07", "md__2github_2workspace_2CHANGELOG.html#autotoc_md37", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md38", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md39", null ],
        [ "Deprecated", "md__2github_2workspace_2CHANGELOG.html#autotoc_md40", null ]
      ] ],
      [ "[1.3.0] - 2025-04-15", "md__2github_2workspace_2CHANGELOG.html#autotoc_md41", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md42", null ],
        [ "Added", "md__2github_2workspace_2CHANGELOG.html#autotoc_md43", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md44", null ]
      ] ],
      [ "[1.2.0] - 2025-01-15", "md__2github_2workspace_2CHANGELOG.html#autotoc_md45", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md46", null ],
        [ "Added", "md__2github_2workspace_2CHANGELOG.html#autotoc_md47", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md48", null ]
      ] ],
      [ "[1.1.1] - 2024-10-28", "md__2github_2workspace_2CHANGELOG.html#autotoc_md49", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md50", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md51", null ]
      ] ],
      [ "[1.1.0] - 2024-08-04", "md__2github_2workspace_2CHANGELOG.html#autotoc_md52", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md53", null ],
        [ "Added", "md__2github_2workspace_2CHANGELOG.html#autotoc_md54", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md55", null ]
      ] ],
      [ "[1.0.1] - 2024-05-13", "md__2github_2workspace_2CHANGELOG.html#autotoc_md56", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md57", null ],
        [ "Added", "md__2github_2workspace_2CHANGELOG.html#autotoc_md58", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md59", null ]
      ] ],
      [ "[1.0.0] - 2024-03-01", "md__2github_2workspace_2CHANGELOG.html#autotoc_md60", [
        [ "Fixed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md61", null ],
        [ "Added", "md__2github_2workspace_2CHANGELOG.html#autotoc_md62", null ],
        [ "Changed", "md__2github_2workspace_2CHANGELOG.html#autotoc_md63", null ],
        [ "Deprecated", "md__2github_2workspace_2CHANGELOG.html#autotoc_md64", null ]
      ] ]
    ] ],
    [ "Contributing to SuperNOVAS", "md__2github_2workspace_2CONTRIBUTING.html", null ],
    [ "SuperNOVAS vs. astropy", "md_SuperNOVAS__vs__astropy.html", null ],
    [ "Deprecated List", "deprecated.html", null ],
    [ "Topics", "topics.html", "topics" ],
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
"group__observer.html#gab07d86d425d68a62e94e965dd36b7a32",
"group__time.html#ggaa8cfc18bfdf8ebd0198065c9d07e3bbaa90f19bf1771802f4ad20cf14660c8322",
"place_8c.html#adcb9b273dfda5d97a1c01379a16997f9"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';