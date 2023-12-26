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

    bool runOnFunction(Function &M) override {
      errs() << "Analyzing function: " << M.getName() << '\n';
      // 这里添加你的分析逻辑
      return true;
    }
  };
}

char MyAnalysisPass::ID = 0;
static RegisterStandardPasses l0_register_std_pass(
    PassManagerBuilder::EP_EnabledOnOptLevel0,
    [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM) {
        PM.add(new MyAnalysisPass());
    }
);

static RegisterStandardPasses moe_register_std_pass(
    PassManagerBuilder::EP_ModuleOptimizerEarly,
    [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM) {
        PM.add(new MyAnalysisPass());
    }
);
