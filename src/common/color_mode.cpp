#include "color_mode.h"
#include "math.h"
#include "common/logging.h"

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

void blue_orange_function(float t, float& r, float& g, float& b)
{

    if (fabs(t - 1.0f) < 1e-6f) {
        r = g = b = 0.0f;
        return;
    }

    float d = 100.0f * t;
    r = 0.5f + 0.5f * cos(3.0f + d * 0.15f);
    g = 0.5f + 0.5f * cos(3.0f + d * 0.15f + 0.6f);
    b = 0.5f + 0.5f * cos(3.0f + d * 0.15f + 1.0f);
}
void colorful_1_function(float t, float& r, float& g, float& b)
{

    if (fabs(t - 1.0f) < 1e-6f) {
        r = g = b = 0.0f;
        return;
    }

    float d = 200.0f * t;
    r = 0.5f + 0.5f * cos(d + 3.0f);
    g = 0.5f + 0.5f * cos(d * 0.50f + 0.6f);
    b = 0.5f + 0.5f * sin(d * 0.35f + 1.0f);
}
void colorful_2_function(float t, float& r, float& g, float& b)
{
    if (fabs(t - 1.0f) < 1e-6f) {
        r = g = b = 0.0f;
        return;
    }
    float d = 200.0f * t;

    r = 0.5f + 0.5f * cos(d);
    g = 0.5f + 0.5f * cos(d + 1.33);
    b = 0.5f + 0.5f * cos(d + 2.66);
}

void colorful_warm_sunset(float t, float& r, float& g, float& b)
{
    if (fabs(t - 1.0f) < 1e-6f) {
        r = g = b = 0.0f;
        return;
    }
    float d = 200.0f * t;

    r = 0.5 + 0.5 * cos(d + 0.0);
    g = 0.4 + 0.4 * cos(d + 2.0);
    b = 0.2 + 0.2 * cos(d + 4.0);
}

void ocean_function(float t, float& r, float& g, float& b)
{
    if (fabs(t - 1.0f) < 1e-6f) {
        r = g = b = 0.0f;
        return;
    }
    float d = 200.0f * t;

    r = 0.2 + 0.2 * cos(d + 4.0);
    g = 0.5 + 0.5 * cos(d + 2.0);
    b = 0.7 + 0.3 * cos(d + 0.0);
}

void rainbow_function(float t, float& r, float& g, float& b)
{
    if (fabs(t - 1.0f) < 1e-6f) {
        r = g = b = 0.0f;
        return;
    }
    float d = 200.0f * t;

    r = 0.5 + 0.5 * cos(6.0 * d + 0.0);
    g = 0.5 + 0.5 * cos(6.0 * d + 2.0);
    b = 0.5 + 0.5 * cos(6.0 * d + 4.0);
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
    case ColorMode::BLUE_ORANGE_CYCLIC:
        return blue_orange_function;
    case ColorMode::COLORFUL_1:
        return colorful_1_function;
    case ColorMode::COLORFUL_2:
        return colorful_2_function;
    case ColorMode::WARM_SUNSET:
        return colorful_warm_sunset;
    case ColorMode::OCEAN:
        return ocean_function;
    case ColorMode::RAINBOW:
        return rainbow_function;
    default:
        LOG_WARNING("Unimplemented color mode in get_color_function(). Using black-white");
        return black_white_color_function;
    }
}
