#include "worker_task.h"
#include "camera.h"
#include <mpi/mpi.h>
#include <vector>
#include <cstdint>
#include <cmath>
#include <chrono>
#include "fractal.h"
#include "image_settings.h"

#include <random>

// Initialize the random number generator once
std::random_device rd;
std::mt19937 gen(rd()); // Mersenne Twister RNG

// Uniform distribution between 0 and 1
std::uniform_real_distribution<> dist(0.0, 1.0);

void apply_color_map(float& r, float& g, float& b, int iter, int max_iter)
{
    double t = (double)iter / max_iter;
    r = 9.0 * (1 - t) * t * t * t;
    g = 15.0 * (1 - t) * (1 - t) * t * t;
    b = 8.5 * (1 - t) * (1 - t) * (1 - t) * t;
}

void render_block(
    std::vector<uint8_t>& buffer,
    const ImageSettings& image_settings,
    const FractalSettings& fractal_settings,
    const WorkerTask& task,
    const Camera& camera)
{

    auto fractal_function = get_fractal_function(fractal_settings.type);

    double pixel_size_x = 1.0 / image_settings.width;
    double pixel_size_y = 1.0 / image_settings.height;

    // Computes the color for each subpixel of the partial image
    for (int j = 0; j < task.height; ++j) {
        for (int i = 0; i < task.width; ++i) {

            // Computes pixel coordinate [0, image_size - 1]
            int pixel_x = task.x + i;
            int pixel_y = task.y + j;

            float r = 0.0f, g = 0.0f, b = 0.0f;

            // Adds multi sample anti aliasing
            for (int sample = 1; sample <= image_settings.multi_sample_anti_aliasing; sample++) {

                // Random offset in [0, 1)
                double random_offset_x = dist(gen);
                double random_offset_y = dist(gen);

                double sample_x = pixel_x + pixel_size_x * random_offset_x;
                double sample_y = pixel_y + pixel_size_y * random_offset_y;

                // Computes normalized coordinates in range [-1.0, 1.0]
                double nx = ((double)sample_x / image_settings.width - 0.5) * 2.0;
                double ny = ((double)sample_y / image_settings.height - 0.5) * 2.0;

                // Computes world coordinates with camera
                double wx, wy;
                camera.to_world(nx, ny, wx, wy);
                int iter = fractal_function(wx, wy, fractal_settings);

                float sample_r, sample_g, sample_b;

                apply_color_map(
                    sample_r,
                    sample_g,
                    sample_b,
                    iter,
                    fractal_settings.max_iterations);

                r += sample_r;
                g += sample_g;
                b += sample_b;
            }

            // Stores color into buffer by averaging the colors and mapping to [0, 255]
            int idx = (j * task.width + i) * 3;
            buffer[idx] = r / image_settings.multi_sample_anti_aliasing * 255;
            buffer[idx + 1] = g / image_settings.multi_sample_anti_aliasing * 255;
            buffer[idx + 2] = b / image_settings.multi_sample_anti_aliasing * 255;
        }
    }
}

void worker(
    int rank,
    const ImageSettings& image_settings,
    const Camera& camera,
    const FractalSettings& fractal_settings)
{

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
            render_block(buffer, image_settings, fractal_settings, task, camera);

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
