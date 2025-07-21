import os

import cv2
import open3d as o3d

import numpy as np
from stable_baselines3 import A2C

from PythonClient.envs.mediapipe.EnvConfidenceScoresV3 import Env


def draw_skeleton_open3d(multiple_skeletons, vis):
    vis.clear_geometries()


    joint_positions = multiple_skeletons

    joints_pcd = o3d.geometry.PointCloud()
    joints_pcd.points = o3d.utility.Vector3dVector(joint_positions)
    joints_pcd.paint_uniform_color([1, 0, 0])  # Red color for joints

    vis.add_geometry(joints_pcd)

    # Set view control (applies globally)
    # view_control = vis.get_view_control()
    # view_control.set_front([-1.0, 1.0, 1.0])
    # view_control.set_up([0.0, 0.0, 1.0])

    # Update visualization
    vis.poll_events()
    vis.update_renderer()

if __name__ == "__main__":


    # env = Env(width=500, height=500, cam_count=3, mode="train", history_count=10)
    # obs, info = env.reset()
    # while True:
    #     start = time.time()
    #     # for id in range(env.cam_count):
    #     #     image, skeletons = env.client.get_latest_data(id)
    #     #     if image is not None and skeletons is not None:
    #     #         cv2.imshow('Image' + str(id), image)
    #     #         cv2.waitKey(1)
    #     obs, reward, terminated, truncated, info = env.step(env.action_space.sample())
    #     end = time.time()
    #     print(f"FPS: {1 / (end - start)}")
    #     # print(f"Reward: {reward}, Info: {info}")


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
    # env = Env(width=500, height=500, cam_count=3, mode="train", history_count=10)
    # obs, info = env.reset()
    # model = A2C("MlpPolicy", env, verbose=1, tensorboard_log=logdir)
    #
    # checkpoint_callback = CheckpointCallback(
    #     save_freq=10000,  # Save every 10,000 steps
    #     save_path=models_dir,
    #     name_prefix="A2C_model",
    #     save_replay_buffer=True,
    #     save_vecnormalize=True,
    # )
    # model.learn(total_timesteps=100000, progress_bar=True, callback=checkpoint_callback)


    for name in os.listdir("../../models/mediapipe/V3_3C_10S_13L_A2C_100000_VIDEO"):

        env = Env(width=500, height=500, cam_count=3, mode="test", history_count=10)
        obs, inf = env.reset()

        model = A2C("MlpPolicy", env, verbose=1)

        # vis = o3d.visualization.Visualizer()
        # vis.create_window(window_name="Estimate", width=400, height=300)
        # vis1 = o3d.visualization.Visualizer()
        # vis1.create_window(window_name="Truth", width=400, height=300)

        print(f"../models/V3_3C_10S_13L_A2C_100000_VIDEO/" + name[:-4])
        model.load(f"../models/V3_3C_10S_13L_A2C_100000_VIDEO/" + name[:-4])

        rewards = []
        for i in range(1000):
            for id in range(env.cam_count):
                image, skeletons = env.client.get_latest_data(id)
                if image is not None and skeletons is not None:
                    cv2.imshow('Image' + str(id), image)
                    cv2.waitKey(1)
            action, _ = model.predict(obs, deterministic=True)
            obs, reward, terminated, truncated, info = env.step(action)
            rewards.append(reward)
            print(f"Step: {i}, Reward: {reward}, Action: {action}")

            # draw_skeleton_open3d(env.current_predicted_skeletons, vis)
            # draw_skeleton_open3d(env.current_skeletons[action], vis1)
        print(f"Sum of Rewards: {np.sum(rewards)}")
        print(f"Average Reward: {np.mean(rewards)}")
        print(f"Successful Predictions: {np.sum(np.array(rewards) > 0)}/{len(rewards)}")

        env.close()

    # env = Env(width=500, height=500, cam_count=3, mode="test", history_count=10)
    # obs, inf = env.reset()
    #
    # model = A2C("MlpPolicy", env, verbose=1)
    #
    # # vis = o3d.visualization.Visualizer()
    # # vis.create_window(window_name="Estimate", width=400, height=300)
    # # vis1 = o3d.visualization.Visualizer()
    # # vis1.create_window(window_name="Truth", width=400, height=300)
    #
    # model.load("../models/V3_3C_10S_13L_A2C_100000_VIDEO/A2C_model_90000_steps")
    #
    # rewards = []
    # for i in range(10000):
    #     for id in range(env.cam_count):
    #         image, skeletons = env.client.get_latest_data(id)
    #         if image is not None and skeletons is not None:
    #             cv2.imshow('Image' + str(id), image)
    #             cv2.waitKey(1)
    #     action, _ = model.predict(obs, deterministic=True)
    #     obs, reward, terminated, truncated, info = env.step(action)
    #     rewards.append(reward)
    #     print(f"Step: {i}, Reward: {reward}, Action: {action}")
    #
    #     # draw_skeleton_open3d(env.current_predicted_skeletons, vis)
    #     # draw_skeleton_open3d(env.current_skeletons[action], vis1)
    # print(f"Sum of Rewards: {np.sum(rewards)}")
    # print(f"Average Reward: {np.mean(rewards)}")
    # print(f"Successful Predictions: {np.sum(np.array(rewards) > 0)}/{len(rewards)}")
    #
    # env.close()