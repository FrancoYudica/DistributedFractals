#include "fractal.h"
#include "common.h"

std::unique_ptr<FractalSampler> get_fractal_sampler(FractalType type)
{
    switch (type) {
    case FractalType::MANDELBROT:
        return std::make_unique<MandelbrotFractalSampler>();
        break;

    case FractalType::JULIA:
        return std::make_unique<JuliaFractalSampler>();
    default:
        return std::make_unique<MandelbrotFractalSampler>();
    }
}