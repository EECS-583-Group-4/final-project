// This pass only performs the intraprocedural analysis of Spindle

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Attributes.h"

#include "MAS.h"

namespace {

	MAS::MASNode *getUD(MAS::MASNode *node);

    struct MASPass : public llvm::PassInfoMixin<MASPass> {

        llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM) {

            //WLA to do intraprocedural analysis 

            // This is a backwards dataflow analysis where we start at stores to 
            // addresses and then traverse the U-D chain backwards to get the 
            // dependencies, which will end up in leaf nodes
            // and categorized by type. 

            // How to iterate over U-D chain

			MAS::MAS *curr_mas = new MAS::MAS();

            struct StoreVisitor : public llvm::InstVisitor<StoreVisitor> {
				MAS::MAS *curr_mas;

                inline void visitStoreInst(llvm::StoreInst &I) {
                    // Now we can get the U-D chain

                    llvm::errs() << "U-D CHAIN FOR: " << I << "\n";

                    // for (llvm::Use &U : I.operands()) {
                    //     llvm::Value *v = U.get();
                    //     llvm::errs() << *v << "\n";
                    // }

					MAS::MASNode *node = new MAS::MASNode(&I);

					curr_mas->addRoot(node);

					getUD(node);

                    llvm::errs() << " -----------------------------------\n";
                }
            };

            StoreVisitor storeDepMaker;
			storeDepMaker.curr_mas = curr_mas;

            storeDepMaker.visit(F);

			llvm::errs() << "------ META ANALYSIS OF THE MAS ------\n";

			for (MAS::MASNode *root : curr_mas->getRoots()) {
				llvm::errs() << *(root->getValue()) << "\n";
				for (MAS::MASNode *child : root->getChildren()) {
					llvm::errs() <<  "    |" << *(child->getValue()) << "\n";
					if (child->getLabel() != MAS::UNSET) {
						llvm::errs() << "    | I AM A LEAF NODE OF TYPE = " << child->getLabel() << "\n";
					}
					for (MAS::MASNode *child2 : child->getChildren()) {
						llvm::errs() <<  "    |    |" << *(child2->getValue()) << "\n";
					}
				}
			}

			llvm::errs() << "---------------------\n";

			llvm::errs() << " -------- BIG TEST --------- \n";
			for (MAS::MASNode *r : curr_mas->getRoots()) {
				r->visitNodes(0);
			}


            return llvm::PreservedAnalyses::all();
        }

    };

	// NOTE THAT RIGHT NOW THIS IGNORES CALL INSTRUCTIONS BECAUSE WE 
	// ARE JUST DOING INTRAPROCEDURAL ANALYSIS HERE
	MAS::MASNode *getUD(MAS::MASNode *node) {
		llvm::errs() << "TYPE = " << *(node->getValue()->getType()) << " GETTING UD FOR " << *node << "\n";
		llvm::Value *v = node->getValue();
		if (llvm::Instruction *I = llvm::dyn_cast<llvm::Instruction>(v)) {
			int opcnt = 0;
			for (llvm::Use &U : I->operands()) {
				if (!llvm::isa<llvm::CallInst>(U)) {
					MAS::MASNode *child = new MAS::MASNode(U.get());
					node->addChild(child);
					llvm::errs() << "OPERAND " << opcnt << "\n";
					llvm::Value *nv = U.get();
					llvm::errs() << *nv << "\n";
					getUD(child);
					opcnt+=1;
				}
			}

			if (opcnt == 0) {
				// Categorize leaf node
			}
		}
		else if (v != nullptr) {
			llvm::errs() << "END OF UD WITH: " << *v << "\n";

			// Categorize leaf node 
			if (llvm::isa<llvm::Constant>(v)) {
				node->setLabel(MAS::CONST);
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