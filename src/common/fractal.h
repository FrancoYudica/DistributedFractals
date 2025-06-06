#pragma once
#include "settings/fractal_settings.h"
#include "common.h"

typedef float(FractalSampler)(number wx, number wy, const FractalSettings& settings);

float mandelbrot_sampler(number, number, const FractalSettings&);
float julia_sampler(number, number, const FractalSettings&);

FractalSampler* get_fractal_sampler(FractalType);