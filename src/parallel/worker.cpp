#include "worker_task.h"
#include <mpi/mpi.h>
#include <cstdint>
#include <cmath>
#include "worker.h"
#include "common/renderer.h"

void worker(
    int rank,
    const ImageSettings& image_settings,
    const Camera& camera,
    const FractalSettings& fractal_settings)
{

    while (true) {

        MPI_Status status;

        // Sends task request to master
        MPI_Send(NULL, 0, MPI_BYTE, 0, Tag::REQUEST, MPI_COMM_WORLD);

        // Waits until a message with any tag is received
        MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        // When the tag is task, master sent task
        if (status.MPI_TAG == Tag::TASK) {
            WorkerTask task;
            MPI_Recv(&task, sizeof(WorkerTask), MPI_BYTE, 0, Tag::TASK, MPI_COMM_WORLD, &status);

            // Creates a buffer to store the partial image pixels
            // std::vector<uint8_t> buffer(task.width * task.height * 3);
            uint32_t buffer_length = task.width * task.height * 3;
            uint8_t* buffer = new uint8_t[buffer_length];

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
            MPI_Send(&task, sizeof(WorkerTask), MPI_BYTE, 0, Tag::RESULT, MPI_COMM_WORLD);
            MPI_Send(buffer, buffer_length, MPI_BYTE, 0, Tag::RESULT, MPI_COMM_WORLD);
            delete buffer;

        } else if (status.MPI_TAG == Tag::TERMINATE) {
            MPI_Recv(NULL, 0, MPI_BYTE, 0, Tag::TERMINATE, MPI_COMM_WORLD, &status);
            break;
        }
    }
}
