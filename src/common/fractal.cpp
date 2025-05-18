#include <iostream>
#include "fractal.h"
#include "common.h"

const number two(2.0);
const number four(4.0);

float compute_mandelbrot(
    number world_x,
    number world_y,
    const FractalSettings& settings)
{
    number zx(0.0);
    number zy(0.0);
    int iter = 0;

    number length_squared = zx * zx + zy * zy;
    number xtemp;
    while (length_squared < 4.0 && iter < settings.max_iterations) {
        xtemp = zx * zx - zy * zy + world_x;
        zy = two * zx * zy + world_y;
        zx = xtemp;
        iter++;
        length_squared = zx * zx + zy * zy;
    }

    // Computes smooth t in range [0.0, max_iterations]
    number smooth_t = (number)iter - LOG2(LOG(length_squared) / LOG(four));

    // Normalizes
    return (float)(smooth_t / (number)settings.max_iterations);
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
    number xtemp;
    while (length_squared < 4.0 && iter < settings.max_iterations) {
        xtemp = zx * zx - zy * zy + Cx;
        zy = two * zx * zy + Cy;
        zx = xtemp;
        iter++;
        length_squared = zx * zx + zy * zy;
    }

    // Computes smooth t in range [0.0, max_iterations]
    number smooth_t = (number)iter - LOG2(LOG(length_squared) / LOG(four));

    // Normalizes
    return (float)(smooth_t / (number)settings.max_iterations);
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
