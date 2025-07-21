# chumpy/ch.py
# want_out = 'out' in inspect.getargspec(func).args
# ->
# want_out = 'out' in inspect.signature(func).parameters

# chumpy/__innit__.py (only in test project)
# from numpy import bool, int, float, complex, object, unicode, str, nan, inf
# ->
# from numpy import bool, float16, complex64, object_, nan, inf

# romp/main.py
# providers=['TensorrtExecutionProvider', 'CUDAExecutionProvider', 'CPUExecutionProvider'])
# ->
# providers = ['DmlExecutionProvider', 'TensorrtExecutionProvider', 'CUDAExecutionProvider', 'CPUExecutionProvider'])
# romp.prepare_smpl -source_dir='E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/pose_estimation/bev/smpl_model_data'
# bev.prepare_smil -source_dir='E:/Vag/Programs/UnrealEngine/UE 5.5/DemoSimulationUE5/PythonClient/pose_estimation/bev/smpl_model_data'


import torch
import cv2
from bev.model import BEVv1
from my_romp.utils import img_preprocess

# # ---- Load model ----
# model = BEVv1()
# state_dict = torch.load('C:/Users/vag03/.romp/BEV.pth', map_location='cpu')
# model.load_state_dict(state_dict, strict=False)
# model.eval()
#
# # ---- Image preprocessing ----
# image = cv2.imread('E:/Vag/Programs/Pycharm/Test/smpl_model_data/Solvay_conference_1927_all_people.jpg')
# image = cv2.resize(cv2.cvtColor(image, cv2.COLOR_BGR2RGB), (512,512))
#
# input_tensor, image_pad_info = img_preprocess(image)
#
# # ---- Define ONNX wrapper ----
# class BEVv1ONNXWrapper(torch.nn.Module):
#     def __init__(self, bev_model):
#         super().__init__()
#         self.bev_model = bev_model
#
#     def forward(self, x):
#         while True:
#             output = self.bev_model(x)
#             if output is not None:
#                 # Select output tensors you want exported
#                 return output['center_map'], output['params_pred']
#                 break
#
# onnx_model = BEVv1ONNXWrapper(model)
#
# # ---- Test output shape ----
# output = onnx_model(input_tensor)
# print(type(output))
# print([o.shape for o in output])
#
# # ---- Export to ONNX ----
# save_file = 'E:/Vag/Programs/Pycharm/Test/BEV.onnx'
# torch.onnx.export(
#     onnx_model,
#     input_tensor,
#     save_file,
#     input_names=['image'],
#     output_names=['center_map', 'params_pred'],
#     export_params=True,
#     opset_version=12,
#     do_constant_folding=True
# )
# print('BEV onnx saved into: ', save_file)


import onnx
# onnx_model1 = onnx.load('C:/Users/vag03/.romp/BEV.onnx')
# for node in onnx_model1.graph.node:
#     if node.op_type == "MaxPool":
#         for attr in node.attribute:
#             if attr.name == "dilations":
                # print(attr.ints)

class BEVv1ONNXWrapper(torch.nn.Module):
    def __init__(self, bev_model):
        super().__init__()
        self.bev_model = bev_model

    def forward(self, x):
        output = self.bev_model(x)
        # print(output)
        return (output['params_pred'], output['cam_czyx'], output['center_map'], output['center_map_3d'], output['pred_batch_ids'], output['pred_czyxs'], output['center_confs'])

model = BEVv1().to('cpu')
state_dict = torch.load('C:/Users/vag03/.romp/BEV.pth', map_location=torch.device('cpu'))
model.load_state_dict(state_dict, strict=False)
model.eval()
save_file = 'C:/Users/vag03/.romp/BEV.onnx'

image = cv2.imread('E:/Vag/Programs/Pycharm/Test/smpl_model_data/Solvay_conference_1927_all_people.jpg')
image = cv2.resize(cv2.cvtColor(image, cv2.COLOR_BGR2RGB), (512, 512))
image, image_pad_info = img_preprocess(image)
# image = torch.from_numpy(image).unsqueeze(0)
print(image.shape)

onnx_model = BEVv1ONNXWrapper(model)

torch.onnx.export(onnx_model, (image),
                  save_file,
                  input_names=['image'],
                  output_names=['params_pred', 'cam_czyx', 'center_map', 'center_map_3d', 'pred_batch_ids', 'pred_czyxs', 'center_confs'],
                  export_params=True,
                  opset_version=20,
                  do_constant_folding=True)
print('BEV onnx saved into: ', save_file)


import onnx
model = onnx.load(save_file)
graph = model.graph

for node in graph.node:
    if node.op_type == "MaxPool":
        for attr in node.attribute:
            if attr.name == "dilations":
                # if its length != len(kernel_shape), drop it
                ks = next(a for a in node.attribute if a.name == "kernel_shape").ints
                if len(attr.ints) != len(ks):
                    node.attribute.remove(attr)
                break

onnx.save(model, save_file)

