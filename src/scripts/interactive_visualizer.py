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

FPS = 60
ZOOM_BOX_SIZE = 64
HOST = '0.0.0.0'  
PORT = 5001       


class Camera:
    def __init__(self):
        self.x = 0
        self.y = 0
        self.zoom = 1.0
    
    def to_world(self, normalized_screen_pos):
        return (
            normalized_screen_pos[0] / self.zoom + self.x,
            normalized_screen_pos[1] / self.zoom + self.y
        )

    def to_pixel(self, world_pos):
        return (
            self.zoom * (world_pos[0] - self.x),
            self.zoom * (world_pos[1] - self.y)
        )

    def __str__(self):
        return f"Camera({self.x}, {self.y}, {self.zoom})"

# w1: top-left world coordinate
# w2: bottom-right world coordinate
# returns: zoom for a perfectly framed camera
def compute_zoom(w1, w2) -> float:
    world_width = abs(w2[0] - w1[0])
    if world_width == 0:
        return float('inf') 
    return 2.0 / world_width

def image_generated_callback(png_data):
    global image
    image_stream = io.BytesIO(png_data)
    image = pygame.image.load(image_stream, "image.png")
    print(camera)

# Simple TCP server that accepts connections and expects 
# a PNG formatted buffer image. 
def run_server():
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
            data = b''
            expected_size = int.from_bytes(client_socket.recv(4), byteorder='big')

            while len(data) < expected_size:
                packet = client_socket.recv(4096)
                if not packet:
                    break
                data += packet

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
        2.0 * pixel[1] / screen.get_height() - 1.0
    )


def generate_image(renderer_args, np, executable_path):
    command = [
        'mpirun', '-np', str(np), executable_path,
        '--output_network',
        '--zoom', str(camera.zoom),
        '-cx', str(camera.x),
        '-cy', str(camera.y),
        '--width', str(screen.get_width()),
        '--height', str(screen.get_height())
    ] + renderer_args

    print("Running:", ' '.join(command))
    subprocess.run(command, check=True)

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
    text = f"x={camera.x:.4f}  y={camera.y:.4f}  zoom={camera.zoom:.4f}"
    surface = font.render(text, True, (255, 255, 255))
    screen.blit(surface, (10, 10))


parser = argparse.ArgumentParser()
parser.add_argument('--np', type=int, default=4, help="Number of processors")
parser.add_argument("--executable_path", type=str, default="../../build/fractal_mpi", help="Filepath of the rendering program")
args, renderer_args = parser.parse_known_args()

pygame.init()
font = pygame.font.SysFont(None, 24)

is_holding = False
camera = Camera()
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
generate_image(renderer_args, args.np, args.executable_path)

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
            generate_image(renderer_args, args.np, args.executable_path)

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