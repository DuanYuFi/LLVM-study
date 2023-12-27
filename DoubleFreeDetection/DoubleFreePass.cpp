#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/LegacyPassManager.h"

#include <iostream>
#include <set>
#include <memory>

using namespace llvm;

namespace {
  struct DoubleFreeDetectionPass : public FunctionPass {
    static char ID;
    DoubleFreeDetectionPass() : FunctionPass(ID) {}

    std::set<Value*> taintedVariables;

    std::map<Value*, Value*> disjointSet;

    Value* find(Value *V) {
      if (disjointSet.find(V) == disjointSet.end()) {
        disjointSet[V] = V;
      }

      if (disjointSet[V] == V) {
        return V;
      }

      disjointSet[V] = find(disjointSet[V]);
      return disjointSet[V];
    }

    // Check if a variable is already tainted
    bool isTainted(Value *V) {
      return find(V) == find(nullptr);
    }

    // Mark a variable as tainted
    void setTaint(Value *V) {
      jointValue(nullptr, V);
    }

    void jointValue(Value *Vfather, Value *Vson) {
      disjointSet[find(Vson)] = find(Vfather);
    }

    bool runOnFunction(Function &F) override {
      errs() << "In function " << F.getName() << '\n';
      for (auto &BB : F) {
        for (auto &I : BB) {
          if (auto *callInst = dyn_cast<CallInst>(&I)) {
            if (Function *calledFunction = callInst->getCalledFunction()) {
               if (calledFunction->getName() == "free") {
                Value *arg = callInst->getArgOperand(0); // Get the argument passed to free

                if (isTainted(arg)) {
                  errs() << "Warning: Potential double-free detected\n";
                } else {
                  setTaint(arg);
                }
              }
            }
          }
          else if (auto *storeInst = dyn_cast<StoreInst>(&I)) {
            
            Value *lhs = storeInst->getValueOperand();
            Value *rhs = storeInst->getPointerOperand();

            if (isa<Constant>(lhs) || isa<Constant>(rhs)) {
              continue;
            }

            jointValue(lhs, rhs);
            
          }
          else if (auto *loadInst = dyn_cast<LoadInst>(&I)) {
            Value *lhs = loadInst;
            Value *rhs = loadInst->getPointerOperand();

            if (isa<Constant>(lhs) || isa<Constant>(rhs)) {
              continue;
            }
            
            jointValue(rhs, lhs);
          }
        }
      }

      return false;
    }
  };
}

char DoubleFreeDetectionPass::ID = 0;

static RegisterStandardPasses l0_register_std_pass(
    PassManagerBuilder::EP_EnabledOnOptLevel0,
    [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM) {
        PM.add(new DoubleFreeDetectionPass());
    }
);

static RegisterStandardPasses moe_register_std_pass(
    PassManagerBuilder::EP_ModuleOptimizerEarly,
    [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM) {
        PM.add(new DoubleFreeDetectionPass());
    }
);
