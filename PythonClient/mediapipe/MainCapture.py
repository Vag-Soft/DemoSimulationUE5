import os
from simulation.ThreadedTCPClient import ThreadedTCPClient


if __name__ == "__main__":
    cam_count = 3
    num_samples = 2000
    image_dir = "test_data/images"
    skeleton_dir = "test_data/skeletons"

    client = ThreadedTCPClient(cam_count=cam_count)
    client.start()

    try:
        sample = 0
        while sample < num_samples:
            for cam_id in range(cam_count):
                while True:
                    image, skeletons = client.get_latest_data(cam_id)
                    if image is not None and skeletons is not None:
                        img_path = os.path.join(image_dir + f"_cam{cam_id}", f"image_{sample}.png")
                        skel_path = os.path.join(skeleton_dir + f"_cam{cam_id}", f"skeleton_{sample}.npy")
                        # cv2.imwrite(img_path, image)
                        # np.save(skel_path, skeletons)
                        break
            sample += 1
    finally:
        client.stop()