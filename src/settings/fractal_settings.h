#pragma once

enum FractalType {
    FRACTAL_MANDELBROT = 0,
    FRACTAL_JULIA
};

enum ColorModeType {
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

    JuliaSettings julia_settings;

    FractalSettings()
        : max_iterations(128)
        , type(FRACTAL_MANDELBROT)
    {
    }
};
