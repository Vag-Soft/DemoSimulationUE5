import struct
import threading
from collections import deque
import socket
import cv2
import numpy as np
from typing import Optional


class ThreadedTCPClient:
    def __init__(self, host: str = "127.0.0.1", port: int = 7777, cam_count: int = 1):
        self.host = host
        self.port = port
        self.client_socket: Optional[socket.socket] = None

        self.running = False

        self.camera_locations = np.zeros((cam_count, 3))
        self.camera_rotations = np.zeros((cam_count, 3))
        self.cam_count = cam_count
        self.image_deqs = [deque(maxlen=3) for _ in range(cam_count)]
        self.skeleton_deqs = [deque(maxlen=3) for _ in range(cam_count)]

        self.thread: Optional[threading.Thread] = None

    def start(self):
        self.running = True
        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.client_socket.connect((self.host, self.port))

        self.thread = threading.Thread(target=self._receive_loop)
        self.thread.daemon = True
        self.thread.start()

    def stop(self):
        self.running = False
        if self.client_socket:
            self.client_socket.close()
        if self.thread:
            self.thread.join()

    def _receive_loop(self):
        for i in range(self.cam_count):
            cam_id = recv_all(self.client_socket, 4)
            cam_id = int.from_bytes(cam_id, "little")

            camera_data = recv_all(self.client_socket, 48)
            # UE5 has 2 different rotation systems, here roll-x, pitch-y, yaw-z
            cam_x, cam_y, cam_z, pitch, yaw, roll = struct.unpack("d d d d d d", camera_data)
            cam_x = float(f"{cam_x:.5f}")
            cam_y = float(f"{cam_y:.5f}")
            cam_z = float(f"{cam_z:.5f}")
            pitch = float(f"{pitch:.5f}")
            yaw = float(f"{yaw:.5f}")
            roll = float(f"{roll:.5f}")

            self.camera_locations[cam_id] = np.array([cam_x, cam_y, cam_z])
            self.camera_rotations[cam_id] = np.array([roll, pitch, yaw])



        self.client_socket.send("images".encode('utf-8'))
        while self.running:
            try:
                id, image = receive_image(self.client_socket)
                skeletons = receive_skeletons(self.client_socket)
                skeletons = process_bones(skeletons)

                self.image_deqs[id].append(image)
                self.skeleton_deqs[id].append(skeletons)
            except Exception as e:
                print(f"Error in receive loop: {e}")
                break

    def get_latest_data(self, id: int = 0):
        if len(self.image_deqs[id]) == 0 or len(self.skeleton_deqs[id]) == 0:
            # print(f"No data available for camera {id}. Queues are empty.")
            return None, None
        return self.image_deqs[id].popleft(), self.skeleton_deqs[id].popleft()



# TODO: Normalize images?
def receive_image(socket):
    # Receiving the id of the image
    id_bytes = recv_all(socket, 4)
    id = int.from_bytes(id_bytes, "little")

    # Receiving the size of the image data
    image_data_size_bytes = recv_all(socket, 4)
    image_data_size = int.from_bytes(image_data_size_bytes, "little")

    # Receiving the image data
    image_data = recv_all(socket, image_data_size)

    image_np = np.frombuffer(image_data, dtype=np.uint8)
    image = cv2.imdecode(image_np, cv2.IMREAD_UNCHANGED)

    return id, image

def receive_skeletons(socket):
    # Receive the number of skeletons (4 bytes)
    num_skeletons_data = recv_all(socket, 4)
    num_skeletons = int.from_bytes(num_skeletons_data, "little")
    # print(f"Receiving {num_skeletons} skeletons...")

    # Receive the number of bones (4 bytes)
    num_bones_data = recv_all(socket, 4)
    num_bones = int.from_bytes(num_bones_data, "little")
    # print(f"Receiving {num_bones} bone transforms...")

    # Read all bone transforms
    # Shape: (num_skeletons, num_bones, 3)
    # Each entry stores the position (px, py, pz) of a bone
    character_transforms = np.zeros((num_skeletons, num_bones, 3))
    for _ in range(num_skeletons):
        # Receive the id of the character
        id_data = recv_all(socket, 4)
        skeleton_idx = int.from_bytes(id_data, "little")

        # Shape: (num_bones, 3), where each row is [px, py, pz]
        bone_positions = np.zeros((num_bones, 3))
        for _ in range(num_bones):
            # Read 32 bytes (bone index + position + rotation)
            data = recv_all(socket, 32)

            # Unpack: int32 + 3 floats (FVector) + 4 floats (FQuat)
            bone_index, px, py, pz, qx, qy, qz, qw = struct.unpack("i f f f f f f f", data)

            character_transforms[skeleton_idx, bone_index] = [px, py, pz]

    return character_transforms

def process_bones(character_transforms):
    # Define the remapping of bone IDs
    new_idxs = {9: 0,
                38: 1,
                11: 2,
                39: 3,
                12: 4,
                42: 5,
                15: 6,
                64: 7,
                72: 8,
                65: 9,
                73: 10,
                66: 11,
                74: 12}
    return character_transforms[:, list(new_idxs.keys())]


def recv_all(socket, size):
    data = bytearray()
    while len(data) < size:
        packet = socket.recv(size - len(data))
        if not packet:
            raise ConnectionError("Socket connection broken")
        data.extend(packet)
    return data