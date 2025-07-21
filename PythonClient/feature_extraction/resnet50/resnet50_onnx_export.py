import torch
import torchvision.models as models


if __name__ == "__main__":
    output_path= "resnet50.onnx"
    model = models.resnet50(pretrained=True)

    feature_model = torch.nn.Sequential(*list(model.children())[:-1])
    feature_model.eval()

    dummy_input = torch.randn(1, 3, 224, 224)

    torch.onnx.export(
        feature_model,
        dummy_input,
        output_path,
        export_params=True,
        opset_version=12,
        do_constant_folding=True,
        input_names=["input"],
        output_names=["output"],
        dynamic_axes={"input": {0: "batch_size"}, "output": {0: "batch_size"}}
    )

    print(f"Model exported to {output_path}")