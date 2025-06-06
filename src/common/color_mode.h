#pragma once
#include "settings/fractal_settings.h"

// Defines the color mode function
typedef void(ColorFunction)(float, float&, float&, float&);

/// @brief Given the color mode, returns a function that computes the color of each sample
ColorFunction* get_color_function(ColorMode);