#pragma once

struct ImageSettings {

    /// @brief Image size
    int width, height;

    /// @brief Specifies the amount of iterations per pixel to avoid aliasing
    int multi_sample_anti_aliasing;

    ImageSettings()
        : multi_sample_anti_aliasing(1)
        , width(512)
        , height(512)
    {
    }
};