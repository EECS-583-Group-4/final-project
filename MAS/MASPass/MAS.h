// MAS.h actually defines the MAS structure
#ifndef MAS_H
#define MAS_H

#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
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

        size_t getLoopIndVarEnd() const;
        size_t getLoopIndVarStart() const; 

        void setLoopIndVarEnd(size_t n);
        void setLoopIndVarStart(size_t n);

        MASNode *visitNodes(size_t depth = 0);

    private:
        llvm::Value *value;
        std::vector<MASNode *> children;
        LEAF_TYPE label = UNSET;
        size_t loop_ind_var_end;
        size_t loop_ind_var_start;
    };

    llvm::raw_ostream& operator<< (llvm::raw_ostream& os, const MASNode& obj);

    class MAS {
    public:
        MAS(llvm::Function *F, llvm::LoopAnalysis::Result *li, llvm::ScalarEvolutionAnalysis::Result *SE);
        std::vector<MASNode *> getRoots();
        void addRoot(MASNode *r, llvm::LoopAnalysis::Result *li, llvm::ScalarEvolutionAnalysis::Result *SE);
        std::vector<MASNode *> getLeaves(MASNode *r);
        void calculate();
    private: 
        std::vector<MASNode *> root_nodes;
        llvm::Function *F; 
        llvm::LoopAnalysis::Result *li; 
        llvm::ScalarEvolutionAnalysis::Result *SE;
    };

    llvm::raw_ostream& operator<< (llvm::raw_ostream& os, const MAS& obj);

} 

#endif