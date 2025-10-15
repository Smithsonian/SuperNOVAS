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
    
    if(WIN32)
       # Windows-specific settings
       message("Configuring for Windows...")
       set(LIBM "")
       add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    else()
        if (MINGW)
            # No need to link agains math lib on MinGW
            set(LIBM "")
        else()
            # Link against math libs on all other platforms
            find_library(LIBM m)
        endif()
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

if(ENABLE_CALCEPH)
    list(APPEND EXAMPLE_PROGRAMS example-calceph)
endif()

if(ENABLE_CSPICE)
    list(APPEND EXAMPLE_PROGRAMS example-cspice)
endif()

# Link against binaries in the local supernovas lib/ directory
link_directories(${CMAKE_BINARY_DIR}/lib)

# The ephemeris data to use for CALCEPH / CSPICE testing
set(EPHEM ${PROJECT_SOURCE_DIR}/test/ephem/de440s-j2000.bsp)

# Build each example
foreach(EXAMPLE ${EXAMPLE_PROGRAMS})
    set(EXAMPLE_SOURCE ${EXAMPLE}.c)

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_SOURCE})
        add_executable(${EXAMPLE} ${EXAMPLE_SOURCE})

        # Link against the supernovas library
        target_link_libraries(${EXAMPLE} PRIVATE
            supernovas
            ${LIBM}
        )

        # Special handling for CALCEPH example
        if(${EXAMPLE} STREQUAL "example-calceph")
            target_link_libraries(${EXAMPLE} PRIVATE
                solsys-calceph
                calceph
            )
            #add_test(NAME ${EXAMPLE} COMMAND ${EXAMPLE} ${EPHEM})
            
        # Special handling for CSPICE example
        elseif(${EXAMPLE} STREQUAL "example-cspice")
            target_link_libraries(${EXAMPLE} PRIVATE
                solsys-cspice
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
