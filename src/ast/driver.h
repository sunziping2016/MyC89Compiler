#ifndef MYC89COMPILER_CONTEXT_H
#define MYC89COMPILER_CONTEXT_H

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/raw_ostream.h>

namespace c89c {
    class Driver {
    public:
        explicit Driver(const llvm::StringRef &module_id): m_module(module_id, m_context) {}

        llvm::LLVMContext &context() { return m_context; }

        void output(llvm::raw_ostream &out);

    private:
        llvm::LLVMContext m_context;
        llvm::Module m_module;
    };
}

#endif //MYC89COMPILER_CONTEXT_H
