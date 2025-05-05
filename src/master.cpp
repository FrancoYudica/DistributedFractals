#include "image_utils.h"
#include "worker_task.h"
#include <mpi/mpi.h>
#include <vector>
#include <cstdint>
#include <cmath>
#include <chrono>

void master(
    int num_procs,
    int image_width,
    int image_height,
    int block_size)
{
    std::chrono::time_point start = std::chrono::high_resolution_clock::now();

    std::vector<uint8_t> imagen(image_width * image_height * 3);

    int blocks_x = (image_width + block_size - 1) / block_size;
    int blocks_y = (image_height + block_size - 1) / block_size;
    int block_count = blocks_x * blocks_y;

    std::vector<WorkerTask> tareas;

    // Generar bloques correctamente, incluyendo los de borde
    for (int by = 0; by < blocks_y; ++by) {
        for (int bx = 0; bx < blocks_x; ++bx) {
            int x = bx * block_size;
            int y = by * block_size;
            int w = std::min(block_size, image_width - x);
            int h = std::min(block_size, image_height - y);
            tareas.push_back(WorkerTask { x, y, w, h });
        }
    }

    int tareas_enviadas = 0;
    int tareas_recibidas = 0;
    MPI_Status status;

    while (tareas_recibidas < block_count) {
        int flag = 0;
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
        if (flag) {
            int source = status.MPI_SOURCE;

            if (status.MPI_TAG == Tag::REQUEST) {
                MPI_Recv(NULL, 0, MPI_BYTE, source, Tag::REQUEST, MPI_COMM_WORLD, &status);

                if (tareas_enviadas < tareas.size()) {
                    WorkerTask t = tareas[tareas_enviadas++];
                    MPI_Send(&t, sizeof(WorkerTask), MPI_BYTE, source, Tag::TASK, MPI_COMM_WORLD);
                } else {
                    MPI_Send(NULL, 0, MPI_BYTE, source, Tag::TERMINATE, MPI_COMM_WORLD);
                }

            } else if (status.MPI_TAG == Tag::RESULT) {
                WorkerTask result;
                MPI_Recv(&result, sizeof(WorkerTask), MPI_BYTE, source, Tag::RESULT, MPI_COMM_WORLD, &status);

                std::vector<uint8_t> bloque(result.width * result.height * 3);
                MPI_Recv(bloque.data(), bloque.size(), MPI_BYTE, source, Tag::RESULT, MPI_COMM_WORLD, &status);

                for (int j = 0; j < result.height; ++j) {
                    for (int i = 0; i < result.width; ++i) {
                        int src_idx = (j * result.width + i) * 3;
                        int dst_idx = ((result.y + j) * image_width + (result.x + i)) * 3;
                        imagen[dst_idx] = bloque[src_idx];
                        imagen[dst_idx + 1] = bloque[src_idx + 1];
                        imagen[dst_idx + 2] = bloque[src_idx + 2];
                    }
                }
                ++tareas_recibidas;
            }
        }
    }

    // Asegurar que todos los procesos terminen
    for (int i = 1; i < num_procs; ++i) {
        MPI_Send(NULL, 0, MPI_BYTE, i, Tag::TERMINATE, MPI_COMM_WORLD);
    }

    std::chrono::time_point end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Computation took: " << duration.count() << " ms\n";

    save_image("mandelbrot.png", imagen.data(), image_width, image_height);
}