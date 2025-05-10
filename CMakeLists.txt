cmake_minimum_required(VERSION 3.14)

project(fractal_mpi CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force static build for zlib
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)

option(BUILD_TESTS "Build test executables" OFF)

# Habilitar FetchContent para descargar libpng
include(FetchContent)

# También trae zlib (libpng depende de ella)
FetchContent_Declare(
    zlib
    URL https://sourceforge.net/projects/teca/files/TECA_deps/zlib-1.3.tar.gz/download
)

FetchContent_MakeAvailable(zlib)

# Descargar libpng
FetchContent_Declare(
    libpng
    URL https://download.sourceforge.net/libpng/libpng-1.6.40.tar.gz
)

# Esta línea es crucial para que se compile con zlib
set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(libpng)

# Adds executable
set(
    COMMON_SOURCES 
    src/common/color_mode.cpp
    src/common/fractal.cpp
    src/common/renderer.cpp)


# Finds MPI
find_package(MPI REQUIRED)

add_executable(
    fractal_mpi 
    src/parallel/main.cpp 
    src/parallel/master.cpp 
    src/parallel/worker.cpp 
    ${COMMON_SOURCES})

target_include_directories(fractal_mpi PRIVATE src)
target_link_libraries(fractal_mpi PRIVATE MPI::MPI_CXX png zlib)

add_executable(
    sequential 
    src/sequential/main.cpp 
    ${COMMON_SOURCES})

target_include_directories(sequential PRIVATE src)
target_link_libraries(sequential PRIVATE MPI::MPI_CXX png zlib)


if(BUILD_TESTS)
    message(STATUS "Compiling tests...")

    add_executable(fractal_tests
        src/tests/image_save_test.cpp
    )

    target_link_libraries(fractal_tests PRIVATE png)
    target_include_directories(fractal_tests PRIVATE src) 
endif()

