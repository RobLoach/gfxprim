cmake_minimum_required(VERSION 3.11)
project(
    GFXprim
    DESCRIPTION "GFXprim is Open-source modular 2D bitmap graphics library with emphasis on speed and correctness."
    VERSION 1.0.0
    LANGUAGES C
)

option(GFXPRIM_BUILD_STATIC "Build Static" ON)
option(GFXPRIM_BUILD_SHARED "Build Shared" OFF)

set(GFXPRIM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(GFXPRIM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

file(GLOB GFXPRIM_SOURCES
    ${GFXPRIM_SOURCE_DIR}/backends/*.c
    ${GFXPRIM_SOURCE_DIR}/core/*.c
    ${GFXPRIM_SOURCE_DIR}/filters/*.c
    ${GFXPRIM_SOURCE_DIR}/gfx/*.c
    ${GFXPRIM_SOURCE_DIR}/grabbers/*.c
    ${GFXPRIM_SOURCE_DIR}/input/*.c
    ${GFXPRIM_SOURCE_DIR}/loaders/*.c
    ${GFXPRIM_SOURCE_DIR}/text/*.c
    ${GFXPRIM_SOURCE_DIR}/utils/*.c
    ${GFXPRIM_SOURCE_DIR}/widgets/*.c
)

if(GFXPRIM_BUILD_STATIC)
    add_library(gfxprim STATIC ${GFXPRIM_SOURCES})
    target_include_directories(gfxprim PUBLIC ${GFXPRIM_INCLUDE_DIR})
endif()
