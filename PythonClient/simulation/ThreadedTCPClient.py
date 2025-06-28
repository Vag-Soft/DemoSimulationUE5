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
        data = self.client_socket.recv(1024)
        print("Received from Unreal:", data.decode('utf-8'))


        while self.running:
            self.client_socket.send("images".encode('utf-8'))
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



def receive_image(socket):
    # Receiving the id of the image
    id_bytes = socket.recv(4)
    id = int.from_bytes(id_bytes, "little")

    # Receiving the size of the image data
    image_data_size_bytes = socket.recv(4)
    image_data_size = int.from_bytes(image_data_size_bytes, "little")

    # Receiving the image data
    image_data = socket.recv(image_data_size)

    image_np = np.frombuffer(image_data, dtype=np.uint8)
    image = cv2.imdecode(image_np, cv2.IMREAD_UNCHANGED)

    return id, image

def receive_skeletons(socket):
    # Receive the camera position (24 bytes for double x, y, z)
    camera_position_data = socket.recv(24)
    cam_x, cam_y, cam_z = struct.unpack("d d d", camera_position_data)
    # print(f"Camera position: ({cam_x}, {cam_y}, {cam_z})")

    # Receive the number of skeletons (4 bytes)
    num_skeletons_data = socket.recv(4)
    num_skeletons = int.from_bytes(num_skeletons_data, "little")
    # print(f"Receiving {num_skeletons} skeletons...")

    # Receive the number of bones (4 bytes)
    num_bones_data = socket.recv(4)
    num_bones = int.from_bytes(num_bones_data, "little")
    # print(f"Receiving {num_bones} bone transforms...")

    # Read all bone transforms
    # Shape: (num_skeletons, num_bones, 3)
    # Each entry stores the position (px, py, pz) of a bone
    character_transforms = np.zeros((num_skeletons, num_bones, 3))
    for _ in range(num_skeletons):
        # Receive the id of the character
        id_data = socket.recv(4)
        skeleton_idx = int.from_bytes(id_data, "little")

        # Shape: (num_bones, 3), where each row is [px, py, pz]
        bone_positions = np.zeros((num_bones, 3))
        for _ in range(num_bones):
            # Read 32 bytes (bone index + position + rotation)
            data = socket.recv(32)

            # Unpack: int32 + 3 floats (FVector) + 4 floats (FQuat)
            bone_index, px, py, pz, qx, qy, qz, qw = struct.unpack("i f f f f f f f", data)

            character_transforms[skeleton_idx, bone_index] = [px - cam_x, pz - cam_z, py - cam_y]

    return character_transforms

def process_bones(character_transforms):
    # Define the remapping of bone IDs
    new_idxs = {9: 0,
                11: 1,
                38: 2,
                12: 3,
                39: 4,
                15: 5,
                42: 6,
                72: 7,
                64: 8,
                73: 9,
                65: 10,
                74: 11,
                66: 12}

    # Initialize a NumPy array for the processed skeletons
    num_skeletons, num_bones, _ = character_transforms.shape
    num_remapped_bones = len(new_idxs)
    processed_skeletons = np.zeros((num_skeletons, num_remapped_bones, 3))  # Shape: (num_skeletons, num_remapped_bones, 3)

    for skeleton_idx in range(num_skeletons):
        skeleton = character_transforms[skeleton_idx]

        # Extract the root bone position
        root_bone = skeleton[9]  # Position of bone 9 (x, y, z)

        # Process and remap the bones
        for old_bone_id, new_bone_id in new_idxs.items():
            processed_skeletons[skeleton_idx, new_bone_id] = skeleton[old_bone_id] - root_bone


        # Compute the maximum distance from the origin for normalization
        max_distance = np.max(np.linalg.norm(processed_skeletons[skeleton_idx], axis=1))

        # Normalize the skeleton
        processed_skeletons[skeleton_idx] = processed_skeletons[skeleton_idx] / max_distance

    return processed_skeletons
