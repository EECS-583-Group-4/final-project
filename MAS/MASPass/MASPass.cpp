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

namespace {

	MAS::MASNode *getUD(MAS::MASNode *node, llvm::LoopAnalysis::Result *li);
	MAS::LEAF_TYPE categorizeNode(MAS::MASNode *node);

	int LoopRotatePass();

    struct MASPass : public llvm::PassInfoMixin<MASPass> {

		MAS::MAS *curr_mas = new MAS::MAS();

        llvm::PreservedAnalyses run(llvm::Function &F, llvm::FunctionAnalysisManager &FAM) {

			llvm::LoopAnalysis::Result &li = FAM.getResult<llvm::LoopAnalysis>(F);
			auto &SE = FAM.getResult<llvm::ScalarEvolutionAnalysis>(F);
			// auto &LAMProxy = FAM.getResult<llvm::LoopAnalysisManagerFunctionProxy>(F).getManager();

			for (llvm::Loop *lo : li.getLoopsInPreorder()) {
				auto value = SE.getBackedgeTakenCount(lo);
				llvm::errs() << "TYPE OF BACK TAK CNT = ";
				value->getType()->print(llvm::errs());
				llvm::errs() << " = ";
				value->print(llvm::errs());
				llvm::errs() << "\n";
				auto lIndVar = lo->getCanonicalInductionVariable();
				llvm::errs() << "Ind var = " << *lIndVar << "\n";
			}

            //WLA to do intraprocedural analysis 

            // This is a backwards dataflow analysis where we start at stores to 
            // addresses and then traverse the U-D chain backwards to get the 
            // dependencies, which will end up in leaf nodes
            // and categorized by type. 

            // How to iterate over U-D chain

			llvm::errs() << "PRINTING THE IR\n";
			for (llvm::BasicBlock &B : F) {
					for (llvm::Instruction &I : B) {
					llvm::errs() << I << "\n"; 
				}
			}

            struct StoreVisitor : public llvm::InstVisitor<StoreVisitor> {
				MAS::MAS *curr_mas;
				llvm::LoopAnalysis::Result *li;

                inline void visitLoadInst(llvm::LoadInst &I) {
                    // Now we can get the U-D chain

                    // llvm::errs() << "U-D CHAIN FOR: " << I << "\n";

					MAS::MASNode *node = new MAS::MASNode(&I);

					curr_mas->addRoot(node);

					getUD(node, li);

                    // llvm::errs() << " -----------------------------------\n";
                }
            };

			for (llvm::Loop *lo : li.getLoopsInPreorder()) {
				llvm::errs() << "HERE BE IND VAR!\n";
				// llvm::ScalarEvolutionAnalysis::Result *scevi = LAMProxy.getCachedResult<llvm::ScalarEvolutionAnalysis>(*lo);
				// llvm::errs() << lo->getCanonicalInductionVariable()->getName() << "\n";
				// scevi->getTripCountFromExitCount(scevi->getExitCount(lo, lo->getExitBlock()))->print(llvm::errs());
				
			}

            StoreVisitor storeDepMaker;
			storeDepMaker.curr_mas = curr_mas;
			storeDepMaker.li = &li;

            storeDepMaker.visit(F);

			llvm::errs() << "\n========== MAS FOR FUNCTION = " << F.getName() << " ===========\n";
			for (MAS::MASNode *r : curr_mas->getRoots()) {
				r->visitNodes(0);
			}


            return llvm::PreservedAnalyses::none();
        }

    };

	// This essentially implements the WorkList Algorithm outlined in the Spindle Paper
	MAS::MASNode *getUD(MAS::MASNode *node, llvm::LoopAnalysis::Result *li) {
		llvm::Value *v = node->getValue();
		if (llvm::Instruction *I = llvm::dyn_cast<llvm::Instruction>(v)) {
			//int opcnt = 0;
			for (llvm::Use &U : I->operands()) {

				MAS::MASNode *child = new MAS::MASNode(U.get());
				node->addChild(child);

				if (llvm::isa<llvm::PHINode>(child->getValue())) {
					llvm::errs() << "PHI NODE HERE!\n";
				}
				
				if (categorizeNode(child) == MAS::UNSET) {
					getUD(child, li);
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
		else if (llvm::isa<llvm::PHINode>(node->getValue())) {
			node->setLabel(MAS::LOOP_IND_VAR);
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