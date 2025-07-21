import torch
import numpy as np
import cv2
import os
import yaml
import json
from smplx import SMPLX
import trimesh
import pyrender
from PythonClient.pose_estimation.yolo.yolo import YoloPoseEstimator
from PythonClient.simulation.threaded_tcp_client import ThreadedTCPClient
from PythonClient.utils import transform_utils
import subprocess


# Function to convert YOLO keypoints to OpenPose format
def convert_yolo_to_openpose(keypoints, image_width, image_height):
    """
    Convert YOLO keypoints to OpenPose format
    YOLO keypoints: [x, y, confidence] normalized (0-1)
    OpenPose keypoints: [x, y, confidence] in pixel coordinates
    """
    # YOLO keypoint order (example):
    # 0: nose, 1: left_eye, 2: right_eye, ...
    # OpenPose keypoint order:
    # 0: nose, 1: neck, 2: right_shoulder, ...

    # Map YOLO keypoints to OpenPose keypoints
    # You'll need to adjust this mapping based on your YOLO model's keypoint order
    yolo_to_openpose_map = {
        0: 0,  # nose
        5: 2,  # right shoulder
        6: 5,  # left shoulder
        7: 3,  # right elbow
        8: 6,  # left elbow
        9: 4,  # right wrist
        10: 7,  # left wrist
        11: 8,  # right hip
        12: 11,  # left hip
        13: 9,  # right knee
        14: 12,  # left knee
        15: 10,  # right ankle
        16: 13  # left ankle
    }

    # Initialize OpenPose keypoints with zeros
    openpose_keypoints = np.zeros((25, 3))

    # Map YOLO keypoints to OpenPose format
    for yolo_idx, op_idx in yolo_to_openpose_map.items():
        if yolo_idx < len(keypoints):
            # Convert normalized coordinates to pixel coordinates
            x = keypoints[yolo_idx, 0]# * image_width
            y = keypoints[yolo_idx, 1]# * image_height
            confidence = 1.0  # Or use actual confidence if available

            openpose_keypoints[op_idx] = [x, y, confidence]

    return openpose_keypoints


# Function to prepare data for SMPLify-X
def prepare_smplify_data(image, keypoints, output_dir):
    """
    Prepare data for SMPLify-X
    """
    # Create directories
    image_dir = os.path.join(output_dir, 'images')
    keypoint_dir = os.path.join(output_dir, 'keypoints')

    os.makedirs(image_dir, exist_ok=True)
    os.makedirs(keypoint_dir, exist_ok=True)

    # Save image
    image_path = os.path.join(image_dir, 'image.jpg')
    cv2.imwrite(image_path, image)

    # Convert keypoints to OpenPose format
    height, width = image.shape[:2]
    openpose_keypoints = convert_yolo_to_openpose(keypoints, width, height)

    # Format as OpenPose JSON
    person_data = {
        "pose_keypoints_2d": openpose_keypoints.flatten().tolist()
    }

    keypoint_data = {
        "version": 1.5,
        "people": [person_data]
    }

    # Save keypoints
    keypoint_path = os.path.join(keypoint_dir, 'image_keypoints.json')
    with open(keypoint_path, 'w') as f:
        json.dump(keypoint_data, f)

    return image_path, keypoint_path


# Function to run SMPLify-X
def run_smplify_x(data_folder, output_folder, model_folder, vposer_ckpt, visualize=False):
    """
    Run SMPLify-X using subprocess
    """
    cmd = [
        'python', 'E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/smplify_x/smplifyx/main.py',
        '--config', 'E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/smplify_x/cfg_files/fit_smplx.yaml',
        '--data_folder', data_folder,
        '--output_folder', output_folder,
        '--visualize', str(visualize),
        '--model_folder', model_folder,
        '--vposer_ckpt', vposer_ckpt
    ]

    result = subprocess.run(cmd, capture_output=True, text=True)
    return result


# Function to load SMPL-X result
def load_smplx_result(result_folder):
    """
    Load SMPL-X result from pickle file
    """
    import pickle

    # Find the result file
    for root, dirs, files in os.walk(result_folder):
        for file in files:
            if file.endswith('.pkl'):
                with open(os.path.join(root, file), 'rb') as f:
                    data = pickle.load(f)
                    return data

    return None


# Main function
if __name__ == '__main__':
    # Initialize TCP client for UE5 data
    client = ThreadedTCPClient(cam_count=3)
    client.start()

    # Wait for valid data
    camera = 0
    while True:
        image, skeletons = client.get_latest_data(camera)
        if skeletons is not None:
            break

    # Estimate 2D keypoints using YOLO
    yolo = YoloPoseEstimator()
    results = yolo.estimate_pose(image)
    if results is None:
        print("No keypoints detected")
        exit()

    pose_keypoints = results.keypoints.numpy().xy  # [num_people, 17, 2]

    # Set up directories
    temp_data_dir = "temp_data"
    temp_output_dir = "temp_output"
    os.makedirs(temp_data_dir, exist_ok=True)
    os.makedirs(temp_output_dir, exist_ok=True)

    # Set paths to SMPL-X models and VPoser
    model_folder = "E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/smpl/models"
    vposer_ckpt = "E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/smpl/models/vposer_v1_0/vposer_v1_0"  # You need to provide this

    # Process each detected person
    meshes = []
    for i in range(pose_keypoints.shape[0]):
        person_keypoints = pose_keypoints[i]  # [17, 2]

        # Prepare data for SMPLify-X
        image_path, keypoint_path = prepare_smplify_data(
            image, person_keypoints, temp_data_dir
        )

        # Run SMPLify-X
        result = run_smplify_x(
            temp_data_dir,
            temp_output_dir,
            model_folder,
            vposer_ckpt,
            visualize=False
        )

        # Load SMPL-X result
        smplx_result = load_smplx_result(temp_output_dir)
        if smplx_result is None:
            print(f"Failed to get SMPL-X result for person {i + 1}")
            continue

        # Extract parameters
        betas = torch.tensor(smplx_result['betas'], dtype=torch.float32)
        body_pose = torch.tensor(smplx_result['body_pose'], dtype=torch.float32)
        global_orient = torch.tensor(smplx_result['global_orient'], dtype=torch.float32)

        # Load SMPL-X model
        smpl = SMPLX(
            model_path=os.path.join(model_folder, 'SMPLX_NEUTRAL.npz'),
            gender='neutral',
            num_pca_comps=12
        )

        # Generate mesh
        smpl_output = smpl(
            betas=betas.unsqueeze(0),
            body_pose=body_pose.unsqueeze(0),
            global_orient=global_orient.unsqueeze(0)
        )

        vertices = smpl_output.vertices.detach().numpy()[0]
        faces = smpl.faces

        meshes.append((vertices, faces))

    # Visualize meshes
    scene = pyrender.Scene(ambient_light=[0.3, 0.3, 0.3])

    # Add each mesh to the scene with a slight offset
    for i, (vertices, faces) in enumerate(meshes):
        mesh = trimesh.Trimesh(vertices=vertices, faces=faces)
        mesh_pyrender = pyrender.Mesh.from_trimesh(mesh, material=pyrender.MetallicRoughnessMaterial(
            baseColorFactor=[np.random.rand(), np.random.rand(), np.random.rand(), 1.0]
        ))
        node = pyrender.Node(mesh=mesh_pyrender, translation=np.array([i * 2.0, 0.0, 0.0]))
        scene.add_node(node)

    # Set up camera
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

    # Export meshes to OBJ files
    for i, (vertices, faces) in enumerate(meshes):
        with open(f'output_mesh_person_{i + 1}.obj', 'w') as f:
            for v in vertices:
                f.write(f'v {v[0]} {v[1]} {v[2]}\n')
            for face in faces:
                f.write(f'f {face[0] + 1} {face[1] + 1} {face[2] + 1}\n')