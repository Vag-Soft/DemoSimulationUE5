import numpy as np
import onnxruntime as ort
import cv2
from typing import List, Dict, Tuple

from PythonClient.simulation.threaded_tcp_client import ThreadedTCPClient


class ResNet50FeatureExtractor:
    def __init__(self, model_path="E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/feature_extraction/resnet50/resnet50.onnx", model_input_size=(224, 224), original_input_size=(500, 500)):
        # Utilizes AMD gpu through DirectML
        providers = ['DmlExecutionProvider']
        self.session = ort.InferenceSession(model_path, providers=providers)

        self.input_name = self.session.get_inputs()[0].name
        self.output_layer = self.session.get_outputs()[0].name

        self.model_input_size = model_input_size
        self.original_input_size = original_input_size

    def preprocess_batch_images(self, images):
        resized_images = np.array([cv2.resize(img, self.model_input_size) for img in images])

        # TODO: float16 may be more efficient but requires model modifications
        normalized_images = resized_images.astype(np.float32) / 255.0

        # TODO: Try without
        mean = np.array([0.485, 0.456, 0.406]).reshape(1, 1, 1, 3)
        std = np.array([0.229, 0.224, 0.225]).reshape(1, 1, 1, 3)
        normalized_images = (normalized_images - mean) / std

        processed_images = np.transpose(normalized_images, (0, 3, 1, 2))

        return processed_images

    def extract_features(self, batch_images):

        processed_images = self.preprocess_batch_images(batch_images)
        if processed_images.dtype != np.float32:
            processed_images = processed_images.astype(np.float32)

        inputs = {self.input_name: processed_images}

        outputs = self.session.run([self.output_layer], inputs)
        features = outputs[0]

        return features.squeeze()

#
# if __name__ == '__main__':
#     client = ThreadedTCPClient(cam_count=3)
#     client.start()
#
#     camera = 0
#     while True:
#         image, skeletons = client.get_latest_data(camera)
#         if skeletons is not None:
#             break
#
#     batch = np.array([image,image,image,image,image])
#
#     extractor = ResNet50FeatureExtractor()
#     features = extractor.extract_features(batch)