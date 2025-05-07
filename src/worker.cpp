#include "worker_task.h"
#include "camera.h"
#include <mpi/mpi.h>
#include <vector>
#include <cstdint>
#include <cmath>
#include <chrono>
#include "fractal.h"

void apply_color_map(uint8_t& r, uint8_t& g, uint8_t& b, int iter, int max_iter)
{
    double t = (double)iter / max_iter;
    r = (uint8_t)(9 * (1 - t) * t * t * t * 255);
    g = (uint8_t)(15 * (1 - t) * (1 - t) * t * t * 255);
    b = (uint8_t)(8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255);
}

void render_block(
    std::vector<uint8_t>& buffer,
    int image_width,
    int image_height,
    const FractalSettings& fractal_settings,
    const WorkerTask& task,
    const Camera& camera)
{

    auto fractal_function = get_fractal_function(fractal_settings.type);

    // Computes the color for each subpixel of the partial image
    for (int j = 0; j < task.height; ++j) {
        for (int i = 0; i < task.width; ++i) {

            // Computes pixel coordinate [0, image_size - 1]
            int pixel_x = task.x + i;
            int pixel_y = task.y + j;

            // Computes normalized coordinates in range [-1.0, 1.0]
            double nx = ((double)pixel_x / image_width - 0.5) * 2.0;
            double ny = ((double)pixel_y / image_height - 0.5) * 2.0;

            // Computes world coordinates with camera
            double wx, wy;
            camera.to_world(nx, ny, wx, wy);

            int iter = fractal_function(wx, wy, fractal_settings);
            int idx = (j * task.width + i) * 3;
            apply_color_map(
                buffer[idx],
                buffer[idx + 1],
                buffer[idx + 2],
                iter,
                fractal_settings.max_iterations);
        }
    }
}

void worker(
    int rank,
    int image_width,
    int image_height)
{

    Camera camera;
    camera.zoom = 1.0;
    camera.x = -0.5;
    camera.y = 0.0;

    FractalSettings fractal_settings;
    fractal_settings.max_iterations = 256;
    fractal_settings.type = FRACTAL_MANDELBROT;

    std::chrono::time_point start = std::chrono::high_resolution_clock::now();
    std::chrono::time_point end = std::chrono::high_resolution_clock::now();

    while (true) {

        MPI_Status status;

        // Sends task request to master
        MPI_Send(NULL, 0, MPI_BYTE, 0, Tag::REQUEST, MPI_COMM_WORLD);

        // Waits until a message with any tag is received
        MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        // When the tag is task, master sent task
        if (status.MPI_TAG == Tag::TASK) {
            // Get start time
            start = std::chrono::high_resolution_clock::now();

            WorkerTask task;
            MPI_Recv(&task, sizeof(WorkerTask), MPI_BYTE, 0, Tag::TASK, MPI_COMM_WORLD, &status);

            // Creates a buffer to store the partial image pixels
            std::vector<uint8_t> buffer(task.width * task.height * 3);

            // Renders the block into buffer
            render_block(buffer, image_width, image_height, fractal_settings, task, camera);

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
