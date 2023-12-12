#include "MAS.h"
#include <cmath>
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/Transforms/Utils/Mem2Reg.h"
#include "llvm/Transforms/Utils/LoopSimplify.h"

namespace MAS
{

    LEAF_TYPE categorizeNode(MASNode *node, llvm::LoopAnalysis::Result *li, llvm::ScalarEvolutionAnalysis::Result *SE);
    MASNode *getUD(MASNode *node, llvm::LoopAnalysis::Result *li, llvm::ScalarEvolutionAnalysis::Result *SE);

    const char *READ_LEAF_TYPE[] = {"CONST", "BASE_MEM_ADDR", "FUNC_PARAM", "DATA_DEP_VAR", "FUNC_RET_VAL", "LOOP_IND_VAR", "OPERATION", "UNSET", "MEM_ALLOC", "MEM_DEALLOC"};

    MASNode::MASNode(llvm::Value *v)
    {
        value = v;
    }

    llvm::Value *MASNode::getValue() const { return value; }

    void MASNode::addChild(MASNode *child)
    {
        children.push_back(child);
    }

    void MASNode::setLabel(LEAF_TYPE t)
    {
        label = t;
    }

    LEAF_TYPE MASNode::getLabel() const { return label; }

    std::vector<MASNode *> MASNode::getChildren() { return children; }

    size_t MASNode::getLoopIndVarStart() const
    {
        if (this->label == LOOP_IND_VAR)
        {
            return this->loop_ind_var_start;
        }
        return 0;
    }

    size_t MASNode::getLoopIndVarEnd() const
    {
        if (this->label == LOOP_IND_VAR)
        {
            return this->loop_ind_var_end;
        }
        return 0;
    }

    void MASNode::setLoopIndVarStart(size_t n)
    {
        if (this->label == LOOP_IND_VAR)
        {
            this->loop_ind_var_start = n;
        }
        return;
    }

    void MASNode::setLoopIndVarEnd(size_t n)
    {
        if (this->label == LOOP_IND_VAR)
        {
            this->loop_ind_var_end = n;
        }
        return;
    }

    MASNode *MASNode::visitNodes(size_t depth)
    {
        if (this != nullptr)
        {
            for (int i = 0; i < depth; i++)
            {
                llvm::errs() << "    |";
            }
            llvm::errs() << *this << "\n";
            for (MASNode *c : children)
            {
                c->visitNodes(depth + 1);
            }
        }
        return nullptr;
    }

    void vNodes(MASNode *node, std::vector<MASNode *> *leaves)
    {
        if (node != nullptr)
        {
            if (node->getChildren().size() > 0)
            {
                for (MASNode *c : node->getChildren())
                {
                    vNodes(c, leaves);
                }
            }
            else
            {
                leaves->push_back(node);
                return;
            }
        }
        return;
    }

    llvm::raw_ostream &operator<<(llvm::raw_ostream &os, const MASNode &obj)
    {

        if (llvm::isa<llvm::Instruction>(obj.getValue()))
        {
            if (llvm::isa<llvm::CallInst>(obj.getValue()))
            {
                llvm::CallInst *c = llvm::dyn_cast<llvm::CallInst>(obj.getValue());
                os << "  ";
                obj.getValue()->printAsOperand(os);
                os << " = Return Val of " << c->getCalledFunction()->getName();
            }
            else if (obj.getLabel() == LOOP_IND_VAR)
            {
                os << *(obj.getValue()) << " (Start = " << obj.getLoopIndVarStart() << ", ";
                os << "End = " << obj.getLoopIndVarEnd() << ") ";
            }
            else
            {
                os << *(obj.getValue());
            }
        }
        else
        {
            os << "  ";
            obj.getValue()->printAsOperand(os);
        }

        os << " (" << READ_LEAF_TYPE[obj.getLabel()] << ")";

        return os;
    }

    MAS::MAS(llvm::Function *F, llvm::FunctionAnalysisManager *FAM)
    {
        this->F = F;
        this->FAM = FAM;
        this->li = &(FAM->getResult<llvm::LoopAnalysis>(*F));
        this->SE = &(FAM->getResult<llvm::ScalarEvolutionAnalysis>(*F));
    }

    void MAS::print()
    {
        std::vector<MASNode *> leaves;

        llvm::errs() << "\n========== MAS FOR FUNCTION = " << F->getName() << " ===========\n";
        for (MASNode *r : this->getRoots())
        {
            r->visitNodes();
        }
    }

    void MAS::calculate()
    {
        llvm::PromotePass mem2reg = llvm::PromotePass();
        mem2reg.run(*F, *FAM);
        llvm::LoopSimplifyPass loopsim = llvm::LoopSimplifyPass();
        loopsim.run(*F, *FAM);
        this->li = &(FAM->getResult<llvm::LoopAnalysis>(*F));
        this->SE = &(FAM->getResult<llvm::ScalarEvolutionAnalysis>(*F));
        struct LoadVisitor : public llvm::InstVisitor<LoadVisitor>
        {
            llvm::LoopAnalysis::Result *li;
            llvm::ScalarEvolutionAnalysis::Result *SE;
            MAS *curr_mas;

            inline void visitLoadInst(llvm::LoadInst &I)
            {
                MASNode *node = new MASNode(&I);

                curr_mas->addRoot(node, li, SE);
            }
        };

        LoadVisitor loadDepMaker;
        loadDepMaker.curr_mas = this;
        loadDepMaker.li = li;
        loadDepMaker.SE = SE;

        loadDepMaker.visit(F);
        // Add stores to the MAS
        struct StoreVisitor : public llvm::InstVisitor<StoreVisitor>
        {
            llvm::LoopAnalysis::Result *li;
            llvm::ScalarEvolutionAnalysis::Result *SE;
            MAS *curr_mas;

            inline void visitStoreInst(llvm::StoreInst &I)
            {
                // Process the StoreInst instruction
                // Create a MASNode and add it to the MAS instance
                MASNode *node = new MASNode(&I);
                curr_mas->addRoot(node, li, SE);
            }
        };

        StoreVisitor storeDepMaker;
        storeDepMaker.curr_mas = this;
        storeDepMaker.li = li;
        storeDepMaker.SE = SE;

        storeDepMaker.visit(F);

        // Add malloc instructions to the MAS
        struct MallocVisitor : public llvm::InstVisitor<MallocVisitor>
        {
            llvm::LoopAnalysis::Result *li;
            llvm::ScalarEvolutionAnalysis::Result *SE;
            MAS *curr_mas;

            inline void visitCallInst(llvm::CallInst &I)
            {
                llvm::Function *calledFunc = I.getCalledFunction();
                if (calledFunc)
                {
                    std::string funcName = calledFunc->getName().str();
                    if (funcName == "malloc" || funcName == "calloc" || funcName == "realloc")
                    {
                        MASNode *node = new MASNode(&I);
                        node->setLabel(MEM_ALLOC);
                        curr_mas->addRoot(node, li, SE);
                    }
                }
            }
        };

        MallocVisitor mallocDepMaker;
        mallocDepMaker.curr_mas = this;
        mallocDepMaker.li = li;
        mallocDepMaker.SE = SE;

        mallocDepMaker.visit(F);

        // Add free instructions to the MAS
        struct FreeVisitor : public llvm::InstVisitor<FreeVisitor>
        {
            llvm::LoopAnalysis::Result *li;
            llvm::ScalarEvolutionAnalysis::Result *SE;
            MAS *curr_mas;

            inline void visitCallInst(llvm::CallInst &I)
            {
                llvm::Function *calledFunc = I.getCalledFunction();
                if (calledFunc)
                {
                    std::string funcName = calledFunc->getName().str();
                    if (funcName == "free" || funcName == "delete" || funcName == "delete[]")
                    {
                        MASNode *node = new MASNode(&I);
                        node->setLabel(MEM_DEALLOC);
                        curr_mas->addRoot(node, li, SE);
                    }
                }
            }
        };

        FreeVisitor freeDepMaker;
        freeDepMaker.curr_mas = this;
        freeDepMaker.li = li;
        freeDepMaker.SE = SE;

        freeDepMaker.visit(F);
    }

    std::vector<MASNode *> MAS::getRoots() { return root_nodes; }

    void MAS::addRoot(MASNode *r, llvm::LoopAnalysis::Result *li, llvm::ScalarEvolutionAnalysis::Result *SE)
    {
        root_nodes.push_back(r);

        getUD(r, li, SE);
    }

    std::vector<MASNode *> *MAS::getLeaves(MASNode *r)
    {
        std::vector<MASNode *> *leaves = new std::vector<MASNode *>;

        vNodes(r, leaves);

        return leaves;
    }

    // This essentially implements the WorkList Algorithm outlined in the Spindle Paper
    MASNode *getUD(MASNode *node, llvm::LoopAnalysis::Result *li, llvm::ScalarEvolutionAnalysis::Result *SE)
    {
        llvm::Value *v = node->getValue();
        if (llvm::Instruction *I = llvm::dyn_cast<llvm::Instruction>(v))
        {
            // int opcnt = 0;
            for (llvm::Use &U : I->operands())
            {

                MASNode *child = new MASNode(U.get());
                node->addChild(child);
                LEAF_TYPE ty = categorizeNode(child, li, SE);
                if (ty == OPERATION || ty == UNSET)
                {
                    getUD(child, li, SE);
                    // opcnt+=1;
                }
            }
        }
        else if (v != nullptr)
        {
            categorizeNode(node, li, SE);
            return node;
        }
        // Definitely still want this though
        return nullptr;
    }

    int performBinaryOp(int v1, int v2, llvm::Instruction::BinaryOps b)
    {
        if (b == llvm::Instruction::BinaryOps::Add)
        {
            return v1 + v2;
        }
        else if (b == llvm::Instruction::BinaryOps::Sub)
        {
            return v1 - v2;
        }
        else if (b == llvm::Instruction::BinaryOps::Mul)
        {
            return v1 * v2;
        }
        else if (b == llvm::Instruction::BinaryOps::UDiv || b == llvm::Instruction::BinaryOps::SDiv)
        {
            return v1 / v2;
        }
        else if (b == llvm::Instruction::BinaryOps::URem || b == llvm::Instruction::BinaryOps::SRem)
        {
            return v1 % v2;
        }
        else if (b == llvm::Instruction::BinaryOps::Shl)
        {
            return v1 << v2;
        }
        else if (b == llvm::Instruction::BinaryOps::LShr || b == llvm::Instruction::BinaryOps::AShr)
        {
            return v1 >> v2;
        }
        else if (b == llvm::Instruction::BinaryOps::And)
        {
            return v1 & v2;
        }
        else if (b == llvm::Instruction::BinaryOps::Or)
        {
            return v1 | v2;
        }
        else if (b == llvm::Instruction::BinaryOps::Xor)
        {
            return v1 ^ v2;
        }
        else
        {
            // unsupported operation type
            assert(false);
        }
    }

    bool isValueInsideLoop(llvm::Value *V, llvm::Loop *L)
    {
        for (llvm::BasicBlock *BB : L->blocks())
        {
            for (llvm::Instruction &I : *BB)
            {
                if (&I == V)
                {
                    return true;
                }
            }
        }
        return false;
    }

    int executeAndSimulateLoop(int start, int count, int inc, llvm::Instruction::BinaryOps b)
    {
        for (int i = 0; i < count; ++i)
        {
            start = performBinaryOp(start, inc, b);
        }
        return start;
    }

    int simulateLoop(int start, int count, int inc, llvm::Instruction::BinaryOps b)
    {
        if (b == llvm::Instruction::BinaryOps::Add)
        {
            return start + count * inc;
        }
        else if (b == llvm::Instruction::BinaryOps::Sub)
        {
            return start - count * inc;
        }
        else if (b == llvm::Instruction::BinaryOps::Mul)
        {
            return (double)start * pow(inc, count);
        }
        else if (b == llvm::Instruction::BinaryOps::UDiv || b == llvm::Instruction::BinaryOps::SDiv)
        {
            return (double)start * pow(1.0 / (double)inc, count);
        }
        else if (b == llvm::Instruction::BinaryOps::URem || b == llvm::Instruction::BinaryOps::SRem)
        {
            return executeAndSimulateLoop(start, count, inc, b);
        }
        else if (b == llvm::Instruction::BinaryOps::Shl)
        {
            return start << (count * inc);
        }
        else if (b == llvm::Instruction::BinaryOps::LShr || b == llvm::Instruction::BinaryOps::AShr)
        {
            return start >> (count * inc);
        }
        else if (b == llvm::Instruction::BinaryOps::And)
        {
            return executeAndSimulateLoop(start, count, inc, b);
        }
        else if (b == llvm::Instruction::BinaryOps::Or)
        {
            return executeAndSimulateLoop(start, count, inc, b);
        }
        else if (b == llvm::Instruction::BinaryOps::Xor)
        {
            return executeAndSimulateLoop(start, count, inc, b);
        }
        else
        {
            // unsupported operation type
            assert(false);
        }
    }

    LEAF_TYPE categorizeNode(MASNode *node, llvm::LoopAnalysis::Result *li, llvm::ScalarEvolutionAnalysis::Result *SE)
    {
        if (llvm::isa<llvm::ConstantInt>(node->getValue()))
        {
            // llvm::errs() << "TYPE OF LEAF IS = CONST \n";
            node->setLabel(CONST);
        }
        else if (llvm::isa<llvm::AllocaInst>(node->getValue()))
        {
            // llvm::errs() << "TYPE OF LEAF IS = BASE_MEM_ADDR \n";
            node->setLabel(BASE_MEM_ADDR);
        }
        else if (llvm::isa<llvm::CallInst>(node->getValue()))
        {
            // llvm::errs() << "TYPE FO LEAF IS = FUNC_RET_VAL \n";

            // Check whether this is a call to a memory allocation or deallocation
            llvm::CallInst *callInst = llvm::cast<llvm::CallInst>(node->getValue());
            llvm::Function *calledFunction = callInst->getCalledFunction();
            if (calledFunction)
            {
                // Check function name
                std::string funcName = calledFunction->getName().str();
                if (funcName == "malloc" || funcName == "calloc" || funcName == "realloc")
                {
                    node->setLabel(MEM_ALLOC);
                }
                else if (funcName == "free" || funcName == "delete" || funcName == "delete[]")
                {
                    node->setLabel(MEM_DEALLOC);
                }
                else
                {
                    node->setLabel(FUNC_RET_VAL);
                }
            }
            else
            {
                node->setLabel(FUNC_RET_VAL);
            }
        }
        // This currently assumes phi node -> loopindvar, which is only true
        // b/c of the subset of passes being run
        else if (llvm::isa<llvm::PHINode>(node->getValue()))
        {
            bool found = false;
            for (llvm::Loop *lo : li->getLoopsInPreorder())
            {
                if (lo && node->getValue() == lo->getCanonicalInductionVariable())
                {
                    auto ocnt = SE->getSmallConstantTripCount(lo);
                    if (ocnt <= 2)
                    {
                        continue;
                    }
                    found = true;
                    node->setLabel(LOOP_IND_VAR);
                    node->setLoopIndVarStart(0);      // Def of CanonicalIndVar
                    node->setLoopIndVarEnd(ocnt - 2); // For some reason this is 2 higher than it should be?
                }
                else if (isValueInsideLoop(node->getValue(), lo))
                {
                    // aggressively try to find the bounds of non canonical loop
                    // if the loop is irregular, default to calling it a DATA_DEP_VAR
                    llvm::PHINode *ph = llvm::cast<llvm::PHINode>(node->getValue());
                    llvm::Value *st = ph->getIncomingValue(0);
                    if (!llvm::isa<llvm::ConstantInt>(st))
                    {
                        continue;
                    }
                    int start = llvm::cast<llvm::ConstantInt>(st)->getSExtValue();
                    llvm::Value *inc = ph->getIncomingValue(1);
                    if (!llvm::isa<llvm::BinaryOperator>(inc))
                    {
                        continue;
                    }
                    auto ocnt = SE->getSmallConstantTripCount(lo);
                    if (ocnt == 0)
                    {
                        continue;
                    }
                    llvm::Instruction::BinaryOps b = llvm::cast<llvm::BinaryOperator>(inc)->getOpcode();
                    for (llvm::Use &U : llvm::cast<llvm::BinaryOperator>(inc)->operands())
                    {
                        if (llvm::isa<llvm::ConstantInt>(U.get()))
                        {
                            int increment = llvm::cast<llvm::ConstantInt>(U.get())->getSExtValue();
                            node->setLabel(LOOP_IND_VAR);
                            node->setLoopIndVarStart(start);
                            node->setLoopIndVarEnd(simulateLoop(start, ocnt - 2, increment, b)); // For some reason this is 2 higher than it should be?
                            found = true;
                        }
                    }
                }
            }

            if (!found)
            {
                node->setLabel(DATA_DEP_VAR);
            }
        }
        else if (llvm::isa<llvm::BinaryOperator>(node->getValue()))
        {
            // llvm::errs() << "TYPE FO LEAF IS = OPERATION \n";
            node->setLabel(OPERATION);
        }
        else if (llvm::isa<llvm::SExtInst>(node->getValue()))
        {
            // llvm::errs() << "TYPE FO LEAF IS = OPERATION \n";
            node->setLabel(OPERATION);
        }
        else
        {
            node->setLabel(UNSET);
        }

        return node->getLabel();
    }

    MASNode *searchForValue(MASNode *cur, llvm::Value *val)
    {
        if (cur->getValue() == val)
        {
            return cur;
        }
        for (MASNode *child : cur->getChildren())
        {
            MASNode *response = searchForValue(child, val);
            if (response)
            {
                return response;
            }
        }
        return nullptr;
    }

    MASNode *MAS::getNode(llvm::Value *val)
    {
        for (MASNode *r : this->getRoots())
        {
            MASNode *response = searchForValue(r, val);
            if (response)
            {
                return response;
            }
        }
        return nullptr;
    }

    bool searchOnlyStaticNodes(MASNode *node)
    {
        LEAF_TYPE lt = node->getLabel();
        if (lt != CONST && lt != LOOP_IND_VAR && lt != OPERATION)
        {
            return false;
        }
        for (MASNode *child : node->getChildren())
        {
            if (!searchOnlyStaticNodes(child))
            {
                return false;
            }
        }

        return true;
    }

    bool searchHasLoopInduction(MASNode *node)
    {
        LEAF_TYPE lt = node->getLabel();
        if (lt == LOOP_IND_VAR)
        {
            return true;
        }
        for (MASNode *child : node->getChildren())
        {
            if (searchHasLoopInduction(child))
            {
                return true;
            }
        }

        return false;
    }

    bool MASNode::isLoopInductionBased()
    {
        for (MASNode *l : this->getChildren())
        {
            if (!searchOnlyStaticNodes(l))
            {
                return false;
            }
        }
        for (MASNode *l : this->getChildren())
        {
            if (searchHasLoopInduction(l))
            {
                return true;
            }
        }

        return false;
    }

    int computeChildren(MASNode *node, bool start)
    {
        if (node->getLabel() == CONST)
        {
            assert(llvm::isa<llvm::ConstantInt>(node->getValue()));
            llvm::ConstantInt *tmp = llvm::cast<llvm::ConstantInt>(node->getValue());
            return tmp->getSExtValue();
        }
        else if (node->getLabel() == LOOP_IND_VAR)
        {
            if (start)
            {
                return node->getLoopIndVarStart();
            }
            else
            {
                return node->getLoopIndVarEnd();
            }
        }
        else if (node->getChildren().size() == 1)
        {
            // sign extension case
            // TODO modify to support more unary operators
            return computeChildren(node->getChildren()[0], start);
        }
        else if (node->getChildren().size() == 2)
        {
            assert(llvm::isa<llvm::BinaryOperator>(node->getValue()));
            llvm::BinaryOperator *tmp = llvm::cast<llvm::BinaryOperator>(node->getValue());
            int v1 = computeChildren(node->getChildren()[0], start);
            int v2 = computeChildren(node->getChildren()[1], start);
            return performBinaryOp(v1, v2, tmp->getOpcode());
        }
        else
        {
            // this case should be impossible
            assert(false);
        }
    }

    int MASNode::getTrueLoopStart()
    {
        return computeChildren(this, true);
    }

    int MASNode::getTrueLoopEnd()
    {
        return computeChildren(this, false);
    }
}