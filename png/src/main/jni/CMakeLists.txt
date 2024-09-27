cmake_minimum_required(VERSION 3.4.1)
project(pngquant)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPNG_ARM_NEON_OPT=0 -O3")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    -Wl,--gc-sections")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--gc-sections")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections")

add_library(libimagequant STATIC IMPORTED)
set_target_properties(libimagequant PROPERTIES
    IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/pngquant/lib/libimagequant_sys.a
)

add_library(pngquant SHARED
    native_glue.c

    libpng-android/jni/png.c
    libpng-android/jni/pngerror.c
    libpng-android/jni/pngget.c
    libpng-android/jni/pngmem.c
    libpng-android/jni/pngpread.c
    libpng-android/jni/pngread.c
    libpng-android/jni/pngrio.c
    libpng-android/jni/pngrtran.c
    libpng-android/jni/pngrutil.c
    libpng-android/jni/pngset.c
    #libpng-android/jni/pngtest.c
    libpng-android/jni/pngtrans.c
    libpng-android/jni/pngwio.c
    libpng-android/jni/pngwrite.c
    libpng-android/jni/pngwtran.c
    libpng-android/jni/pngwutil.c
    
    pngquant/pngquant.c
    pngquant/pngquant_opts.c
    pngquant/rwpng.c
    
)

include_directories(pngquant
    libpng-android/jni/
    pngquant/
)

target_link_libraries(pngquant
    log
    z
    libimagequant
)
