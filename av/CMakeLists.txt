# -------------------------------------------------------------------------
# Build avcpp library
# -------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.3)

project( av )

# -----------------------   Compile library          -----------------------

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
aux_source_directory(${PROJECT_SOURCE_DIR} LIBAVCPP_SOURCES )
aux_source_directory(${PROJECT_SOURCE_DIR}/discid DISCID_SOURCES )
add_library(${PROJECT_NAME} ${DISCID_SOURCES} ${LIBAVCPP_SOURCES} )
#include_directories(${INCLUDES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES} -ldl)
install(TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION lib LIBRARY DESTINATION lib COMPONENT library)
