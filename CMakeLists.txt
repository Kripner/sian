cmake_minimum_required (VERSION 3.5)
project (sian VERSION 1.0.0 LANGUAGES CXX)


find_package(PkgConfig REQUIRED)

pkg_check_modules(CAIRO_PKG REQUIRED IMPORTED_TARGET cairo)

set(files
    src/animation/animated_value.cc
    src/animation/payload_type.cc
    src/animator.cc
    src/color.cc
    src/config.cc
    src/logger.cc
    src/objects/circle.cc
    src/objects/horizontal_layout.cc
    src/objects/line.cc
    src/objects/object.cc
    src/objects/rectangle.cc
    src/objects/rectangle_container.cc
    src/objects/shape.cc
    src/offset.cc
    src/pace_value.cc
    src/scene.cc)

add_library(sian STATIC
    ${files})
target_include_directories(sian
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(sian PUBLIC
    PkgConfig::CAIRO_PKG)

add_executable(sample
    src/main.cc)
target_link_libraries(sample PUBLIC sian)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()
