// MAS.h actually defines the MAS structure
#ifndef MAS_H
#define MAS_H

#include "llvm/IR/Value.h"
#include <vector>

namespace MAS {

    typedef struct MASNode {
        llvm::Value *v;
        std::vector<MASNode *> children;
    } MASNode, *p_MASNode;

    typedef struct {
        std::vector<MASNode *> roots;
    } MAS, *p_MAS;

} 

#endif