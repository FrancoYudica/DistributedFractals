"""
Simple test/sample image server that accepts connections of the cluster to store the image
This is super important if the mpi image generation cluster is ran on a distributed system
"""
import socket

HOST = '0.0.0.0'  
PORT = 5001       

# Creates TCP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((HOST, PORT))
server_socket.listen()

while True:
    client_socket, addr = server_socket.accept()

    # Creates empty byte array
    data = b''

    # First, get the buffer size
    expected_size = int.from_bytes(client_socket.recv(4), byteorder='big')

    # Then, receives all the image bytes
    while len(data) < expected_size:
        packet = client_socket.recv(4096)
        if not packet:
            break
        data += packet

    print(f"Image received: {len(data)} bytes")

    # Stores
    with open("image.png", "wb") as f:
        f.write(data)

    client_socket.close()