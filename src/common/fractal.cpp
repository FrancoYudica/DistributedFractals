#include "fractal.h"
#include "common.h"
#include "common/logging.h"

FractalSampler* get_fractal_sampler(FractalType type)
{
    switch (type) {
    case FractalType::MANDELBROT:
        return mandelbrot_sampler;
        break;

    case FractalType::JULIA:
        return julia_sampler;
    default:
        LOG_WARNING("Received unexpected fractal type: " << (int)type << ". Using Mandelbrot by default");
        return mandelbrot_sampler;
    }
}