#pragma once

#include <onnx/onnx_pb.h>

#include <chainerx/dtype.h>

#include <runtime/xcvm.h>

namespace oniku {

class Model;

namespace runtime {

void MakeHumanReadableValue(onnx::TensorProto* tensor);

void StripLargeValue(onnx::TensorProto* tensor, int num_elements);

chainerx::Dtype XChainerTypeFromONNX(onnx::TensorProto::DataType xtype);

InOuts LoadParams(const Model& model);

void StripONNXModel(onnx::ModelProto* model);

}  // namespace runtime
}  // namespace oniku
