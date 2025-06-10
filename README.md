# DistributedFractals

**DistributedFractals** is a command-line application for rendering fractals (e.g., Mandelbrot and Julia sets) using CPU-based parallelism with MPI. It enables high-resolution image generation across multiple processing units, making it suitable for both local and distributed rendering setups.

<p align="center">
  <img src="https://github.com/user-attachments/assets/622866fd-3f48-4243-b9f9-58f7aa330c76" alt="Mandelbrot Set" width="45%"/>
  <img src="https://github.com/user-attachments/assets/ec467539-f3b3-4ea8-b981-d3363a5af00d" alt="Julia Set" width="45%"/>
</p>

---

## 🚀 Overview

This project explores the intersection of fractal rendering and distributed computing using the [Message Passing Interface (MPI)](https://www.mpi-forum.org/). The main goals are:

- Efficiently generate complex fractal images using CPU parallelism.
- Provide flexibility via a command-line interface for full customization.

---

## Features

- High-resolution fractal rendering using MPI-based parallelism
- Implements a master/worker architecture:
  - The master process generates a queue of image blocks (tasks).
  - Workers pull tasks dynamically as they finish their current ones, ensuring better load balancing.
  - Each block contributes to a portion of the final image.
- Support for Mandelbrot and Julia sets (extensible)
- Adjustable rendering parameters via CLI
- Interactive fractal exploration with zoom support
- Multiple output modes: save to disk or stream via network

## 🛠️ Build Instructions

> ⚠️ **Note**: This project currently supports **Linux only**. It relies on POSIX features and has not been tested on Windows or macOS.

### Requirements

- CMake >= 3.14
- MPI implementation (e.g., MPICH or OpenMPI)
  ```bash
  sudo apt install openmpi-bin libopenmpi-dev openmpi-common
  ```
- C++17-compatible compiler

### Compiling

To compile the project:

```bash
mkdir build
cd build
cmake -DCMAKE_CXX_COMPILER=mpicxx -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3" ..
make
```

## ▶️ Running the Application

### Parallel (MPI) Execution

```bash
## To run the distributed version with 8 processes
mpirun -np 8 ./fractal_mpi
```

```bash
## This renders a Mandelbrot image using 4 processes and saves it to mandelbrot.png.
mpirun -np 4 ./fractal_mpi -w 1080 -h 720 -z 1 -cx -0.7 -cy 0.0 -i 64 -t 0 -s 4 -od mandelbrot.png
```

### Sequential Version

A non-MPI version is also available:

```bash
./sequential
```

## Output mode

After the image is generated, the program can output at the following modes:

- **Disk**: Stores the generated image in the specified output filepath
- **Network**: Connects to a remote server and sends the generated _.png_ image buffer through TCP. Note that this involves a server. A simple implementation of this server is located at `src/scripts/image_server.py`

## ⚙️ Command-Line Arguments

| Option(s)               | Argument(s)                 | Description                                                  |
|-------------------------|-----------------------------|--------------------------------------------------------------|
| `-od`, `--output_disk`  | `[opt filename]`            | Save output image to disk. Defaults to `output.png`.         |
| `-on`, `--output_network` | `[opt IP [opt port]]`      | Send output image over TCP. Defaults to IP `0.0.0.0`, port `5001`. |
| `-w`, `--width`         | `<int>`                     | Image width in pixels.                                       |
| `-h`, `--height`        | `<int>`                     | Image height in pixels.                                      |
| `-s`, `--samples`       | `<int>`                     | Number of MSAA samples. Must be a perfect square number      |
| `-b`, `--block_size`    | `<int>`                     | Size in pixels of the MPI image task.                        |
| `-z`, `--zoom`          | `<float>`                   | Zoom level of the camera.                                    |
| `-cx`, `--camera_x`     | `<float>`                   | Camera X position.                                           |
| `-cy`, `--camera_y`     | `<float>`                   | Camera Y position.                                           |
| `-i`, `--iterations`    | `<int>`                     | Max iterations for fractal.                                  |
| `-t`, `--type`          | `<int>`                     | Fractal type ID.                                             |
| `--color_mode`          | `<int>`                     | Color mode type ID.                                          |
| `--julia-cx`            | `<float>`                   | Real component of Julia set C constant.                      |
| `--julia-cy`            | `<float>`                   | Imaginary component of Julia set C constant.                 |
| `--quiet`               | *(none)*                    | Disables all console messages.                               |
| `--help`                | *(none)*                    | Show this help message.                                      |


## Interactive visualizer

A simple interactive visualizer is also included, built using Python and Pygame. This viewer allows users to explore the fractal image by zooming into selected regions. It accepts the same command-line arguments as the fractal renderer and passes them directly to the compiled executable. Therefore, make sure the renderer has already been built before running the visualizer.

### Setup

Creating python virtual environment

```bash
cd src/scripts
python3 -m venv .venv
source .venv/bin/activate
```

Installing pygame

```bash
pip install pygame
```

### Running

```bash
python3 ./interactive_visualizer.py
```

You can also specify the number of mpi processors with argument `--np <x>` and the rendering executable path `--executable_path <path>`.

## Video renderer

This project also includes a simple video renderer script that generates a sequence of frames to create a video. It repeatedly executes the compiled binary while interpolating the camera position and zoom level across frames.

## Whitepaper

For the initial version of this project, a detailed whitepaper was created that delves into the implementation intricacies, emphasizing experimentation and the parallel programming architecture behind it. You can access the whitepaper in its dedicated repository, available under the [releases section](https://github.com/FrancoYudica/DistributedFractals-Whitepaper/releases).

## Samples!

<p align="center">
  <img src="https://github.com/user-attachments/assets/005fee64-f44e-4a69-853c-2708112aa659" alt="mandelbrot_sample_4" width="45%"/>
  <img src="https://github.com/user-attachments/assets/5e5cb1f3-f7ea-4c4d-b696-9c299812c163" alt="mandelbrot_sample3" width="45%"/>
</p>

<p align="center">
  <img src="https://github.com/user-attachments/assets/d4c14b7c-2c8d-499c-b026-9dd86e882f14" alt="mandelbrot_sample3" width="90%"/>
</p>


<p align="center">
  <img src="https://github.com/user-attachments/assets/5a8ac827-8a0f-4eba-917e-a781b46ac175" alt="mandelbrot_sample_4" width="45%"/>
  <img src="https://github.com/user-attachments/assets/23fe7f5d-d7f3-4372-b008-cf1a4d4dce3d" alt="mandelbrot_sample3" width="45%"/>
</p>

<p align="center">
  <img src="https://github.com/user-attachments/assets/49491bbd-6587-4dd7-9e71-49e50a79139c" alt="mandelbrot_sample3" width="90%"/>
</p>


<p align="center">
  <img src="https://github.com/user-attachments/assets/d286290b-7377-4edd-a5e8-7bb5effb248c" alt="mandelbrot_sample3" width="45%"/>
  <img src="https://github.com/user-attachments/assets/d816ceef-0ea1-4711-9289-cfff7910bdec" alt="mandelbrot_sample3" width="45%"/>
</p>


## Video

<div align=center>

[![Watch the video](https://img.youtube.com/vi/RaeWO0s3MXI/0.jpg)](https://www.youtube.com/watch?v=RaeWO0s3MXI)

Watch the video in YouTube!
</div>
