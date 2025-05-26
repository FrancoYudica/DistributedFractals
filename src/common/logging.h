#pragma once
#include <iostream>

extern bool _s_logging_verbose;

#define LOG(X)                       \
    if (_s_logging_verbose) {        \
        std::cout << X << std::endl; \
    }

#define LOG_ERROR(X) LOG("[ERROR]: " << X)
#define LOG_SUCCESS(X) LOG("[SUCCESS]: " << X)
#define LOG_STATUS(X) LOG("[STATUS]: " << X)
#define LOG_WARNING(X) LOG("[WARNING]: " << X)

void set_logging_enabled(bool enabled);