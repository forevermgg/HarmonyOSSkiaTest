# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(HarmonyOSSkiaTest)

add_definitions(-DOHOS_PLATFORM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__cplusplus=201703L -std=c++17 -Wall -fno-omit-frame-pointer -pthread -fexceptions -frtti")

#头文件目录
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/skia)
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/${OHOS_ARCH})
message("PROJECT_SOURCE_DIR : ${PROJECT_SOURCE_DIR}")
message("OHOS_ARCH : ${OHOS_ARCH}")
set(SK_GL=1)
add_definitions(-DSK_GL=1)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

if(DEFINED PACKAGE_FIND_FILE)
    include(${PACKAGE_FIND_FILE})
endif()

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)


add_compile_definitions(SK_GL)
add_compile_definitions(SK_GANESH)
add_definitions(-DSK_GANESH=1)

# 查找相关库 (包括OpenGL ES相关库和XComponent提供的ndk接口)
find_library(
    # Sets the name of the path variable.
    EGL-lib
    # Specifies the name of the NDK library that
    # you want CMake to locate.
    EGL
)

find_library(
    # Sets the name of the path variable.
    GLES-lib
    # Specifies the name of the NDK library that
    # you want CMake to locate.
    GLESv3
)

find_library(
    # Sets the name of the path variable.
    hilog-lib
    # Specifies the name of the NDK library that
    # you want CMake to locate.
    hilog_ndk.z
)

find_library(
    # Sets the name of the path variable.
    libace-lib
    # Specifies the name of the NDK library that
    # you want CMake to locate.
    ace_ndk.z
)

find_library(
    # Sets the name of the path variable.
    libnapi-lib
    # Specifies the name of the NDK library that
    # you want CMake to locate.
    ace_napi.z
)

find_library(
    # Sets the name of the path variable.
    libuv-lib
    # Specifies the name of the NDK library that
    # you want CMake to locate.
    uv
)

add_library(nativerender SHARED
    render/egl_core.cpp
    render/plugin_render.cpp
    manager/plugin_manager.cpp
    utils/napi/napi_utils.cpp
    WindowContext.cpp
    GLWindowContext.cpp
    GLWindowContext_ohos.cpp
    Surface.cpp
    napi_init.cpp
)
target_link_libraries(nativerender PUBLIC
${EGL-lib} ${GLES-lib} ${hilog-lib} ${libace-lib} ${libnapi-lib} ${libuv-lib} libc++.a libc.a
libskia.so)