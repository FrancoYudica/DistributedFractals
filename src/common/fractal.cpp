#include <iostream>
#include "fractal.h"
#include <math.h>

float compute_mandelbrot(
    double world_x,
    double world_y,
    const FractalSettings& settings)
{
    double zx = 0, zy = 0;
    int iter = 0;

    double length_squared = zx * zx + zy * zy;

    while (length_squared < 4.0 && iter < settings.max_iterations) {
        double xtemp = zx * zx - zy * zy + world_x;
        zy = 2.0 * zx * zy + world_y;
        zx = xtemp;
        iter++;
        length_squared = zx * zx + zy * zy;
    }

    // Computes smooth t in range [0.0, max_iterations]
    float smooth_t = float(iter) - log2(log(length_squared) / log(4.0));

    // Normalizes
    return smooth_t / settings.max_iterations;
}

float compute_julia(
    double world_x,
    double world_y,
    const FractalSettings& settings)
{
    // Z(n+1) = Z(n)^2 + C
    // Constant C = Cx + Cyi
    const double Cx = settings.julia_settings.Cx;
    const double Cy = settings.julia_settings.Cy;

    double zx = world_x, zy = world_y;
    int iter = 0;

    double length_squared = zx * zx + zy * zy;

    while (length_squared < 4.0 && iter < settings.max_iterations) {
        double xtemp = zx * zx - zy * zy + Cx;
        zy = 2.0 * zx * zy + Cy;
        zx = xtemp;
        iter++;
        length_squared = zx * zx + zy * zy;
    }

    // Computes smooth t in range [0.0, max_iterations]
    float smooth_t = float(iter) - log2(log(length_squared) / log(4.0));

    // Normalizes
    return smooth_t / settings.max_iterations;
}

std::function<float(double, double, const FractalSettings&)> get_fractal_function(
    FractalType type)
{
    switch (type) {
    case FRACTAL_MANDELBROT:
        return compute_mandelbrot;
        break;
    case FRACTAL_JULIA:
        return compute_julia;
        break;

    default:
        std::cout << "Unimplemented function for fractal type: " << type << std::endl;
        exit(1);
        break;
    }
}
