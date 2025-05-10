#include <iostream>
#include "color_mode.h"
#include "math.h"

/// @brief Black an white color mode is black if it's inside the set, otherwise white
void black_white_color_function(float t, float& r, float& g, float& b)
{
    r = g = b = (float)(t > 0.0);
}

/// @brief Sets the rgb channels to t, using a simple grayscale palette
void grayscale_color_function(float t, float& r, float& g, float& b)
{
    r = std::min(t, 1.0f);
    g = std::min(t, 1.0f);
    b = std::min(t, 1.0f);
}

void blue_green_red_function(float t, float& r, float& g, float& b)
{
    float one_minus_t = 1.0 - t;
    r = 9.0 * one_minus_t * t * t * t;
    g = 15.0 * one_minus_t * one_minus_t * t * t;
    b = 8.5 * one_minus_t * one_minus_t * one_minus_t * t;
}

ColorFunction get_color_function(ColorMode mode)
{
    switch (mode) {
    case ColorMode::BLACK_WHITE:
        return black_white_color_function;
    case ColorMode::GRAYSCALE:
        return grayscale_color_function;
    case ColorMode::BLUE_GREEN_RED:
        return blue_green_red_function;
    default:
        std::cout << "Unimplemented color mode in get_color_function()." << std::endl;
        return black_white_color_function;
    }
}
