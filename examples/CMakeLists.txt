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

# Build each example
foreach(EXAMPLE ${EXAMPLE_PROGRAMS})
    set(EXAMPLE_SOURCE ${EXAMPLE}.c)

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${EXAMPLE_SOURCE})
        add_executable(${EXAMPLE} ${EXAMPLE_SOURCE})

        # Link against the supernovas library
        target_link_libraries(${EXAMPLE} PRIVATE
            supernovas
            $<$<BOOL:${HAVE_LIBM}>:m>
        )

        # Special handling for CALCEPH example
        if(${EXAMPLE} STREQUAL "example-calceph")
            target_link_libraries(${EXAMPLE} PRIVATE
                solsys-calceph
                calceph
            )
        endif()

        # Set RPATH for the example to find the library
        if(NOT WIN32)
            set_target_properties(${EXAMPLE} PROPERTIES
                INSTALL_RPATH "$ORIGIN/../lib"
                BUILD_RPATH "${CMAKE_BINARY_DIR}/lib"
            )
        endif()

        # Add a custom target to run this specific example
        add_custom_target(run-${EXAMPLE}
            COMMAND ${EXAMPLE}
            DEPENDS ${EXAMPLE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Running ${EXAMPLE}"
            VERBATIM
        )

        message(STATUS "Added example: ${EXAMPLE}")
    else()
        message(WARNING "Source file ${EXAMPLE_SOURCE} not found - ${EXAMPLE} will not be built")
    endif()
endforeach()
