import cv2
import numpy as np
import matplotlib.pyplot as plt
import mediapipe as mp

mp_pose = mp.solutions.pose
pose = mp_pose.Pose(static_image_mode=False,
                    model_complexity=2,
                    enable_segmentation=False,
                    min_detection_confidence=0.5)

def pose_estimation(frame):
    results = pose.process(frame)

    return results

def process_results(results):
    # Map the desired landmarks to new indices
    new_idxs = {0: 0,
                12: 1,
                11: 2,
                14: 3,
                13: 4,
                16: 5,
                15: 6,
                24: 7,
                23: 8,
                26: 9,
                25: 10,
                28: 11,
                27: 12}

    if results.pose_world_landmarks:
        # Extract the root bone position as a NumPy array
        root_bone = np.array([results.pose_world_landmarks.landmark[0].x,
                              results.pose_world_landmarks.landmark[0].y,
                              results.pose_world_landmarks.landmark[0].z])

        # Initialize a NumPy array to store the skeleton
        num_bones = len(new_idxs)
        skeleton = np.zeros((num_bones, 3))  # Shape: (num_bones, 3)

        # Populate the skeleton array with relative positions
        for idx, landmark in enumerate(results.pose_world_landmarks.landmark):
            if idx in new_idxs:
                new_idx = new_idxs[idx]
                position = np.array([landmark.x, landmark.y, landmark.z]) - root_bone
                skeleton[new_idx] = position
        # Check if any of the root landmarks are negative and swap the sign of the whole column if needed
        for i in range(3):
            if root_bone[i] < 0:
                skeleton[:, i] = -skeleton[:, i]

        # Compute the maximum distance from the origin for normalization
        max_distance = np.max(np.linalg.norm(skeleton, axis=1))

        # Normalize the skeleton
        normalized_skeleton = skeleton / max_distance

        visibilities = np.array([results.pose_world_landmarks.landmark[id].visibility for id in new_idxs.keys()])




        # left_shoulder = np.array([results.pose_world_landmarks.landmark[11].x,
        #                           results.pose_world_landmarks.landmark[11].y,
        #                           results.pose_world_landmarks.landmark[11].z])
        # right_shoulder = np.array([results.pose_world_landmarks.landmark[12].x,
        #                            results.pose_world_landmarks.landmark[12].y,
        #                            results.pose_world_landmarks.landmark[12].z])
        #
        # # Shoulder vector points from right to left shoulder
        # shoulder_vec = left_shoulder - right_shoulder
        #
        # # Calculate rotation to align the skeleton with your simulation's front direction
        # # This depends on your world space convention
        # facing_angle = np.arctan2(shoulder_vec[2], shoulder_vec[0])
        #
        # # Apply rotation around Y axis to the normalized skeleton
        # rotation = R.from_euler('y', facing_angle)
        # normalized_skeleton = (rotation.as_matrix() @ normalized_skeleton.T).T



        return normalized_skeleton, visibilities
    return None


from scipy.spatial.transform import Rotation as R
def skeleton_camera_to_world(skeleton):
    # camera_position = np.array((0.0, 0.0, 0.0))
    # camera_rotation = np.array((0, -4.5, 89.5))


    camera_position = np.array((0.0, 0.0, 0.0))
    camera_rotation = np.array((0, 22, 90))

    r = R.from_euler('ZXY', [camera_rotation[1], camera_rotation[0], camera_rotation[2]], degrees=True)
    rotation_matrix = r.as_matrix()

    pose_in_world = (rotation_matrix @ skeleton.T).T + camera_position
    return pose_in_world