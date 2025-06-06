#pragma once
#include <stdint.h>

enum Tag {
    REQUEST,
    RESULT,
    TERMINATE,
    TASK
};

struct WorkerTask {
    uint32_t x, y;
    uint32_t width, height;
};

WorkerTask get_task_by_id(
    uint64_t id,
    uint32_t block_size,
    uint32_t img_width,
    uint32_t img_height);

uint64_t get_num_tasks(
    uint32_t img_width,
    uint32_t img_height,
    uint32_t block_size);