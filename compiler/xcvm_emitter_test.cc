#include <iostream>
#include <sstream>

#include <gtest/gtest.h>

#include <compiler/onnx.h>

#include <common/log.h>
#include <common/protoutil.h>
#include <compiler/model.h>
#include <compiler/passes.h>
#include <compiler/xcvm_emitter.h>
#include <runtime/xcvm.pb.h>

namespace oniku {
namespace {

const char* kONNXTestDataDir = "onnx/onnx/backend/test/data";

TEST(XCVMTest, Compile) {
    std::string test_path = std::string(kONNXTestDataDir) + "/node/test_add/";
    std::string model_path = test_path + "model.onnx";
    onnx::ModelProto xmodel(LoadLargeProto<onnx::ModelProto>(model_path));
    Model model(xmodel);
    RunDefaultPasses(&model);

    std::ostringstream oss;
    xcvm::Emit(model, oss);

    runtime::XCProgramProto program;
    program.ParseFromString(oss.str());
    // std::cerr << program.DebugString() << std::endl;

    ASSERT_EQ(7, program.instructions_size());
    ASSERT_EQ(runtime::XCInstructionProto::In, program.instructions(0).op());
    ASSERT_EQ(runtime::XCInstructionProto::In, program.instructions(1).op());
    ASSERT_EQ(runtime::XCInstructionProto::Add, program.instructions(2).op());
    ASSERT_EQ(runtime::XCInstructionProto::Free, program.instructions(3).op());
    ASSERT_EQ(runtime::XCInstructionProto::Free, program.instructions(4).op());
    ASSERT_EQ(runtime::XCInstructionProto::Out, program.instructions(5).op());
    ASSERT_EQ(runtime::XCInstructionProto::Free, program.instructions(6).op());
}

}  // namespace
}  // namespace oniku
