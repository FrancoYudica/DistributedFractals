#pragma once

enum FractalType {
    FRACTAL_MANDELBROT = 0,
    FRACTAL_JULIA,

    // Used to tell the valid range of values
    INVALID_LAST
};

enum class ColorMode {
    BLACK_WHITE,
    GRAYSCALE,
    BLUE_GREEN_RED,
    BLUE_ORANGE,

    // Used to tell the valid range of values
    INVALID_LAST
};

struct JuliaSettings {

    // Julia set constant parameters
    double Cx, Cy;

    JuliaSettings()
        : Cx(-0.225)
        , Cy(-0.700)
    {
    }
};

struct FractalSettings {
    int max_iterations;
    FractalType type;
    ColorMode color_mode;

    JuliaSettings julia_settings;

    FractalSettings()
        : max_iterations(128)
        , color_mode(ColorMode::BLUE_GREEN_RED)
        , type(FRACTAL_MANDELBROT)
    {
    }
};
