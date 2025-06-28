from typing import Optional

import gymnasium as gym
import numpy as np
from gymnasium import spaces
import PoseEstimation as pe
from simulation.FakeThreadedTCPClient import FakeThreadedTCPClient
from simulation.ThreadedTCPClient import ThreadedTCPClient


'''
Features:
- Uses MediaPipe for 3D pose estimation from camera images
- Connects to simulation via TCP to receive camera feeds and ground truth 3D skeleton data
- Processes and compares estimated joint positions against ground truth in 3D space
- Supports both real and simulated data sources through ThreadedTCPClient/FakeThreadedTCPClient

Observation Space:
- Shape: (history_count, cam_count, 14), dtype=float32
- Each camera provides 13 joint confidence scores (0-1) from MediaPipe and 1 camera selection flag (0/1)
- Maintains temporal history of states, where only the selected camera contains information

Action Space:
- Discrete(cam_count): Agent selects which camera to process at each step

Reward Function:
- Based on accuracy of pose estimation: reward = 1 - MSE(predicted_skeleton, ground_truth_skeleton)
- Returns -1 if pose estimation fails for the selected view

'''
class Env(gym.Env):
    def __init__(self, width, height, cam_count, history_count=10, mode="train"):
        super().__init__()

        self.mode = mode
        self.width = width
        self.height = height
        self.cam_count = cam_count
        self.history_count = history_count

        self.current_camera = None
        self.current_obs = np.zeros((history_count, cam_count, 14), dtype=np.float32)
        self.current_skeletons = np.zeros((cam_count, 13, 3), dtype=np.float32)

        self.action_space = spaces.Discrete(cam_count)
        self.observation_space = spaces.Box(low=0, high=1, shape=(history_count, cam_count, 14), dtype=np.float32)

        # self.client = ThreadedTCPClient(cam_count=cam_count)
        self.client = FakeThreadedTCPClient(cam_count=cam_count, mode=self.mode)
        self.client.start()

        self.current_predicted_skeletons = np.zeros((13, 3), dtype=np.float32)

    def reset(self, seed: Optional[int] = None, options: Optional[dict] = None):
        super().reset(seed=seed)

        self.current_camera = self.action_space.sample()

        for j in range(self.history_count):
            for i in range(self.cam_count):
                while True:
                    image, skeletons = self.client.get_latest_data(i)
                    if image is not None and skeletons is not None:
                        # self.current_obs[j][i] = np.random.uniform(0, 1, size=(14,))
                        # self.current_obs[j][i][13] = i == self.current_camera
                        self.current_obs[j][i] = np.zeros(14, dtype=np.float32)
                        self.current_skeletons[i] = skeletons
                        break
                    # else:
                    # print(f"No data received from client for camera {i}")

        info = {}
        return self.current_obs, info

    def step(self, action):
        for i in range(self.history_count - 1):
            self.current_obs[i] = self.current_obs[i + 1].copy()

        self.current_camera = action

        while True:
            image, truth_skeletons = self.client.get_latest_data(self.current_camera)
            if image is not None and truth_skeletons is not None:
                predicted_skeletons, visibilities = self.get_processed_pose(image)

                self.current_predicted_skeletons = predicted_skeletons

                # Remove the last confidence scores and add the new ones
                self.current_obs[self.history_count-1] = np.zeros((self.cam_count, 14), dtype=np.float32)
                self.current_obs[self.history_count-1][self.current_camera] = np.append(self.get_confidence_scores(visibilities), 1)
                self.current_skeletons[self.current_camera] = truth_skeletons

                for i in range(self.cam_count):
                    self.current_obs[self.history_count-1][i][13] = i == self.current_camera

                reward = self.calculate_reward(predicted_skeletons)

                terminated = False
                truncated = False
                # if random.random() < 0.001:
                #     terminated = True
                info = {}
                return self.current_obs, reward, terminated, truncated, info
            # else:
            #     print(f"No data received from client for camera {i}")

    def get_processed_pose(self, image):
        pose_results = pe.pose_estimation(image)
        if pose_results.pose_world_landmarks is None:
            return None, None

        return pe.process_results(pose_results)

    def calculate_reward(self, predicted_skeletons):
        if predicted_skeletons is None:
            # print("Pose estimation failed, returning -1 reward")
            return -1

        if predicted_skeletons.shape != self.current_skeletons[self.current_camera].shape:
            raise ValueError("Skeletons must have same shape")

        mse = np.mean(np.square(predicted_skeletons - self.current_skeletons[self.current_camera]))
        return 1-mse

    # Useful for multiple skeletons
    def get_confidence_scores(self, visibilities):
        if visibilities is None:
            return np.zeros(13, dtype=np.float32)

        return visibilities
