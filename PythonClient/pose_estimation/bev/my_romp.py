import numpy as np
import romp

class MyRomp():
    def __init__(self, settings=None):
        if settings is not None:
           self.settings = settings
        else:
            self.settings = romp.main.default_settings
            self.settings.onnx = True
            # self.settings.mode = 'video'
        self.model = romp.ROMP(self.settings)
        self.model.eval()

    def estimate(self, image):
        return self.model(image)

    def process_results(self, results):
        new_ids = {
            15: 0,
            17: 1,
            16: 2,
            19: 3,
            18: 4,
            21: 5,
            20: 6,
            2: 7,
            1: 8,
            5: 9,
            4: 10,
            8: 11,
            7: 12
        }

        joints = results['joints'][:, list(new_ids.keys())]
        cam_trans = results['cam_trans']

        for i in range(joints.shape[0]):
            joints[i] = joints[i] + cam_trans[i]

        joints[:, :, 1] = -joints[:, :, 1]
        joints[:, :,  2] = -joints[:, :, 2]

        # joints = joints - joints[0, 0]

        # hip_dist = np.linalg.norm(joints[0, 1] - joints[0, 3])
        # joints = joints / hip_dist if hip_dist > 0 else joints

        min_vals = np.min(joints, axis=(0, 1), keepdims=True)
        max_vals = np.max(joints, axis=(0, 1), keepdims=True)
        range_vals = max_vals - min_vals

        range_vals[range_vals == 0] = 1.0
        joints = (joints - min_vals) / range_vals

        return joints, results['center_confs']