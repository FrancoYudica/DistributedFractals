"""
Video renderer takes a target position and zooms. Zoom is exponentially
interpolated.

This program outputs a folder with all the images, then another program such 
as ffmpeg can be used to build the video with a command such as:

    ffmpeg -framerate 60 -i ./frame_%d.png   -c:v libx264   -preset veryslow   -crf 18   -pix_fmt yuv420p   -vf "format=yuv420p"   ../video.mp4 -y

"""
import argparse
import subprocess
from subprocess import CalledProcessError
import math
import os
import time
from datetime import datetime
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
    

def render_frame(
    frame_index, 
    command,
    retry_limit = 5):
    
    for i in range(retry_limit):
        try:
            # Generates the image
            subprocess.run(command, check=True, capture_output=True)
            return
                    
        except CalledProcessError as e:
            # Print to console
            print(f"[ERROR] Frame {frame_index} failed on iteration {i}")
            print(f"Command: {' '.join(command)}")
            # stderr may be bytes; decode for readability
            stderr = e.stderr.decode(errors='ignore') if e.stderr else "(no stderr)"
            print(f"Error output:\n{stderr}")

    raise Exception(f"Unable to render frame after {retry_limit} attempts")

def main():
    parser = argparse.ArgumentParser(description="Batch render fractals at different zoom levels")
    parser.add_argument('--program', required=True, help='Path to the MPI program (e.g. ./fractal_mpi)')
    parser.add_argument('--z0', type=float, default=1.0, help='Starting zoom level')
    parser.add_argument('--z1', type=float, default=10.0, help='Ending zoom level')
    parser.add_argument('--frames', type=int, default=5, help='Number of zoom steps')
    parser.add_argument('--output_folder', type=str, default='.', help='Base name for output images')
    parser.add_argument('--hostfile', type=str, default='', help='MPI hostfile filepath')
    parser.add_argument('--np', type=int, default=8, help='MPI processes count')
    parser.add_argument('--net_interface', type=str, default='')

    args, cpp_args = parser.parse_known_args()

    # Creates a directory to store the images
    timestamp_str = datetime.now().strftime("%Y-%m-%d_%H:%M:%S")
    root_dir = os.path.join(args.output_folder, f"video_{timestamp_str}")
    os.mkdir(root_dir)

    frames_dir = os.path.join(root_dir, "frames")
    os.mkdir(frames_dir)

    # Open log file once, write header
    log_path = os.path.join(root_dir, f"render.log")
    with open(log_path, "w") as log_file:
        log_file.write("frame,zoom_level,time_seconds,command\n")

    # Saves as many images as zoom levels
    for frame in range(args.frames):

        zoom = get_zoom_exponential(
            Decimal(frame / (args.frames - 1)),
            Decimal(args.z0),
            Decimal(args.z1)
        )

        iterations = get_iterations(
            zoom,
            Decimal(512),
            Decimal(64))

        output_name = os.path.join(frames_dir, f"frame_{frame}.png")

        command = ['mpirun']

        if args.hostfile != '':
            command.extend(['-hostfile', args.hostfile])

        if args.net_interface != '':
            command.extend(['--mca', 'btl_tcp_if_include', args.net_interface])

        command.extend([
            '-np', str(args.np),
            args.program,
            '--zoom', str(zoom),
            '--iterations', str(iterations),
            '-od', output_name
        ])

        command.extend(cpp_args)

        t0 = time.perf_counter()
        render_frame(frame, command)
        elapsed = time.perf_counter() - t0

        progress = float(frame) / (args.frames - 1) * 100.0
        zoom_level = float(decimal_log2(zoom))
        print(f"PROGRESS: {progress:.4f}% - Zoom level {zoom_level:.4f} - Frame time {elapsed:.4f} sec")

        # Append a line to the CSV log
        with open(log_path, "a") as log_file:
            # Escape command list into a single string for CSV
            command_str = ' '.join(command).replace(',', ';')
            line = f"{frame},{zoom_level:.6f},{elapsed:.6f},{command_str}\n"
            log_file.write(line)
    
if __name__ == '__main__':

    rendering_t0 = time.perf_counter()
    main()
    rendering_time = time.perf_counter() - rendering_t0
    print(f"Rendering took {rendering_time} seconds")
