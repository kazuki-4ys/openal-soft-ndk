cmake_minimum_required(VERSION 3.22.1)
project("openal_soft_ndk")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DAL_BUILD_LIBRARY -DAL_ALEXT_PROTOTYPES -DALSOFT_SUPPORTS_OPENSLES)

include_directories(
    ../../../../include
    ../../../../common
    ../../../..
)

file(GLOB_RECURSE OPENAL_SOURCES
    "../../../../al/*.cpp"
    "../../../../alc/*.cpp"
    "../../../../common/*.cpp"
    "../../../../core/*.cpp"
)

add_library(openal SHARED
    ${OPENAL_SOURCES}
)

target_link_libraries(openal
    OpenSLES
    log
)

add_library(openal_soft_ndk SHARED
        native-lib.cpp
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

target_include_directories(openal_soft_ndk PRIVATE
        ${ANDROID_NDK}/sources/android/native_app_glue)

target_link_libraries(openal_soft_ndk
        openal
        GLESv3
        EGL
        android
        log)