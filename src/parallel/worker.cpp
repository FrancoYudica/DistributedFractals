#include "worker_task.h"
#include <mpi/mpi.h>
#include <cstdint>
#include <cmath>
#include "worker.h"
#include "common/renderer.h"

void worker(
    uint32_t rank,
    uint32_t block_size,
    const ImageSettings& image_settings,
    const Camera& camera,
    const FractalSettings& fractal_settings)
{

    // Creates a buffer to store the partial image pixels
    uint32_t buffer_len = block_size * block_size * 3;
    uint8_t* buffer = new uint8_t[buffer_len];

    while (true) {

        MPI_Status status;

        // Sends task request to master
        MPI_Send(NULL, 0, MPI_BYTE, 0, Tag::REQUEST, MPI_COMM_WORLD);

        // Waits until a message with any tag is received
        MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        // When the tag is task, master sent task
        if (status.MPI_TAG == Tag::TASK) {
            uint64_t task_id;
            MPI_Recv(&task_id, 1, MPI_INT64_T, 0, Tag::TASK, MPI_COMM_WORLD, &status);

            auto task = get_task_by_id(
                task_id,
                block_size,
                image_settings.width,
                image_settings.height);

            // Renders the block into buffer
            render_block(
                buffer,
                image_settings,
                fractal_settings,
                camera,
                task.x,
                task.y,
                task.width,
                task.height);

            // Sends task and buffer with contents
            MPI_Send(&task_id, 1, MPI_INT64_T, 0, Tag::RESULT, MPI_COMM_WORLD);
            MPI_Send(buffer, buffer_len, MPI_BYTE, 0, Tag::RESULT, MPI_COMM_WORLD);

        } else if (status.MPI_TAG == Tag::TERMINATE) {
            MPI_Recv(NULL, 0, MPI_BYTE, 0, Tag::TERMINATE, MPI_COMM_WORLD, &status);
            break;
        }
    }

    delete buffer;
}
