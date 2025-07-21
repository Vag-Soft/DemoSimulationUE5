import torch
import numpy as np
from smplx import SMPLX
import trimesh
import pyrender
import cv2
from PythonClient.pose_estimation.yolo.yolo import YoloPoseEstimator
from PythonClient.simulation.threaded_tcp_client import ThreadedTCPClient
from PythonClient.utils import transform_utils


# Simplified SMPLify-like optimization for one person
class SMPLify:
    def __init__(self, camera_pos, camera_rot, fov_deg, width, height, smpl_model, num_steps=100):
        self.camera_pos = camera_pos
        self.camera_rot = camera_rot
        self.fov_deg = fov_deg
        self.width = width
        self.height = height
        self.smpl = smpl_model
        self.num_steps = num_steps

    def fit(self, keypoints_2d, camera_intrinsics, camera_extrinsics):
        # Validate keypoints (expecting 17 COCO joints)
        if keypoints_2d.shape != (17, 2):
            raise ValueError(f"Expected 17 keypoints, got shape {keypoints_2d.shape}")

        # Initial SMPL-X parameters
        pose = torch.zeros(1, 72, dtype=torch.float32)  # 24 joints * 3 (body + global orient)
        betas = torch.zeros(1, 10, dtype=torch.float32)  # Shape parameters
        transl = torch.zeros(1, 3, dtype=torch.float32)  # Translation
        left_hand_pose = torch.zeros(1, 6, dtype=torch.float32)  # 6 PCA components for left hand
        right_hand_pose = torch.zeros(1, 6, dtype=torch.float32)  # 6 PCA components for right hand
        expression = torch.zeros(1, 10, dtype=torch.float32)  # Zero out facial expression

        # Optimization
        optimizer = torch.optim.Adam([pose, betas, transl], lr=0.01)  # Optimize only body parameters
        for _ in range(self.num_steps):
            optimizer.zero_grad()
            # Forward pass: Generate 3D joints from SMPL-X
            smpl_output = self.smpl(
                body_pose=pose[:, 3:],  # Exclude global orientation
                global_orient=pose[:, :3],
                betas=betas,
                transl=transl,
                left_hand_pose=left_hand_pose,
                right_hand_pose=right_hand_pose,
                expression=expression
            )
            joints_3d = smpl_output.joints[0]  # [N_joints, 3]
            # Project 3D joints to 2D using transform_utils
            joints_2d_pred = torch.tensor(
                transform_utils.project_3D_to_2D(joints_3d, self.camera_pos, self.camera_rot, self.fov_deg, self.width,
                                                 self.height),
                dtype=torch.float32
            )
            # Loss: Distance between predicted and input 2D keypoints
            loss = torch.mean((joints_2d_pred[:17] - keypoints_2d) ** 2)  # 17 COCO body joints
            loss.backward()
            optimizer.step()

        return pose[0], betas[0], transl[0]


# Process multiple skeletons and return meshes
def process_multiple_skeletons(keypoints_arr, camera_pos, camera_rot, fov_deg, width, height, smpl_model,
                               camera_intrinsics, camera_extrinsics):
    meshes = []
    smplify = SMPLify(camera_pos, camera_rot, fov_deg, width, height, smpl_model)

    for i in range(keypoints_arr.shape[0]):
        # Convert keypoints to tensor
        keypoints_2d = torch.tensor(keypoints_arr[i], dtype=torch.float32)
        # Fit SMPL-X to keypoints
        pose, betas, transl = smplify.fit(keypoints_2d, camera_intrinsics, camera_extrinsics)
        # Generate 3D mesh
        smpl_output = smpl_model(
            body_pose=pose[3:].unsqueeze(0),
            global_orient=pose[:3].unsqueeze(0),
            betas=betas.unsqueeze(0),
            transl=transl.unsqueeze(0),
            left_hand_pose=torch.zeros(1, 6, dtype=torch.float32),
            right_hand_pose=torch.zeros(1, 6, dtype=torch.float32),
            expression=torch.zeros(1, 10, dtype=torch.float32)
        )
        vertices = smpl_output.vertices.detach().numpy()[0]  # [10475, 3]
        faces = smpl_model.faces  # Mesh faces
        meshes.append((vertices, faces))

    return meshes


# Visualize meshes in a 3D scene
def visualize_meshes(meshes):
    scene = pyrender.Scene(ambient_light=[0.3, 0.3, 0.3])

    # Add each mesh to the scene with a slight offset
    for i, (vertices, faces) in enumerate(meshes):
        mesh = trimesh.Trimesh(vertices=vertices, faces=faces)
        mesh_pyrender = pyrender.Mesh.from_trimesh(mesh, material=pyrender.MetallicRoughnessMaterial(
            baseColorFactor=[np.random.rand(), np.random.rand(), np.random.rand(), 1.0]
        ))
        node = pyrender.Node(mesh=mesh_pyrender, translation=np.array([i * 2.0, 0.0, 0.0]))
        scene.add_node(node)

    # Set up camera (aligned with UE5 camera for visualization)
    camera = pyrender.PerspectiveCamera(yfov=np.pi / 3.0, aspectRatio=1.0)
    camera_pose = np.array([
        [1.0, 0.0, 0.0, 0.0],
        [0.0, 1.0, 0.0, 0.0],
        [0.0, 0.0, 1.0, 5.0],
        [0.0, 0.0, 0.0, 1.0]
    ])
    scene.add(camera, pose=camera_pose)

    # Add light
    light = pyrender.DirectionalLight(color=[1.0, 1.0, 1.0], intensity=3.0)
    scene.add(light, pose=camera_pose)

    # Render the scene
    viewer = pyrender.Viewer(scene, use_raymond_lighting=True, run_in_thread=False)


if __name__ == '__main__':
    image = cv2.imread('/PythonClient/simulation/yolo/test_data_2people/images_cam0/image_0.png')
    camera = 0  # Assuming we are using the first camera
    skeletons = np.load('/PythonClient/simulation/yolo/test_data_2people/skeletons_cam0/skeleton_0.npy')

    camera_locations = np.array([[793.58, 1185.2, 192.33],
                                      [1734.2, 1181.8, 130.28],
                                      [1273.2, 735.02, 138.41]])
    camera_rotations = np.array([[0.0, 0.0, 0.0],
                                      [0.0, 0.0, 180],
                                      [0.0, 0.0, 90]])


    # Estimate 2D keypoints using YOLO11
    yolo = YoloPoseEstimator()
    pose_keypoints = yolo.estimate_pose(image).keypoints.numpy().xy

    # Load SMPL-X model
    smpl = SMPLX(
        model_path='/PythonClient/smpl/models/SMPLX_NEUTRAL.npz',
        gender='neutral',
        num_pca_comps=6  # Explicitly set 6 PCA components for hands
    )

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

    # Process multiple skeletons
    meshes = process_multiple_skeletons(pose_keypoints, camera_position, rotation_degrees, fov_degrees,
                                        image_width, image_height, smpl, camera_intrinsics, camera_extrinsics)

    # Visualize meshes
    visualize_meshes(meshes)

    # Export meshes to OBJ files
    for i, (vertices, faces) in enumerate(meshes):
        with open(f'output_mesh_person_{i + 1}.obj', 'w') as f:
            for v in vertices:
                f.write(f'v {v[0]} {v[1]} {v[2]}\n')
            for face in faces:
                f.write(f'f {face[0] + 1} {face[1] + 1} {face[2] + 1}\n')