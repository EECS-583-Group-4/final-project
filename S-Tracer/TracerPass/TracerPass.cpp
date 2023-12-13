// This pass only performs the intraprocedural analysis of Spindle

#include "../../MAS/MASPass/MAS.h"

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Format.h"
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

#include  <iostream>

namespace
{

  struct TracerPass : public llvm::PassInfoMixin<TracerPass>
  {

    llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM)
    {
      int base_mem_addr_index = 0, use_base_mem_addr_index;
      std::map<llvm::Value*,int> base_mem_addr;
      std::map<llvm::Value*,int>::iterator it_use_base_mem_addr_index;

      MAS::MAS curr_mas = MAS::MAS(&F, &FAM);

      curr_mas.calculate();

      //Declare the file output, taken from a post on Piazza endorsed by the GSI
      std::error_code EC;
      llvm::sys::fs::OpenFlags flags = llvm::sys::fs::OF_Append ;
      llvm::raw_fd_ostream fout = llvm::raw_fd_ostream("static_trace.txt", EC, flags);

      llvm::Module* module = F.getParent();
      llvm::LLVMContext &context = module->getContext();
      llvm::IRBuilder<> builder(context);      
      llvm::Type *intType = llvm::Type::getInt32Ty(context);


      // Declare C standard library printf 
      std::vector<llvm::Type *> printfArgsTypes({llvm::Type::getInt8PtrTy(context)});
      llvm::FunctionType *printfType = llvm::FunctionType::get(intType, printfArgsTypes, true);
      auto printfFunc = module->getOrInsertFunction("printf", printfType);

      // The format string for the printf function, declared as a global literal
      std::vector<llvm::Value *> argsV;

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

 

      //std::vector<std::pair<std::string instruction, ptr p>> list;
      //std::vector<MASNode *> masnodes = curr_mas.getRoots();
      
      for (MAS::MASNode *r : curr_mas.getRoots()){
        if(llvm::isa<llvm::LoadInst>(r->getValue())){
          fout << "Load ";
        }
        else if(llvm::isa<llvm::StoreInst>(r->getValue())){
          fout << "Store ";
        }
        std::vector<MAS::MASNode *> *leaves = curr_mas.getLeaves(r);
        for(MAS::MASNode *leaf : *leaves){
          if(leaf->getLabel()==MAS::LEAF_TYPE(MAS::BASE_MEM_ADDR)){
            llvm::Instruction *inst = llvm::dyn_cast<llvm::Instruction>(leaf->getValue());
            it_use_base_mem_addr_index = base_mem_addr.find(leaf->getValue());
            if (it_use_base_mem_addr_index != base_mem_addr.end()){
              use_base_mem_addr_index = it_use_base_mem_addr_index->second;
            }
            else {
              base_mem_addr.insert(std::pair<llvm::Value*,int>(leaf->getValue(),base_mem_addr_index));
              use_base_mem_addr_index = base_mem_addr_index;
              base_mem_addr_index++;
              std::vector<llvm::Value *> inputs;
              llvm::Value *number = builder.getInt32(use_base_mem_addr_index);

              llvm::Value *format = builder.CreateGlobalStringPtr("BASE_MEM_ADDR %d %x\n", "str", 0U, module);
              inputs.push_back(format);
              inputs.push_back(number);
              inputs.push_back(inst);
              argsV = inputs;

              builder.SetInsertPoint(inst->getParent()->getTerminator());
              builder.CreateCall(printfFunc, argsV, "calltmp");
            }
            fout << "BASE_MEM_ADDR " << llvm::format_decimal(use_base_mem_addr_index, 1) << " ";
          }
          if(leaf->getLabel()==MAS::LEAF_TYPE(MAS::LOOP_IND_VAR)){
            int start = leaf->getTrueLoopStart();
            int end = leaf->getTrueLoopEnd();
            int offset = leaf->getStep();
            fout << "START " << llvm::format_decimal(start, 1) << 
                    " END " << llvm::format_decimal(end, 1) << 
                    " OFFSET " << llvm::format_decimal(offset, 1) << " ";
          }
        }
        fout << "\n";
      }

      return llvm::PreservedAnalyses::all();
    }
  };
}

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo()
{
  return {
      LLVM_PLUGIN_API_VERSION, "TracerPass", "v0.1",
      [](llvm::PassBuilder &PB)
      {
        PB.registerPipelineParsingCallback(
            [](llvm::StringRef Name, llvm::FunctionPassManager &FPM,
               llvm::ArrayRef<llvm::PassBuilder::PipelineElement>)
            {
              if (Name == "TracerPass")
              {
                FPM.addPass(TracerPass());
                return true;
              }
              return false;
            });
      }};
}