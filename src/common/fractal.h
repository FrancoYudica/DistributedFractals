#pragma once
#include <functional>
#include "settings/fractal_settings.h"
#include "common.h"

/// @brief Given the fractal type, returns a function that computes that fractal
std::function<float(number, number, const FractalSettings&)> get_fractal_function(FractalType);