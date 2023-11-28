#include "MAS.h"
#include "llvm/Support/raw_ostream.h"

namespace MAS {

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
            //llvm::errs() << "DEPTH = " << depth;
            for (int i = 0; i < depth; i++) {
                llvm::errs() << "    |";
            }
            llvm::errs() << *this << "\n";
            for (MASNode *c : children) {
                c->visitNodes(depth+1);
            }
        }
        //llvm::errs() << "\n";
        return nullptr;
    }

    llvm::raw_ostream& operator<< (llvm::raw_ostream& os, const MASNode& obj) {
        
        os << *(obj.getValue());

        return os;
    }

    std::vector<MASNode *> MAS::getRoots() { return root_nodes; }

    void MAS::addRoot(MASNode *r) {
        root_nodes.push_back(r);
    }

    llvm::raw_ostream& operator<< (llvm::raw_ostream& os, const MAS& obj) {

    }
    

}