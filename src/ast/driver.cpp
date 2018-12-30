#include <llvm/IR/PassManager.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/Analysis/InstructionSimplify.h>

#include "driver.h"

void c89c::Driver::output(llvm::raw_ostream &out) {
    llvm::PassManager<llvm::Module> pm;
    llvm::AnalysisManager<llvm::Module> am;
    pm.addPass(llvm::PrintModulePass(out));
    pm.run(m_module, am);
}
