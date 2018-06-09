cmake_minimum_required(VERSION 3.3)

project(avcpp)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

SET(MAJOR_VERSION "0")
SET(MINOR_VERSION "2")
SET(PATCH_VERSION "3")

# -----------------------   Options and Parameters  -----------------------

option(build_tests "Build all unit tests." OFF)
option(build_samples "Build samples." OFF)
option(build_documentation "Build the project documentation." OFF)
option(BUILD_SHARED_LIBS "Build shared linked library." OFF)
option(BUILD_WITH_CONAN "Use Conan package mangager." ON)

SET(DOCFILES "${CMAKE_CURRENT_SOURCE_DIR}/docs/" CACHE PATH "The path to the documentation files.")
SET(TESTFILES "/testfiles" CACHE TESTFILES "The path to the testfiles.")

SET(LIBAVCPP_VERSION "master" CACHE STRING "The tagged version.")

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set (THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads REQUIRED)

if(!BUILD_SHARED_LIBS)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    SET(CMAKE_EXE_LINKER_FLAGS "-static")
endif(!BUILD_SHARED_LIBS)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

add_definitions(-DTESTFILES="${TESTFILES}/")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conanfile.py.in ${CMAKE_CURRENT_SOURCE_DIR}/conanfile.py @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/.travis.yml.in ${CMAKE_CURRENT_SOURCE_DIR}/.travis.yml @ONLY)

# ---------------------   External Dependencies       ---------------------

if( ${BUILD_WITH_CONAN} )
    message( "conan packages" )
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
    SET(LIBRARIES ${LIBRARIES} ${CONAN_LIBS})
else( ${BUILD_WITH_CONAN} )
    #find_package(OpenSSL REQUIRED)
    message( "load packages" )
    find_package(GTest REQUIRED)
    find_package(FFmpeg REQUIRED AVFORMAT AVUTIL AVCODEC AVFILTER AVUTIL POSTPROC SWSCALE)

    SET(INCLUDES ${INCLUDES} ${FFMPEG_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} )
    SET(LIBRARIES ${LIBRARIES} ${FFMPEG_LIBRARIES} ${GTEST_BOTH_LIBRARIES} Threads::Threads)
endif( ${BUILD_WITH_CONAN} )

add_subdirectory ( av )
if( build_samples )
    add_subdirectory ( samples )
endif()
if( build_tests )
    add_subdirectory ( test )
endif()

# -----------------------   Doxygen                 -----------------------
if (build_documentation)
    find_package(Doxygen)
    if( DOXYGEN_FOUND )
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
#        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/templates/header.html ${CMAKE_CURRENT_SOURCE_DIR}/docs/header.html COPYONLY)
##        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/templates/doxygen.css ${CMAKE_CURRENT_SOURCE_DIR}/docs/doxygen.css COPYONLY)
#        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/templates/footer.html ${CMAKE_CURRENT_SOURCE_DIR}/docs/footer.html COPYONLY)
#        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/templates/doxy-boot.js ${CMAKE_CURRENT_SOURCE_DIR}/docs/doxy-boot.js COPYONLY)
#        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/templates/customdoxygen.css ${CMAKE_CURRENT_SOURCE_DIR}/docs/customdoxygen.css COPYONLY)

#        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/templates/jquery.smartmenus.bootstrap.css ${CMAKE_CURRENT_SOURCE_DIR}/docs/jquery.smartmenus.bootstrap.css COPYONLY)
#        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/templates/jquery.smartmenus.bootstrap.js ${CMAKE_CURRENT_SOURCE_DIR}/docs/jquery.smartmenus.bootstrap.js COPYONLY)
#        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/templates/jquery.smartmenus.js ${CMAKE_CURRENT_SOURCE_DIR}/docs/jquery.smartmenus.js COPYONLY)

        add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen" VERBATIM
        )
    endif(DOXYGEN_FOUND)
endif(build_documentation)

# -------------------------------------------------------------------------
# -----------------------   Generate Package        -----------------------
# -------------------------------------------------------------------------
IF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")
SET(CPACK_GENERATOR "TGZ")
SET(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
SET(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)

#if(CDS_TAG_VERSION MATCHES "^([0-9]+)[.]([0-9]+)[.]([0-9]+)$" )
#    string(REPLACE "." ";" CDS_TAG_VERSION_STRING ${CDS_TAG_VERSION})
#    list(GET CDS_TAG_VERSION_STRING 0 MAJOR_VERSION)
#    list(GET CDS_TAG_VERSION_STRING 1 MINOR_VERSION)
#    list(GET CDS_TAG_VERSION_STRING 2 PATCH_VERSION)
#else()
#    SET(MAJOR_VERSION "0")
#    SET(MINOR_VERSION "0")
#    SET(PATCH_VERSION "0")
#endif()

SET(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")

SET(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")

INCLUDE(CPack)

ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
