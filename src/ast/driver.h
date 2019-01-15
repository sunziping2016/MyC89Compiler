#ifndef MYC89COMPILER_DRIVER_H
#define MYC89COMPILER_DRIVER_H

#include <unordered_map>
#include <string>
#include <memory>
#include <variant>
#include <vector>
#include <cassert>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/raw_ostream.h>

namespace c89c {
    class Value;
    class Type;
    class FunctionType;

    class Driver {
    public:
        typedef std::variant<std::unique_ptr<Value>, std::unique_ptr<Type>> NameItem;
        struct Scope {
            std::unordered_map<std::string, NameItem> names;
        };
        struct FunctionPrototypeItem {
            enum Linkage {EXTERNAL, INTERNAL} linkage;
            std::unique_ptr<FunctionType> type;
        };

        explicit Driver(const llvm::StringRef &module_id):
                m_module(module_id, m_context), m_want_identifier(false),
                m_function(nullptr), m_block(nullptr) {
            m_scopes.emplace_back();
        }
        llvm::LLVMContext &context() { return m_context; }
        llvm::Module &module() { return m_module; }

        void output(llvm::raw_ostream &out);

        Scope &topScope() {
            return m_scopes.back();
        }
        std::unordered_map<std::string, NameItem>::iterator findInAllScope(const std::string &name);
        std::unordered_map<std::string, NameItem>::iterator allScopeEnd() {
            return m_scopes.front().names.end();
        }
        std::unordered_map<std::string, FunctionPrototypeItem>::iterator findInAllFunctions(const std::string &name) {
            return m_functions.find(name);
        }
        std::unordered_map<std::string, FunctionPrototypeItem>::iterator allFunctionsEnd() {
            return m_functions.end();
        }
        void addToAllFunctions(const std::string &name, FunctionPrototypeItem &&item) {
            m_functions[name] = std::move(item);
        }

        bool wantIdentifier() const {
            return m_want_identifier;
        }
        void setWantIdentifier(bool want_identifier) {
            m_want_identifier = want_identifier;
        }

        llvm::Function *function() {
            return m_function;
        }
        void setFunction(llvm::Function *function) {
            m_function = function;
        }
        bool isGlobal() {
            return m_function == nullptr;
        }

        llvm::BasicBlock *block() {
            return m_block;
        }
        void setBasicBlock(llvm::BasicBlock *block) {
            m_block = block;
        }
    private:
        llvm::LLVMContext m_context;
        llvm::Module m_module;

        std::vector<Scope> m_scopes;
        std::unordered_map<std::string, FunctionPrototypeItem> m_functions;
        bool m_want_identifier;

        llvm::Function *m_function;
        llvm::BasicBlock *m_block;
    };
}

#endif //MYC89COMPILER_DRIVER_H
