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
  struct MemoryLeakPass : public FunctionPass {
    static char ID;
    MemoryLeakPass() : FunctionPass(ID) {}

    Value* constTaintTag = nullptr;

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
      return find(V) == find(constTaintTag);
    }

    bool connected(Value *v1, Value *v2) {
      return find(v1) == find(v2);
    }

    // Mark a variable as tainted
    void setTaint(Value *V) {
      jointValue(constTaintTag, V);
    }

    void jointValue(Value *Vfather, Value *Vson) {
      auto rootF = find(Vfather);
      auto rootS = find(Vson);

      if (rootF == rootS) {
        return ;
      }

      disjointSet[rootS] = find(rootF);
    }

    void removeTaint(Value* V) {
      ;
    }

    bool runOnFunction(Function &F) override {

      errs() << "In function " << F.getName() << '\n';

      for (auto &BB : F) {
        for (auto &I : BB) {
          // errs() << I << '\n';
          if (auto *callInst = dyn_cast<CallInst>(&I)) {
            
            if (Function *calledFunction = callInst->getCalledFunction()) {
              // Handle malloc
              if (calledFunction->getName() == "malloc") {
                taintedVariables.insert(&I);
              }
              // Handle free
              else if (calledFunction->getName() == "free") {
                Value *arg = callInst->getArgOperand(0);
                // if (isTainted(arg)) {
                //   removeTaint(arg);
                // } else {
                //   errs() << "Error: Freeing uninitialized memory\n";
                // }
                jointValue(arg, nullptr);
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

      // Check for memory leaks
      for (auto *V : taintedVariables) {
        if (!connected(V, nullptr)) {
          errs() << "Warning: memory leak " << *V << '\n';
        }
      }

      return false;
    }
  };
}

char MemoryLeakPass::ID = 0;

static RegisterStandardPasses l0_register_std_pass(
    PassManagerBuilder::EP_EnabledOnOptLevel0,
    [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM) {
        PM.add(new MemoryLeakPass());
    }
);

static RegisterStandardPasses moe_register_std_pass(
    PassManagerBuilder::EP_ModuleOptimizerEarly,
    [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM) {
        PM.add(new MemoryLeakPass());
    }
);
