// This pass only performs the intraprocedural analysis of Spindle

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/raw_ostream.h"

#include "MAS.h"

namespace {

	MAS::p_MASNode getUD(MAS::p_MASNode node);

    struct MASPass : public llvm::PassInfoMixin<MASPass> {

        llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM) {

            //WLA to do intraprocedural analysis 

            // This is a backwards dataflow analysis where we start at stores to 
            // addresses and then traverse the U-D chain backwards to get the 
            // dependencies, which will end up in leaf nodes
            // and categorized by type. 

            // How to iterate over U-D chain

			MAS::p_MAS curr_mas = new MAS::MAS;

            struct StoreVisitor : public llvm::InstVisitor<StoreVisitor> {
				MAS::p_MAS curr_mas;

                inline void visitStoreInst(llvm::StoreInst &I) {
                    // Now we can get the U-D chain

                    llvm::errs() << "U-D CHAIN FOR: " << I << "\n";

                    // for (llvm::Use &U : I.operands()) {
                    //     llvm::Value *v = U.get();
                    //     llvm::errs() << *v << "\n";
                    // }

					MAS::p_MASNode node = new MAS::MASNode;
					node->v = &I;

					curr_mas->roots.push_back(node);

					getUD(node);

                    llvm::errs() << " -----------------------------------\n";
                }
            };

            StoreVisitor storeDepMaker;
			storeDepMaker.curr_mas = curr_mas;

            storeDepMaker.visit(F);

			llvm::errs() << "------ META ANALYSIS OF THE MAS ------\n";

			for (MAS::p_MASNode root : curr_mas->roots) {
				llvm::errs() << *(root->v) << "\n";
				for (MAS::p_MASNode child : root->children) {
					llvm::errs() <<  "    |" << *(child->v) << "\n";
					if (child->label != MAS::UNSET) {
						llvm::errs() << "    | I AM A LEAF NODE OF TYPE = " << child->label << "\n";
					}
					for (MAS::p_MASNode child2 : child->children) {
						llvm::errs() <<  "    |    |" << *(child2->v) << "\n";
					}
				}
			}

			llvm::errs() << "---------------------\n";


            return llvm::PreservedAnalyses::all();
        }

    };

	MAS::p_MASNode getUD(MAS::p_MASNode node) {
		llvm::Value *v = node->v;
		if (llvm::Instruction *I = llvm::dyn_cast<llvm::Instruction>(v)) {
			int opcnt = 0;
			for (llvm::Use &U : I->operands()) {
				MAS::p_MASNode child = new MAS::MASNode;
				child->v = U.get();
				node->children.push_back(child);
				llvm::errs() << "OPERAND " << opcnt << "\n";
				llvm::Value *nv = U.get();
				llvm::errs() << *nv << "\n";
				getUD(child);
				opcnt+=1;
			}

			if (opcnt == 0) {
				// Categorize leaf node
			}
		}
		else if (v != nullptr) {
			llvm::errs() << "END OF UD WITH: " << *v << "\n";

			// Categorize leaf node 
			if (llvm::isa<llvm::Constant>(v)) {
				node->label = MAS::CONST;
			}
			return node;
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