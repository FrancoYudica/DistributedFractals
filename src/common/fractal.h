#pragma once
#include <functional>
#include <memory>
#include "settings/fractal_settings.h"
#include "common.h"

class FractalSampler {
public:
    virtual float sample(
        number world_x,
        number world_y,
        const FractalSettings& settings)
        = 0;
};

class MandelbrotFractalSampler : public FractalSampler {

    float sample(
        number world_x,
        number world_y,
        const FractalSettings& settings);
};

class JuliaFractalSampler : public FractalSampler {

    float sample(
        number world_x,
        number world_y,
        const FractalSettings& settings);
};

std::unique_ptr<FractalSampler> get_fractal_sampler(FractalType);