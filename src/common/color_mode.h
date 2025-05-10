#pragma once
#include <functional>
#include "settings/fractal_settings.h"

// Defines the color mode function
typedef std::function<void(float, float&, float&, float&)> ColorFunction;

/// @brief Given the color mode, returns a function that computes the color of each sample
ColorFunction get_color_function(ColorMode);