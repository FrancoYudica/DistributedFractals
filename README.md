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
- Compare performance between sequential and distributed rendering.
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

| Option                                                  | Description                                                                                 |
| ------------------------------------------------------- | ------------------------------------------------------------------------------------------- |
| `-od`, `--output_disk <opt filename>`                   | Save output image to disk. Defaults to 'output.png' if no filename is provided              |
| `-on`, `--output_network <opt ip <opt port> <opt UUID>` | Send output image over TCP. Defaults to IP 0.0.0.0, port 5001 and no UUID if not specified. |
| `-w`, `--width <int>`                                   | Image width in pixels                                                                       |
| `-h`, `--height <int>`                                  | Image height in pixels                                                                      |
| `-s`, `--samples <int>`                                 | Number of MSAA samples                                                                      |
| `-b`, `--block_size <int>`                              | Block size in pixels per MPI task                                                           |
| `-z`, `--zoom <float>`                                  | Zoom level of the camera                                                                    |
| `-cx`, `--camera_x <float>`                             | X position of the camera                                                                    |
| `-cy`, `--camera_y <float>`                             | Y position of the camera                                                                    |
| `-i`, `--iterations <int>`                              | Maximum number of fractal iterations                                                        |
| `-t`, `--type <int>`                                    | Fractal type ID (e.g., 0 = Mandelbrot, 1 = Julia, ...)                                      |
| `--color_mode <int>`                                    | Color mode ID                                                                               |
| `--julia-cx <float>`                                    | Real component of Julia set constant `C`                                                    |
| `--julia-cy <float>`                                    | Imaginary component of Julia set constant `C`                                               |
| `--help`                                                | Display help message                                                                        |

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

## Whitepaper

For the initial version of this project, a detailed whitepaper was created that delves into the implementation intricacies, emphasizing experimentation and the parallel programming architecture behind it. You can access the whitepaper in its dedicated repository, available under the [releases section](https://github.com/FrancoYudica/DistributedFractals-Whitepaper/releases).

## Samples!

<p align="center">
  <img src="https://github.com/user-attachments/assets/40d67eec-7bf6-479b-9399-f4f84f9a52ef" alt="mandelbrot_sample_4" width="45%"/>
  <img src="https://github.com/user-attachments/assets/2f84ac84-a359-4c5c-9d81-e06e43479957" alt="mandelbrot_sample3" width="45%"/>
</p>
<p align="center">
  <img src="https://github.com/user-attachments/assets/3d94d4c9-8efd-4bc7-b4d9-68653f33ea39" alt="mandelbrot_sample2" width="45%"/>
  <img src="https://github.com/user-attachments/assets/11713464-3240-471b-82e3-efc9ac077d0b" alt="mandelbrot_sample1" width="45%"/>
</p>
<p align="center">
  <img src="https://github.com/user-attachments/assets/1fa634cd-6421-469d-b7dd-8860de59cff8" alt="mandelbrot_sample0" width="45%"/>
  <img src="https://github.com/user-attachments/assets/cb0073a7-d29c-4abc-a5e3-036c7300dede" alt="julia_sample4" width="45%"/>
</p>
<p align="center">
  <img src="https://github.com/user-attachments/assets/df66eed8-cbdd-41e8-897d-2a4832163b02" alt="julia_sample3" width="45%"/>
  <img src="https://github.com/user-attachments/assets/f4d52548-35fd-4511-897d-92e1dd2b332a" alt="julia_sample1" width="45%"/>
</p>
<p align="center">
  <img src="https://github.com/user-attachments/assets/7561da1e-d7db-4042-83b8-0a17f7ce16a9" alt="julia_sample0" width="45%"/>
  <img src="https://github.com/user-attachments/assets/5131ed23-f094-4782-b4f5-3e360796d7fc" alt="julia_sample0" width="45%"/>
</p>
