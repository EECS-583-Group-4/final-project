// MAS.h actually defines the MAS structure
#ifndef MAS_H
#define MAS_H

#include "llvm/IR/Value.h"
#include <vector>

namespace MAS {

    enum LEAF_TYPE {CONST, BASE_MEM_ADDR, FUNC_PARAM, DATA_DEP_VAR, FUNC_RET_VAL, LOOP_IND_VAR, UNSET};

    typedef struct MASNode {
        llvm::Value *v;
        std::vector<MASNode *> children;
        LEAF_TYPE label = UNSET;
    } MASNode, *p_MASNode;

    typedef struct {
        std::vector<MASNode *> roots;
    } MAS, *p_MAS;

} 

#endif