#include "master.h"
#include "worker.h"
#include "mpi/mpi.h"
#include "image_settings.h"

void load_args(
    int argc,
    char** argv,

    ImageSettings& image_settings,
    int& block_size)
{
    for (int arg_index = 1; arg_index < argc; arg_index++) {
        const char* parameter = argv[arg_index];

        // Ensures that parameters with values have their value
        if (arg_index + 1 >= argc) {
            std::cout << "Missing value of parameter \"" << parameter << "\"" << std::endl;
            break;
        }

        const char* value = argv[++arg_index];

        // Image width parameter
        if (!strcmp(parameter, "-w")) {
            image_settings.width = atoi(value);
        }
        // Image height parameter
        else if (!strcmp(parameter, "-h")) {
            image_settings.height = atoi(value);
        }
        // Image samples parameter
        else if (!strcmp(parameter, "-s")) {
            image_settings.multi_sample_anti_aliasing = atoi(value);
        }

        // Block size parameter
        else if (!strcmp(parameter, "-b")) {
            block_size = atoi(value);
        }
        // Unrecognized parameter
        else {
            std::cout << "Unrecognized parameter \"" << parameter << "\"" << std::endl;
        }
    }
}

int main(int argc, char** argv)
{

    ImageSettings image_settings;
    int block_size;
    block_size = 64;

    MPI_Init(&argc, &argv);

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // Loads console arguments
    if (rank == 0) {
        load_args(argc, argv, image_settings, block_size);
        std::cout << "Settings - Image resolution(" << image_settings.width << "x" << image_settings.height << ") Block size(" << block_size << ")" << std::endl;
    }

    // Broadcasts arguments to workers
    MPI_Bcast(&image_settings.width, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&image_settings.height, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&image_settings.multi_sample_anti_aliasing, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&block_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Runs Master/Worker functions
    if (rank == 0) {
        master(num_procs, block_size, image_settings);
    }

    else {
        worker(rank, image_settings);
    }

    MPI_Finalize();
    return 0;
}