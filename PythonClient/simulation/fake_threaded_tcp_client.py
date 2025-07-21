import os
import threading
import time
from typing import Optional

import cv2
import numpy as np

from PythonClient.utils import transform_utils


class FakeThreadedTCPClient:
    def __init__(self, cam_count: int = 1, mode = "train"):
        self.running = False

        self.mode = mode
        self.data_path = "E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/simulation/yolo/train_data_2people" if mode == "train" else "E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/simulation/yolo/test_data_2people"
        self.sample_count = len(os.listdir(f"{self.data_path}/images_cam0"))
        self.loaded_samples = 0
        self.last_ids = [0] * cam_count
        self.cam_count = cam_count
        self.images = np.zeros((cam_count, self.sample_count, 500, 500, 3), dtype=np.uint8)
        self.skeletons = np.zeros((cam_count, self.sample_count, 2, 13, 3), dtype=np.float32)
        self.camera_locations = np.array([[793.58, 1185.2, 192.33],
                                          [1734.2, 1181.8, 130.28],
                                          [1273.2, 735.02, 138.41]])
        self.camera_rotations = np.array([[0.0, 0.0, 0.0],
                                          [0.0, 0.0, 180],
                                          [0.0, 0.0, 90]])

        self.thread: Optional[threading.Thread] = None

    def start(self):
        self.running = True

        self.thread = threading.Thread(target=self._load_data)
        self.thread.daemon = True
        self.thread.start()

    def stop(self):
        self.running = False
        if self.thread:
            self.thread.join()

    def _load_data(self):
        for sample in range(self.sample_count):
            for cam in range(self.cam_count):
                self.images[cam][sample] = cv2.imread(f"{self.data_path}/images_cam{cam}/image_{sample}.png")
                self.skeletons[cam][sample] = np.load(f"{self.data_path}/skeletons_cam{cam}/skeleton_{sample}.npy")
            self.loaded_samples += 1


    def get_latest_data(self, id: int = 0):
        while self.loaded_samples <= self.last_ids[id] % self.sample_count:
            print(f"Waiting for data to load... {self.loaded_samples}/{self.sample_count}")
            time.sleep(0.1)

        image = self.images[id][self.last_ids[id] % self.sample_count]
        skeletons = self.skeletons[id][self.last_ids[id] % self.sample_count]

        self.last_ids[id] += 1
        return image, skeletons
