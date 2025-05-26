#include "logging.h"

bool _s_logging_verbose = true;

void set_logging_enabled(bool enabled)
{
    _s_logging_verbose = enabled;
}