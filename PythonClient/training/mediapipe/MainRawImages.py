from collections import deque

import cv2
import open3d as o3d
import time

from PythonClient.pose_estimation.mediapipe import mediapipe as pe
import numpy as np

from PythonClient.envs.mediapipe.EnvRawImages import Env
from PythonClient.simulation.ThreadedTCPClient import ThreadedTCPClient


def main():
    cam_count = 2
    image_deqs = [deque(maxlen=5) for _ in range(cam_count)]
    skeleton_deqs = [deque(maxlen=5) for _ in range(cam_count)]
    # Create and start the threaded client
    # client = ThreadedTCPClient(image_deqs=image_deqs, skeleton_deqs=skeleton_deqs, cam_count=cam_count)
    client = ThreadedTCPClient(cam_count=cam_count)
    client.start()

    # Open3D visualization
    vis = o3d.visualization.Visualizer()
    vis.create_window(window_name="Estimation Visualizer", width=800, height=600)
    vis1 = o3d.visualization.Visualizer()
    vis1.create_window(window_name="Skeleton Visualizer", width=800, height=600)

    # Benchmark stuff
    start = time.time()
    count = 0

    try:
        while True:
            count += 1

            # Get latest data from queues
            image_data, skeleton_data = client.get_latest_data(0)

            if image_data is not None and skeleton_data is not None:
                id = 0
                image = image_data
                skeletons = skeleton_data

                # Process image for pose estimation
                pose = pe.pose_estimation(image)
                skel, _v = pe.process_results(pose)

                if skel is not None:
                    arr = np.array([skel])
                    draw_skeleton_open3d(arr, vis)
                else:
                    vis.clear_geometries()
                    vis.poll_events()
                    vis.update_renderer()

                if skeletons is not None:
                    draw_skeleton_open3d(skeletons, vis1)
                else:
                    vis1.clear_geometries()
                    vis1.poll_events()
                    vis1.update_renderer()

                # Optional: Display image
                cv2.imshow(f'Image{id}', image)
                if cv2.waitKey(1) & 0xFF == ord('q'):
                    break

                # FPS counter
                if time.time() - start > 1:
                    print(f"FPS: {count}")
                    start = time.time()
                    count = 0

    except KeyboardInterrupt:
        print("Stopping client...")
    finally:
        client.stop()
        cv2.destroyAllWindows()
        vis.destroy_window()
        vis1.destroy_window()


def draw_skeleton_open3d(multiple_skeletons, vis):
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
    # main()


    env = Env(width=500, height=500, cam_count=3)
    obs, info = env.reset()
    while True:
        start = time.time()
        # for id, image in enumerate(obs):
        #     cv2.imshow('Image' + str(id), image)
        #     cv2.waitKey(1)
        obs, reward, terminated, truncated, info = env.step(0)
        end = time.time()
        print(f"FPS: {1 / (end - start)}")
        print(f"Reward: {reward}")


    #
    # models_dir = f"../models/A2C-{int(time.time())}"
    # logdir = f"../logs/A2C-{int(time.time())}"
    #
    # if not os.path.exists(models_dir):
    #     os.makedirs(models_dir)
    #
    # if not os.path.exists(logdir):
    #     os.makedirs(logdir)
    #
    #
    # env = Env(width=500, height=500, cam_count=2)
    # obs, info = env.reset()
    # model = A2C("MlpPolicy", env, verbose=1, tensorboard_log=logdir)
    #
    #
    # model.learn(total_timesteps=500, progress_bar=True)
    # model.save(f"{models_dir}/A2C_model")
    #
    # actions = []
    # while True:
    #     for id, image in enumerate(obs):
    #         cv2.imshow('Image' + str(id), image)
    #         cv2.waitKey(1)
    #     action, _ = model.predict(obs, deterministic=True)
    #     actions.append(action)
    #     obs, reward, terminated, truncated, info = env.step(action)
    #     print(f"Reward: {reward}, Action: {action}")
    #
    # env.close()