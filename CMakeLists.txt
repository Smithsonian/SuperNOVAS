cmake_minimum_required(VERSION 3.20)

# Project definition
project(SuperNOVAS
    VERSION 1.5.0
    DESCRIPTION "SuperNOVAS astrometry library"
    HOMEPAGE_URL "https://smithsonian.github.io/SuperNOVAS/"
    LANGUAGES C
)

# Default build type if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    message(STATUS "Setting build type to 'Release' as none was specified.")
endif()

# Include required modules
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(FeatureSummary)
include(CheckLibraryExists)

# Build options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_STATIC_LIBS "Build static libraries in addition to shared" ON)
option(BUILD_EXAMPLES "Build example programs" ON)
option(ENABLE_CALCEPH "Enable CALCEPH support" OFF)
option(ENABLE_INSTALL "Enable installation" ON)

# Set feature descriptions for summary
add_feature_info(SharedLibs BUILD_SHARED_LIBS "Build shared libraries")
add_feature_info(StaticLibs BUILD_STATIC_LIBS "Build static libraries")
add_feature_info(Examples BUILD_EXAMPLES "Build example programs")
add_feature_info(CALCEPH ENABLE_CALCEPH "CALCEPH ephemeris support")

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Debug postfix for libraries
set(CMAKE_DEBUG_POSTFIX d)

# Position Independent Code for shared libs
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# RPATH settings for installed binaries
#if(NOT APPLE)
#    set(CMAKE_INSTALL_RPATH ${ORIGIN}/../${CMAKE_INSTALL_LIBDIR})
#else()
#    set(CMAKE_INSTALL_RPATH @loader_path/../${CMAKE_INSTALL_LIBDIR})
#endif()
#set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

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

# CALCEPH
if(ENABLE_CALCEPH)
    find_package(calceph CONFIG REQUIRED)
    list(APPEND SUPERNOVAS_COMPILE_DEFINITIONS USE_CALCEPH=1)
    set(CALCEPH_FOUND TRUE)
    message(STATUS "Found calceph")
endif()

# Find threads (may be needed for some platforms)
find_package(Threads)

# Main library target
add_library(supernovas ${SUPERNOVAS_CORE_SOURCES})
add_library(SuperNOVAS::supernovas ALIAS supernovas)

set_target_properties(supernovas PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    POSITION_INDEPENDENT_CODE ON
    EXPORT_NAME supernovas
    OUTPUT_NAME supernovas
    CLEAN_DIRECT_OUTPUT ON
)

target_include_directories(supernovas
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(supernovas PRIVATE ${SUPERNOVAS_COMPILE_DEFINITIONS})

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

# Link libraries
target_link_libraries(supernovas
    PRIVATE
        $<$<BOOL:${HAVE_LIBM}>:m>
        $<$<BOOL:${CMAKE_THREAD_LIBS_INIT}>:${CMAKE_THREAD_LIBS_INIT}>
)

# Optional static build
if(BUILD_STATIC_LIBS)
    add_library(supernovas_static STATIC ${SUPERNOVAS_CORE_SOURCES})
    add_library(SuperNOVAS::supernovas_static ALIAS supernovas_static)

    set_target_properties(supernovas_static PROPERTIES
        OUTPUT_NAME supernovas
        VERSION ${PROJECT_VERSION}
        POSITION_INDEPENDENT_CODE ON
        EXPORT_NAME supernovas_static
        CLEAN_DIRECT_OUTPUT ON
    )

    target_include_directories(supernovas_static PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    target_compile_definitions(supernovas_static PRIVATE
        ${SUPERNOVAS_COMPILE_DEFINITIONS}
    )

    target_compile_options(supernovas_static PRIVATE
        $<$<C_COMPILER_ID:GNU,Clang>:${CMAKE_C_COMPILE_OPTIONS_PIC}>
    )

    target_link_libraries(supernovas_static
        PRIVATE
            $<$<BOOL:${HAVE_LIBM}>:m>
            $<$<BOOL:${CMAKE_THREAD_LIBS_INIT}>:${CMAKE_THREAD_LIBS_INIT}>
    )
endif()

# Compatibility alias
add_library(novas ALIAS supernovas)

# Plugins
if(ENABLE_CALCEPH)
    add_library(solsys-calceph SHARED src/solsys-calceph.c)
    add_library(SuperNOVAS::solsys-calceph ALIAS solsys-calceph)

    set_target_properties(solsys-calceph PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        POSITION_INDEPENDENT_CODE ON
    )

    target_include_directories(solsys-calceph PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    target_compile_definitions(solsys-calceph PRIVATE ${SUPERNOVAS_COMPILE_DEFINITIONS})

    target_link_libraries(solsys-calceph PRIVATE
        supernovas
        calceph
        $<$<BOOL:${HAVE_LIBM}>:m>
    )
endif()

# CIO data generator
add_executable(cio_file src/cio_file.c)
target_include_directories(cio_file PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(cio_file PRIVATE $<$<BOOL:${HAVE_LIBM}>:m>)

set(CIO_INPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/data/CIO_RA.TXT)
set(CIO_OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/cio_ra.bin)

add_custom_command(
    OUTPUT ${CIO_OUTPUT_FILE}
    COMMAND cio_file ${CIO_INPUT_FILE} ${CIO_OUTPUT_FILE}
    DEPENDS cio_file ${CIO_INPUT_FILE}
    COMMENT "Generating CIO binary data file"
    VERBATIM
)

add_custom_target(cio_data ALL DEPENDS ${CIO_OUTPUT_FILE})

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Installation
if(ENABLE_INSTALL)
    set(INSTALL_TARGETS supernovas)
    if(BUILD_STATIC_LIBS AND TARGET supernovas_static)
        list(APPEND INSTALL_TARGETS supernovas_static)
    endif()
    if(ENABLE_CALCEPH)
        list(APPEND INSTALL_TARGETS solsys-calceph)
    endif()

    install(TARGETS ${INSTALL_TARGETS}
        EXPORT SuperNOVASTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.h"
    )

    install(FILES
        data/CIO_RA.TXT
        ${CIO_OUTPUT_FILE}
        DESTINATION ${CMAKE_INSTALL_DATADIR}/supernovas
    )

    install(EXPORT SuperNOVASTargets
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
    )

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/supernovas.pc.in
        ${CMAKE_CURRENT_BINARY_DIR}/supernovas.pc
        @ONLY
    )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/supernovas.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
endif()

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

# Summary
feature_summary(WHAT ALL)
message(STATUS "")
message(STATUS "SuperNOVAS Configuration Summary:")
message(STATUS "  Version:           ${PROJECT_VERSION}")
message(STATUS "  Build type:        ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install prefix:    ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Compiler:          ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "")
message(STATUS "Options:")
message(STATUS "  Shared libraries:  ${BUILD_SHARED_LIBS}")
message(STATUS "  Static libraries:  ${BUILD_STATIC_LIBS}")
if(ENABLE_CALCEPH)
    message(STATUS "  CALCEPH support:   YES")
else()
    message(STATUS "  CALCEPH support:   NO")
endif()
message(STATUS "")
message(STATUS "Detected:")
if(HAVE_LIBM)
    message(STATUS "  Math library:      Found")
else()
    message(STATUS "  Math library:      Not required")
endif()
if(CMAKE_THREAD_LIBS_INIT)
    message(STATUS "  Threads:           ${CMAKE_THREAD_LIBS_INIT}")
elseif(CMAKE_USE_PTHREADS_INIT)
    message(STATUS "  Threads:           pthreads")
elseif(CMAKE_USE_WIN32_THREADS_INIT)
    message(STATUS "  Threads:           Win32 threads")
else()
    message(STATUS "  Threads:           Built-in")
endif()
message(STATUS "")
