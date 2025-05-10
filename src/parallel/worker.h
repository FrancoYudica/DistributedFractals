#pragma once
#include "common/settings/settings.h"
#include "common/fractal.h"

void worker(
    int rank,
    const ImageSettings& img_settings,
    const Camera& camera,
    const FractalSettings& fractal_settings);