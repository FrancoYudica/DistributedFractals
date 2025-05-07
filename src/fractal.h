#pragma once
#include <functional>

enum FractalType {
    FRACTAL_MANDELBROT = 0,
    FRACTAL_JULIA
};

struct FractalSettings {
    int max_iterations;
    FractalType type;
};

/// @brief Given the fractal type, returns a function that computes that fractal
std::function<int(double, double, const FractalSettings&)> get_fractal_function(FractalType);