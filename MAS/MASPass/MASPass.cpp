// This pass only performs the intraprocedural analysis of Spindle

#include "MAS.h"

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Attributes.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopIterator.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"
#include "llvm/IR/CFG.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/Transforms/Utils/Mem2Reg.h"

namespace
{

  struct MASPass : public llvm::PassInfoMixin<MASPass>
  {

    llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM)
    {
      llvm::PromotePass mem2reg = llvm::PromotePass();
      mem2reg.run(F, FAM);

      llvm::LoopAnalysis::Result &li = FAM.getResult<llvm::LoopAnalysis>(F);
      auto &SE = FAM.getResult<llvm::ScalarEvolutionAnalysis>(F);

      MAS::MAS *curr_mas = new MAS::MAS(&F, &li, &SE);
      curr_mas->calculate();

      // Below is some debug you can uncomment to make sure no loads are getting missed

      // llvm::errs() << "\n========== IR FOR FUNCTION = " << F.getName() << " ===========\n";
      // for (llvm::BasicBlock &B : F) {
      // 	for (llvm::Instruction &I : B) {
      // 		llvm::errs() << I << "\n";
      // 	}
      // }

      // WLA to do intraprocedural analysis

      // This is a backwards dataflow analysis where we start at stores to
      // addresses and then traverse the U-D chain backwards to get the
      // dependencies, which will end up in leaf nodes
      // and categorized by type.

      std::vector<MAS::MASNode *> leaves;

      llvm::errs() << "\n========== MAS FOR FUNCTION = " << F.getName() << " ===========\n";
      for (MAS::MASNode *r : curr_mas->getRoots())
      {
        r->visitNodes();
      }

      for (MAS::MASNode *r : curr_mas->getRoots())
      {
        leaves = *(curr_mas->getLeaves(r));
        llvm::errs() << "PRINTING LEAF NODES OF ROOT = " << *r << "\n";
        for (MAS::MASNode *l : leaves)
        {
          llvm::errs() << *l << "\n";
        }
      }

      free(curr_mas);

      return llvm::PreservedAnalyses::all();
    }
  };
}

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo()
{
  return {
      LLVM_PLUGIN_API_VERSION, "MASPass", "v0.1",
      [](llvm::PassBuilder &PB)
      {
        PB.registerPipelineParsingCallback(
            [](llvm::StringRef Name, llvm::FunctionPassManager &FPM,
               llvm::ArrayRef<llvm::PassBuilder::PipelineElement>)
            {
              if (Name == "MASPass")
              {
                FPM.addPass(MASPass());
                return true;
              }
              return false;
            });
      }};
}