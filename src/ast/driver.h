#ifndef MYC89COMPILER_CONTEXT_H
#define MYC89COMPILER_CONTEXT_H

#include <unordered_map>
#include <string>
#include <memory>
#include <variant>
#include <vector>
#include <cassert>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/raw_ostream.h>

namespace c89c {
    class Value;
    class Type;

    typedef std::variant<std::unique_ptr<Value>, std::unique_ptr<Type>> ItemType;
    typedef std::unordered_map<std::string, ItemType> ScopeType;

    class Driver {
    public:
        explicit Driver(const llvm::StringRef &module_id):
                m_module(module_id, m_context), m_want_typename(true) {
            m_scopes.emplace_back();
        }
        llvm::LLVMContext &context() { return m_context; }
        void output(llvm::raw_ostream &out);

        ScopeType &topScope() {
            return m_scopes.back();
        }
        ScopeType::iterator findInAllScope(const std::string &name) {
            ScopeType::iterator item;
            for (auto scope = m_scopes.rbegin(); scope != m_scopes.rend(); ++scope) {
                item = scope->find(name);
                if (item != scope->end())
                    return item;
            }
            return item;

        }
        ScopeType::iterator allScopeEnd() {
            return m_scopes.front().end();
        }

        bool wantTypename() const {
            return m_want_typename;
        }
        void setWantTypename(bool want_typename) {
            m_want_typename = want_typename;
        }

    private:
        llvm::LLVMContext m_context;
        llvm::Module m_module;

        std::vector<ScopeType> m_scopes;
        bool m_want_typename;
    };
}

#endif //MYC89COMPILER_CONTEXT_H
