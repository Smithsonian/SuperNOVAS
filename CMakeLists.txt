# CMake configuration file for the SuperNOVAS library
#
# Authors: Kiran Shila, Attila Kovacs
#
# Usage: 
#   
#  To build under a 'build/' directory:
#
#    $ cmake -B build [OPTIONS]
#    $ cmake --build build
#  
#  And then to install, e.g. under '/usr/local':
#    $ [sudo ] cmake --install --prefix=/usr/local
#
#  You may install just a specific component via the --component option, such
#  as 'Runtime', 'Development', or 'Data'.
#


cmake_minimum_required(VERSION 3.20)

# Project definition
project(SuperNOVAS
    VERSION 1.5.0
    DESCRIPTION "SuperNOVAS astrometry library"
    HOMEPAGE_URL "https://smithsonian.github.io/SuperNOVAS/"
    LANGUAGES C
)

# Include required modules
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(FeatureSummary)
include(CheckLibraryExists)

# Build options
option(BUILD_SHARED_LIBS "Build as shared libraries instead of static" OFF)
option(BUILD_DOC "Build HTML documetation" OFF)
option(BUILD_TESTING "Build regression test suite" ON)
option(BUILD_EXAMPLES "Build example programs" OFF)
option(BUILD_BENCHMARK "Build benchmark programs" OFF)
option(ENABLE_CALCEPH "Enable CALCEPH support" OFF)
option(ENABLE_CSPICE "Enable CSPICE support" OFF)
option(ENABLE_SOLSYS1 "Enable legacy support for DE200 -- DE421 via eph_manager()" OFF)
option(ENABLE_SOLSYS2 "Enable legacy support for custom Fortran PLEPH plugin" OFF)

# Set feature descriptions for summary
add_feature_info(SharedLibs BUILD_SHARED_LIBS "Build as shared libraries")
add_feature_info(Examples BUILD_EXAMPLES "Build and test example programs")
add_feature_info(Testing BUILD_TESTING "Run regression testing")
add_feature_info(Calceph-Plugin ENABLE_CALCEPH "Optional ephemeris support via CALCEPH")
add_feature_info(CSPICE-Plugin ENABLE_CSPICE "Optional ephemeris support via CSPICE")
add_feature_info(Solsys1-Plugin ENABLE_SOLSYS1 "Legacy ephemeris support via eph_manager()")
add_feature_info(Solsys2-Plugin ENABLE_SOLSYS2 "Legacy ephemeris support via Fortran PLEPH library")
add_feature_info(Documentation BUILD_DOC "Developer documentation (HTML)")
add_feature_info(Benchmarks BUILD_BENCHMARK "Build benchmarking programs")


# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Debug postfix for libraries
set(CMAKE_DEBUG_POSTFIX d)

# Position Independent Code for shared libs
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Windows-specific settings
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# Sources
set(SUPERNOVAS_CORE_SOURCES
    src/target.c
    src/observer.c
    src/earth.c
    src/equinox.c
    src/system.c
    src/transform.c
    src/cio.c
    src/orbital.c
    src/spectral.c
    src/grav.c
    src/nutation.c
    src/timescale.c
    src/frames.c
    src/place.c
    src/calendar.c
    src/refract.c
    src/naif.c
    src/parse.c
    src/plugin.c
    src/util.c
    src/planets.c
    src/itrf.c
    src/solsys3.c
    src/solsys-ephem.c
)

# Preprocessor definitions
set(SUPERNOVAS_COMPILE_DEFINITIONS
    BUILTIN_SOLSYS3=1
    BUILTIN_SOLSYS_EPHEM_READER=1
    DEFAULT_SOLSYS=3
)

# Check for math library
check_library_exists(m sin "" HAVE_LIBM)

# CALCEPH plugin
if(ENABLE_CALCEPH)
    find_package(calceph COMPONENTS Development REQUIRED CONFIG)
    set_package_properties(calceph PROPERTIES
        URL "https://calceph.imcce.fr"
        DESCRIPTION  "is designed to access the binary planetary ephemeris files"
        TYPE REQUIRED
        PURPOSE "Provides ephemeris support for SuperNOVAS"
    )
    list(APPEND SUPERNOVAS_COMPILE_DEFINITIONS USE_CALCEPH=1)
    set(CALCEPH_FOUND TRUE)
    message(STATUS "Found calceph")
endif()

# CSPICE plugin
if(ENABLE_CSPICE)
    find_library(cspice cspice REGISTRY_VIEW TARGET REQUIRED)
    list(APPEND SUPERNOVAS_COMPILE_DEFINITIONS USE_CSPICE=1)
    set(CSPICE_FOUND TRUE)
    message(STATUS "Found cspice")
endif()

# ----------------------------------------------------------------------------
# Build the 'supernovas' library as a static OR as a shared lib.
# depending on BUILD_SHARED_LIBS
add_library(supernovas ${SUPERNOVAS_CORE_SOURCES})

# Alias to SuperNOVAS:: namespace
add_library(SuperNOVAS::supernovas ALIAS supernovas)

set_target_properties(supernovas PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    POSITION_INDEPENDENT_CODE ON
    EXPORT_NAME supernovas
    OUTPUT_NAME supernovas
    CLEAN_DIRECT_OUTPUT ON
)

target_include_directories(supernovas PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include/eop
    ${CMAKE_CURRENT_SOURCE_DIR}/include/nutation
)

target_include_directories(supernovas PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_definitions(supernovas PRIVATE ${SUPERNOVAS_COMPILE_DEFINITIONS})

# ----------------------------------------------------------------------------
if(CMAKE_BUILD_TYPE)
    # Compiler-specific warnings and flags
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(supernovas PRIVATE
            $<$<CONFIG:Debug>:-g;-O0;-Wall;-Wextra;-Wpedantic;-Wconversion;-Wshadow>
            $<$<CONFIG:Release>:-O3;-DNDEBUG;-Wall;-Wextra>
            $<$<CONFIG:RelWithDebInfo>:-O2;-g;-DNDEBUG;-Wall;-Wextra>
            $<$<CONFIG:MinSizeRel>:-Os;-DNDEBUG>
        )

        # Additional security flags
        if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
            target_compile_options(supernovas PRIVATE
                -fstack-protector-strong
                -D_FORTIFY_SOURCE=2
            )
        endif()
    elseif(MSVC)
        target_compile_options(supernovas PRIVATE
            /W4
            /permissive-
            $<$<CONFIG:Debug>:/Zi;/Od;/RTC1>
            $<$<CONFIG:Release>:/O2;/DNDEBUG>
        )
    endif()
endif()

# Link with math libs as necessary
target_link_libraries(supernovas
    PRIVATE
        $<$<BOOL:${HAVE_LIBM}>:m>
        $<$<BOOL:${CMAKE_THREAD_LIBS_INIT}>:${CMAKE_THREAD_LIBS_INIT}>
)

# Compatibility alias
add_library(novas ALIAS supernovas)

# ----------------------------------------------------------------------------
# Plugins

# configure plugin build (w/o linking)
function(config_plugin PLUGIN)
    add_library(${PLUGIN} src/${PLUGIN}.c)
    add_library(SuperNOVAS::${PLUGIN} ALIAS ${PLUGIN})

    set_target_properties(${PLUGIN} PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        POSITION_INDEPENDENT_CODE ON
    )

    target_include_directories(${PLUGIN} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_compile_definitions(${PLUGIN} PRIVATE ${SUPERNOVAS_COMPILE_DEFINITIONS})
endfunction()

# plugins via external libraries (e.g. calceph or cspice). 
function(solsys_plugin PLUGIN_LIB)
    config_plugin(solsys-${PLUGIN_LIB})
    
    target_link_libraries(solsys-${PLUGIN_LIB} PRIVATE
        supernovas
        ${PLUGIN_LIB}
        $<$<BOOL:${HAVE_LIBM}>:m>
    )
endfunction()

# legacy solsys[1-2] plugins
function(legacy_solsys_plugin IDX)
    config_plugin(solsys${IDX})

    target_link_libraries(solsys${IDX} PRIVATE
        supernovas
        $<$<BOOL:${HAVE_LIBM}>:m>
    )
endfunction()

# Process plugin options...
if(ENABLE_CALCEPH)
    solsys_plugin(calceph)
endif()

if(ENABLE_CSPICE)
    solsys_plugin(cspice)
endif()

if(ENABLE_SOLSYS1)
    legacy_solsys_plugin(1)
endif()

if(ENABLE_SOLSYS2)
    legacy_solsys_plugin(2)
endif()

# ----------------------------------------------------------------------------
# Testing, examples, and benchmarks...

# Run tests for the enabled options below...
enable_testing()

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Testing
if(BUILD_TESTING)
    add_subdirectory(test)
endif()

# Benchmarking
if(BUILD_BENCHMARK)
    add_subdirectory(benchmark)
endif()

if(BUILD_DOC) 
    find_package(Doxygen)
        set_package_properties(Doxygen PROPERTIES
        URL "https://doxygen.nl/"
        DESCRIPTION  "is a widely-used documentation generator tool in  software development"
        TYPE REQUIRED
        PURPOSE "Enables HTML documentation for SuperNOVAS"
    )

    if(Doxygen_FOUND)
        add_executable(docedit EXCLUDE_FROM_ALL src/docedit.c)
   
        # Generate headless docs and run Doxygen on them.
        add_custom_target(project_docs 
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
            COMMAND docedit
            COMMAND Doxygen::doxygen Doxyfile.local
        )
        
        # Clean up generated docs after doxygen
        add_custom_target(clean_docs)
        if(TARGET clean_docs)
           file(REMOVE Doxyfile.local README-orig.md)
        endif()

        add_dependencies(supernovas clean_docs)
        add_dependencies(clean_docs project_docs)
    endif()
endif()

# ----------------------------------------------------------------------------
# Installation
set(INSTALL_TARGETS supernovas)
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME Runtime)

if(ENABLE_CALCEPH)
    list(APPEND INSTALL_TARGETS solsys-calceph)
endif()

if(ENABLE_CSPICE)
    list(APPEND INSTALL_TARGETS solsys-cspice)
endif()

if(ENABLE_SOLSYS1)
    list(APPEND INSTALL_TARGETS solsys1)
endif()

if(ENABLE_SOLSYS2)
    list(APPEND INSTALL_TARGETS solsys2)
endif()

install(TARGETS ${INSTALL_TARGETS}
    EXPORT SuperNOVASTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(BUILD_DOC AND TARGET project_docs)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
        DESTINATION ${CMAKE_INSTALL_DOCDIR}/supernovas COMPONENT Development
    )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/supernovas.tag
        DESTINATION ${CMAKE_INSTALL_DOCDIR}/supernovas COMPONENT Development
    )
endif()

# ------------------------------------------------------------------------
# Install files for the Data component

install(FILES data/CIO_RA.TXT
    COMPONENT Data
    DESTINATION ${CMAKE_INSTALL_DATADIR}/supernovas
)

# ------------------------------------------------------------------------
# Install files for the Development component

install(FILES include/novas.h include/nutation.h include/solarsystem.h
    COMPONENT Development
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(ENABLE_CALCEPH)
   install(FILES include/novas-calceph.h
       COMPONENT Development
       DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
   )
endif()

if(ENABLE_CSPICE)
   install(FILES include/novas-cspice.h
       COMPONENT Development
       DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
   )
endif()

if(ENABLE_SOLSYS1)
   install(FILES include/eph_manager.h
       COMPONENT Development
       DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
   )
endif()

install(EXPORT SuperNOVASTargets
    COMPONENT Development
    FILE SuperNOVASTargets.cmake
    NAMESPACE SuperNOVAS::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SuperNOVAS
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/SuperNOVASConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SuperNOVASConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SuperNOVAS
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/SuperNOVASConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/SuperNOVASConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/SuperNOVASConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SuperNOVAS
    COMPONENT Development
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/supernovas.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/supernovas.pc
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/supernovas.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    COMPONENT Development
)

# ----------------------------------------------------------------------------
# Uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
    )

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    )
endif()

# ----------------------------------------------------------------------------
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE SuperNOVAS)

# ----------------------------------------------------------------------------
# Summary
feature_summary(WHAT ALL)
message(STATUS "")
message(STATUS "SuperNOVAS Configuration Summary:")
message(STATUS "  Version:           ${PROJECT_VERSION}")
if(CMAKE_BUILD_TYPE)
    message(STATUS "  Build type:        ${CMAKE_BUILD_TYPE}")
else()
    message(STATUS "  Build type:        (default) -- external CFLAGS only")
endif()
message(STATUS "  Install prefix:    ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Compiler:          ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "")

