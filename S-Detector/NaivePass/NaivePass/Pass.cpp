#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"

#include <iostream>
#include <string>

using namespace llvm;

namespace
{

    struct NaivePass : public PassInfoMixin<NaivePass>
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
            std::vector<Value *> argsV({str});
            CallInst *call = builder.CreateCall(printfFunc, argsV);
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

        PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM)
        {
            std::vector<GetElementPtrInst *> arrays;
            for (auto &BB : F)
            {
                for (auto &I : BB)
                {
                    if (auto *GEP = dyn_cast<GetElementPtrInst>(&I))
                    {
                        arrays.push_back(GEP);
                    }
                }
            }

            for (auto &GEP : arrays)
            {
                Type *eleType = GEP->getSourceElementType();
                if (isa<ArrayType>(eleType))
                {
                    for (auto &Index : GEP->indices())
                    {
                        ArrayType *arrTy = cast<ArrayType>(eleType);
                        int numElements = arrTy->getNumElements();
                        Value *actual = Index.get();
                        if (!dyn_cast<ConstantInt>(actual))
                        {
                            Value *upper = ConstantInt::get(actual->getType(), numElements - 1);
                            Value *lower = ConstantInt::get(actual->getType(), 0);
                            // error if actual < 0
                            insertIfBlock(GEP, actual, lower, F);
                            // error if numElements - 1 < actual
                            insertIfBlock(GEP, upper, actual, F);
                        }
                    }
                }
            }

            return PreservedAnalyses::all();
        }
    };
}

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo()
{
    return {
        LLVM_PLUGIN_API_VERSION, "NaivePass", "v0.1",
        [](PassBuilder &PB)
        {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>)
                {
                    if (Name == "naive-pass")
                    {
                        FPM.addPass(NaivePass());
                        return true;
                    }
                    return false;
                });
        }};
}
