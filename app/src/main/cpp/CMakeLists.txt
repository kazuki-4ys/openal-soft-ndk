cmake_minimum_required(VERSION 3.22.1)
project("openal_soft_ndk")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DAL_BUILD_LIBRARY -DAL_ALEXT_PROTOTYPES -DALSOFT_SUPPORTS_OPENSLES)

include_directories(
    ../../../../include
    ../../../../common
    ../../../../gsl/include
    ../../../../fmt-11.2.0/include
    ../../../..
)

file(GLOB_RECURSE OPENAL_SOURCES
    "../../../../al/*.cpp"
    "../../../../alc/*.cpp"
    "../../../../common/*.cpp"
    "../../../../core/*.cpp"
    "../../../../fmt-11.2.0/src/*.cc"
)

list(FILTER OPENAL_SOURCES EXCLUDE REGEX "../../../../alc/backends/(alsa|coreaudio|dsound|jack|oss|pipewire|portaudio|pulseaudio|sdl2|sdl3|solaris|wasapi|winmm|oboe|sndio)\\.cpp")
list(FILTER OPENAL_SOURCES EXCLUDE REGEX "../../../../router/.*")
list(FILTER OPENAL_SOURCES EXCLUDE REGEX "../../../../core/rtkit\\.cpp")

if(ANDROID_ABI MATCHES "armeabi-v7a" OR ANDROID_ABI MATCHES "arm64-v8a")
    list(FILTER OPENAL_SOURCES EXCLUDE REGEX "../../../../core/mixer/mixer_sse.*\\.cpp")
    if(ANDROID_ABI STREQUAL "armeabi-v7a")
        add_definitions("-mfpu=neon")
    endif()
elseif(ANDROID_ABI MATCHES "x86" OR ANDROID_ABI MATCHES "x86_64")
    list(FILTER OPENAL_SOURCES EXCLUDE REGEX "../../../../core/mixer/mixer_neon\\.cpp")
endif()

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