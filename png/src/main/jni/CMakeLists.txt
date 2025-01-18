cmake_minimum_required(VERSION 3.4.1)
project(pngquant)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g -rdynamic -no-pie")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
add_compile_options(
  -ffunction-sections
  -fdata-sections
  -fvisibility=hidden
)
add_link_options(
  -fuse-ld=lld
  -Wl,--gc-sections
  -Wl,--strip-all
)

option(ZLIB_NG "zlib_ng" OFF)
option(ZLIB_NGS "zlib_ng_static" OFF)
option(ARCH "ARCH" OFF)
option(OS "os" OFF)

if(ARCH)
if(OS)
set(SUFFIX _${CMAKE_SYSTEM_NAME}_${CMAKE_SYSTEM_PROCESSOR})
else()
set(SUFFIX _${CMAKE_SYSTEM_PROCESSOR})
endif()
else()
set(SUFFIX "")
endif()

add_library(libimagequant STATIC IMPORTED)
set_target_properties(libimagequant PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/libimagequant_sys${SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}
)
set(SRC jni.c
    spng.c
    spngutil.c)


if(ZLIB_NG)

if(ZLIB_NGS)
add_library(zlib-ng STATIC IMPORTED)
set_target_properties(zlib-ng PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/libz${SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}
)
set(LIB libimagequant zlib-ng)
else()
add_subdirectory(zlib-ng)
set(LIB libimagequant zlibstatic)
endif()

else()
find_package(ZLIB REQUIRED)
set(LIB libimagequant ZLIB::ZLIB)
endif()


if(CMAKE_SYSTEM_NAME MATCHES "Android")
list(APPEND LIB jnigraphics)
find_library(jnigraphics-lib jnigraphics)
endif()

add_library(pngquant SHARED
    ${SRC}
)

target_link_options(pngquant PUBLIC
  -Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/export.map
)

target_link_libraries(pngquant
  PUBLIC
  -Wl,--start-group
  ${LIB}
  -Wl,--end-group
  -Wl,--exclude-libs=ALL
)