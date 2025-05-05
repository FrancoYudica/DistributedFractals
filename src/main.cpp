#include "master.h"
#include "worker.h"
#include "mpi/mpi.h"

int main(int argc, char** argv)
{
    int width, height, block_size;
    width = 512;
    height = 512;
    block_size = 64;

    for (int arg_index = 1; arg_index < argc; arg_index++) {
        if (!strcmp(argv[arg_index], "-w")) {
            arg_index++;
            width = atoi(argv[arg_index]);
        }
        if (!strcmp(argv[arg_index], "-h")) {
            arg_index++;
            height = atoi(argv[arg_index]);
        }
        if (!strcmp(argv[arg_index], "-b")) {
            arg_index++;
            block_size = atoi(argv[arg_index]);
        }
    }
    std::cout << "Generating Image...  " << width << "x" << height << ". Block size" << block_size << std::endl;
    MPI_Init(&argc, &argv);

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (rank == 0) {
        master(num_procs, width, height, block_size);
    } else {
        worker(rank, width, height);
    }

    MPI_Finalize();
    return 0;
}
