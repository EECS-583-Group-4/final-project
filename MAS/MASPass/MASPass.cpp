// This pass only performs the intraprocedural analysis of Spindle

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/raw_ostream.h"

namespace {

    enum LEAF_TYPE {CONST, BASE_MEM_ADDR, FUNC_PARAM, DATA_DEP_VAR, FUNC_RET_VAL, LOOP_IND_VAR};

	llvm::Value *getUD(llvm::Value *v);

    struct MASPass : public llvm::PassInfoMixin<MASPass> {

        llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM) {

            //WLA to do intraprocedural analysis 

            // This is a backwards dataflow analysis where we start at stores to 
            // addresses and then traverse the U-D chain backwards to get the 
            // dependencies, which will end up in leaf nodes
            // and categorized by type. 

            // How to iterate over U-D chain

            struct StoreVisitor : public llvm::InstVisitor<StoreVisitor> {
                inline void visitStoreInst(llvm::StoreInst &I) {
                    // Now we can get the U-D chain

                    llvm::errs() << "U-D CHAIN FOR: " << I << "\n";

                    // for (llvm::Use &U : I.operands()) {
                    //     llvm::Value *v = U.get();
                    //     llvm::errs() << *v << "\n";
                    // }

					getUD(&I);

                    llvm::errs() << " ----------------------------------- \n";
                }
            };

            StoreVisitor storeDepMaker;

            storeDepMaker.visit(F);


            return llvm::PreservedAnalyses::all();
        }

    };

	llvm::Value *getUD(llvm::Value *v) {
		if (llvm::Instruction *I = llvm::dyn_cast<llvm::Instruction>(v)) {
			int opcnt = 0;
			for (llvm::Use &U : I->operands()) {
				llvm::errs() << "OPERAND " << opcnt << "\n";
				llvm::Value *nv = U.get();
				llvm::errs() << *nv << "\n";
				getUD(nv);
				opcnt+=1;
			}
		}
		else if (v != nullptr) {
			llvm::errs() << "END OF UD WITH: " << *v << "\n";
			return v;
		}
		return nullptr;
	}
}

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION, "MASPass", "v0.1",
    [](llvm::PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
        [](llvm::StringRef Name, llvm::FunctionPassManager &FPM,
        llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {
          if(Name == "MASPass"){
            FPM.addPass(MASPass());
            return true;
          }
          return false;
        }
      );
    }
  };
}