import argparse
import subprocess
import math
import os
import time

def main():
    parser = argparse.ArgumentParser(description="Batch render fractals at different zoom levels")

    parser.add_argument('--program', required=True, help='Path to the MPI program (e.g. ./fractal_mpi)')
    parser.add_argument('--zoom_start', type=float, default=1.0, help='Starting zoom level')
    parser.add_argument('--zoom_end', type=float, default=10.0, help='Ending zoom level')
    parser.add_argument('--zoom_steps', type=int, default=5, help='Number of zoom steps')
    parser.add_argument('--output_folder', type=str, default='.', help='Base name for output images')

    args, cpp_args = parser.parse_known_args()

    # Creates a directory to store the images
    dir_name = os.path.abspath(f"{args.output_folder}/images_{int(time.time())}")
    os.mkdir(dir_name)
    print(dir_name)

    # Computes zoom levels to make a continuous zoom
    # This involves using a logarithmic zooming to compensate
    log_zoom_start = math.log10(args.zoom_start)
    log_zoom_end = math.log10(args.zoom_end)
    zoom_levels = []
    for i in range(args.zoom_steps):
        t = i / max(args.zoom_steps - 1, 1)
        log_zoom = log_zoom_start + t * (log_zoom_end - log_zoom_start)
        zoom_levels.append(10.0 ** log_zoom)

    # Saves as many images as zoom levels
    for i, zoom in enumerate(zoom_levels):
        output_name = os.path.join(dir_name, f"fractal_zoom_{i}.png")
        command = [
            'mpirun', '-np', '8', args.program,
            '--zoom', str(zoom),
            '--output', output_name
        ] + cpp_args

        print("Running:", ' '.join(command))
        subprocess.run(command, check=True)
    
    command = f"ffmpeg -framerate 10 -i {dir_name}/fractal_zoom_%d.png -c:v libx264 -pix_fmt yuv420p {os.path.join(dir_name, "fractal_zoom.mp4")} -y".split(" ")
    print("Running ffmpeg to convert to video. {}")
    subprocess.run(command, check=True)

if __name__ == '__main__':
    main()
