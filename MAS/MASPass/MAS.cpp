#include "MAS.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"

namespace MAS {

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

    llvm::raw_ostream& operator<< (llvm::raw_ostream& os, const MASNode& obj) {
        
        if (llvm::isa<llvm::Instruction>(obj.getValue())) {
            if (llvm::isa<llvm::CallInst>(obj.getValue())) {
                llvm::CallInst *c = llvm::dyn_cast<llvm::CallInst>(obj.getValue());
                os << "  ";
                obj.getValue()->printAsOperand(os);
                os << " = Return Val of " << c->getCalledFunction()->getName();
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

    std::vector<MASNode *> MAS::getRoots() { return root_nodes; }

    void MAS::addRoot(MASNode *r) {
        root_nodes.push_back(r);
    }

    std::vector<MASNode *> MAS::getLeaves(MASNode *r) {
        
    }
    

}