"""
Video renderer takes a target position and zooms. It interpolates camera position and zoom
It also stores a session file, allowing the program to continue rendering in different executions

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
import json

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
    return int(base_iter + decimal_log2(1 + zoom) * scale)
    

def run_command_and_retry(
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


# Linear interpolation
def lerp(a: Decimal, b: Decimal, t: Decimal):
    return a + (b - a) * t

def ease_in_out_power(t, p=2.0):
    if t < 0.5:
        return 0.5 * pow(2 * t, p)
    else:
        return 1 - 0.5 * pow(2 * (1 - t), p)

class Camera:
    def __init__(self):
        self.x = Decimal(0)
        self.y = Decimal(0)
        self.zoom = Decimal(1)

def interpolate_camera(
    z0: Decimal,
    z1: Decimal,
    cx0: Decimal,
    cy0: Decimal,
    cx1: Decimal,
    cy1: Decimal,
    t: Decimal
):
    
    camera = Camera()
    # Transforms linear t
    camera.zoom = get_zoom_exponential(t, z0, z1)
    # Interpolates camera positions (cx0, cy0), (cx1, cy1)
    scale = camera.zoom / z0
    max_scale = z1 / z0
    # Normalized position interpolation factor
    s = (1 - 1 / scale) / (1 - 1 / max_scale)
    camera.x = lerp(Decimal(cx0), Decimal(cx1), s)
    camera.y = lerp(Decimal(cy0), Decimal(cy1), s)
    return camera

class RendererSession:
    """
    Stores all constant data required for rendering a video.

    Keeping this data separate allows the renderer to resume or continue 
    rendering the same video across multiple executions of the program.
    """
    def __init__(self):

        self.rendered_frames: int
        self.frames: int
        
        # Camera settings
        self.z0: Decimal
        self.z1: Decimal
        self.cx0: Decimal
        self.cy0: Decimal
        self.cx1: Decimal
        self.cy1: Decimal
        self.iterations_base: int
        self.iterations_scale: int
        self.frames_smooth: int
        self.program_arguments: list = []

    def to_dict(self) -> dict:
        session = {
            "z0": str(self.z0),
            "z1": str(self.z1),
            "cx0": str(self.cx0),
            "cy0": str(self.cy0),
            "cx1": str(self.cx1),
            "cy1": str(self.cy1),
            "iterations_base": self.iterations_base,
            "iterations_scale": self.iterations_scale,
            "frames_smooth": self.frames_smooth,
            "frames": self.frames,
            "program_arguments": self.program_arguments,
            "rendered_frames": self.rendered_frames
        }
        return session
    
    def save_disk(self, filepath: str):
        str_session = json.dumps(self.to_dict(), indent=4)
        with open(filepath, "w") as file:
            file.write(str_session)

    @staticmethod
    def load_from_disk(filepath: str):
        str_session = ''
        with open(filepath, "r") as file:
            str_session = "".join(file.readlines())
        
        session_data = json.loads(str_session)
        session = RendererSession()

        session.rendered_frames = session_data['rendered_frames']
        session.frames = session_data['frames']
        session.z0 = Decimal(session_data['z0'])
        session.z1 = Decimal(session_data['z1'])
        session.cx0 = Decimal(session_data['cx0'])
        session.cy0 = Decimal(session_data['cy0'])
        session.cx1 = Decimal(session_data['cx1'])
        session.cy1 = Decimal(session_data['cy1'])
        session.iterations_base = session_data['iterations_base']
        session.iterations_scale = session_data['iterations_scale']
        session.frames_smooth = session_data['frames_smooth']
        session.program_arguments = session_data['program_arguments']
        return session

class RendererSettings:
    """
    Stores all settings, including the session
    """
    def __init__(self):
        self.session = RendererSession()

        self.program_path: str

        # MPI specific attributes
        self.host_file: str
        self.np: int
        self.net_interface: str

def main(settings: RendererSettings, root_dir: str):

    session: RendererSession = settings.session

    frames_dir = os.path.join(root_dir, "frames")

    if not os.path.exists(frames_dir):
        os.mkdir(frames_dir)

    # Open log file once, write header
    log_path = os.path.join(root_dir, f"render.log")
    with open(log_path, "w") as log_file:
        log_file.write("frame,zoom_level,time_seconds,command\n")

    # Saves as many images as zoom levels
    for frame in range(session.rendered_frames, session.frames):
        
        t_linear = frame / (session.frames - 1)

        # Transforms linear t
        ratio = float(session.frames_smooth) / session.frames
        t = ease_in_out_power(t_linear, 1 + ratio)
        camera = interpolate_camera(
            session.z0,
            session.z1,
            session.cx0,
            session.cy0,
            session.cx1,
            session.cy1,
            Decimal(t)   
        )
        
        # Computes iterations based on zoom
        iterations = get_iterations(
            camera.zoom,
            Decimal(session.iterations_base),
            Decimal(session.iterations_scale))

        output_name = os.path.join(frames_dir, f"frame_{frame}.png")

        command = ['mpirun']

        if settings.host_file != '':
            command.extend(['-hostfile', settings.host_file])

        if settings.net_interface != '':
            command.extend(['--mca', 'btl_tcp_if_include', settings.net_interface])

        command.extend([
            '-np', str(settings.np),
            settings.program_path,
            '--zoom', str(camera.zoom),
            '-cx', str(camera.x),
            '-cy', str(camera.y),
            '--iterations', str(iterations),
            '-od', output_name
        ])

        command.extend(session.program_arguments)

        t0 = time.perf_counter()
        run_command_and_retry(frame, command)
        elapsed = time.perf_counter() - t0

        progress = float(frame) / (session.frames - 1) * 100.0
        zoom_level = float(decimal_log2(camera.zoom))
        print(f"PROGRESS: {progress:.4f}% - Zoom level {zoom_level:.4f} - Frame time {elapsed:.4f} sec")

        # Append a line to the CSV log
        with open(log_path, "a") as log_file:
            # Escape command list into a single string for CSV
            command_str = ' '.join(command).replace(',', ';')
            line = f"{frame},{zoom_level:.6f},{elapsed:.6f},{command_str}\n"
            log_file.write(line)

        # Updates session rendered frames
        session.rendered_frames = frame + 1
        session.save_disk(os.path.join(root_dir, "session.json"))

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="Batch render fractals at different zoom levels")
    parser.add_argument('--program', required=True, help='Path to the MPI program (e.g. ./fractal_mpi)')
    parser.add_argument('--hostfile', type=str, default='', help='MPI hostfile filepath')
    parser.add_argument('--np', type=int, default=8, help='MPI processes count')
    parser.add_argument('--net_interface', type=str, default='')

    parser.add_argument('--frames', type=int, default=5, help='Number of frames')
    parser.add_argument('--rendered_frames', type=int, default=0, help='Number already rendered frames')
    parser.add_argument('--z0', type=str, default=1.0, help='Starting zoom level')
    parser.add_argument('--z1', type=str, default=10.0, help='Ending zoom level')
    parser.add_argument('--cx0', type=str, default=0.0, help='Starting camera X')
    parser.add_argument('--cy0', type=str, default=0.0, help='Starting camera Y')
    parser.add_argument('--cx1', type=str, default=0.0, help='Ending camera X')
    parser.add_argument('--cy1', type=str, default=0.0, help='Ending camera Y')
    parser.add_argument('--iterations_base', type=int, default=512)
    parser.add_argument('--iterations_scale', type=int, default=64)
    parser.add_argument('--frames_smooth', type=int, default=1, help='Amount of frames where camera speed smooths in and out')

    parser.add_argument('--session', type=str, default='', help='Path to the session.json file. Rendering continues from session')

    args, cpp_args = parser.parse_known_args()


    root_dir: str = ''
    settings = RendererSettings()

    if args.session != '':
        settings.session = RendererSession.load_from_disk(args.session)
        root_dir = os.path.dirname(args.session)
        print(f"Loaded session located at: {root_dir}")

    else:
        settings.session.frames = args.frames
        settings.session.rendered_frames = args.rendered_frames
        settings.session.z0 = Decimal(args.z0)
        settings.session.z1 = Decimal(args.z1)
        settings.session.cx0 = Decimal(args.cx0)
        settings.session.cy0 = Decimal(args.cy0)
        settings.session.cx1 = Decimal(args.cx1)
        settings.session.cy1 = Decimal(args.cy1)
        settings.session.iterations_base = args.iterations_base
        settings.session.iterations_scale = args.iterations_scale
        settings.session.frames_smooth = args.frames_smooth
        settings.session.program_arguments = cpp_args

        # Creates a directory to store the images
        timestamp_str = datetime.now().strftime("%Y-%m-%d_%H:%M:%S")
        root_dir = os.path.join(args.output_folder, f"video_{timestamp_str}")
        os.mkdir(root_dir)
        print(f"Session created for the first time at {root_dir}")

    # Loads settings from args
    settings.program_path = args.program
    settings.np = args.np
    settings.host_file = args.hostfile
    settings.net_interface = args.net_interface

    rendering_t0 = time.perf_counter()
    main(settings, root_dir)
    rendering_time = time.perf_counter() - rendering_t0
    print(f"Rendering took {rendering_time} seconds")