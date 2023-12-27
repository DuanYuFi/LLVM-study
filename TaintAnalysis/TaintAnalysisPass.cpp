#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
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
  struct TaintAnalysisPass : public FunctionPass {
    static char ID;
    TaintAnalysisPass() : FunctionPass(ID) {}

    // Set to store taint sources (e.g., function arguments)
    std::set<Value*> taintSources;

    // Check if a value is tainted
    bool isTainted(Value *V) {
      return taintSources.find(V) != taintSources.end();
    }

    // Mark a value as tainted
    void addTaintSource(Value *V) {
      taintSources.insert(V);
    }

    // Main analysis method
    bool runOnFunction(Function &F) override {
      // Identify taint sources, e.g., function arguments
      for (auto &Arg : F.args()) {
        outs() << "Add taint to " << Arg << '\n';
        addTaintSource(&Arg);
      }

      errs() << "In Function: " << demangle(F.getName()) << '\n';

      // Iterate over all instructions
      for (auto &BB : F) {
        for (auto &I : BB) {
          // Propagate taint
            // errs() << I << '\n';
          if (auto *storeInst = dyn_cast<StoreInst>(&I)) {
            
            Value *lhs = storeInst->getValueOperand();
            Value *rhs = storeInst->getPointerOperand();
            if (isTainted(lhs)) {
                errs() << "Found propagation: " << *lhs << " -> " << *rhs << '\n';
                addTaintSource(rhs);
            }
          }
          else if (auto *loadInst = dyn_cast<LoadInst>(&I)) {
            Value *lhs = loadInst;
            Value *rhs = loadInst->getPointerOperand();
            if (isTainted(rhs)) {
                errs() << "Found propagation: " << *rhs << " -> " << *lhs << '\n';
                addTaintSource(lhs);
            }
          }
        }
      }

      // Further analysis can be done here to check for tainted data usage

      return false;
    }
  };
}

char TaintAnalysisPass::ID = 0;

static RegisterStandardPasses l0_register_std_pass(
    PassManagerBuilder::EP_EnabledOnOptLevel0,
    [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM) {
        PM.add(new TaintAnalysisPass());
    }
);

static RegisterStandardPasses moe_register_std_pass(
    PassManagerBuilder::EP_ModuleOptimizerEarly,
    [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM) {
        PM.add(new TaintAnalysisPass());
    }
);
