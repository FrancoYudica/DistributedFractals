#pragma once
#include <stdint.h>

enum Tag {
    REQUEST,
    RESULT,
    TERMINATE,
    TASK
};

struct WorkerTask {
    int32_t x, y;
    int32_t width, height;
};
