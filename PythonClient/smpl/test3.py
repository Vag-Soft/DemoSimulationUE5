import cv2
import torch
from smplx import SMPL
import numpy as np

from PythonClient.pose_estimation.yolo.yolo import YoloPoseEstimator
from PythonClient.utils import transform_utils

# chumpy/ch.py
# want_out = 'out' in inspect.getargspec(func).args
# ->
# want_out = 'out' in inspect.signature(func).parameters

model_path = 'E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/smpl/mpips_smplify_public_v2/smplify_public/code/models/'  # Folder containing SMPL_NEUTRAL.npz

# Load the SMPL model with gender 'neutral'
model = SMPL(model_path=model_path, gender='NEUTRAL')




image = cv2.imread('E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/simulation/yolo/test_data_2people/images_cam0/image_14.png')
camera = 0  # Assuming we are using the first camera
skeletons = np.load('E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/simulation/yolo/test_data_2people/skeletons_cam0/skeleton_14.npy')

camera_locations = np.array([[793.58, 1185.2, 192.33],
                             [1734.2, 1181.8, 130.28],
                             [1273.2, 735.02, 138.41]])
camera_rotations = np.array([[0.0, 0.0, 0.0],
                             [0.0, 0.0, 180],
                             [0.0, 0.0, 90]])

# Estimate 2D keypoints using YOLO11
yolo = YoloPoseEstimator()
keypoints_2d = yolo.estimate_pose(image).keypoints.numpy().xyn[0]

# Camera parameters from UE5
camera_position = camera_locations[camera]  # [x, y, z]
rotation_degrees = camera_rotations[camera]
rotation_matrix = transform_utils.euler_to_rotation_matrix(rotation_degrees)
# Camera extrinsic matrix [R | t]
camera_extrinsics = np.eye(4, dtype=np.float32)
camera_extrinsics[:3, :3] = rotation_matrix
camera_extrinsics[:3, 3] = camera_position

# Camera intrinsics: [fx, fy, cx, cy]
image_width, image_height = 500, 500  # Replace with your image dimensions
fov_degrees = 90.0  # Replace with your FOV in degrees
fov_rad, cx, cy, fx, fy = transform_utils.calculate_intrinsics(image_width, image_height, fov_degrees)
camera_intrinsics = np.array([fx, fy, cx, cy], dtype=np.float32)










# Set shape to default (average)
betas = torch.zeros([1, 10], dtype=torch.float32)

# Initialize pose and camera
pose = torch.zeros([1, 69], requires_grad=True)
global_orient = torch.zeros([1, 3], requires_grad=True)
transl = torch.zeros([1, 3], requires_grad=True)
camera_scale = torch.tensor([1.0], requires_grad=True)
camera_trans = torch.zeros([2], requires_grad=True)

# Define optimizer
optimizer = torch.optim.Adam([pose, global_orient, camera_scale, camera_trans], lr=0.01)

# SMPL joint regressor indices matching your keypoints
joint_idxs = [
    15,  # Nose     → Head (SMPL 15)
    -1,  # Left Eye → Not available in SMPL
    -1,  # Right Eye → Not available in SMPL
    -1,  # Left Ear → Not available in SMPL
    -1,  # Right Ear → Not available in SMPL
    16,  # Left Shoulder → Left Shoulder (SMPL 13)
    17,  # Right Shoulder → Right Shoulder (SMPL 14)
    18,  # Left Elbow → Left Elbow (SMPL 16)
    20,  # Right Elbow → Right Elbow (SMPL 17)
    21,  # Left Wrist → Left Wrist (SMPL 18)
    19,  # Right Wrist → Right Wrist (SMPL 19)
    1,   # Left Hip → Left Hip (SMPL 1)
    2,   # Right Hip → Right Hip (SMPL 2)
    4,   # Left Knee → Left Knee (SMPL 4)
    5,   # Right Knee → Right Knee (SMPL 5)
    7,   # Left Ankle → Left Ankle (SMPL 7)
    8    # Right Ankle → Right Ankle (SMPL 8)
]

valid_idxs = [i for i, idx in enumerate(joint_idxs) if idx != -1]
smpl_joint_indices = [idx for idx in joint_idxs if idx != -1]

for i in range(1000):
    optimizer.zero_grad()
    output = model(betas=betas, body_pose=pose, global_orient=global_orient, transl=transl)
    joints_3d = output.joints[0, smpl_joint_indices, :] # (N, 3)

    projected_2d = transform_utils.project_3D_to_2D_torch(joints_3d, camera_position, rotation_degrees, 90, 500, 500)  # (N, 2)
    keypoints_2d_valid = keypoints_2d[valid_idxs]  # (N, 2)

    # Convert to tensors for loss
    # projected_2d_tensor = torch.from_numpy(projected_2d).float()
    keypoints_2d_tensor = torch.from_numpy(keypoints_2d_valid).float()
    loss = torch.nn.functional.mse_loss(projected_2d, keypoints_2d_tensor)

    loss.backward()
    optimizer.step()



import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

vertices = output.vertices[0].detach().cpu().numpy()  # (6890, 3)
joints = output.joints[0].detach().cpu().numpy()      # (24, 3)

fig = plt.figure(figsize=(8, 8))
ax = fig.add_subplot(111, projection='3d')
ax.scatter(vertices[:, 0], vertices[:, 1], vertices[:, 2], s=0.1, color='blue', alpha=0.5, label="Vertices")
ax.scatter(joints[:, 0], joints[:, 1], joints[:, 2], s=20, color='red', label="Joints")

# Draw skeleton
# smpl_skeleton = [
#     (0, 1), (0, 2), (0, 3), (3, 6), (6, 9), (9, 12), (12, 15), (1, 4), (2, 5),
#     (4, 7), (5, 8), (7, 10), (8, 11), (13, 16), (14, 17), (16, 18), (17, 19)
# ]
# for (i, j) in smpl_skeleton:
#     ax.plot([joints[i, 0], joints[j, 0]], [joints[i, 1], joints[j, 1]], [joints[i, 2], joints[j, 2]], 'k-', linewidth=1)

ax.set_xlabel("X")
ax.set_ylabel("Y")
ax.set_zlabel("Z")
ax.legend()
ax.set_title("SMPL 3D Mesh and Joints")

plt.show()


import cv2

# After optimization
output = model(betas=betas, body_pose=pose, global_orient=global_orient, transl=transl)
joints_3d = output.joints[0, smpl_joint_indices, :].detach().cpu().numpy()
projected_2d = transform_utils.project_3D_to_2D(np.array([joints_3d]), camera_position, rotation_degrees, 90, 500, 500)[0]

image_vis = image.copy()
for pt in projected_2d.astype(int):
    cv2.circle(image_vis, tuple(pt), 3, (0,255,0), -1)

cv2.imshow('Projected SMPL Joints', image_vis)
cv2.waitKey(0)
cv2.destroyAllWindows()