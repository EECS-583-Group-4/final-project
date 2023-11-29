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
	MAS::LEAF_TYPE categorizeNode(MAS::MASNode *node);

    struct MASPass : public llvm::PassInfoMixin<MASPass> {

		MAS::MAS *curr_mas = new MAS::MAS();

        llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM) {

            //WLA to do intraprocedural analysis 

            // This is a backwards dataflow analysis where we start at stores to 
            // addresses and then traverse the U-D chain backwards to get the 
            // dependencies, which will end up in leaf nodes
            // and categorized by type. 

            // How to iterate over U-D chain

            struct StoreVisitor : public llvm::InstVisitor<StoreVisitor> {
				MAS::MAS *curr_mas;

                inline void visitStoreInst(llvm::StoreInst &I) {
                    // Now we can get the U-D chain

                    // llvm::errs() << "U-D CHAIN FOR: " << I << "\n";

					MAS::MASNode *node = new MAS::MASNode(&I);

					curr_mas->addRoot(node);

					getUD(node);

                    // llvm::errs() << " -----------------------------------\n";
                }
            };

            StoreVisitor storeDepMaker;
			storeDepMaker.curr_mas = curr_mas;

            storeDepMaker.visit(F);

			llvm::errs() << "\n========== MAS FOR FUNCTION = " << F.getName() << " ===========\n";
			for (MAS::MASNode *r : curr_mas->getRoots()) {
				r->visitNodes(0);
			}


            return llvm::PreservedAnalyses::all();
        }

    };

	// This essentially implements the WorkList Algorithm outlined in the Spindle Paper
	MAS::MASNode *getUD(MAS::MASNode *node) {
		llvm::Value *v = node->getValue();
		if (llvm::Instruction *I = llvm::dyn_cast<llvm::Instruction>(v)) {
			//int opcnt = 0;
			for (llvm::Use &U : I->operands()) {
				MAS::MASNode *child = new MAS::MASNode(U.get());
				node->addChild(child);
				
				if (categorizeNode(child) == MAS::UNSET) {
					getUD(child);
					//opcnt+=1;
				}
				else {
					// If we wanted to add a property to nodes to specificly mark them
					// as leaf nodes when they are, this is where we would set that
				}
			}
		}
		// Not sure we need this anymore
		else if (v != nullptr) {
			// llvm::errs() << "END OF UD WITH: " << *v << "\n";

			// Categorize leaf node 
			// if (llvm::isa<llvm::Constant>(v)) {
			// 	node->setLabel(MAS::CONST);
			// }
			categorizeNode(node);
			return node;
		}
		// Definitely still want this though
		return nullptr;
	}

	// This function handles the logic of how to categorize a given node. 
	// For the most part it seems like we can do this based on the instruction type,
	// but the issue is for stuff like DATA_DEP_VAR, where it's not so clear.
	// We are mostly concerned with computable types for the moment for the project though, 
	// so categorizing these other types may not be as necessary yet 
	MAS::LEAF_TYPE categorizeNode(MAS::MASNode *node) {
		if (llvm::isa<llvm::Constant>(node->getValue())) {
			// llvm::errs() << "TYPE OF LEAF IS = CONST \n"; 
			node->setLabel(MAS::CONST);
		}
		else if (llvm::isa<llvm::AllocaInst>(node->getValue())) {
			// llvm::errs() << "TYPE OF LEAF IS = BASE_MEM_ADDR \n"; 
			node->setLabel(MAS::BASE_MEM_ADDR);
		}
		else if (llvm::isa<llvm::CallInst>(node->getValue())) {
			// llvm::errs() << "TYPE FO LEAF IS = FUNC_RET_VAL \n";
			node->setLabel(MAS::FUNC_RET_VAL);
		}
		else {
			// llvm::errs() << "NOW CATEGORIZING " << *node << "\n";
			// llvm::errs() << "TYPE OF LEAF IS = " << *(node->getValue()->getType()) << "\n"; 
			node->setLabel(MAS::UNSET);
		}

		return node->getLabel();
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