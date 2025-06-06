#pragma once
#include <stdint.h>
#include "settings/image_settings.h"
#include "settings/fractal_settings.h"
#include "settings/camera.h"

void render_block(
    uint8_t* buffer,
    const ImageSettings& image_settings,
    const FractalSettings& fractal_settings,
    const Camera& camera,
    uint32_t x,
    uint32_t y,
    uint32_t width,
    uint32_t height);