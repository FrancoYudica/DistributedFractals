#include "common/common.h"
#include "master.h"
#include "worker.h"
#include "mpi/mpi.h"

int main(int argc, char** argv)
{

    Settings settings;

    MPI_Init(&argc, &argv);

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (rank == 0) {
        load_args(argc, argv, settings);

        std::cout << "Settings - Image resolution(" << settings.image.width << "x" << settings.image.height
                  << ") Block size(" << settings.block_size << ") Zoom(" << (double)settings.camera.zoom << ") Camera(" << (double)settings.camera.x << ", " << (double)settings.camera.y
                  << ") Max Iterations(" << settings.fractal.max_iterations << ") Type(" << settings.fractal.type << ")\n";
    }

    // Broadcasts arguments to workers
    MPI_Bcast(&settings.image.width, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&settings.image.height, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&settings.image.multi_sample_anti_aliasing, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&settings.block_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&settings.fractal, sizeof(FractalSettings), MPI_BYTE, 0, MPI_COMM_WORLD);

    // Camera position and zoom can't don't fit in 128bits, therefore sending those numbers
    // as a string is necessary
    char camera_position_x[NUMBER_SERIAL_SIZE];
    char camera_position_y[NUMBER_SERIAL_SIZE];
    char camera_zoom[NUMBER_SERIAL_SIZE];

    SERIALIZE_NUM(settings.camera.x, camera_position_x);
    SERIALIZE_NUM(settings.camera.y, camera_position_y);
    SERIALIZE_NUM(settings.camera.zoom, camera_zoom);

    MPI_Bcast(camera_position_x, sizeof(camera_position_x), MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Bcast(camera_position_y, sizeof(camera_position_y), MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Bcast(camera_zoom, sizeof(camera_zoom), MPI_CHAR, 0, MPI_COMM_WORLD);

    DESERIALIZE_NUM(settings.camera.x, camera_position_x);
    DESERIALIZE_NUM(settings.camera.y, camera_position_y);
    DESERIALIZE_NUM(settings.camera.zoom, camera_zoom);

    // Runs Master/Worker functions
    if (rank == 0) {
        master(num_procs, settings);
    }

    else {
        worker(
            rank,
            settings.image,
            settings.camera,
            settings.fractal);
    }

    MPI_Finalize();
    return 0;
}