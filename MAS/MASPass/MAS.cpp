#include "MAS.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/Transforms/Utils/Mem2Reg.h"

namespace MAS
{

    LEAF_TYPE categorizeNode(MASNode *node, llvm::LoopAnalysis::Result *li, llvm::ScalarEvolutionAnalysis::Result *SE);
    MASNode *getUD(MASNode *node, llvm::LoopAnalysis::Result *li, llvm::ScalarEvolutionAnalysis::Result *SE);

    const char *READ_LEAF_TYPE[] = {"CONST", "BASE_MEM_ADDR", "FUNC_PARAM", "DATA_DEP_VAR", "FUNC_RET_VAL", "LOOP_IND_VAR", "OPERATION", "UNSET"};

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
        return ULONG_MAX;
    }

    size_t MASNode::getLoopIndVarEnd() const
    {
        if (this->label == LOOP_IND_VAR)
        {
            return this->loop_ind_var_end;
        }
        return ULONG_MAX;
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
                if (ty == UNSET || ty == OPERATION)
                {
                    getUD(child, li, SE);
                    // opcnt+=1;
                }
                else
                {
                    // If we wanted to add a property to nodes to specificly mark them
                    // as leaf nodes when they are, this is where we would set that
                }
            }
        }
        // Not sure we need this anymore
        else if (v != nullptr)
        {
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

    LEAF_TYPE categorizeNode(MASNode *node, llvm::LoopAnalysis::Result *li, llvm::ScalarEvolutionAnalysis::Result *SE)
    {
        if (llvm::isa<llvm::Constant>(node->getValue()))
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
            node->setLabel(FUNC_RET_VAL);
        }
        // This currently assumes phi node -> loopindvar, which is only true
        // b/c of the subset of passes being run
        else if (llvm::isa<llvm::PHINode>(node->getValue()))
        {
            for (llvm::Loop *lo : li->getLoopsInPreorder())
            {
                if (node->getValue() == lo->getCanonicalInductionVariable())
                {
                    node->setLabel(LOOP_IND_VAR);
                    node->setLoopIndVarStart(0); // Def of CanonicalIndVar
                    auto cnt = SE->getBackedgeTakenCount(lo);
                    auto ocnt = SE->getSmallConstantTripCount(lo);
                    node->setLoopIndVarEnd(ocnt - 2); // For some reason this is 2 higher than it should be?
                }
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
        bool loopInductionBased = true;
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

    size_t MASNode::getTrueLoopStart()
    {
        llvm::errs() << "CHECKING LOOP START\n"
                     << *this;

        for (MASNode *l : this->getChildren())
        {
        }
        return 0;
    }

    size_t MASNode::getTrueLoopEnd()
    {

        for (MASNode *l : this->getChildren())
        {
            // Check if this is an arithmetic operation
            if (l->getLabel() == OPERATION)
            {
                //
                // Get the type of operation
                llvm::Value *v = l->getValue();

                if (llvm::Instruction *I = llvm::dyn_cast<llvm::Instruction>(v))
                {
                    if (I->getOpcode() == llvm::Instruction::Add)
                    {
                        //
                    }
                }
            }
        }
        return 0;
    }
}