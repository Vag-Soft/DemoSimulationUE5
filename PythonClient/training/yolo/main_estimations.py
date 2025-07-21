import os
import random
import time

import cv2
import matplotlib.pyplot as plt
import numpy as np
import torch
from stable_baselines3 import A2C, PPO
from stable_baselines3.common.callbacks import CheckpointCallback

from PythonClient.envs.yolo.env_estimations_v2 import Env
from PythonClient.simulation.fake_threaded_tcp_client import FakeThreadedTCPClient


def set_seed(seed=42):
    random.seed(seed)
    np.random.seed(seed)
    torch.manual_seed(seed)
    if torch.cuda.is_available():
        torch.cuda.manual_seed(seed)


# Apply seed before creating environment
set_seed(42)


if __name__ == "__main__":


    # env = Env(width=500, height=500, cam_count=3, max_skeletons=1, mode="train")
    # obs, info = env.reset()
    # while True:
    #     start = time.time()
    #     for id in range(1):
    #         image, skeletons = env.current_images[id], env.current_skeletons[id]
    #         if image is not None and skeletons is not None:
    #             cv2.imshow('Image' + str(id), image)
    #             cv2.waitKey(1)
    #     obs, reward, terminated, truncated, info = env.step(0)
    #     end = time.time()
    #     print(f"FPS: {1 / (end - start)}")
    #     print(f"Reward: {reward}, Info: {info}")






    # models_dir = f"E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/models/yolo/{int(time.time())}"
    # logdir = f"E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/logs/yolo/{int(time.time())}"
    #
    # if not os.path.exists(models_dir):
    #     os.makedirs(models_dir)
    #
    # if not os.path.exists(logdir):
    #     os.makedirs(logdir)
    #
    #
    # env = Env(width=500, height=500, cam_count=3, max_skeletons=1, mode="train")
    # obs, info = env.reset()
    # model = A2C("MlpPolicy", env, verbose=1, tensorboard_log=logdir)
    #
    # checkpoint_callback = CheckpointCallback(
    #     save_freq=1000,
    #     save_path=models_dir,
    #     name_prefix="A2C",
    #     save_replay_buffer=True,
    #     save_vecnormalize=True,
    # )
    # model.learn(total_timesteps=100000, progress_bar=True, callback=checkpoint_callback)











    client = FakeThreadedTCPClient(cam_count=3, mode="test")
    client.start()

    base_model_path = "E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/models/yolo/EstimationsV3_1P_3C_5S_A2C_100000"

    steps = 0
    sums, avgs, unsuccessful = [], [], []
    camera_0_actions, camera_1_actions, camera_2_actions = [], [], []
    while True:
        env = Env(width=500, height=500, cam_count=3, max_skeletons=1, fake_client=client, mode="test")
        obs, inf = env.reset()
        model = A2C("MultiInputPolicy", env, verbose=1)


        steps += 3000
        model_path = os.path.join(base_model_path, "A2C_" + str(steps) + "_steps.zip")
        if not os.path.exists(model_path):
            print(f"Model path {model_path} does not exist, skipping...")
            break
        print(f"Loading model from {model_path}")
        model.load(model_path)

        rewards, actions = [], []
        for i in range(2000):
            # for id in range(env.cam_count):
            #     image, skeletons = env.current_images[id], env.current_skeletons[id]
            #     if image is not None and skeletons is not None:
            #         cv2.imshow('Image' + str(id), image)
            #         cv2.waitKey(1)
            action, _ = model.predict(obs, deterministic=True)
            obs, reward, terminated, truncated, info = env.step(action)
            rewards.append(reward)
            actions.append(action)
            # print(f"Step: {i}, Reward: {reward}, Action: {action}")

        sums.append(np.sum(rewards))
        avgs.append(np.mean(rewards))
        unsuccessful.append(np.sum(np.array(rewards) < 0) / len(rewards))

        camera_0_actions.append(np.sum(np.array(actions) == 0) / len(actions))
        camera_1_actions.append(np.sum(np.array(actions) == 1) / len(actions))
        camera_2_actions.append(np.sum(np.array(actions) == 2) / len(actions))

        env.close()

    np.save(os.path.join(base_model_path, "sums.npy"), np.array(sums))
    np.save(os.path.join(base_model_path, "avgs.npy"), np.array(avgs))
    np.save(os.path.join(base_model_path, "unsuccessful.npy"), np.array(unsuccessful))
    np.save(os.path.join(base_model_path, "camera_0_actions.npy"), np.array(camera_0_actions))
    np.save(os.path.join(base_model_path, "camera_1_actions.npy"), np.array(camera_1_actions))
    np.save(os.path.join(base_model_path, "camera_2_actions.npy"), np.array(camera_2_actions))



    plt.plot(sums, label='Sum of Rewards')
    plt.xlabel('Steps in 10k')
    plt.ylabel('Sum')
    plt.legend()
    plt.show()
    plt.plot(avgs, label='Average Reward')
    plt.xlabel('Steps in 10k')
    plt.ylabel('Average')
    plt.legend()
    plt.show()
    plt.plot(unsuccessful, label='Unsuccessful Actions')
    plt.xlabel('Steps in 10k')
    plt.ylabel('Unsuccessful')
    plt.legend()
    plt.show()

    plt.plot(camera_0_actions, label='Camera 0 Actions')
    plt.plot(camera_1_actions, label='Camera 1 Actions')
    plt.plot(camera_2_actions, label='Camera 2 Actions')
    plt.xlabel('Steps in 10k')
    plt.ylabel('Action Percentage')
    plt.legend()
    plt.show()

    print(f"Best model with sum {np.max(sums)} at index {sums.index(np.max(sums))}")
    print(f"Best model with avg {np.max(avgs)} at index {avgs.index(np.max(avgs))}")
    print(f"Best model with unsuccessful {np.min(unsuccessful)} at index {unsuccessful.index(np.min(unsuccessful))}")