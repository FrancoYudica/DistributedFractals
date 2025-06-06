#include "worker_task.h"

#include <cmath>

WorkerTask get_task_by_id(
    uint64_t id,
    uint32_t block_size,
    uint32_t img_width,
    uint32_t img_height)
{
    uint64_t x_tasks = (img_width + block_size - 1) / block_size;

    // Reverse mapping from id to (x, y)
    uint32_t id_x = id % x_tasks;
    uint32_t id_y = id / x_tasks;

    uint32_t x = id_x * block_size;
    uint32_t y = id_y * block_size;

    // Clamp block size to not exceed image bounds
    uint32_t w = std::min(block_size, img_width - x);
    uint32_t h = std::min(block_size, img_height - y);

    return { x, y, w, h };
}

uint64_t get_num_tasks(
    uint32_t img_width,
    uint32_t img_height,
    uint32_t block_size)
{
    uint64_t x_tasks = (img_width + block_size - 1) / block_size;
    uint64_t y_tasks = (img_height + block_size - 1) / block_size;
    return x_tasks * y_tasks;
}