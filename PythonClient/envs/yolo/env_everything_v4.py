from typing import Optional
import gymnasium as gym
import numpy as np
from gymnasium import spaces
from scipy.optimize import linear_sum_assignment

from PythonClient.pose_estimation.yolo.yolo import YoloPoseEstimator
from PythonClient.feature_extraction.resnet50.resnet50 import ResNet50FeatureExtractor
from PythonClient.simulation.threaded_tcp_client import ThreadedTCPClient
from PythonClient.simulation.fake_threaded_tcp_client import FakeThreadedTCPClient
from PythonClient.utils import plot_utils, transform_utils

'''
Idk
'''
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
            "previous_poses": spaces.Box(low=-np.inf, high=np.inf, shape=(history, cam_count, max_skeletons, 13, 2), dtype=np.float32),
            "previous_confidences": spaces.Box(low=0, high=1, shape=(history, cam_count, max_skeletons, 13), dtype=np.float32)
        })

        self.current_camera = None
        self.current_obs = self.observation_space.sample()
        self.current_skeletons = np.zeros((cam_count, max_skeletons, 13, 3), dtype=np.float32)
        self.current_images = np.zeros((cam_count, height, width, 3), dtype=np.uint8)

        # self.client = ThreadedTCPClient(cam_count=cam_count)
        # self.client.start()
        if fake_client:
            self.client = fake_client
        else:
            self.client = FakeThreadedTCPClient(cam_count=cam_count, mode=self.mode)
            self.client.start()

        self.feature_extractor = ResNet50FeatureExtractor()
        self.pose_estimator = YoloPoseEstimator()

    def reset(self, seed: Optional[int] = None, options: Optional[dict] = None):
        super().reset(seed=seed)

        self.current_camera = self.action_space.sample()

        for i in range(self.cam_count):
            while True:
                image, skeletons = self.client.get_latest_data(i)
                if image is not None and skeletons is not None:
                    self.current_images[i] = image
                    self.current_skeletons[i] = skeletons[:self.max_skeletons]
                    break

        features = self.feature_extractor.extract_features(self.current_images)
        self.current_obs["features"] = np.zeros((self.history, self.cam_count, 2048), dtype=np.float32)
        self.current_obs["features"][0] = features

        self.current_obs["previous_poses"] = np.zeros((self.history, self.cam_count, self.max_skeletons, 13, 2), dtype=np.float32)

        self.current_obs["previous_confidences"] = np.zeros((self.history, self.cam_count, self.max_skeletons, 13), dtype=np.float32)

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
                    self.current_skeletons[i] = skeletons[:self.max_skeletons]
                    break
                # else:
                #     print(f"No data received from client for camera {i}")

        features = self.feature_extractor.extract_features(self.current_images)
        self.current_obs["features"][1:] = self.current_obs["features"][:-1]
        self.current_obs["features"][0] = features

        self.current_obs["previous_poses"][1:] = self.current_obs["previous_poses"][:-1]
        new_poses = np.zeros((self.cam_count, self.max_skeletons, 13, 2), dtype=np.float32)
        if processed_skeletons is not None:
            for i, skeleton in enumerate(processed_skeletons[:self.max_skeletons]):
                new_poses[self.current_camera, i] = skeleton
        self.current_obs["previous_poses"][0] = new_poses

        self.current_obs["previous_confidences"][1:] = self.current_obs["previous_confidences"][:-1]
        new_confidences = np.zeros((self.cam_count, self.max_skeletons, 13), dtype=np.float32)
        if confidences is not None:
            for i, confidence in enumerate(confidences[:self.max_skeletons]):
                new_confidences[self.current_camera, i] = confidence
        self.current_obs["previous_confidences"][0] = new_confidences

        terminated = False
        truncated = False
        # if random.random() < 0.001:
        #     terminated = True
        info = {}
        return self.current_obs, reward, terminated, truncated, info

    def estimate_poses(self, frame):
        pose_results = self.pose_estimator.estimate_pose(frame)
        if pose_results is None:
            return None, None

        processed_skeletons, confidences = self.pose_estimator.process_results(pose_results)

        return processed_skeletons, confidences

    def calculate_reward(self, processed_skeletons):
        if processed_skeletons is None:
            return -1

        projected_skeletons = transform_utils.project_3D_to_2D(
            self.current_skeletons[self.current_camera],
            self.client.camera_locations[self.current_camera],
            self.client.camera_rotations[self.current_camera],
            self.camera_fov_deg,
            self.width,
            self.height)

        # Create distance matrix between all pairs of skeletons
        cost_matrix = np.zeros((len(projected_skeletons), len(processed_skeletons)))
        for i, proj in enumerate(projected_skeletons):
            for j, est in enumerate(processed_skeletons):
                # Calculate distance between corresponding joints
                cost_matrix[i, j] = np.mean(np.linalg.norm(proj - est, axis=1))

        # Find optimal matching using Hungarian algorithm
        row_ind, col_ind = linear_sum_assignment(cost_matrix)

        # Calculate average matching error
        avg_error = np.mean([cost_matrix[i, j] for i, j in zip(row_ind, col_ind)]) if len(row_ind) > 0 else 1.0

        # Count unmatched skeletons
        unmatched = (len(projected_skeletons) - len(row_ind)) + (len(processed_skeletons) - len(col_ind))

        # Final reward: higher when error is lower and fewer unmatched skeletons
        reward = 1.0 - avg_error - 0.9 * unmatched

        return reward