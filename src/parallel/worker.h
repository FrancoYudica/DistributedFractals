#pragma once
#include <stdint.h>
#include "common/settings/settings.h"
#include "common/fractal.h"

void worker(
    uint32_t rank,
    uint32_t block_size,
    const ImageSettings& img_settings,
    const Camera& camera,
    const FractalSettings& fractal_settings);