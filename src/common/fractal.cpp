#include <iostream>
#include "fractal.h"
#include "common.h"

float compute_mandelbrot(
    number world_x,
    number world_y,
    const FractalSettings& settings)
{
    number zx = 0, zy = 0;
    int iter = 0;

    number length_squared = zx * zx + zy * zy;

    while (length_squared < 4.0 && iter < settings.max_iterations) {
        number xtemp = zx * zx - zy * zy + world_x;
        zy = 2.0 * zx * zy + world_y;
        zx = xtemp;
        iter++;
        length_squared = zx * zx + zy * zy;
    }

    // Computes smooth t in range [0.0, max_iterations]
    number smooth_t = number(iter) - LOG2(LOG(length_squared) / LOG(4.0));

    // Normalizes
    return smooth_t / settings.max_iterations;
}

float compute_julia(
    number world_x,
    number world_y,
    const FractalSettings& settings)
{
    // Z(n+1) = Z(n)^2 + C
    // Constant C = Cx + Cyi
    const number Cx = settings.julia_settings.Cx;
    const number Cy = settings.julia_settings.Cy;

    number zx = world_x, zy = world_y;
    int iter = 0;

    number length_squared = zx * zx + zy * zy;

    while (length_squared < 4.0 && iter < settings.max_iterations) {
        number xtemp = zx * zx - zy * zy + Cx;
        zy = 2.0 * zx * zy + Cy;
        zx = xtemp;
        iter++;
        length_squared = zx * zx + zy * zy;
    }

    // Computes smooth t in range [0.0, max_iterations]
    number smooth_t = number(iter) - LOG2(LOG(length_squared) / LOG(4.0));

    // Normalizes
    return smooth_t / settings.max_iterations;
}

std::function<float(number, number, const FractalSettings&)> get_fractal_function(
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
