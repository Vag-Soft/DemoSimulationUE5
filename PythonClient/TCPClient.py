import socket
import struct
import time
import numpy as np
import cv2

import open3d as o3d
from PythonClient.pose_estimation.mediapipe import mediapipe as pe


def main():
    # Connect to the server
    host = "127.0.0.1"
    port = 7777
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((host, port))


    data = client_socket.recv(1024)
    print("Received from Unreal:", data.decode('utf-8'))



    # request = input("Enter a request: ")
    request = "images"
    while request != "q":

        if request == 'images':
            # Benchmark stuff
            start = time.time()
            count = 0

            # Open3D visualization
            vis = o3d.visualization.Visualizer()
            vis.create_window(window_name="Estimation Visualizer", width=800, height=600)
            vis1 = o3d.visualization.Visualizer()
            vis1.create_window(window_name="Skeleton Visualizer", width=800, height=600)

            # Sending the request to start streaming
            client_socket.send(request.encode('utf-8'))
            while True:
                count += 1

                # Receiving the image
                id, image = receive_image(client_socket)
                # Receiving the skeletons
                skeletons = receive_skeletons(client_socket)
                skeletons = process_bones(skeletons)

                pose = pe.pose_estimation(image)
                skel, _v = pe.process_results(pose)

                if skel is None:
                    vis.clear_geometries()
                    vis.poll_events()
                    vis.update_renderer()
                else:
                    arr = np.array([skel])

                    draw_skeleton_open3d(arr, vis)

                # Drawing the skeletons
                # draw_skeleton_open3d(skeletons, vis1)
                # plot_single_skeleton(skeletons[0])

                # # Drawing the image
                # if id == 0:
                #     cv2.imshow('Image1', image)
                # elif id == 1:
                #     cv2.imshow('Image2', image)
                # if cv2.waitKey(1) & 0xFF == ord('q'):
                #     break

                # Printing FPS
                if time.time() - start > 1:
                    print(count)
                    start = time.time()
                    count = 0

            cv2.destroyAllWindows()

        request = input("Enter a request: ")

    client_socket.close()

def receive_image(socket):
    # Receiving the id of the image
    id_bytes = socket.recv(4)
    id = int.from_bytes(id_bytes, "little")

    # Receiving the size of the image data
    image_data_size_bytes = socket.recv(4)
    image_data_size = int.from_bytes(image_data_size_bytes, "little")

    # Receiving the image data
    image_data = socket.recv(image_data_size)

    image_np = np.frombuffer(image_data, dtype=np.uint8)
    image = cv2.imdecode(image_np, cv2.IMREAD_UNCHANGED)

    return id, image

def receive_skeletons(socket):
    # Receive the camera position (24 bytes for double x, y, z)
    camera_position_data = socket.recv(24)
    cam_x, cam_y, cam_z = struct.unpack("d d d", camera_position_data)
    # print(f"Camera position: ({cam_x}, {cam_y}, {cam_z})")

    # Receive the number of skeletons (4 bytes)
    num_skeletons_data = socket.recv(4)
    num_skeletons = int.from_bytes(num_skeletons_data, "little")
    # print(f"Receiving {num_skeletons} skeletons...")

    # Receive the number of bones (4 bytes)
    num_bones_data = socket.recv(4)
    num_bones = int.from_bytes(num_bones_data, "little")
    # print(f"Receiving {num_bones} bone transforms...")

    # Read all bone transforms
    # Shape: (num_skeletons, num_bones, 3)
    # Each entry stores the position (px, py, pz) of a bone
    character_transforms = np.zeros((num_skeletons, num_bones, 3))
    for _ in range(num_skeletons):
        # Receive the id of the character
        id_data = socket.recv(4)
        skeleton_idx = int.from_bytes(id_data, "little")

        # Shape: (num_bones, 3), where each row is [px, py, pz]
        bone_positions = np.zeros((num_bones, 3))
        for _ in range(num_bones):
            # Read 32 bytes (bone index + position + rotation)
            data = socket.recv(32)

            # Unpack: int32 + 3 floats (FVector) + 4 floats (FQuat)
            bone_index, px, py, pz, qx, qy, qz, qw = struct.unpack("i f f f f f f f", data)

            character_transforms[skeleton_idx, bone_index] = [px - cam_x, pz - cam_z, py - cam_y]

    return character_transforms

def process_bones(character_transforms):
    # Define the remapping of bone IDs
    new_idxs = {9: 0,
                11: 1,
                38: 2,
                12: 3,
                39: 4,
                15: 5,
                42: 6,
                72: 7,
                64: 8,
                73: 9,
                65: 10,
                74: 11,
                66: 12}

    # Initialize a NumPy array for the processed skeletons
    num_skeletons, num_bones, _ = character_transforms.shape
    num_remapped_bones = len(new_idxs)
    processed_skeletons = np.zeros((num_skeletons, num_remapped_bones, 3))  # Shape: (num_skeletons, num_remapped_bones, 3)

    for skeleton_idx in range(num_skeletons):
        skeleton = character_transforms[skeleton_idx]

        # Extract the root bone position
        root_bone = skeleton[9]  # Position of bone 9 (x, y, z)

        # Process and remap the bones
        for old_bone_id, new_bone_id in new_idxs.items():
            processed_skeletons[skeleton_idx, new_bone_id] = skeleton[old_bone_id] - root_bone


        # Compute the maximum distance from the origin for normalization
        max_distance = np.max(np.linalg.norm(processed_skeletons[skeleton_idx], axis=1))

        # Normalize the skeleton
        processed_skeletons[skeleton_idx] = processed_skeletons[skeleton_idx] / max_distance

    return processed_skeletons

def plot_single_skeleton(skeleton):
    """
    Plots a single skeleton using Open3D and allows interactive mouse movements.

    Parameters:
    - skeleton: np.array of shape (bone_count, 3)
                Contains the joint positions for the skeleton.
    """
    # Create an Open3D point cloud for the joints
    joints_pcd = o3d.geometry.PointCloud()
    joints_pcd.points = o3d.utility.Vector3dVector(skeleton)
    joints_pcd.paint_uniform_color([1, 0, 0])  # Red color for joints

    # Create an Open3D visualization window
    vis = o3d.visualization.Visualizer()
    vis.create_window(window_name="Skeleton Viewer")

    # Add the point cloud to the visualization
    vis.add_geometry(joints_pcd)

    # Set the initial view control
    view_control = vis.get_view_control()
    view_control.set_front([-1.0, 1.0, 1.0])  # Set the front view
    view_control.set_up([0.0, 0.0, 1.0])      # Align the up direction
    view_control.set_zoom(0.8)               # Adjust zoom level

    # Run the visualization
    vis.run()

    # Destroy the visualization window after exiting
    vis.destroy_window()
def draw_skeleton_open3d(multiple_skeletons, vis):
    """
    Draws multiple skeletons in Open3D visualization.

    Parameters:
    - multiple_skeletons: np.array of shape (skeleton_count, bone_count, 3)
                          Contains the joint positions for all skeletons.
    - vis: Open3D visualization object.
    """
    # Clear previous geometries
    vis.clear_geometries()

    # Iterate over all skeletons
    skeleton_count, bone_count, _ = multiple_skeletons.shape
    for skeleton_idx in range(skeleton_count):
        # Extract joint positions for the current skeleton
        joint_positions = multiple_skeletons[skeleton_idx]

        # Create an Open3D point cloud for joints
        joints_pcd = o3d.geometry.PointCloud()
        joints_pcd.points = o3d.utility.Vector3dVector(joint_positions)
        joints_pcd.paint_uniform_color([1, 0, 0])  # Red color for joints

        # Add the joints point cloud to the visualization
        vis.add_geometry(joints_pcd)

    # Set view control (applies globally)
    # view_control = vis.get_view_control()
    # view_control.set_front([-1.0, 1.0, 1.0])
    # view_control.set_up([0.0, 0.0, 1.0])

    # Update visualization
    vis.poll_events()
    vis.update_renderer()

if __name__ == "__main__":
    main()
