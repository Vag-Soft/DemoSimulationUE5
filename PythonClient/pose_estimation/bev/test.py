import time

import bev
import cv2

import open3d as o3d
import numpy as np
import my_romp


def visualize_skeleton_joints(keypoints, cam_trans=None, connections=None, colors=None):
    """
    Visualize 3D skeleton keypoints using Open3D

    Args:
        keypoints: numpy array of shape (N, M, 3) where N is number of skeletons,
                  M is number of joints, and 3 is the x,y,z coordinates
        cam_trans: numpy array of shape (N, 3) representing camera translations for each skeleton
        connections: list of tuples where each tuple is (joint_idx1, joint_idx2)
                    indicating which joints should be connected
        colors: list of colors for each skeleton, if None, random colors are generated
    """
    # Create Open3D visualizer
    vis = o3d.visualization.Visualizer()
    vis.create_window()

    # Default SMPL model connections if none provided
    if connections is None:
        connections = [
            (0, 1), (0, 2), (0, 3),  # Pelvis connections
            (3, 6), (6, 9), (9, 12), (12, 15),  # Spine to head
            (9, 13), (13, 16), (16, 18), (18, 20),  # Left arm
            (9, 14), (14, 17), (17, 19), (19, 21),  # Right arm
            (1, 4), (4, 7), (7, 10),  # Right leg
            (2, 5), (5, 8), (8, 11)  # Left leg
        ]

    # Generate random colors if not provided
    if colors is None:
        colors = [np.random.rand(3) for _ in range(keypoints.shape[0])]

    # Process each skeleton
    for i in range(keypoints.shape[0]):
        joints = keypoints[i].copy()

        # Apply camera translation if provided
        if cam_trans is not None:
            # Adjust joint positions using camera translation
            joints += cam_trans[i]

        # Create point cloud for joints
        joint_cloud = o3d.geometry.PointCloud()
        joint_cloud.points = o3d.utility.Vector3dVector(joints)
        joint_cloud.colors = o3d.utility.Vector3dVector(np.tile(colors[i], (joints.shape[0], 1)))
        vis.add_geometry(joint_cloud)

        # Create lines for skeleton connections
        valid_connections = [(s, e) for s, e in connections if s < joints.shape[0] and e < joints.shape[0]]
        if valid_connections:
            line_set = o3d.geometry.LineSet()
            line_set.points = o3d.utility.Vector3dVector(joints)
            line_set.lines = o3d.utility.Vector2iVector(np.array(valid_connections))
            line_set.colors = o3d.utility.Vector3dVector(np.tile(colors[i], (len(valid_connections), 1)))
            vis.add_geometry(line_set)

    # Add coordinate frame and set rendering options
    vis.add_geometry(o3d.geometry.TriangleMesh.create_coordinate_frame(size=0.5))
    opt = vis.get_render_option()
    opt.background_color = np.array([0.1, 0.1, 0.1])
    opt.point_size = 5.0
    opt.line_width = 2.0

    # Run visualization
    vis.run()
    vis.destroy_window()

# settings = bev.main.default_settings
# # settings is just a argparse Namespace. To change it, for instance, you can change mode via
# # settings.mode='video'
# # settings.onnx = True
# bev_model = bev.BEV(settings)
# # outputs = bev_model(cv2.imread('smpl_model_data/image_0.png')) # please note that we take the input image in BGR format (cv2.imread).
# img = cv2.imread('smpl_model_data/Solvay_conference_1927_all_people.jpg')
#
# # while True:
# start = time.time()
# outputs = bev_model(img)
# visualize_skeleton_joints(outputs['joints'], outputs['cam_trans'])
#
# print(outputs)
# end = time.time()
# print(f"Time taken: {end - start:.4f} seconds")
#


settings = romp.main.default_settings
# settings is just a argparse Namespace. To change it, for instance, you can change mode via
# settings.mode='video'
settings.onnx = True
bev_model = romp.ROMP(settings)
# outputs = bev_model(cv2.imread('smpl_model_data/image_0.png')) # please note that we take the input image in BGR format (cv2.imread).
img = cv2.imread('smpl_model_data/Solvay_conference_1927_all_people.jpg')

# while True:
start = time.time()
outputs = bev_model(img)
visualize_skeleton_joints(outputs['joints'], outputs['cam_trans'])

print(outputs)
end = time.time()
print(f"Time taken: {end - start:.4f} seconds")

