cmake_minimum_required(VERSION 3.14)

project(fractal_mpi CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
add_executable(fractal_mpi
    src/main.cpp
    src/master.cpp
    src/worker.cpp
)

# Finds MPI
find_package(MPI REQUIRED)

target_include_directories(fractal_mpi PRIVATE ${MPI_CXX_INCLUDE_PATH})
target_link_libraries(fractal_mpi PRIVATE MPI::MPI_CXX png zlib)


if(BUILD_TESTS)
    message(STATUS "Compiling tests...")

    add_executable(fractal_tests
        src/tests/image_save_test.cpp
    )

    target_link_libraries(fractal_tests PRIVATE png)
    target_include_directories(fractal_tests PRIVATE src) 
endif()

