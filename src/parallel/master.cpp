#include "worker_task.h"
#include <mpi/mpi.h>
#include <cstdint>
#include <cmath>
#include <chrono>
#include "parallel/master.h"
#include "common/output_handler.h"
#include "common/logging.h"
#include <string.h>

void master(
    uint32_t num_procs,
    const Settings& settings)
{
    std::chrono::time_point start = std::chrono::high_resolution_clock::now();

    uint8_t* image = new uint8_t[settings.image.width * settings.image.height * 3];

    uint64_t num_tasks = get_num_tasks(settings.image.width, settings.image.height, settings.block_size);
    uint32_t sent_task_count = 0;
    uint32_t completed_task_count = 0;
    uint32_t recv_buffer_size = settings.block_size * settings.block_size * 3;
    uint8_t* recv_buffer = new uint8_t[recv_buffer_size];

    while (completed_task_count < num_tasks) {
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        uint32_t source = status.MPI_SOURCE;

        if (status.MPI_TAG == Tag::REQUEST) {
            MPI_Recv(NULL, 0, MPI_BYTE, source, Tag::REQUEST, MPI_COMM_WORLD, &status);
            if (sent_task_count < num_tasks) {
                // Sends task to worker
                uint64_t task_id = sent_task_count++;
                MPI_Send(&task_id, 1, MPI_INT64_T, source, Tag::TASK, MPI_COMM_WORLD);
            } else {
                MPI_Send(NULL, 0, MPI_BYTE, source, Tag::TERMINATE, MPI_COMM_WORLD);
            }

        } else if (status.MPI_TAG == Tag::RESULT) {
            // Receives worker task id
            uint64_t task_id;
            MPI_Recv(&task_id, 1, MPI_INT64_T, source, Tag::RESULT, MPI_COMM_WORLD, &status);
            WorkerTask result = get_task_by_id(
                task_id,
                settings.block_size,
                settings.image.width,
                settings.image.height);

            // Receives subimage buffer
            MPI_Recv(recv_buffer, recv_buffer_size, MPI_BYTE, source, Tag::RESULT, MPI_COMM_WORLD, &status);

            // Copies subimage buffer into main image buffer
            for (uint32_t j = 0; j < result.height; ++j) {
                uint32_t dest_index = 3 * ((result.y + j) * settings.image.width + result.x);
                uint8_t* dest_ptr = &image[dest_index];
                uint8_t* src_ptr = &recv_buffer[j * result.width * 3];
                memcpy(dest_ptr, src_ptr, result.width * sizeof(uint8_t) * 3);
            }
            ++completed_task_count;
            LOG_STATUS("Worker " << source << " completed task. " << 100.0 * (float)completed_task_count / num_tasks << "%");
        }
    }

    // Sends termination tag to all workers
    for (uint32_t i = 1; i < num_procs; ++i) {
        MPI_Send(NULL, 0, MPI_BYTE, i, Tag::TERMINATE, MPI_COMM_WORLD);
    }

    std::chrono::time_point end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    LOG_STATUS("Image generated in " << duration.count() << " ms");

    // Creates output handler based on the settings mode
    std::shared_ptr<OutputHandler> output_handler = OutputHandler::factory_create(settings.output_settings.mode);

    bool success = output_handler->save_output(
        image,
        settings.image.width,
        settings.image.height,
        settings.output_settings);

    delete image;

    if (!success) {
        LOG_ERROR("Unable to output image...");
    } else {
        LOG_SUCCESS("Image outputted");
    }
}