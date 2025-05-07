#pragma once
#include "settings/settings.h"
#include "fractal.h"

void worker(
    int rank,
    const ImageSettings& img_settings,
    const Camera& camera,
    const FractalSettings& fractal_settings);