# Additional CMake configuration file for building the example programs of the 
# supernovas library. The examples are also added to the test suite so they 
# may be checked for functionality.
#
# To invoke simply configure the cmake build in the supernovas directory with
# the -DBUILD_EXAMPLES=ON option.
#
# Author: Attila Kovacs

cmake_minimum_required(VERSION 3.20)

# Configuration in case this is a standalone project....
if(NOT PROJECT_NAME)
    project(supernovas-examples 
        VERSION 1.5.0
        DESCRIPTION "SuperNOVAS example programs"
        HOMEPAGE_URL "https://smithsonian.github.io/SuperNOVAS/"
        LANGUAGES C
    )
    
    include(GNUInstallDirs)
        
    find_package(supernovas)

    if(WIN32)
        set(MATH "")
    else()
        find_library(MATH_LIBRARY m REQUIRED)
        set(MATH m)
    endif()
endif()

# List of example programs to build
set(EXAMPLE_PROGRAMS
    example-star
    example-time
    example-high-z
    example-orbital
    example-rise-set
)

if(SUPERNOVAS_solsys-calceph_FOUND)
    list(APPEND EXAMPLE_PROGRAMS example-calceph)
endif()

if(SUPERNOVAS_solsys-cspice_FOUND)
    list(APPEND EXAMPLE_PROGRAMS example-cspice)
endif()

# The ephemeris data to use for CALCEPH / CSPICE testing
set(EPHEM ${PROJECT_SOURCE_DIR}/test/ephem/de440s-j2000.bsp)

message("include dirs is ${SUPERNOVAS_INCLIDE_DIRS}")

# Build each example
foreach(EXAMPLE ${EXAMPLE_PROGRAMS})
    set(EXAMPLE_SOURCE ${EXAMPLE}.c)

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_SOURCE})
        add_executable(${EXAMPLE} ${EXAMPLE_SOURCE})

        target_include_directories(${EXAMPLE} PRIVATE ${SUPERNOVAS_INCLUDE_DIRS})

        # Link against the supernovas library
        target_link_libraries(${EXAMPLE} PRIVATE
            supernovas::core
            ${MATH}
        )

        # Special handling for CALCEPH example
        if(${EXAMPLE} STREQUAL "example-calceph")
            find_dependency(calceph)
        
            target_link_libraries(${EXAMPLE} PRIVATE
                supernovas::solsys-calceph
                calceph
            )
            #add_test(NAME ${EXAMPLE} COMMAND ${EXAMPLE} ${EPHEM})
            
        # Special handling for CSPICE example
        elseif(${EXAMPLE} STREQUAL "example-cspice")
       	    find_dependency(cspice)
        
            target_link_libraries(${EXAMPLE} PRIVATE
                supernovas::solsys-cspice
                cspice
            )
            #add_test(NAME ${EXAMPLE} COMMAND ${EXAMPLE} ${EPHEM})
        
        # All other tests
        else()
            add_test(NAME ${EXAMPLE} COMMAND ${EXAMPLE})
        endif()

    else()
        message(WARNING "Source file ${EXAMPLE_SOURCE} not found - ${EXAMPLE} will not be built")
    endif()
endforeach()
