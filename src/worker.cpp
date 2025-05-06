#include "worker_task.h"
#include <mpi/mpi.h>
#include <vector>
#include <cstdint>
#include <cmath>
#include <chrono>

int compute_fractal(int x, int y, int image_width, int image_height, int max_iterations)
{
    double zoom = 1.0;
    double center_x = -0.5;
    double center_y = 0.0;

    double fx = ((double)x / image_width - 0.5) * 2.0 / zoom + center_x;
    double fy = ((double)y / image_height - 0.5) * 2.0 / zoom + center_y;

    double zx = 0, zy = 0;
    int iter = 0;
    while (zx * zx + zy * zy < 4.0 && iter < max_iterations) {
        double tmp = zx * zx - zy * zy + fx;
        zy = 2.0 * zx * zy + fy;
        zx = tmp;
        iter++;
    }
    return iter;
}

void apply_color_map(uint8_t& r, uint8_t& g, uint8_t& b, int iter, int max_iter)
{
    double t = (double)iter / max_iter;
    r = (uint8_t)(9 * (1 - t) * t * t * t * 255);
    g = (uint8_t)(15 * (1 - t) * (1 - t) * t * t * 255);
    b = (uint8_t)(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);
}

void worker(
    int rank,
    int image_width,
    int image_height)
{
    MPI_Status status;
    WorkerTask task;

    std::chrono::time_point start = std::chrono::high_resolution_clock::now();
    std::chrono::time_point end = std::chrono::high_resolution_clock::now();

    while (true) {

        // Sends task request to master
        MPI_Send(NULL, 0, MPI_BYTE, 0, Tag::REQUEST, MPI_COMM_WORLD);

        // Waits until a message with any tag is received
        MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        // When the tag is task, master sent task
        if (status.MPI_TAG == Tag::TASK) {
            // Get start time
            start = std::chrono::high_resolution_clock::now();

            MPI_Recv(&task, sizeof(WorkerTask), MPI_BYTE, 0, Tag::TASK, MPI_COMM_WORLD, &status);

            // Creates a buffer to store the partial image pixels
            std::vector<uint8_t> buffer(task.width * task.height * 3);

            int max_iterations = 510;

            // Computes the color for each subpixel of the partial image
            for (int j = 0; j < task.height; ++j) {
                for (int i = 0; i < task.width; ++i) {

                    int iter = compute_fractal(
                        task.x + i,
                        task.y + j,
                        image_width,
                        image_height,
                        max_iterations);
                    int idx = (j * task.width + i) * 3;
                    apply_color_map(
                        buffer[idx],
                        buffer[idx + 1],
                        buffer[idx + 2],
                        iter,
                        max_iterations);
                }
            }

            // Sends task and buffer with contents
            MPI_Send(&task, sizeof(WorkerTask), MPI_BYTE, 0, Tag::RESULT, MPI_COMM_WORLD);
            MPI_Send(buffer.data(), buffer.size(), MPI_BYTE, 0, Tag::RESULT, MPI_COMM_WORLD);
            end = std::chrono::high_resolution_clock::now();

            // Calculate duration in microseconds
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

            std::cout << "Execution time: " << duration.count() << " microseconds\n";

        } else if (status.MPI_TAG == Tag::TERMINATE) {
            MPI_Recv(NULL, 0, MPI_BYTE, 0, Tag::TERMINATE, MPI_COMM_WORLD, &status);
            break;
        }
    }
}
