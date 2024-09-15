cmake_minimum_required(VERSION 3.4.1)
project(pngquant)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g -rdynamic -no-pie")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    -Wl,--gc-sections,--as-needed")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--gc-sections,--as-needed")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections,--as-needed")

add_library(libimagequant STATIC IMPORTED)
set_target_properties(libimagequant PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/libimagequant_sys.a
)
add_library(zlib-ng STATIC IMPORTED)
set_target_properties(zlib-ng PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/libz.a
)
set(SRC jni.c
    spng.c
    spngutil.c)
set(LIB libimagequant
    zlib-ng)
if(CMAKE_SYSTEM_NAME MATCHES "Android")
list(APPEND SRC
android_jni.c
)
list(APPEND LIB
${log-lib}
jnigraphics
)
find_library(log-lib log)
find_library(jnigraphics-lib jnigraphics)
endif()
add_library(pngquant SHARED
    ${SRC}
)
#find_package(ZLIB REQUIRED)
target_link_libraries(pngquant
${LIB}
)
