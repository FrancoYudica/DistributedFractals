import socket

HOST = '0.0.0.0'
PORT = 5001

def recv_exact(sock, num_bytes):
    """Receive exactly num_bytes from the socket."""
    data = b''
    while len(data) < num_bytes:
        packet = sock.recv(num_bytes - len(data))
        if not packet:
            raise ConnectionError("Client disconnected")
        data += packet
    return data

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((HOST, PORT))
server_socket.listen()

print(f"Listening on {HOST}:{PORT}")

while True:
    client_socket, addr = server_socket.accept()
    print(f"Connection from {addr}")

    try:
        # Step 1: Receive UUID length (4 bytes depending on sender)
        uuid_len_bytes = recv_exact(client_socket, 4)
        uuid_len = int.from_bytes(uuid_len_bytes, byteorder='big')

        # Step 2: Receive UUID string
        uuid_bytes = recv_exact(client_socket, uuid_len)
        uuid = uuid_bytes.decode('utf-8')
        print(f"UUID: {uuid}")

        # Step 3: Receive buffer size (4 bytes for uint32)
        buf_size_bytes = recv_exact(client_socket, 4)
        buf_size = int.from_bytes(buf_size_bytes, byteorder='big')
        print(buf_size)

        # Step 4: Receive the buffer (image or binary data)
        data = recv_exact(client_socket, buf_size)

        print(f"Received {len(data)} bytes of data")

        # Save it using UUID in filename
        filename = f"{uuid}.png"
        with open(filename, "wb") as f:
            f.write(data)
        print(f"Saved image as {filename}")

    except Exception as e:
        print(f"Error: {e}")

    finally:
        client_socket.close()
