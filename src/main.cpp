#include "master.h"
#include "worker.h"
#include "mpi/mpi.h"
#include "settings/settings.h"

void load_args(
    int argc,
    char** argv,
    Settings& settings)
{
    for (int arg_index = 1; arg_index < argc; arg_index++) {
        const char* parameter = argv[arg_index];

        // Ensures that parameters with values have their value
        if (arg_index + 1 >= argc) {
            std::cout << "Missing value of parameter \"" << parameter << "\"" << std::endl;
            break;
        }

        const char* value = argv[++arg_index];

        if (!strcmp(parameter, "-o") || !strcmp(parameter, "--output")) {
            strcpy(settings.output_path, value);
        }

        // Image width parameter
        if (!strcmp(parameter, "-w") || !strcmp(parameter, "--width")) {
            settings.image.width = atoi(value);
        }
        // Image height parameter
        else if (!strcmp(parameter, "-h") || !strcmp(parameter, "--height")) {
            settings.image.height = atoi(value);
        }
        // Image samples parameter
        else if (!strcmp(parameter, "-s") || !strcmp(parameter, "--samples")) {
            settings.image.multi_sample_anti_aliasing = atoi(value);
        }

        // Block size parameter
        else if (!strcmp(parameter, "-b") || !strcmp(parameter, "--block_size")) {
            settings.block_size = atoi(value);
        }

        // Camera Zoom
        else if (!strcmp(parameter, "-z") || !strcmp(parameter, "--zoom")) {
            settings.camera.zoom = atof(value);
        }

        // Camera X pos
        else if (!strcmp(parameter, "-cx") || !strcmp(parameter, "--camera_x")) {
            settings.camera.x = atof(value);
        }

        // Camera Y pos
        else if (!strcmp(parameter, "-cy") || !strcmp(parameter, "--camera_y")) {
            settings.camera.y = atof(value);
        }

        // Fractal Setting Max Iteration
        else if (!strcmp(parameter, "-i") || !strcmp(parameter, "--iterations")) {
            settings.fractal.max_iterations = atoi(value);
        }

        // Fractal Setting Type of Fractal Function
        else if (!strcmp(parameter, "-t") || !strcmp(parameter, "--type")) {
            settings.fractal.type = static_cast<FractalType>(atoi(value));
        }

        // Unrecognized parameter
        else {
            std::cout << "Unrecognized parameter \"" << parameter << "\"" << std::endl;
        }
    }
}

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