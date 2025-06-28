from typing import Optional

import gymnasium as gym
import open3d as o3d
import numpy as np
from gymnasium import spaces
import PoseEstimation as pe
from simulation.ThreadedTCPClient import ThreadedTCPClient

"""
This environment connects to a simulation via ThreadedTCPClient that provides camera images
and ground truth 3D skeletons. The agent learns to select the optimal camera view for
accurate pose estimation.

Observation Space:
    Box(0, 255, (cam_count, height, width, 3), dtype=uint8) - RGB images from all cameras

Action Space:
    Discrete(cam_count) - Selection of which camera to use for pose estimation

Reward Function:
    1 - MSE between MediaPipe-estimated skeleton and ground truth skeleton for the selected camera,
    or -1 if pose estimation fails

Key Components:
    - MediaPipe-based pose estimation (via PoseEstimation module)
    - 3D skeleton representation (13 joints with XYZ coordinates)
    - TCP connection to simulation providing synchronized camera feeds and ground truth data
    - Open3D visualization for skeleton comparison
"""
class Env(gym.Env):
    def __init__(self, width, height, cam_count):
        super().__init__()

        self.current_camera = None
        self.current_obs = np.zeros((cam_count, height, width, 3), dtype=np.uint8)
        self.current_skeletons = np.zeros((cam_count, 13, 3), dtype=np.float32)

        self.width = width
        self.height = height
        self.cam_count = cam_count

        self.action_space = spaces.Discrete(cam_count)
        self.observation_space = spaces.Box(low=0, high=255, shape=(cam_count, height, width, 3), dtype=np.uint8)

        self.client = ThreadedTCPClient(cam_count=cam_count)
        self.client.start()

        self.vis = o3d.visualization.Visualizer()
        self.vis.create_window(window_name="Old", width=800, height=600)
        self.vis1 = o3d.visualization.Visualizer()
        self.vis1.create_window(window_name="New", width=800, height=600)
        self.vis2 = o3d.visualization.Visualizer()
        self.vis2.create_window(window_name="Truth", width=800, height=600)

    def reset(self, seed: Optional[int] = None, options: Optional[dict] = None):
        super().reset(seed=seed)
        
        self.current_camera = self.action_space.sample()

        for i in range(self.cam_count):
            while True:
                image, skeletons = self.client.get_latest_data(i)
                if image is not None and skeletons is not None:
                    self.current_obs[i] = image
                    self.current_skeletons[i] = skeletons
                    break
                # else:
                    # print(f"No data received from client for camera {i}")

        info = {}
        return self.current_obs, info


    def step(self, action):
        self.current_camera = action

        for i in range(self.cam_count):
            while True:
                image, skeletons = self.client.get_latest_data(i)
                if image is not None and skeletons is not None:
                    self.current_obs[i] = image
                    self.current_skeletons[i] = skeletons
                    break
                # else:
                #     print(f"No data received from client for camera {i}")

        reward = self.calculate_reward()

        terminated = False
        truncated = False
        # if random.random() < 0.001:
        #     terminated = True
        info = {}
        return self.current_obs, reward, terminated, truncated, info

    def calculate_reward(self):
        pose_result = pe.pose_estimation(self.current_obs[self.current_camera])
        if pose_result.pose_world_landmarks is None:
            # print("Pose estimation failed, returning -1 reward")
            return -1

        processed_skeleton, _v = pe.process_results(pose_result)

        if processed_skeleton.shape != self.current_skeletons[self.current_camera].shape:
            raise ValueError("Skeletons must have same shape")

        mse = np.mean(np.square(processed_skeleton - self.current_skeletons[self.current_camera]))
        return 1-mse
