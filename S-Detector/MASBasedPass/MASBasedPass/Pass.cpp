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
                Type *curType = GEP->getSourceElementType();
                for (auto it = GEP->idx_begin() + 1; it != GEP->idx_end(); ++it)
                {
                    if (!curType)
                    {
                        break;
                    }
                    Value *actual = it->get();

                    if (isa<ArrayType>(curType))
                    {
                        ArrayType *arrTy = cast<ArrayType>(curType);
                        int numElements = arrTy->getNumElements();

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
                    curType = getNextType(curType, actual);
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
