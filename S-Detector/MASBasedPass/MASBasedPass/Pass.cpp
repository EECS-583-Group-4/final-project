#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Attributes.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopIterator.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"
#include "llvm/IR/CFG.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/LoopAnalysisManager.h"

#include <iostream>
#include <string>

#include "../../../MAS/MASPass/MAS.h"

using namespace llvm;

namespace
{

    struct MASBasedPass : public PassInfoMixin<MASBasedPass>
    {

        void insertPrint(IRBuilder<> &builder, LLVMContext &context, Module *module)
        {
            Type *intType = Type::getInt32Ty(context);

            // Declare C standard library printf
            std::vector<Type *> printfArgsTypes({Type::getInt8PtrTy(context)});
            FunctionType *printfType = FunctionType::get(intType, printfArgsTypes, true);
            FunctionCallee printfFunc = module->getOrInsertFunction("printf", printfType);

            // The format string for the printf function, declared as a global literal
            Value *str = builder.CreateGlobalStringPtr("Out of bounds access detected\n", "str");
            std::vector<Value *> argsVprintf({str});
            builder.CreateCall(printfFunc, argsVprintf);

            // insert exit function
            std::vector<Type *> params({Type::getInt32Ty(context)});
            FunctionType *exitType = FunctionType::get(Type::getVoidTy(context), params, false);
            FunctionCallee exitFunc = module->getOrInsertFunction("exit", exitType);

            Value *one = ConstantInt::get(Type::getInt32Ty(context), 1);
            std::vector<Value *> argsVexit({one});
            builder.CreateCall(exitFunc, argsVexit);
        }

        void insertIfBlock(GetElementPtrInst *GEP, Value *v1, Value *v2, Function &F)
        {
            Module *module = F.getParent();
            LLVMContext &context = module->getContext();
            BasicBlock *BB = GEP->getParent();
            IRBuilder<> builder(BB);
            BasicBlock *mergeBlock = BB->splitBasicBlock(GEP, "split", false);

            BasicBlock *ifBlock = BasicBlock::Create(context, "ifBlock", &F);
            BB->getTerminator()->eraseFromParent();

            Value *condition = builder.CreateICmpSLT(v1, v2);
            builder.CreateCondBr(condition, ifBlock, mergeBlock);

            builder.SetInsertPoint(ifBlock);

            insertPrint(builder, context, module);
            builder.CreateBr(mergeBlock);
        }

        Type *getNextType(Type *curType, Value *curI)
        {
            if (isa<ArrayType>(curType))
            {
                ArrayType *ty = cast<ArrayType>(curType);
                return ty->getElementType();
            }
            else if (isa<PointerType>(curType))
            {
                PointerType *ty = cast<PointerType>(curType);
                return ty->getArrayElementType();
            }
            else if (isa<VectorType>(curType))
            {
                VectorType *ty = cast<VectorType>(curType);
                return ty->getElementType();
            }
            else if (isa<StructType>(curType))
            {
                StructType *ty = cast<StructType>(curType);
                if (dyn_cast<ConstantInt>(curI))
                {
                    ConstantInt *test = cast<ConstantInt>(curI);
                    return ty->getElementType(test->getSExtValue());
                }
                return ty->getElementType(0);
            }
            return nullptr;
        }

        struct checkDetails
        {
            GetElementPtrInst *GEP;
            Value *index;
            int numElements;
        };

        void performStaticArrayCheck(checkDetails &c, MAS::MASNode *cur_node)
        {
            size_t lowest_index = cur_node->getTrueLoopStart();
            size_t highest_index = cur_node->getTrueLoopEnd();
            if (lowest_index >= 0 && lowest_index < c.numElements)
            {
                if (highest_index >= 0 && highest_index < c.numElements)
                {
                    return;
                }
            }
            errs() << "S-DETECTOR FOUND ERROR: Array index will go out of bounds \n";
            exit(1);
        }

        void insertDynamicArrayCheck(checkDetails &c, Function &F)
        {
            Value *upper = ConstantInt::get(c.index->getType(), c.numElements - 1);
            Value *lower = ConstantInt::get(c.index->getType(), 0);
            // error if index < 0
            insertIfBlock(c.GEP, c.index, lower, F);
            // error if numElements - 1 < index
            insertIfBlock(c.GEP, upper, c.index, F);
        }

        PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM)
        {
            MAS::MAS curr_mas = MAS::MAS(&F, &FAM);
            curr_mas.calculate();
            curr_mas.print();

            std::vector<checkDetails>
                checks;
            for (auto &BB : F)
            {
                for (auto &I : BB)
                {
                    if (auto *GEP = dyn_cast<GetElementPtrInst>(&I))
                    {
                        Type *curType = GEP->getSourceElementType();
                        for (auto it = GEP->idx_begin() + 1; it != GEP->idx_end(); ++it)
                        {
                            if (!curType)
                            {
                                break;
                            }
                            Value *index = it->get();

                            if (isa<ArrayType>(curType))
                            {
                                ArrayType *arrTy = cast<ArrayType>(curType);
                                int numElements = arrTy->getNumElements();

                                if (!dyn_cast<ConstantInt>(index))
                                {
                                    checks.push_back({GEP, index, numElements});
                                }
                            }
                            curType = getNextType(curType, index);
                        }
                    }
                }
            }

            for (auto &check : checks)
            {
                MAS::MASNode *cur_node = curr_mas.getNode(check.index);
                if (cur_node && cur_node->isLoopInductionBased())
                {
                    errs() << "FOUND LOOP INDUCTION BASED " << *cur_node << "\n";
                    performStaticArrayCheck(check, cur_node);
                }
                else
                {
                    insertDynamicArrayCheck(check, F);
                }
            }

            return PreservedAnalyses::all();
        }
    };
}

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo()
{
    return {
        LLVM_PLUGIN_API_VERSION, "MASBasedPass", "v0.1",
        [](PassBuilder &PB)
        {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>)
                {
                    if (Name == "masbased-pass")
                    {
                        FPM.addPass(MASBasedPass());
                        return true;
                    }
                    return false;
                });
        }};
}
