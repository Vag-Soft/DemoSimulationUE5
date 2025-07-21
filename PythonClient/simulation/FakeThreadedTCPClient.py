import os
import threading
from typing import Optional

import cv2
import numpy as np

class FakeThreadedTCPClient:
    def __init__(self, cam_count: int = 1, mode = "train"):
        self.running = False

        self.mode = mode
        self.data_path = "../train_data_2people" if mode == "train" else "../test_data_2people"
        self.sample_count = len(os.listdir(f"{self.data_path}/images_cam0"))
        self.loaded_samples = 0
        self.last_id = 0
        self.cam_count = cam_count
        self.images = [[] for _ in range(cam_count)]
        self.skeletons = [[] for _ in range(cam_count)]

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
                self.images[cam].append(cv2.imread(f"{self.data_path}/images_cam{cam}/image_{sample}.png"))
                self.skeletons[cam].append(np.load(f"{self.data_path}/skeletons_cam{cam}/skeleton_{sample}.npy"))
            self.loaded_samples += 1


    def get_latest_data(self, id: int = 0):
        while self.loaded_samples <= self.last_id % self.sample_count:
            continue

        image = self.images[id][self.last_id % self.sample_count]
        skeleton = self.skeletons[id][self.last_id % self.sample_count]
        self.last_id += 1
        return image, skeleton
