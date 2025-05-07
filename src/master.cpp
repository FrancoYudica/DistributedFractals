#include "image_utils.h"
#include "worker_task.h"
#include <mpi/mpi.h>
#include <vector>
#include <cstdint>
#include <cmath>
#include <chrono>
#include "master.h"

void master(
    int num_procs,
    const Settings& settings)
{
    std::chrono::time_point start = std::chrono::high_resolution_clock::now();

    std::vector<uint8_t> image(settings.image.width * settings.image.height * 3);
    int width = settings.image.width, height = settings.image.height;
    int block_size = settings.block_size;
    int blocks_x = (width + settings.block_size - 1) / block_size;
    int blocks_y = (height + block_size - 1) / block_size;
    int block_count = blocks_x * blocks_y;

    std::vector<WorkerTask> worker_tasks;

    // Splits the image into blocks, creating the WorkerTasks
    for (int by = 0; by < blocks_y; ++by) {
        for (int bx = 0; bx < blocks_x; ++bx) {
            int x = bx * block_size;
            int y = by * block_size;
            int w = std::min(block_size, width - x);
            int h = std::min(block_size, height - y);
            worker_tasks.push_back(WorkerTask { x, y, w, h });
        }
    }

    int sent_task_count = 0;
    int completed_task_count = 0;
    MPI_Status status;

    while (completed_task_count < block_count) {
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        int source = status.MPI_SOURCE;

        if (status.MPI_TAG == Tag::REQUEST) {
            MPI_Recv(NULL, 0, MPI_BYTE, source, Tag::REQUEST, MPI_COMM_WORLD, &status);

            if (sent_task_count < worker_tasks.size()) {
                WorkerTask t = worker_tasks[sent_task_count++];
                MPI_Send(&t, sizeof(WorkerTask), MPI_BYTE, source, Tag::TASK, MPI_COMM_WORLD);
            } else {
                MPI_Send(NULL, 0, MPI_BYTE, source, Tag::TERMINATE, MPI_COMM_WORLD);
            }

        } else if (status.MPI_TAG == Tag::RESULT) {

            // Receives worker task
            WorkerTask result;
            MPI_Recv(&result, sizeof(WorkerTask), MPI_BYTE, source, Tag::RESULT, MPI_COMM_WORLD, &status);

            // Receives subimage buffer
            std::vector<uint8_t> buffer(result.width * result.height * 3);
            MPI_Recv(buffer.data(), buffer.size(), MPI_BYTE, source, Tag::RESULT, MPI_COMM_WORLD, &status);

            // Copies subimage buffer into main image buffer
            for (int j = 0; j < result.height; ++j) {
                for (int i = 0; i < result.width; ++i) {
                    int src_idx = (j * result.width + i) * 3;
                    int dst_idx = ((result.y + j) * width + (result.x + i)) * 3;
                    image[dst_idx] = buffer[src_idx];
                    image[dst_idx + 1] = buffer[src_idx + 1];
                    image[dst_idx + 2] = buffer[src_idx + 2];
                }
            }
            ++completed_task_count;
        }
    }

    // Sends termination tag to all workers
    for (int i = 1; i < num_procs; ++i) {
        MPI_Send(NULL, 0, MPI_BYTE, i, Tag::TERMINATE, MPI_COMM_WORLD);
    }

    std::chrono::time_point end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Computation took: " << duration.count() << " ms\n";

    save_image(settings.output_path, image.data(), width, height);
}