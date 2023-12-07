// MAS.h actually defines the MAS structure
#ifndef MAS_H
#define MAS_H

#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"
#include <vector>
// #include <iostream>

namespace MAS {

    enum LEAF_TYPE {CONST, BASE_MEM_ADDR, FUNC_PARAM, DATA_DEP_VAR, FUNC_RET_VAL, LOOP_IND_VAR, UNSET};

    class MASNode {
    public: 
        MASNode(llvm::Value *v);
        llvm::Value *getValue() const;
        void addChild(MASNode *);

        void setLabel(LEAF_TYPE t);
        LEAF_TYPE getLabel() const;

        std::vector<MASNode *> getChildren();

        MASNode *visitNodes(size_t depth);

    private:
        llvm::Value *value;
        std::vector<MASNode *> children;
        LEAF_TYPE label = UNSET;
    };

    llvm::raw_ostream& operator<< (llvm::raw_ostream& os, const MASNode& obj);

    class MAS {
    public:
        std::vector<MASNode *> getRoots();
        void addRoot(MASNode *r);
        std::vector<MASNode *> getLeaves(MASNode *r);
    private: 
        std::vector<MASNode *> root_nodes;
    };

    llvm::raw_ostream& operator<< (llvm::raw_ostream& os, const MAS& obj);

} 

#endif