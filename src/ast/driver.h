#ifndef MYC89COMPILER_CONTEXT_H
#define MYC89COMPILER_CONTEXT_H

#include <unordered_map>
#include <string>
#include <memory>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/raw_ostream.h>

namespace c89c {
    class Value;
    class Type;

    class Scope {
        friend class Driver;
    private:
        std::unordered_map<std::string, std::unique_ptr<Value>> m_values;
        std::unordered_map<std::string, std::unique_ptr<Type>> m_types;
    };

    class Driver {
    public:
        explicit Driver(const llvm::StringRef &module_id): m_module(module_id, m_context) {}

        llvm::LLVMContext &context() { return m_context; }

        void output(llvm::raw_ostream &out);

    private:
        llvm::LLVMContext m_context;
        llvm::Module m_module;
        Scope m_global, m_local;
    };
}

#endif //MYC89COMPILER_CONTEXT_H
