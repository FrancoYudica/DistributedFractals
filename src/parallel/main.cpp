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
                  << ") Block size(" << settings.block_size << ") Zoom(" << settings.camera.zoom << ") Camera(" << settings.camera.x << ", " << settings.camera.y
                  << ") Max Iterations(" << settings.fractal.max_iterations << ") Type(" << settings.fractal.type << ")\n";
    }

    // Broadcasts arguments to workers
    MPI_Bcast(&settings.image.width, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&settings.image.height, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&settings.image.multi_sample_anti_aliasing, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&settings.block_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Bcast(&settings.camera, sizeof(Camera), MPI_BYTE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&settings.fractal, sizeof(FractalSettings), MPI_BYTE, 0, MPI_COMM_WORLD);

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