import cv2
from ultralytics import YOLO


# .venv/Lib/site-packages/ultralytics/nn/autobackend.py line 249
# providers = ["CPUExecutionProvider"] ->
# providers = ['DmlExecutionProvider', "CPUExecutionProvider"]


class YoloPoseEstimator:
    def __init__(self, model_path="nano" , width=500, height=500):
        models = {
            "nano": "E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/pose_estimation/yolo/yolo11n-pose.onnx",
            "small": "E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/pose_estimation/yolo/yolo11s-pose.onnx",
            "medium": "E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/pose_estimation/yolo/yolo11m-pose.onnx",
            "large": "E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/pose_estimation/yolo/yolo11l-pose.onnx",
            "xlarge": "E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/pose_estimation/yolo/yolo11x-pose.onnx",
        }
        self.model = YOLO(models[model_path])
        self.width = width
        self.height = height

    def estimate_pose(self, frame):
        frame = cv2.resize(frame, (256, 256))
        results = self.model.predict(source=frame, show=False, verbose=False)
        if not results[0].keypoints or len(results) == 0:
            return None
        return results[0]

    def process_results(self, results):
        new_ids = {
            0: 0,
            6: 1,
            5: 2,
            8: 3,
            7: 4,
            10: 5,
            9: 6,
            12: 7,
            11: 8,
            14: 9,
            13: 10,
            16: 11,
            15: 12
        }

        keypoints = results.keypoints.numpy().xyn[:, list(new_ids.keys())]
        confidences = results.keypoints.numpy().conf[:, list(new_ids.keys())]

        return keypoints, confidences
