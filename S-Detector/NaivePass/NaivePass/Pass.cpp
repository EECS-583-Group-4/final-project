#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>

using namespace llvm;

namespace {

struct NaivePass : public PassInfoMixin<NaivePass> {

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

                    // Print the indices being used to access the pointer
                    errs() << "Indices being used to access the pointer:\n";
                    for (auto &Index : GEP->indices()) {
                        Index->print(errs());
                        errs() << "\n";
                    }

                    // TODO: Insert instructions to check if current operand is in bounds based on current type
                    
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