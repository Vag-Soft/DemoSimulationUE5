from typing import Optional

import gymnasium as gym
import numpy as np
from gymnasium import spaces
from PythonClient.pose_estimation.mediapipe import mediapipe as pe
from PythonClient.simulation.FakeThreadedTCPClient import FakeThreadedTCPClient

'''
Features:
- Connects to a simulation via TCP to receive camera images and ground-truth 3D skeleton data
- Uses MediaPipe for pose estimation to extract 3D joint positions and confidence scores
- Maintains a temporal history of observations across all cameras
- Trains an agent to select optimal camera views based on pose estimation quality
- Supports both real and simulated data sources through ThreadedTCPClient/FakeThreadedTCPClient

Observation Space:
- Shape: (history_count, cam_count, 14), dtype=np.float32
- Each camera provides 13 joint confidence scores (0-1) and 1 camera selection flag (0/1)
- Maintains history_count previous states, with only the selected camera updated each step 
(unselected cameras have stale information

Action Space:
- Discrete(cam_count): Agent selects which camera to process at each step

Reward Function:
- Based on Mean Squared Error (MSE) between MediaPipe-predicted joint positions
  and simulation ground-truth 3D skeleton positions
- Higher reward (closer to 1) for more accurate pose estimation
- Returns -1 if pose estimation fails

Processing Pipeline:
1. Agent selects a camera
2. Environment receives image from selected camera
3. MediaPipe processes image to extract 3D pose landmarks
4. Environment calculates reward by comparing predicted pose to ground truth
5. Updates observation history with new confidence scores and camera selection state
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
