#pragma once
#include "image_settings.h"
#include "camera.h"
#include "fractal.h"

void worker(
    int rank,
    const ImageSettings& img_settings,
    const Camera& camera,
    const FractalSettings& fractal_settings);