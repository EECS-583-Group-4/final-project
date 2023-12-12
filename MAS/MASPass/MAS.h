// MAS.h actually defines the MAS structure
#ifndef MAS_H
#define MAS_H

#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include <vector>
// #include <iostream>

namespace MAS
{

    enum LEAF_TYPE
    {
        CONST,
        BASE_MEM_ADDR,
        FUNC_PARAM,
        DATA_DEP_VAR,
        FUNC_RET_VAL,
        LOOP_IND_VAR,
        OPERATION,
        UNSET,
        MEM_ALLOC,
        MEM_DEALLOC
    };

    class MASNode
    {
    public:
        MASNode(llvm::Value *v);

        bool isLoopInductionBased();
        int getTrueLoopStart();
        int getTrueLoopEnd();
        int getStep();
        llvm::Value *getValue() const;
        LEAF_TYPE getLabel() const;

        std::vector<MASNode *> getChildren();

        size_t getLoopIndVarEnd() const;
        size_t getLoopIndVarStart() const;

        void addChild(MASNode *);
        void setLabel(LEAF_TYPE t);
        void setLoopIndVarEnd(size_t n);
        void setLoopIndVarStart(size_t n);
        void setStep(size_t n);
        MASNode *visitNodes(size_t depth = 0);

    private:
        llvm::Value *value;
        std::vector<MASNode *> children;
        LEAF_TYPE label = UNSET;
        size_t loop_ind_var_end;
        size_t loop_ind_var_start;
        size_t step;
    };

    llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const MASNode &obj);

    class MAS
    {
    public:
        MAS(llvm::Function *F, llvm::FunctionAnalysisManager *FAM);
        std::vector<MASNode *> getRoots();
        void addRoot(MASNode *r, llvm::LoopAnalysis::Result *li, llvm::ScalarEvolutionAnalysis::Result *SE);
        std::vector<MASNode *> *getLeaves(MASNode *r);
        void calculate();
        MASNode *getNode(llvm::Value *val);
        void print();

    private:
        std::vector<MASNode *> root_nodes;
        llvm::Function *F;
        llvm::FunctionAnalysisManager *FAM;
        llvm::LoopAnalysis::Result *li;
        llvm::ScalarEvolutionAnalysis::Result *SE;
    };

    llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const MAS &obj);
}

#endif