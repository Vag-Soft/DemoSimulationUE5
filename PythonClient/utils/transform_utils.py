import numpy as np
import torch

from PythonClient.pose_estimation.yolo.yolo import YoloPoseEstimator
from PythonClient.simulation.threaded_tcp_client import ThreadedTCPClient
from PythonClient.utils.plot_utils import plot2d_multiple_skeletons, overlay_multiple_skeleton_joints, \
    show_image_matplotlib_and_opencv


def euler_to_rotation_matrix(camera_rot):
    pitch = camera_rot[0]
    yaw = camera_rot[1]
    roll = camera_rot[2]


    pitch = np.deg2rad(-pitch)
    yaw = np.deg2rad(-yaw)
    roll = np.deg2rad(roll)

    Rx = np.array([
        [1, 0, 0],
        [0, np.cos(pitch), -np.sin(pitch)],
        [0, np.sin(pitch), np.cos(pitch)]
    ])

    Ry = np.array([
        [np.cos(yaw), 0, np.sin(yaw)],
        [0, 1, 0],
        [-np.sin(yaw), 0, np.cos(yaw)]
    ])

    Rz = np.array([
        [np.cos(roll), -np.sin(roll), 0],
        [np.sin(roll), np.cos(roll), 0],
        [0, 0, 1]
    ])

    return Rz @ Ry @ Rx  # Unreal Engine order: Roll → Yaw → Pitch


def calculate_intrinsics(fov_deg, height, width):
    fov_rad = np.deg2rad(fov_deg)
    fx = (width / 2) / np.tan(fov_rad / 2)
    fy = (height / 2) / np.tan(fov_rad / 2)
    cx = width / 2
    cy = height / 2
    return fov_rad, cx, cy, fx, fy


def project_3D_to_2D(joints_3d, camera_pos, camera_rot, fov_deg, width, height):
    M, N, _ = joints_3d.shape

    fov_rad, cx, cy, fx, fy = calculate_intrinsics(fov_deg, height, width)

    R = euler_to_rotation_matrix(camera_rot).T
    T = np.array(camera_pos).reshape(1, 1, 3)

    joints_relative = joints_3d - T
    joints_camera = np.einsum('ij,mnj->mni', R, joints_relative)

    x = joints_camera[..., 1]
    y = joints_camera[..., 2]
    z = joints_camera[..., 0]

    # Project the skeletons behind the camera as well
    # TODO: Try different methods for handling this
    z = np.abs(z)

    u = fx * (x / z) + cx
    v = -fy * (y / z) + cy

    projected = np.stack((u, v), axis=-1)

    projected[:, :, 0] = projected[:, :, 0] / width
    projected[:, :, 1] = projected[:, :, 1] / height
    return projected

def project_3D_to_2D_torch(joints_3d, camera_pos, camera_rot, fov_deg, width, height):
    fov_rad, cx, cy, fx, fy = calculate_intrinsics(fov_deg, height, width)

    R = euler_to_rotation_matrix(camera_rot).T
    R = torch.tensor(R, dtype=torch.float32)
    T = torch.tensor(camera_pos, dtype=torch.float32, device=joints_3d.device).reshape(1, 3)

    joints_relative = joints_3d - T
    joints_camera = torch.matmul(joints_relative, R)

    x = joints_camera[:, 1]
    y = joints_camera[:, 2]
    z = joints_camera[:, 0]
    z = torch.abs(z)

    u = fx * (x / z) + cx
    v = -fy * (y / z) + cy

    projected = torch.stack((u, v), dim=1)
    projected[:, 0] = projected[:, 0] / width
    projected[:, 1] = projected[:, 1] / height

    return projected

# if __name__ == '__main__':
#     client = ThreadedTCPClient(cam_count=3)
#     client.start()
#
#     camera = 0
#     while True:
#         image, skeletons = client.get_latest_data(camera)
#         if skeletons is not None:
#             break
#
#
#     projected = project_3D_to_2D(skeletons, client.camera_locations[camera], client.camera_rotations[camera], fov_deg=90, width=500, height=500)
#     plot2d_multiple_skeletons(projected, "Projected Ground Truth")
#     overlay_multiple_skeleton_joints(image, projected)
#
#
#     yolo = YoloPoseEstimator()
#     pose = yolo.estimate_pose(image)
#     pose, _ = yolo.process_results(pose)
#     plot2d_multiple_skeletons(pose, "Yolo Estimation")
#     overlay_multiple_skeleton_joints(image, pose)
