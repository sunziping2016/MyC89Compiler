#include <llvm/IR/PassManager.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/Analysis/InstructionSimplify.h>

#include "driver.h"
#include "type.h"
#include "declaration.h"
#include "expression.h"

void c89c::Driver::output(llvm::raw_ostream &out) {
    llvm::PassManager<llvm::Module> pm;
    llvm::AnalysisManager<llvm::Module> am;
    pm.addPass(llvm::PrintModulePass(out));
    pm.run(m_module, am);
}

std::unordered_map<std::string, c89c::Driver::NameItem>::iterator c89c::Driver::findInAllScope(const std::string &name) {
    std::unordered_map<std::string, NameItem>::iterator item;
    for (auto scope = m_scopes.rbegin(); scope != m_scopes.rend(); ++scope) {
        item = scope->names.find(name);
        if (item != scope->names.end())
            return item;
    }
    return item;
}
