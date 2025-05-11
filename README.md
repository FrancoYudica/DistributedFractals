# DistributedFractals

**DistributedFractals** is a command-line application for rendering fractals (e.g., Mandelbrot and Julia sets) using CPU-based parallelism with MPI. It enables high-resolution image generation across multiple processing units, making it suitable for both local and distributed rendering setups.

![Mandelbrot Render](https://github.com/user-attachments/assets/0c468c27-a5c8-4965-afba-c9587f26a1de)

---

## 🚀 Overview

This project explores the intersection of fractal rendering and distributed computing using the [Message Passing Interface (MPI)](https://www.mpi-forum.org/). The main goals are:

- Efficiently generate complex fractal images using CPU parallelism.
- Compare performance between sequential and distributed rendering.
- Provide flexibility via a command-line interface for full customization.

---

## 🛠️ Build Instructions

To compile the project:

```bash
mkdir build
cd build
cmake ..
make
```
## ▶️ Running the Application
### Parallel (MPI) Execution

To run the distributed version with 8 processes:
```bash
mpirun -np 8 ./fractal_mpi
```
### Sequential Version
A non-MPI version is also available:
```bash
./sequential
```

## ⚙️ Command-Line Arguments

| Option                     | Description                                                |
|----------------------------|------------------------------------------------------------|
| `-o`, `--output <path>`    | Output image file path                                     |
| `-w`, `--width <int>`      | Image width in pixels                                      |
| `-h`, `--height <int>`     | Image height in pixels                                     |
| `-s`, `--samples <int>`    | Number of MSAA samples                                     |
| `-b`, `--block_size <int>` | Block size in pixels per MPI task                          |
| `-z`, `--zoom <float>`     | Zoom level of the camera                                   |
| `-cx`, `--camera_x <float>`| X position of the camera                                   |
| `-cy`, `--camera_y <float>`| Y position of the camera                                   |
| `-i`, `--iterations <int>` | Maximum number of fractal iterations                       |
| `-t`, `--type <int>`       | Fractal type ID (e.g., 0 = Mandelbrot, 1 = Julia, ...)     |
| `--color_mode <int>`       | Color mode ID                                              |
| `--julia-cx <float>`       | Real component of Julia set constant `C`                   |
| `--julia-cy <float>`       | Imaginary component of Julia set constant `C`              |
| `--help`                   | Display help message                                       |
