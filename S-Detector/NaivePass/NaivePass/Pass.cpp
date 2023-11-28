#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>
#include <string>

using namespace llvm;

namespace {

struct NaivePass : public PassInfoMixin<NaivePass> {

    void insertPrint(Instruction & I, Module * module){
        LLVMContext &context = module->getContext();
        IRBuilder<> builder(context);

        Type *intType = Type::getInt32Ty(context);

        // Declare C standard library printf 
        std::vector<Type *> printfArgsTypes({Type::getInt8PtrTy(context)});
        FunctionType *printfType = FunctionType::get(intType, printfArgsTypes, true);
        FunctionCallee printfFunc = module->getOrInsertFunction("printf", printfType);

        // The format string for the printf function, declared as a global literal
        //Value *str = builder.CreateGlobalStringPtr(StringRef("Out of bounds access detected\n"));
        // Value* str = ("Out of bounds access detected\n");
        // std::vector<Value *> argsV({str});
        // CallInst * newPrint = CallInst::Create(printfFunc, argsV);
        // I.insertAfter(newPrint);
    }

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
        // References:
        // https://llvm.org/docs/GetElementPtr.html
        // https://llvm.org/docs/LangRef.html#getelementptr-instruction
        // https://www.cs.cornell.edu/courses/cs6120/2019fa/blog/gep/

        // Naive algorithm:
        //     for each call to getelementptr:
        //         cur_type = first type
        //         cur_index = first index
        //         for each index in getelementptr:
        //             // since we are only considering fixed size, the bounds are available in the type
        //             insert instructions to ensure cur_index is in bounds of cur_type
        //             cur_type = convert cur_type (see table in google docs)
        //             cur_index = next index

        std::cout << "Function Name: " << F.getName().str() << "\n";
        
        // Find all calls to getelementptr
        for (auto &BB : F) {
            for (auto &I : BB) {
                if (auto *GEP = dyn_cast<GetElementPtrInst>(&I)) {
                    errs() << "--------------\n";
                    errs() << "Found getelementptr instruction:\n";
                    GEP->print(errs());
                    errs() << "\n";

                    // Print the type being accessed by the getelementptr
                    errs() << "Type of data structure being accessed:\n";
                    GEP->getSourceElementType()->print(errs());
                    errs() << "\n";

                    Type *cur_type = GEP->getSourceElementType();

                    // Print the pointer being accessed by the getelementptr
                    errs() << "Pointer being accessed:\n";
                    GEP->getPointerOperand()->print(errs());
                    errs() << "\n";

                    Value *cur_ptr = GEP->getPointerOperand();
                    
                    Type * eleType = GEP->getSourceElementType();
                    uint64_t numElements = 0;
                    if (isa<ArrayType>(eleType)) {
                        ArrayType *arrTy = cast<ArrayType>(eleType);
                        numElements = arrTy->getNumElements();

                        errs() << "Array size: " << numElements << "\n";
                    }
                    
                    // Print the indices being used to access the pointer
                    errs() << "Indices being used to access the pointer:\n";
                    for (auto &Index : GEP->indices()) {
                        Index->print(errs());

                        // Insert instructions to ensure cur_index is in bounds of cur_type

                        // If there is an out of bounds error, add instruction to print the error
                        // if () {
                            insertPrint(I, F.getParent());
                        // }

                        errs() << "\n";
                    }
                    
                    
                    errs() << "--------------\n";
                }
            }
        }


        return PreservedAnalyses::all();
    }
};
}

extern "C"::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "NaivePass", "v0.1",
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                ArrayRef<PassBuilder::PipelineElement>) {
                    if(Name == "func-name"){
                        FPM.addPass(NaivePass());
                        return true;
                    }
                    return false;
                }
            );
        }
    };
}