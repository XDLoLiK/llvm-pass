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
    using Edge = std::pair<std::string, std::string>;

public:
    GraphvizPass()
        : FunctionPass(id)
        , dot_builder_("dump.dot") {

        dot_builder_.BeginGraph("G");
        dot_builder_.AddAttribute("rankdir=\"TB\"", AttributeType::Graph);
        dot_builder_.AddAttribute("shape=rect", AttributeType::Node);
    }

    ~GraphvizPass() {
        for (auto& edge : edges_to_dump_) {
            dot_builder_.CreateEdge(edge.first, edge.second, EdgeType::NodeToCluster);
            dot_builder_.AddLabel("color=orange");
        }
        dot_builder_.EndGraph();
    }

    virtual bool runOnFunction(llvm::Function& func) {
        if (func.hasName()) {
            StaticDump(func);
            DynamicDump(func);
        }
        return true;
    }

private:
    void StaticDump(llvm::Function& func) {
        /* Function's address serves us as a unique identifier */
        auto func_addr = std::to_string(reinterpret_cast<uint64_t>(&func));
        dot_builder_.BeginSubgraph(func_addr);
        dot_builder_.AddAttribute("rankdir=\"TB\"", AttributeType::Graph);
        dot_builder_.AddAttribute("label=\"" + func.getName().str() + "\"", AttributeType::Graph);

        /* Save function's users for the later dump */
        for (auto user : func.users()) {
            auto user_addr = std::to_string(reinterpret_cast<uint64_t>(user));
            edges_to_dump_.emplace_back(user_addr, func_addr);
        }

        llvm::Instruction* prev_instruction = nullptr;
        for (auto& block : func) {
            for (auto& instruction : block) {
                /* Dump current instruction */
                std::string instruction_str;
                llvm::raw_string_ostream instruction_stream{instruction_str};
                instruction.print(instruction_stream);

                /* Instruction's address serves us as a unique identifier */
                auto instrunction_addr = std::to_string(reinterpret_cast<uint64_t>(&instruction));
                dot_builder_.CreateNode(instrunction_addr);
                dot_builder_.AddLabel("label=\"" + instruction_str + "\"");

                /* Dump instruction's uses */
                for (auto user : instruction.users()) {
                    auto user_addr = std::to_string(reinterpret_cast<uint64_t>(user));
                    dot_builder_.CreateEdge(user_addr, instrunction_addr, EdgeType::NodeToNode);
                    dot_builder_.AddLabel("color=red");
                }

                /* Logical order */
                if (prev_instruction != nullptr) {
                    auto prev_addr = std::to_string(reinterpret_cast<uint64_t>(prev_instruction));
                    dot_builder_.CreateEdge(prev_addr, instrunction_addr, EdgeType::NodeToNode);
                    dot_builder_.AddLabel("color=green");
                }

                /* Update prev */
                prev_instruction = &instruction;
            }
        }

        dot_builder_.EndSubgraph();
    }

    void DynamicDump(llvm::Function& func) {
        /* Prepare builder for IR modification */
        llvm::LLVMContext& context = func.getContext();
        llvm::IRBuilder<> builder{context};
        llvm::Type* ret_type = llvm::Type::getVoidTy(context);

        /* Prepare FuncStartLogger function */
        std::vector<llvm::Type*> logger_start_param_types = {
            builder.getInt8Ty()->getPointerTo(),
            builder.getInt8Ty()->getPointerTo(),
            llvm::Type::getInt32Ty(context)
        };

        llvm::FunctionType* logger_start_func_type =
            llvm::FunctionType::get(ret_type, logger_start_param_types, false);
        llvm::FunctionCallee logger_start_callee =
            func.getParent()->getOrInsertFunction("FuncStartLogger", logger_start_func_type);

        /* Insert a call to FuncStartLogger function in the function begin */
        llvm::BasicBlock& entry_basic_block = func.getEntryBlock();
        builder.SetInsertPoint(&entry_basic_block.front());
        llvm::Value* func_name = builder.CreateGlobalStringPtr(func.getName());
        llvm::Value* args[] = {func_name};
        builder.CreateCall(logger_start_callee, args);

        /* Prepare BinOptLogger function */
        std::vector<llvm::Type*> logger_bin_opt_param_types = {
            llvm::Type::getInt32Ty(context),
            llvm::Type::getInt32Ty(context),
            llvm::Type::getInt32Ty(context),
            builder.getInt8Ty()->getPointerTo(),
            builder.getInt8Ty()->getPointerTo(),
            llvm::Type::getInt32Ty(context)
        };
        
        llvm::FunctionType* logger_bin_opt_func_type =
            llvm::FunctionType::get(ret_type, logger_bin_opt_param_types, false);
        llvm::FunctionCallee logger_bin_opt_callee =
            func.getParent()->getOrInsertFunction("BinOptLogger", logger_bin_opt_func_type);

        /* Prepare CallLogger function */
        std::vector<llvm::Type*> logger_call_param_types = {
            builder.getInt8Ty()->getPointerTo(),
            builder.getInt8Ty()->getPointerTo(),
            llvm::Type::getInt32Ty(context)
        };
        
        llvm::FunctionType* logger_call_func_type =
            llvm::FunctionType::get(ret_type, logger_call_param_types, false);
        llvm::FunctionCallee logger_call_callee =
            func.getParent()->getOrInsertFunction("CallLogger", logger_call_func_type);

        /* Prepare FuncEndLogger function */
        std::vector<llvm::Type*> logger_end_param_types = {
            builder.getInt8Ty()->getPointerTo(),
            llvm::Type::getInt32Ty(context)
        };
        
        llvm::FunctionType* logger_end_func_type =
            llvm::FunctionType::get(ret_type, logger_end_param_types, false);
        llvm::FunctionCallee logger_end_callee =
            func.getParent()->getOrInsertFunction("FuncEndLogger", logger_end_func_type);

        /* Insert loggers for call, binOpt and ret instructions */
        for (auto& block : func) {
            for (auto& instruction : block) {
                llvm::Value* value_addr = llvm::ConstantInt::get(builder.getInt64Ty(), (int64_t)(&instruction));
                if (auto* op = llvm::dyn_cast<llvm::BinaryOperator>(&instruction)) {
                    /* Insert after op */
                    builder.SetInsertPoint(op);
                    builder.SetInsertPoint(&block, ++builder.GetInsertPoint());

                    /* Insert a call to binOptLogFunc function */
                    llvm::Value* lhs = op->getOperand(0);
                    llvm::Value* rhs = op->getOperand(1);
                    llvm::Value* func_name = builder.CreateGlobalStringPtr(func.getName());
                    llvm::Value* op_name = builder.CreateGlobalStringPtr(op->getOpcodeName());
                    llvm::Value* args[] = {op, lhs, rhs, op_name, func_name, value_addr};
                    builder.CreateCall(logger_bin_opt_callee, args);
                }
                if (auto* call = llvm::dyn_cast<llvm::CallInst>(&instruction)) {
                    /* Insert before call */
                    builder.SetInsertPoint(call);

                    /* Insert a call to CallLogger function */
                    llvm::Function* callee = call->getCalledFunction();
                    if (callee) {
                        llvm::Value* callee_name = builder.CreateGlobalStringPtr(callee->getName());
                        llvm::Value* func_name = builder.CreateGlobalStringPtr(func.getName());
                        llvm::Value* args[] = {func_name, callee_name, value_addr};
                        builder.CreateCall(logger_call_callee, args);
                    }
                }
                if (auto* ret = llvm::dyn_cast<llvm::ReturnInst>(&instruction)) {
                    /* Insert before ret */
                    builder.SetInsertPoint(ret);

                    /* Insert a call to funcEndLogFunc function */
                    llvm::Value* func_name = builder.CreateGlobalStringPtr(func.getName());
                    llvm::Value* args[] = {func_name, value_addr};
                    builder.CreateCall(logger_end_callee, args);
                }
            }
        }
    }

private:
    static char id;
    DotBuilder dot_builder_;
    std::vector<Edge> edges_to_dump_;
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
