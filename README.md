# DistributedFractals
![mandelbrot](https://github.com/user-attachments/assets/0c468c27-a5c8-4965-afba-c9587f26a1de)

## Compiling
```bash
mkdir build
cd build
cmake ..
make
```

## Running locally

```bash
mpirun -np 8 ./fractal_mpi
```

Also, there is a sequential version that doesn't use mpi
```bash
./sequential
```

https://github.com/user-attachments/assets/634b76d2-0196-4b7e-9c76-ac6b2a20bc6a

