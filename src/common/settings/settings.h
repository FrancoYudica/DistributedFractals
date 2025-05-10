#pragma once

#include "image_settings.h"
#include "camera.h"
#include "fractal_settings.h"

struct Settings {

    /// @brief Path where the image gets saved
    char output_path[1024];

    int block_size;

    ImageSettings image;
    Camera camera;
    FractalSettings fractal;

    Settings()
        : block_size(32)
        , output_path("./fractal.png")
    {
    }
};