import argparse
import subprocess
import math
import os
import time
from decimal import Decimal, getcontext

# Set precision to about 256 bits (~77 decimal digits)
getcontext().prec = 77

def decimal_log2(decimal: Decimal) -> Decimal:
    return decimal.ln() / Decimal(math.log(2))

def decimal_pow(base: Decimal, exp: Decimal) -> Decimal:
    return (exp * base.ln()).exp()

# This function returns the zoom using an exponential formula
# This way, when the camera applies zoom, it looks linear and constant
def get_zoom_exponential(
        t: Decimal,
        min_zoom: Decimal,
        max_zoom: Decimal
) -> Decimal:
    
    x0 = decimal_log2(min_zoom)
    x1 = decimal_log2(max_zoom / decimal_pow(Decimal(2.0),  x0))
    return decimal_pow(Decimal(2.0), x0 + t * x1)

def get_iterations(
        zoom: Decimal, 
        base_iter: Decimal, 
        scale: Decimal) -> int:
    
    # Uses logarithmic scaling for iterations based on zoom
    return int(base_iter + decimal_log2(zoom) * scale)
    


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
    # Saves as many images as zoom levels
    for frame in range(args.frames):

        zoom = get_zoom_exponential(
            Decimal(frame / (args.frames - 1)),
            Decimal(args.z0),
            Decimal(args.z1)
        )

        iterations = get_iterations(
            zoom,
            Decimal(256),
            Decimal(64))

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

        command.append('--iterations')
        command.append(str(iterations))

        command.append('-od')
        command.append(output_name)

        command.extend(cpp_args)
        print(command)
        
        print(f"PROGRESS: {float(frame) / (args.frames - 1) * 100.0}% Running:", ' '.join(command))
        subprocess.run(command, check=True, capture_output=False)
    
    command = f"ffmpeg -framerate 10 -i {dir_name}/fractal_zoom_%d.png -c:v libx264 -pix_fmt yuv420p {os.path.join(dir_name, "fractal_zoom.mp4")} -y".split(" ")
    print("Running ffmpeg to convert to video. {}")
    subprocess.run(command, check=True)

if __name__ == '__main__':
    main()
