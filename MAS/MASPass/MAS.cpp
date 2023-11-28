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

    llvm::raw_ostream& operator<< (llvm::raw_ostream& os, const MASNode& obj) {
        
        os << *(obj.getValue());

        return os;
    }
    

}