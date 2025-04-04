import socket
import struct
import time
import numpy as np
import cv2

import matplotlib.pyplot as plt

import open3d as o3d

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
            vis.create_window(window_name="Skeleton Visualizer", width=800, height=600)

            # Sending the request to start streaming
            client_socket.send(request.encode('utf-8'))
            while True:
                count += 1

                # Receiving the image
                id, image = receive_image(client_socket)
                # Receiving the skeletons
                skeletons = receive_skeletons(client_socket)

                # Drawing the skeletons
                draw_skeleton_open3d(skeletons, vis)

                # Drawing the image
                if id == 0:
                    cv2.imshow('Image1', image)
                elif id == 1:
                    cv2.imshow('Image2', image)
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break

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
    # Receive the number of skeletons (4 bytes)
    num_skeletons_data = socket.recv(4)
    num_skeletons = int.from_bytes(num_skeletons_data, "little")

    # Receive the number of bones (4 bytes)
    num_bones_data = socket.recv(4)
    num_bones = int.from_bytes(num_bones_data, "little")
    # print(f"Receiving {num_bones} bone transforms...")

    # Read all bone transforms
    character_transforms = {}
    for _ in range(num_skeletons):
        # Receive the id of the character
        id_data = socket.recv(4)
        id = int.from_bytes(id_data, "little")

        bone_transforms = {}
        for _ in range(num_bones):
            # Read 32 bytes (bone index + position + rotation)
            data = socket.recv(32)

            # Unpack: int32 + 3 floats (FVector) + 4 floats (FQuat)
            bone_index, px, py, pz, qx, qy, qz, qw = struct.unpack("i f f f f f f f", data)

            bone_transforms[bone_index] = {
                "Position": (px, py, pz),
                "Rotation": (qx, qy, qz, qw)
            }

        character_transforms[id] = bone_transforms

    return character_transforms


def draw_skeleton_matplot(multiple_skeletons):
    # # Example multiple skeletons dictionary with bone positions
    # multiple_skeletons = {
    #     "skeleton1": {
    #         0: {"Position": (0, 0, 0)},
    #         1: {"Position": (1, 0, 0)},
    #         2: {"Position": (0, 1, 0)},
    #         3: {"Position": (1, 1, 0)},
    #         4: {"Position": (1, -1, 0)},
    #         5: {"Position": (-1, 1, 0)}
    #     },
    #     "skeleton2": {
    #         0: {"Position": (5, 5, 0)},
    #         1: {"Position": (6, 5, 0)},
    #         2: {"Position": (5, 6, 0)},
    #         3: {"Position": (6, 6, 0)},
    #         4: {"Position": (6, 4, 0)},
    #         5: {"Position": (4, 6, 0)}
    #     }
    # }
    #
    # # Common bone hierarchy mapping for each skeleton.
    # # For example, bone 0 is the root; bones 1 and 2 are its children; etc.
    # bone_hierarchy = {
    #     0: [1, 2],
    #     1: [3, 4],
    #     2: [5],
    #     3: [],
    #     4: [],
    #     5: []
    # }

    # Create a 3D plot
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    # Define a list of colors to distinguish skeletons
    colors = ['red', 'blue', 'green', 'orange']

    for idx, (skeleton_name, bone_transforms) in enumerate(multiple_skeletons.items()):
        color = colors[idx % len(colors)]

        # Extract positions: convert each bone's "Position" tuple into a NumPy array
        positions = {bone: np.array(bone_transforms[bone]["Position"]) for bone in bone_transforms}

        # Plot each joint (bone position)
        for bone, pos in positions.items():
            ax.scatter(pos[0], pos[1], pos[2], color=color, s=50)
            # Label the point with skeleton name and bone index
            # ax.text(pos[0], pos[1], pos[2], f"{skeleton_name}:{bone}", color='black', fontsize=8)
        #
        # # Draw bones (lines between parent and child joints)
        # for parent, children in bone_hierarchy.items():
        #     if parent in positions:
        #         parent_pos = positions[parent]
        #         for child in children:
        #             if child in positions:
        #                 child_pos = positions[child]
        #                 ax.plot([parent_pos[0], child_pos[0]],
        #                         [parent_pos[1], child_pos[1]],
        #                         [parent_pos[2], child_pos[2]],
        #                         color=color, linestyle='--')

    # Set labels and title
    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_zlabel("Z")
    ax.set_title("Multiple Skeletons Visualization")

    plt.show()

def draw_skeleton_open3d(multiple_skeletons, vis):
    vis.clear_geometries()

    for idx, (skeleton_name, bone_transforms) in enumerate(multiple_skeletons.items()):
        # Extract joint positions from the skeleton dictionary
        joint_positions = []
        for bone in bone_transforms.keys():
            joint_positions.append(bone_transforms[bone]["Position"])

        # Convert joint positions to numpy array
        joint_positions = np.array(joint_positions)

        # Create an Open3D point cloud for joints
        joints_pcd = o3d.geometry.PointCloud()
        joints_pcd.points = o3d.utility.Vector3dVector(joint_positions)
        joints_pcd.paint_uniform_color([1, 0, 0])  # Red color for joints



        # Update joints and skeleton_lines geometry
        joints_pcd.points = o3d.utility.Vector3dVector(joint_positions)

        vis.add_geometry(joints_pcd)

        # Get the view control object
        view_control = vis.get_view_control()
        view_control.set_front([-1.0, 1.0, 1.0])
        view_control.set_up([0.0, 0.0, 1.0])


        vis.poll_events()
        vis.update_renderer()


if __name__ == "__main__":
    main()
