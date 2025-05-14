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

## Output mode
After the image is generated, the program can output at the following modes:
- **Disk**: Stores the generated image in the specified output filepath
- **Network**: Connects to a remote server and sends the generated *.png* image buffer through TCP. Note that this involves a sever. A simple implementation of this server is located at `src/scripts/image_server.py`


## ⚙️ Command-Line Arguments

| Option                     | Description                                                |
|----------------------------|------------------------------------------------------------|
| `-od`, `--output_disk <opt filename>`    | Save output image to disk. Defaults to 'output.png' if no filename is provided|
| `-on`, `--output_network <opt ip <opt port>>`      |Send output image over TCP. Defaults to IP 0.0.0.0 and port 5001 if not specified.|
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