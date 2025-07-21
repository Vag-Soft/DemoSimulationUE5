from typing import Optional
import gymnasium as gym
import numpy as np
from gymnasium import spaces
from scipy.optimize import linear_sum_assignment

from PythonClient.pose_estimation.bev.my_bev import MyBev
from PythonClient.pose_estimation.yolo.yolo import YoloPoseEstimator
from PythonClient.feature_extraction.resnet50.resnet50 import ResNet50FeatureExtractor
from PythonClient.simulation.threaded_tcp_client import ThreadedTCPClient
from PythonClient.simulation.fake_threaded_tcp_client import FakeThreadedTCPClient
from PythonClient.utils import plot_utils, transform_utils


class Env(gym.Env):
    def __init__(self, width, height, cam_count, max_skeletons, history=5, fake_client=None, mode="train"):
        super().__init__()

        self.camera_fov_deg = 90
        self.width = width
        self.height = height
        self.cam_count = cam_count
        self.max_skeletons = max_skeletons
        self.history = history
        self.mode = mode

        self.action_space = spaces.Discrete(cam_count)
        self.observation_space = spaces.Dict({
            "features": spaces.Box(low=0, high=1, shape=(history, cam_count, 2048), dtype=np.float32),
            "previous_binary_mask": spaces.MultiBinary((history, cam_count))
        })

        self.current_camera = None
        self.current_obs = self.observation_space.sample()
        self.current_skeletons = np.zeros((cam_count, max_skeletons, 13, 3), dtype=np.float32)
        self.current_images = np.zeros((cam_count, height, width, 3), dtype=np.uint8)

        self.current_estimation = None

        # self.client = ThreadedTCPClient(cam_count=cam_count)
        # self.client.start()
        if fake_client:
            self.client = fake_client
        else:
            self.client = FakeThreadedTCPClient(cam_count=cam_count, mode=self.mode)
            self.client.start()

        self.feature_extractor = ResNet50FeatureExtractor()
        self.pose_estimator = MyBev()

    def reset(self, seed: Optional[int] = None, options: Optional[dict] = None):
        super().reset(seed=seed)

        self.current_camera = self.action_space.sample()

        for i in range(self.cam_count):
            while True:
                image, skeletons = self.client.get_latest_data(i)
                if image is not None and skeletons is not None:
                    self.current_images[i] = image
                    min_skeletons = min(skeletons.shape[0], self.max_skeletons)
                    self.current_skeletons[i, :min_skeletons] = skeletons[:min_skeletons]
                    break

        features = self.feature_extractor.extract_features(self.current_images)
        self.current_obs["features"] = np.zeros((self.history, self.cam_count, 2048), dtype=np.float32)
        self.current_obs["features"][0] = features

        self.current_obs["previous_binary_mask"] = np.zeros((self.history, self.cam_count), dtype=np.int8)

        info = {}
        return self.current_obs, info

    def step(self, action):
        self.current_camera = action

        processed_skeletons, confidences = self.estimate_poses(self.current_images[self.current_camera])

        reward = self.calculate_reward(processed_skeletons)

        for i in range(self.cam_count):
            while True:
                image, skeletons = self.client.get_latest_data(i)
                if image is not None and skeletons is not None:
                    self.current_images[i] = image
                    min_skeletons = min(skeletons.shape[0], self.max_skeletons)
                    self.current_skeletons[i, :min_skeletons] = skeletons[:min_skeletons]
                    break
                # else:
                #     print(f"No data received from client for camera {i}")

        features = self.feature_extractor.extract_features(self.current_images)
        self.current_obs["features"][1:] = self.current_obs["features"][:-1]
        self.current_obs["features"][0] = features

        self.current_obs["previous_binary_mask"][1:] = self.current_obs["previous_binary_mask"][:-1]
        new_binary_mask = np.zeros(self.cam_count, dtype=np.int8)
        new_binary_mask[self.current_camera] = 1
        self.current_obs["previous_binary_mask"][0] = new_binary_mask

        self.current_estimation = processed_skeletons

        terminated = False
        truncated = False
        # if random.random() < 0.001:
        #     terminated = True
        info = {}
        return self.current_obs, reward, terminated, truncated, info

    def estimate_poses(self, frame):
        results = self.pose_estimator.estimate(frame)
        if results is None:
            return None, None

        processed_skeletons, confidences = self.pose_estimator.process_results(results)

        return processed_skeletons, confidences

    def calculate_reward(self, processed_skeletons):
        if processed_skeletons is None:
            return -1

        truth_skeletons = self.process_truth_skeletons(self.current_skeletons[self.current_camera], self.current_camera)

        # Create distance matrix between all pairs of skeletons
        cost_matrix = np.zeros((len(truth_skeletons), len(processed_skeletons)))
        for i, proj in enumerate(truth_skeletons):
            for j, est in enumerate(processed_skeletons):
                # Calculate distance between corresponding joints
                cost_matrix[i, j] = np.mean(np.linalg.norm(proj - est, axis=1))

        # Find optimal matching using Hungarian algorithm
        row_ind, col_ind = linear_sum_assignment(cost_matrix)

        # Calculate average matching error
        avg_error = np.mean([cost_matrix[i, j] for i, j in zip(row_ind, col_ind)]) if len(row_ind) > 0 else 1.0

        # Count unmatched skeletons
        unmatched = (len(truth_skeletons) - len(row_ind)) + (len(processed_skeletons) - len(col_ind))
        unmatched_penalty = unmatched / self.max_skeletons  # Normalize by max skeletons

        reward = 1.0 - avg_error - 0.5 * unmatched_penalty

        return reward

    def process_truth_skeletons(self, skeletons, camera_id):
        skeletons = transform_utils.transform_world_to_camera(skeletons, self.client.camera_locations[camera_id], self.client.camera_rotations[camera_id])

        # skeletons = skeletons - skeletons[0, 0]

        # hip_dist = np.linalg.norm(skeletons[0, 1] - skeletons[0, 3])
        # skeletons = skeletons / hip_dist if hip_dist > 0 else skeletons

        min_vals = np.min(skeletons, axis=(0, 1), keepdims=True)
        max_vals = np.max(skeletons, axis=(0, 1), keepdims=True)
        range_vals = max_vals - min_vals

        range_vals[range_vals == 0] = 1.0
        skeletons = (skeletons - min_vals) / range_vals

        return skeletons