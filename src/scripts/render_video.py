import argparse
import subprocess
import math
import os
import time

# This function returns the zoom using an exponential formula
# This way, when the camera applies zoom, it looks linear and constant
def get_zoom_exponential(
        t: float,
        min_zoom: float,
        max_zoom: float
) -> float:
    
    x0 = math.log2(min_zoom)
    x1 = math.log2(max_zoom / (2.0 ** x0))

    return 2.0 ** (x0 + t * x1)

def main():
    parser = argparse.ArgumentParser(description="Batch render fractals at different zoom levels")

    parser.add_argument('--program', required=True, help='Path to the MPI program (e.g. ./fractal_mpi)')
    parser.add_argument('--z0', type=float, default=1.0, help='Starting zoom level')
    parser.add_argument('--z1', type=float, default=10.0, help='Ending zoom level')
    parser.add_argument('--frames', type=int, default=5, help='Number of zoom steps')
    parser.add_argument('--output_folder', type=str, default='.', help='Base name for output images')
    parser.add_argument('--hostfile', type=str, default='', help='MPI hostfile filepath')
    parser.add_argument('--np', type=int, default=8, help='MPI processes count')

    args, cpp_args = parser.parse_known_args()

    # Creates a directory to store the images
    dir_name = os.path.abspath(f"{args.output_folder}/images_{int(time.time())}")
    os.mkdir(dir_name)
    print(dir_name)

    # Saves as many images as zoom levels
    for frame in range(args.frames):

        zoom = get_zoom_exponential(
            frame / (args.frames - 1),
            args.z0,
            args.z1
        )

        output_name = os.path.join(dir_name, f"fractal_zoom_{frame}.png")
        command = ['mpirun']

        if args.hostfile != '':
            command.append('-hostfile')
            command.append(args.hostfile)

        command.append('-np')
        command.append(str(args.np))
        
        command.append(args.program)

        command.append('--zoom')
        command.append(str(zoom))

        command.append('-od')
        command.append(output_name)

        command.extend(cpp_args)
        print(command)
        
        print("Running:", ' '.join(command))
        subprocess.run(command, check=True)
    
    command = f"ffmpeg -framerate 10 -i {dir_name}/fractal_zoom_%d.png -c:v libx264 -pix_fmt yuv420p {os.path.join(dir_name, "fractal_zoom.mp4")} -y".split(" ")
    print("Running ffmpeg to convert to video. {}")
    subprocess.run(command, check=True)

if __name__ == '__main__':
    main()
