#pragma once

#include <map>
#include <string>
#include <vector>

#include <compiler/onnx.h>

#include <compiler/node.h>
#include <compiler/tensor.h>
#include <compiler/type.h>
#include <compiler/value.h>

namespace oniku {

class Type;

class Graph {
public:
    explicit Graph(const onnx::GraphProto& xgraph);
    explicit Graph(const std::string name);
    ~Graph();

    Graph(const Graph&) = delete;
    Graph& operator=(const Graph&) = delete;

    void ToONNX(onnx::GraphProto* xgraph, bool serialize_initializers = true) const;
    std::string DebugString() const;

    const std::vector<Value*>& input_values() const {
        return input_values_;
    }
    const std::vector<Value*>& output_values() const {
        return output_values_;
    }
    const std::vector<Value*>& temp_values() const {
        return temp_values_;
    }
    const std::vector<std::unique_ptr<Value>>& all_values() const {
        return all_values_;
    }
    const std::vector<Node*>& nodes() const {
        return nodes_;
    }

    std::vector<Node*> GetLiveNodes() const;

    // All values which is required to produce `output_values`.
    std::set<Value*> GetNecessaryValues(const std::vector<Value*>& output_values) const;
    // All values which is required to produce `output_values_`.
    std::set<Value*> GetNecessaryValues() const;

    const std::string& name() const {
        return name_;
    }
    const std::string& doc_string() const {
        return doc_string_;
    }

    Value* AddValue(const std::string& name, Value::Kind kind = Value::Kind::kTemp);
    Value* AddValue(const std::string& name, const Type& type, Value::Kind kind = Value::Kind::kTemp);
    Value* AddInputValue(const std::string& name, const Type& type);
    Value* AddOutputValue(const std::string& name, const Type& type);
    Value* AddNullValue();

    template <class T>
    Value* AddConstValue(const std::string& name, const Type& type, const std::vector<T>& data) {
        Value* value = AddInputValue(name, type);
        Tensor* t = new Tensor(value->name(), type.dtype(), type.dims(), data);
        value->ResetInitializer(std::unique_ptr<Tensor>(t));
        return value;
    }
    template <class T>
    Value* AddConstValue(const std::string& name, const Type& type, const std::initializer_list<T>& data) {
        return AddConstValue(name, type, std::vector<T>{data});
    }

    Node* AddNode(
            Node::OpType op_type, const std::vector<Value*>& inputs, const std::vector<Value*>& outputs, const std::string& base = "");

    void DetachNode(Node* node);

    std::vector<Node*> GetTopologicallySortedNodes() const;

    // Returns a map from nodes to the number of their users.
    std::map<Node*, int> GetNecessaryNodesAndInputCounts(const std::vector<Value*>& output_values) const;

    // Gets a sequence of scheduled nodes. Node::order() must be set
    // before calling this function.
    std::vector<const Node*> GetComputationSequence() const;

    std::vector<Value*>* mutable_input_values() {
        return &input_values_;
    }
    std::vector<Value*>* mutable_output_values() {
        return &output_values_;
    }

    void MigrateNodes(const std::vector<Node*>& nodes, const std::vector<Value*> temps, Graph* to);

    void InferShapes();

    void ResetGradients();

    void DeleteDetached();

    void CheckSanity(const std::string& msg) const;

    void DumpSubGraphs(int depth = 0) const;

    void DumpONNXOnFailure(const std::string& filename = "") const;

private:
    std::string GenSym(const std::string& base = "");
    std::string MakeUnique(const std::string& name);

    void AddNodeImpl(std::unique_ptr<Node> node, const std::vector<Value*>& inputs, const std::vector<Value*>& outputs);

    void Construct(const onnx::GraphProto& xgraph);

    std::vector<Value*> output_values_;
    std::vector<Value*> input_values_;
    std::vector<Value*> temp_values_;
    std::vector<std::unique_ptr<Value>> all_values_;
    std::vector<Node*> nodes_;
    std::vector<std::unique_ptr<Node>> nodes_buf_;
    std::string name_;
    std::string doc_string_;

    // A monotonically increasing ID to generate unique symbols.
    std::map<std::string, int> ids_;
};

}  // namespace oniku
