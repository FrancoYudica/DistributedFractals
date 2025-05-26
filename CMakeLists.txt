cmake_minimum_required(VERSION 3.14)

project(fractal_mpi CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build test executables" OFF)
option(USE_PRECISION_32 "Enables 32 bit number precision" OFF)
option(USE_PRECISION_128 "Enables 128 bit number precision" OFF)
option(USE_DYNAMIC_PRECISION "Enables dynamic precision" OFF)


include(FetchContent)

# Download zlib (libpng dependency)
FetchContent_Declare(
    zlib
    URL https://sourceforge.net/projects/teca/files/TECA_deps/zlib-1.3.tar.gz/download
)

FetchContent_MakeAvailable(zlib)

# Download libpng
FetchContent_Declare(
    libpng
    URL https://download.sourceforge.net/libpng/libpng-1.6.40.tar.gz
)

set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(libpng)

# Adds executable
set(
    COMMON_SOURCES 
    src/common/common.cpp
    src/common/logging.cpp
    src/common/color_mode.cpp
    src/common/renderer.cpp
    src/common/output_handler.cpp
    src/common/fractal.cpp
    src/common/fractal_samplers/mandelbrot_fractal_sampler.cpp
    src/common/fractal_samplers/julia_fractal_sampler.cpp)


# Finds MPI
find_package(MPI REQUIRED)

# Creates a common library
add_library(fractal_common STATIC ${COMMON_SOURCES})
target_include_directories(fractal_common PUBLIC src)
target_link_libraries(fractal_common PUBLIC png zlib quadmath)

if(USE_DYNAMIC_PRECISION)
    # Try to find MPFR (and its dependency GMP)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(MPFR REQUIRED mpfr)

    if (MPFR_FOUND)
        message(STATUS "MPFR found via pkg-config")

        # Add include dirs and link libraries to your target
        target_include_directories(fractal_common PUBLIC ${MPFR_INCLUDE_DIRS})
        target_link_libraries(fractal_common PUBLIC ${MPFR_LIBRARIES})

        # Define macro for your code
        target_compile_definitions(fractal_common PUBLIC USE_MPFR)
    else()
        message(STATUS "MPFR not found via pkg-config")
    endif()
elseif(USE_PRECISION_128)
    target_compile_definitions(fractal_common PUBLIC PRECISION_128)
elseif(USE_PRECISION_32)
    target_compile_definitions(fractal_common PUBLIC PRECISION_32)
endif()



# Creates parallel executable
add_executable(fractal_mpi 
    src/parallel/main.cpp 
    src/parallel/master.cpp 
    src/parallel/worker.cpp)
target_link_libraries(fractal_mpi PRIVATE fractal_common MPI::MPI_CXX)

# Creates sequential executable
add_executable(sequential 
    src/sequential/main.cpp)
target_link_libraries(sequential PRIVATE fractal_common)


if(BUILD_TESTS)
    message(STATUS "Compiling tests...")

    add_executable(fractal_tests
        src/tests/image_save_test.cpp
    )

    target_link_libraries(fractal_tests PRIVATE png)
    target_include_directories(fractal_tests PRIVATE src) 
endif()

