#include "MAS.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/InstVisitor.h"

namespace MAS {

    LEAF_TYPE categorizeNode(MASNode *node, llvm::LoopAnalysis::Result *li, llvm::ScalarEvolutionAnalysis::Result *SE);
    MASNode *getUD(MASNode *node, llvm::LoopAnalysis::Result *li, llvm::ScalarEvolutionAnalysis::Result *SE);

    const char* READ_LEAF_TYPE[] = {"CONST", "BASE_MEM_ADDR", "FUNC_PARAM", "DATA_DEP_VAR", "FUNC_RET_VAL", "LOOP_IND_VAR", "UNSET"};

    MASNode::MASNode(llvm::Value *v) {
        value = v;
    }

    llvm::Value *MASNode::getValue() const { return value; }
    
    void MASNode::addChild(MASNode *child) {
        children.push_back(child);
    }

    void MASNode::setLabel(LEAF_TYPE t) {
        label = t;
    }

    LEAF_TYPE MASNode::getLabel() const { return label; }

    std::vector<MASNode *> MASNode::getChildren() { return children; }

    size_t MASNode::getLoopIndVarStart() const { 
        if (this->label == LOOP_IND_VAR) {
            return this->loop_ind_var_start;
        }
        return ULONG_MAX;
    }

    size_t MASNode::getLoopIndVarEnd() const {
        if (this->label == LOOP_IND_VAR) {
            return this->loop_ind_var_end;
        }
        return ULONG_MAX;
    }

    void MASNode::setLoopIndVarStart(size_t n) {
        if (this->label == LOOP_IND_VAR) {
            this->loop_ind_var_start = n;
        }
        return;
    }

    void MASNode::setLoopIndVarEnd(size_t n) {
        if (this->label == LOOP_IND_VAR) {
            this->loop_ind_var_end = n;
        }
        return;
    }

    MASNode *MASNode::visitNodes(size_t depth) {
        if (this != nullptr) {
            for (int i = 0; i < depth; i++) {
                llvm::errs() << "    |";
            }
            llvm::errs() << *this << "\n";
            for (MASNode *c : children) {
                c->visitNodes(depth+1);
            }
        }
        return nullptr;
    }

    void vNodes(MASNode *node, std::vector<MASNode *> *leaves) {
        if (node != nullptr) {
            if (node->getChildren().size() > 0) {
                for (MASNode *c : node->getChildren()) {
                    vNodes(c, leaves);
                }
            }
            else {
                leaves->push_back(node);
                return;
            }
        }
        return;
    }

    llvm::raw_ostream& operator<< (llvm::raw_ostream& os, const MASNode& obj) {
        
        if (llvm::isa<llvm::Instruction>(obj.getValue())) {
            if (llvm::isa<llvm::CallInst>(obj.getValue())) {
                llvm::CallInst *c = llvm::dyn_cast<llvm::CallInst>(obj.getValue());
                os << "  ";
                obj.getValue()->printAsOperand(os);
                os << " = Return Val of " << c->getCalledFunction()->getName();
            }
            else if (obj.getLabel() == LOOP_IND_VAR) {
                os << *(obj.getValue()) << " (Start = " << obj.getLoopIndVarStart() << ", ";
                os << "End = " << obj.getLoopIndVarEnd() << ") ";
            }
            else {
                os << *(obj.getValue());
            }
        }
        else {
            os << "  ";
            obj.getValue()->printAsOperand(os);
        }

        os << " (" << READ_LEAF_TYPE[obj.getLabel()] << ")";

        return os;
    }

    MAS::MAS(llvm::Function *F, llvm::LoopAnalysis::Result *li, llvm::ScalarEvolutionAnalysis::Result *SE) {
        this->F = F;
        this->li = li;
        this->SE = SE;
    }

    void MAS::calculate() {
        struct LoadVisitor : public llvm::InstVisitor<LoadVisitor> {
            llvm::LoopAnalysis::Result *li;
            llvm::ScalarEvolutionAnalysis::Result *SE;
            MAS *curr_mas;

            inline void visitLoadInst(llvm::LoadInst &I) {
                MASNode *node = new MASNode(&I);

                curr_mas->addRoot(node, li, SE);
            }
        };

        LoadVisitor loadDepMaker;
        loadDepMaker.curr_mas = this;
        loadDepMaker.li = li;
        loadDepMaker.SE = SE;

        loadDepMaker.visit(F);
    }

    std::vector<MASNode *> MAS::getRoots() { return root_nodes; }

    void MAS::addRoot(MASNode *r, llvm::LoopAnalysis::Result *li, llvm::ScalarEvolutionAnalysis::Result *SE) {
        root_nodes.push_back(r);

        getUD(r, li, SE);
    }

    std::vector<MASNode *> *MAS::getLeaves(MASNode *r) {
        std::vector<MASNode *> *leaves = new std::vector<MASNode *>;

        vNodes(r, leaves);

        return leaves;
    }

    // This essentially implements the WorkList Algorithm outlined in the Spindle Paper
	MASNode *getUD(MASNode *node, llvm::LoopAnalysis::Result *li, llvm::ScalarEvolutionAnalysis::Result *SE) {
		llvm::Value *v = node->getValue();
		if (llvm::Instruction *I = llvm::dyn_cast<llvm::Instruction>(v)) {
			//int opcnt = 0;
			for (llvm::Use &U : I->operands()) {

				MASNode *child = new MASNode(U.get());
				node->addChild(child);
				
				if (categorizeNode(child, li, SE) == UNSET) {
					getUD(child, li, SE);
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
			categorizeNode(node, li, SE);
			return node;
		}
		// Definitely still want this though
		return nullptr;
	}

    LEAF_TYPE categorizeNode(MASNode *node, llvm::LoopAnalysis::Result *li, llvm::ScalarEvolutionAnalysis::Result *SE) {
		if (llvm::isa<llvm::Constant>(node->getValue())) {
			// llvm::errs() << "TYPE OF LEAF IS = CONST \n"; 
			node->setLabel(CONST);
		}
		else if (llvm::isa<llvm::AllocaInst>(node->getValue())) {
			// llvm::errs() << "TYPE OF LEAF IS = BASE_MEM_ADDR \n"; 
			node->setLabel(BASE_MEM_ADDR);
		}
		else if (llvm::isa<llvm::CallInst>(node->getValue())) {
			// llvm::errs() << "TYPE FO LEAF IS = FUNC_RET_VAL \n";
			node->setLabel(FUNC_RET_VAL);
		}
        // This currently assumes phi node -> loopindvar, which is only true
        // b/c of the subset of passes being run
		else if (llvm::isa<llvm::PHINode>(node->getValue())) {
            for (llvm::Loop *lo : li->getLoopsInPreorder()) {
                if (node->getValue() == lo->getCanonicalInductionVariable()) {
                    node->setLabel(LOOP_IND_VAR);
                    node->setLoopIndVarStart(0); // Def of CanonicalIndVar
                    auto cnt = SE->getBackedgeTakenCount(lo);
                    auto ocnt = SE->getSmallConstantTripCount(lo);
                    node->setLoopIndVarEnd(ocnt-2); // For some reason this is 2 higher than it should be? 
                }
            }
		}
		else {
			// llvm::errs() << "NOW CATEGORIZING " << *node << "\n";
			// llvm::errs() << "TYPE OF LEAF IS = " << *(node->getValue()->getType()) << "\n"; 
			node->setLabel(UNSET);
		}

		return node->getLabel();
	}
    

}