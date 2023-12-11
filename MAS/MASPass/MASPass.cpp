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
#include "llvm/IR/IRBuilder.h"

namespace
{

  struct MASPass : public llvm::PassInfoMixin<MASPass>
  {

    llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM)
    {
      MAS::MAS curr_mas = MAS::MAS(&F, &FAM);

      curr_mas.calculate();

      std::vector<MAS::MASNode *> roots = curr_mas.getRoots();

      for (MAS::MASNode *root : roots) {
        // Declaring some variables 
        llvm::Instruction *call_inst;
        llvm::Module* module = F.getParent();
        llvm::LLVMContext &context = module->getContext();
        llvm::IRBuilder<> builder(context);      
        llvm::Type *intType = llvm::Type::getInt32Ty(context);


        // Declare C standard library printf 
        std::vector<llvm::Type *> printfArgsTypes({llvm::Type::getInt8PtrTy(context)});
        llvm::FunctionType *printfType = llvm::FunctionType::get(intType, printfArgsTypes, true);
        auto printfFunc = module->getOrInsertFunction("printf", printfType);

        // The format string for the printf function, declared as a global literal
        llvm::Value *str = builder.CreateGlobalStringPtr("test\n", "str", 0U, module);
  

        std::vector<llvm::Value *> argsV({str});
        if (llvm::Instruction *ins = llvm::dyn_cast<llvm::Instruction>(root->getValue())) {
          builder.SetInsertPoint(ins);
          builder.CreateCall(printfFunc, argsV, "calltmp");
        }

      }

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
      curr_mas.print();

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