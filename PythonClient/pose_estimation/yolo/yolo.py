import cv2
import numpy as np
import torch
from ultralytics import YOLO


# .venv/Lib/site-packages/ultralytics/nn/autobackend.py line 249
# providers = ["CPUExecutionProvider"] ->
# providers = ['DmlExecutionProvider', "CPUExecutionProvider"]

# Load a model
model = YOLO("yolo11x-pose.onnx")
# Predict with the model
image_path = "bus.jpg"
image = cv2.imread(image_path)

# Predict with the model
results = model(image)  # predict on an image

# Draw keypoints on the image
for result in results:
    xy = result.keypoints.xy  # x and y coordinates
    xyn = result.keypoints.xyn  # normalized
    kpts = result.keypoints.data  # x, y, visibility (if available)

    for p in xy:
        for l in p:
            x = int(l[0].item())
            y = int(l[1].item())
            cv2.circle(image, (x, y), 5, (255, 0, 0), -1)

# Display the image with keypoints
cv2.imshow("Pose Estimation", image)
cv2.waitKey(0)
cv2.destroyAllWindows()

# Optionally save the result
cv2.imwrite("pose_result1.jpg", image)


class YoloPoseEstimator:
    def __init__(self, model_path="yolo11x-pose.onnx"):
        self.model = YOLO(model_path)

    def estimate_pose(self, image):
        results = self.model(image)
        return results

    def draw_keypoints(self, image, results):
        for result in results:
            xy = result.keypoints.xy  # x and y coordinates
            for p in xy:
                for l in p:
                    x = int(l[0].item())
                    y = int(l[1].item())
                    cv2.circle(image, (x, y), 5, (255, 0, 0), -1)
        return image