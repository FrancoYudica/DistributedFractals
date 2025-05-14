#pragma once

#include "image_settings.h"
#include "camera.h"
#include "fractal_settings.h"
#include "output_settings.h"

struct Settings {

    int block_size;

    ImageSettings image;
    Camera camera;
    FractalSettings fractal;
    OutputSettings output_settings;

    Settings()
        : block_size(32)
    {
    }
};