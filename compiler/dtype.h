#pragma once

#include <iosfwd>

#include <compiler/onnx.h>

namespace oniku {

class Dtype {
public:
    // These values must be synchronized with xChainer's.
    enum DataType {
        kUnknown = 0,
        kBool = 1,
        kInt8,
        kInt16,
        kInt32,
        kInt64,
        kUInt8,
        kFloat32,
        kFloat64,
    };

    Dtype() = default;
    explicit Dtype(const onnx::TensorProto::DataType& xtype);
    // Note this is an implicit constructor.
    Dtype(DataType type);

    operator DataType() const {
        return type_;
    }

    onnx::TensorProto::DataType ToONNX() const;
    std::string ToString() const;

    int SizeOf() const;

    bool IsFloat() const {
        return type_ == kFloat32 || type_ == kFloat64;
    }

private:
    DataType type_ = kUnknown;
};

std::ostream& operator<<(std::ostream& os, const Dtype& dtype);

}  // namespace oniku
