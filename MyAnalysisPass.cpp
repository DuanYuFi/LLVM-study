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


using namespace llvm;

namespace {
  struct MyAnalysisPass : public FunctionPass {
    static char ID;
    MyAnalysisPass() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      errs() << "Analyzing function: " << F.getName() << '\n';
      // 这里添加你的分析逻辑
      return false;
    }
  };
}

// char MyAnalysisPass::ID = 0;
// static MyAnalysisPass l0_register_std_pass(
//     PassManagerBuilder::EP_EnabledOnOptLevel0,
//     [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM) {
//       PM.add(new MyAnalysisPass());
//     });

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "MyAnalysisPass", LLVM_VERSION_STRING,
    [](PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [](StringRef Name, FunctionPassManager &FPM, ArrayRef<PassBuilder::PipelineElement>) {
          if (Name == "myanalysis") {
            FPM.addPass(MyAnalysisPass());
            return true;
          }
          return false;
        }
      );
    }
  };
}