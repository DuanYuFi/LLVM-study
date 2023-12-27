// #include "llvm/IR/Function.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
// #include "llvm/Support/raw_ostream.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

// include PassManagerBuilder related
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/LegacyPassManager.h"

#include <cxxabi.h>
#include <iostream>
#include <memory>


using namespace llvm;

std::string demangle(const char* mangledName) {
    int status = 0;
    std::unique_ptr<char, void (*)(void*)> result(
        abi::__cxa_demangle(mangledName, NULL, NULL, &status),
        std::free);
    return (status == 0) ? result.get() : mangledName;
}

std::string demangle(StringRef mangledName) {
  return demangle(mangledName.str().c_str());
}

namespace {
  struct MyAnalysisPass : public FunctionPass {
    static char ID;
    MyAnalysisPass() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      errs() << "Analyzing function: " << demangle(F.getName()) << '\n';
      // 这里添加你的分析逻辑
      // for (auto& arg: F.args()) {
      //   errs() << "arg: " << arg << '\n';
      // }
      return true;
    }
  };

  struct ModuleAnalysis : public ModulePass {
    static char ID;
    ModuleAnalysis() : ModulePass(ID) {}

    bool runOnModule(Module &M) override {

      errs() << "In ModuleAnalysis" << '\n';

      for (auto& F: M.functions()) {
        errs() << "Function: " << demangle(F.getName()) << '\n';
      }

      return true;
    }
  };
}

char MyAnalysisPass::ID = 0;
char ModuleAnalysis::ID = 1;

static RegisterStandardPasses l0_register_std_pass(
    PassManagerBuilder::EP_EnabledOnOptLevel0,
    [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM) {
        PM.add(new MyAnalysisPass());
        PM.add(new ModuleAnalysis());
    }
);

static RegisterStandardPasses moe_register_std_pass(
    PassManagerBuilder::EP_ModuleOptimizerEarly,
    [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM) {
        PM.add(new MyAnalysisPass());
        PM.add(new ModuleAnalysis());
    }
);
