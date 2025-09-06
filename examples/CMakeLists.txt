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

link_directories(${CMAKE_BINARY_DIR}/lib)

set(EPHEM ${PROJECT_SOURCE_DIR}/test/ephem/de440s-j2000.bsp)

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
