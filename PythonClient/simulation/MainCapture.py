import os

import cv2
import numpy as np

from PythonClient.simulation.threaded_tcp_client import ThreadedTCPClient


if __name__ == "__main__":
    cam_count = 3
    num_samples = 10000
    image_dir = "E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/simulation/yolo/train_data_1people/images"
    skeleton_dir = "E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/simulation/yolo/train_data_1people/skeletons"

    client = ThreadedTCPClient(cam_count=cam_count)
    client.start()

    try:
        sample = 0
        while sample < num_samples:
            for cam_id in range(cam_count):
                while True:
                    image, skeletons = client.get_latest_data(cam_id)
                    if image is not None and skeletons is not None:
                        cv2.imshow('Image', image)
                        cv2.waitKey(1)
                        img_path = os.path.join(image_dir + f"_cam{cam_id}", f"image_{sample}.png")
                        skel_path = os.path.join(skeleton_dir + f"_cam{cam_id}", f"skeleton_{sample}.npy")
                        cv2.imwrite(img_path, image)
                        np.save(skel_path, skeletons)
                        break
            print(sample)
            sample += 1
    finally:
        client.stop()