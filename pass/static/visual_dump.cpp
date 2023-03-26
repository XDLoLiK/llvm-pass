/* IR */
#include <llvm-14/llvm/IR/Instruction.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>

/* Common */
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>

/* Dot */
#include "dot_builder.hpp"

namespace {

class GraphvizPass : public llvm::FunctionPass {
public:
    GraphvizPass()
        : FunctionPass(id)
        , dot_builder_("dump.dot") {

        dot_builder_.BeginGraph("G");
    }

    ~GraphvizPass() {
        dot_builder_.EndGraph();
    }

    virtual bool runOnFunction(llvm::Function& func) {
        if (func.hasName()) {
            StaticDump(func);
            // DynamicDump(func);
        }
        return true;
    }

private:
    void StaticDump(llvm::Function& func) {
        // func.print(llvm::outs());
        // return;

        dot_builder_.BeginSubgraph(func.getName().str());
        /* Dump function's users */
        for (auto user : func.users()) {
            auto fnc = std::to_string(reinterpret_cast<uint64_t>(&func));
            auto usr = std::to_string(reinterpret_cast<uint64_t>(user));
            dot_builder_.CreateEdge(fnc, usr, EdgeType::ClusterToCluster);
        }

        for (auto& block : func) {
            for (auto& instruction : block) {
                /* Dump current instruction */
                std::string name{instruction.getOpcodeName()};
                auto instr = std::to_string(reinterpret_cast<uint64_t>(&instruction));
                dot_builder_.CreateNode(instr);
                dot_builder_.AddLabel("label=\"" + name + "\"");
                /* Dump instruction's uses */
                for (auto user : instruction.users()) {
                    auto usr = std::to_string(reinterpret_cast<uint64_t>(user));
                    dot_builder_.CreateEdge(instr, usr, EdgeType::NodeToNode);
                }
            }
        }
        dot_builder_.EndSubgraph();
    }

    void DynamicDump(llvm::Function& func) {
        // Prepare builder for IR modification
        llvm::LLVMContext& Ctx = func.getContext();
        llvm::IRBuilder<> builder(Ctx);
        llvm::Type* retType = llvm::Type::getVoidTy(Ctx);

        // Prepare funcStartLogger function
        std::vector<llvm::Type*> funcStartParamTypes = {
            builder.getInt8Ty()->getPointerTo(),
            builder.getInt8Ty()->getPointerTo(), llvm::Type::getInt32Ty(Ctx)};
        llvm::FunctionType* funcStartLogFuncType =
            llvm::FunctionType::get(retType, funcStartParamTypes, false);
        llvm::FunctionCallee funcStartLogFunc = func.getParent()->getOrInsertFunction(
            "funcStartLogger", funcStartLogFuncType);

        // Insert a call to funcStartLogger function in the function begin
        llvm::BasicBlock& entryBB = func.getEntryBlock();
        builder.SetInsertPoint(&entryBB.front());
        llvm::Value* funcName = builder.CreateGlobalStringPtr(func.getName());
        llvm::Value* args[] = {funcName};
        builder.CreateCall(funcStartLogFunc, args);

        // Prepare binOptLogger function
        std::vector<llvm::Type*> binOptParamTypes = {llvm::Type::getInt32Ty(Ctx),
                                                     llvm::Type::getInt32Ty(Ctx),
                                                     llvm::Type::getInt32Ty(Ctx),
                                                     builder.getInt8Ty()->getPointerTo(),
                                                     builder.getInt8Ty()->getPointerTo(),
                                                     llvm::Type::getInt32Ty(Ctx)};
        llvm::FunctionType* binOptLogFuncType =
            llvm::FunctionType::get(retType, binOptParamTypes, false);
        llvm::FunctionCallee binOptLogFunc =
            func.getParent()->getOrInsertFunction("binOptLogger", binOptLogFuncType);

        // Prepare callLogger function
        std::vector<llvm::Type*> callParamTypes = {builder.getInt8Ty()->getPointerTo(),
                                                   builder.getInt8Ty()->getPointerTo(),
                                                   llvm::Type::getInt32Ty(Ctx)};
        llvm::FunctionType* callLogFuncType =
            llvm::FunctionType::get(retType, callParamTypes, false);
        llvm::FunctionCallee callLogFunc =
            func.getParent()->getOrInsertFunction("callLogger", callLogFuncType);

        // Prepare funcEndLogger function
        std::vector<llvm::Type*> funcEndParamTypes = {
            builder.getInt8Ty()->getPointerTo(), llvm::Type::getInt32Ty(Ctx)};
        llvm::FunctionType* funcEndLogFuncType =
            llvm::FunctionType::get(retType, funcEndParamTypes, false);
        llvm::FunctionCallee funcEndLogFunc =
            func.getParent()->getOrInsertFunction("funcEndLogger", funcEndLogFuncType);

        // Insert loggers for call, binOpt and ret instructions
        for (auto& B : func) {
            for (auto& I : B) {
                llvm::Value* valueAddr =
                    llvm::ConstantInt::get(builder.getInt64Ty(), (int64_t)(&I));
                if (auto* op = llvm::dyn_cast<llvm::BinaryOperator>(&I)) {
                    // Insert after op
                    builder.SetInsertPoint(op);
                    builder.SetInsertPoint(&B, ++builder.GetInsertPoint());

                    // Insert a call to binOptLogFunc function
                    llvm::Value* lhs = op->getOperand(0);
                    llvm::Value* rhs = op->getOperand(1);
                    llvm::Value* funcName = builder.CreateGlobalStringPtr(func.getName());
                    llvm::Value* opName = builder.CreateGlobalStringPtr(op->getOpcodeName());
                    llvm::Value* args[] = {op, lhs, rhs, opName, funcName, valueAddr};
                    builder.CreateCall(binOptLogFunc, args);
                }
                if (auto* call = llvm::dyn_cast<llvm::CallInst>(&I)) {
                    // Insert before call
                    builder.SetInsertPoint(call);

                    // Insert a call to callLogger function
                    llvm::Function* callee = call->getCalledFunction();
                    if (callee) {
                        llvm::Value* calleeName =
                            builder.CreateGlobalStringPtr(callee->getName());
                        llvm::Value* funcName = builder.CreateGlobalStringPtr(func.getName());
                        llvm::Value* args[] = {funcName, calleeName, valueAddr};
                        builder.CreateCall(callLogFunc, args);
                    }
                }
                if (auto* ret = llvm::dyn_cast<llvm::ReturnInst>(&I)) {
                    // Insert before ret
                    builder.SetInsertPoint(ret);

                    // Insert a call to funcEndLogFunc function
                    llvm::Value* funcName = builder.CreateGlobalStringPtr(func.getName());
                    llvm::Value* args[] = {funcName, valueAddr};
                    builder.CreateCall(funcEndLogFunc, args);
                }
            }
        }
    }

private:
    static char id;
    DotBuilder dot_builder_;
};

} /* namespace */

char GraphvizPass::id = 0;

/*
 * Automatically enable the pass.
 * http://adriansampson.net/blog/clangpass.html
*/
static void RegisterGraphvizPass(const llvm::PassManagerBuilder&, llvm::legacy::PassManagerBase& pass_manager) {
  pass_manager.add(new GraphvizPass());
}

static llvm::RegisterStandardPasses RegisterMyPass(llvm::PassManagerBuilder::EP_EarlyAsPossible, RegisterGraphvizPass);
