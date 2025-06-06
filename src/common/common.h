#pragma once
#include "settings/settings.h"
#include <cstring>
#include <cstdlib>
#include <math.h>
#include <cstdint>

void print_help();
bool load_args(uint32_t argc, char** argv, Settings& settings);