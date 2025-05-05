#include "master.h"
#include "worker.h"
#include "mpi/mpi.h"

int WIDTH = 1200;
int HEIGHT = 1200;
int BLOCK_SIZE = 64;

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (rank == 0) {
        master(num_procs, WIDTH, HEIGHT, BLOCK_SIZE);
    } else {
        worker(rank, WIDTH, HEIGHT);
    }

    MPI_Finalize();
    return 0;
}
