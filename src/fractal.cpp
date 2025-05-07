#include <iostream>
#include "fractal.h"

int compute_mandelbrot(
    double world_x,
    double world_y,
    const FractalSettings& settings)
{
    double zx = 0, zy = 0;
    int iter = 0;
    while (zx * zx + zy * zy < 4.0 && iter < settings.max_iterations) {
        double tmp = zx * zx - zy * zy + world_x;
        zy = 2.0 * zx * zy + world_y;
        zx = tmp;
        iter++;
    }
    return iter;
}

std::function<int(double, double, const FractalSettings&)> get_fractal_function(
    FractalType type)
{
    switch (type) {
    case FRACTAL_MANDELBROT:
        return compute_mandelbrot;
        break;

    default:
        std::cout << "Unimplemented function for fractal type: " << type << std::endl;
        exit(1);
        break;
    }
}
