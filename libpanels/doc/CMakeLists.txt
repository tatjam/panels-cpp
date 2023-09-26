# NOTE: This CMake file is only needed to build library documentation. The library is header only!
#       and doesn't need anything other than including the headers in the program.
# Simply include the doc directory into your CMake, and the options to build the libpanels docs will
# appears in your build targets.

option (BUILD_LIBPANELS_DOC "Build libpanels documentation" ON)

# based on https://vicrucann.github.io/tutorials/quick-cmake-doxygen/
find_package(Doxygen)
message("current source: " ${CMAKE_CURRENT_SOURCE_DIR}})
if(DOXYGEN_FOUND AND BUILD_LIBPANELS_DOC)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Writing libpanels docs to: " ${CMAKE_CURRENT_BINARY_DIR})
    add_custom_target(libpanels_doc_doxygen
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            VERBATIM)
else(BUILD_LIBPANELS_DOC)
    message("Doxygen is required to generate library documentation")
endif(DOXYGEN_FOUND AND BUILD_LIBPANELS_DOC)