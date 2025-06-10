# interactive_visualizer.py
# Interactive fractal image viewer using MPI for generation and Pygame for display.
# Users can click to zoom into specific areas of the fractal.
# The camera parameters (x, y, zoom) are updated based on the selected region,
# and the fractal image is regenerated using an external MPI-based program.
# This visualizer creates a server, used to receive the generated image
# Note that both, the server and the rendering program are executed locally
import pygame
import subprocess
import argparse
import socket
import threading
import io
import math
from decimal import Decimal, getcontext

# Set precision to about 256 bits (~77 decimal digits)
getcontext().prec = 77

FPS = 60
ZOOM_BOX_SIZE = 64
HOST = '0.0.0.0'  
PORT = 5001       


class Camera:
    def __init__(self):
        self.x = Decimal(0)
        self.y = Decimal(0)
        self.zoom = Decimal(1)
    
    def to_world(self, normalized_screen_pos):
        # Convert inputs to Decimal too (in case they aren't)
        nx = Decimal(normalized_screen_pos[0])
        ny = Decimal(normalized_screen_pos[1])
        return (
            Decimal(0.5) * nx / self.zoom + self.x,
            Decimal(0.5) * ny / self.zoom + self.y
        )

    def to_pixel(self, world_pos):
        wx = Decimal(world_pos[0])
        wy = Decimal(world_pos[1])
        return (
            self.zoom * (wx - self.x),
            self.zoom * (wy - self.y)
        )

    def __str__(self):
        # Format with full precision
        return f"Camera(x={str(self.x)}, y={str(self.y)}, zoom={str(self.zoom)})"

# w1: top-left world coordinate
# w2: bottom-right world coordinate
# returns: zoom for a perfectly framed camera
def compute_zoom(w1, w2) -> Decimal:
    world_width = abs(w2[0] - w1[0])
    if world_width == 0:
        return float('inf') 
    return Decimal(2) / world_width

def image_generated_callback(png_data):
    global image
    image_stream = io.BytesIO(png_data)
    image = pygame.image.load(image_stream, "image.png")

# Simple TCP server that accepts connections and expects 
# a PNG formatted buffer image. 
def run_server():

    def recv_exact(sock, num_bytes):
        """Receive exactly num_bytes from the socket."""
        data = b''
        while len(data) < num_bytes:
            packet = sock.recv(num_bytes - len(data))
            if not packet:
                raise ConnectionError("Client disconnected")
            data += packet
        return data


    global done
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((HOST, PORT))
    server_socket.listen()
    server_socket.settimeout(1.0)  # Set timeout for accept()

    print(f"Server is listening on port {PORT}")
    while not done:
        try:
            client_socket, _ = server_socket.accept()
        except socket.timeout:
            continue  # Just try again in the next loop iteration

        # Handle the client
        try:
            # Step 1: Receive UUID length (4 bytes depending on sender)
            uuid_len_bytes = recv_exact(client_socket, 4)
            uuid_len = int.from_bytes(uuid_len_bytes, byteorder='big')

            # Step 2: Receive UUID string
            uuid_bytes = recv_exact(client_socket, uuid_len)
            _uuid = uuid_bytes.decode('utf-8')

            # Step 3: Receive buffer size (4 bytes for uint32)
            buf_size_bytes = recv_exact(client_socket, 4)
            buf_size = int.from_bytes(buf_size_bytes, byteorder='big')

            # Step 4: Receive the buffer (image or binary data)
            data = recv_exact(client_socket, buf_size)
            image_generated_callback(data)
        finally:
            client_socket.close()

    server_socket.close()

# Returns min, center and max points in screen coordinates
def get_screen_points():
    pos = pygame.mouse.get_pos()

    min_p = pos[0] - ZOOM_BOX_SIZE, pos[1] - ZOOM_BOX_SIZE
    max_p = pos[0] + ZOOM_BOX_SIZE, pos[1] + ZOOM_BOX_SIZE
    return (
        min_p,
        pos,
        max_p
    )

# Transforms pixel [0, size] to [-1.0, 1.0]
def pixel_to_ndc(pixel):
    return (
        2.0 * pixel[0] / screen.get_width() - 1.0,
        1.0 - 2.0 * pixel[1] / screen.get_height()  # Flip Y
    )



def generate_image(
        render_scale, 
        np, 
        renderer_args, 
        executable_path):

    # Uses logarithmic scaling for the iterations
    base_iterations = Decimal(512)
    iterations_scale = Decimal(64)
    iterations = int(base_iterations + camera.zoom.ln() / Decimal(math.log(2)) * iterations_scale)
    command = [
        'mpirun', '-np', str(np), executable_path,
        '--output_network',
        '--zoom', str(camera.zoom),
        '-cx', str(camera.x),
        '-cy', str(camera.y),
        '--width', str(int(screen.get_width() * render_scale)),
        '--height', str(int(screen.get_height() * render_scale)),
        '--iterations', str(iterations)
    ] + renderer_args

    print("Running:", ' '.join(command))
    subprocess.run(command, check=True, capture_output=False)

def render_selection_rect():
    min_p, center_p, max_p = get_screen_points()
    width = max_p[0] - min_p[0]
    height = max_p[1] - min_p[1]
    pygame.draw.rect(screen, [255, 255, 255], [*min_p, width, height], True)
    pygame.draw.circle(screen, [255, 0, 0], center_p, 8)

# Renders the given image over and transforms it in order to make it fit
def render_image_fit(image):
    screen.fill((0, 0, 0))
    img_aspect = image.get_width() / image.get_height()
    new_width = screen.get_width()
    new_height = int(new_width / img_aspect)
    scaled = pygame.transform.scale(image, (new_width, new_height))
    screen.blit(scaled, (0, 0))

# Updates the camera from the given points
def update_camera_from_selection(min_p, center_p, max_p):
    zoom = compute_zoom(
        camera.to_world(pixel_to_ndc(min_p)),
        camera.to_world(pixel_to_ndc(max_p))
    )
    center_world = camera.to_world(pixel_to_ndc(center_p))
    camera.x, camera.y = center_world
    camera.zoom = zoom

def render_camera_info():
    zoom_level = int(camera.zoom.ln() / Decimal(math.log(2)))
    text = f"Zoom level = {zoom_level:.4f}"
    surface = font.render(text, True, (255, 255, 255))
    screen.blit(surface, (10, 10))


parser = argparse.ArgumentParser()
parser.add_argument('--np', type=int, default=4, help="Number of processors")
parser.add_argument("--executable_path", type=str, default="../../build/fractal_mpi", help="Filepath of the rendering program")
parser.add_argument('--start_cx', type=str, default="-.5")
parser.add_argument('--start_cy', type=str, default="0.0")
parser.add_argument('--start_zoom', type=str, default="0.25")
parser.add_argument('--render_scale', type=float, default=1.0)
parser.add_argument('--zoom_level', type=int, default=-1.0)
args, renderer_args = parser.parse_known_args()

pygame.init()
font = pygame.font.SysFont(None, 24)

is_holding = False
camera = Camera()
camera.x = Decimal(args.start_cx)
camera.y = Decimal(args.start_cy)

if args.zoom_level != -1:
    camera.zoom = Decimal(2.0) ** Decimal(args.zoom_level)
else:
    camera.zoom = Decimal(args.start_zoom)

clock = pygame.time.Clock()
screen = pygame.display.set_mode((720, 720))

update_camera_from_selection(
    (0, 0),
    (screen.get_width() / 2, screen.get_height() / 2),
    (screen.get_width(), screen.get_height())
)
done = False

# Creates and starts server
server_thread = threading.Thread(target=run_server)
server_thread.start()

image: pygame.Surface = None
generate_image(args.render_scale, args.np, renderer_args, args.executable_path)

while not done:

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            done = True
        if event.type == pygame.VIDEORESIZE:
            screen = pygame.display.set_mode(event.size, pygame.RESIZABLE)

        if event.type == pygame.MOUSEBUTTONDOWN:
            is_holding = True

        if event.type == pygame.MOUSEBUTTONUP:
            is_holding = False

            update_camera_from_selection(*get_screen_points())
            generate_image(
                args.render_scale,
                args.np,
                renderer_args,
                args.executable_path)


    # Client rendering -------------------------------------------------------------------------
    if image is not None:
        render_image_fit(image)

    # Renders hint rect
    if is_holding:
        render_selection_rect()

    render_camera_info()

    pygame.display.flip()
    clock.tick(FPS)

pygame.quit()
server_thread.join()