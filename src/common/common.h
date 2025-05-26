#pragma once
#include "settings/settings.h"
#include <cstring>
#include <cstdlib>
#include <math.h>

void print_help();
bool load_args(int argc, char** argv, Settings& settings);