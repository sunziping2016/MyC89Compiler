#ifndef MYC89COMPILER_DECLARATION_H
#define MYC89COMPILER_DECLARATION_H

#include <cstdint>
#include <bitset>
#include <memory>
#include <string>
#include <cassert>
#include <vector>

#include <llvm/IR/Value.h>

namespace c89c {
    class Type;
    class Expression;
    class Driver;

    class TypeSpecifier {
        friend class DeclarationSpecifiers;

    public:
        enum TypeSpecifierFlag {VOID, CHAR, SHORT, INT, LONG, FLOAT, DOUBLE,
                SIGNED, UNSIGNED, STRUCT_OR_UNION /* TODO */, ENUM /* TODO */, TYPENAME,
                MAX_TYPE_SPECIFIER_FLAG /* used for counting */ };
        void set(TypeSpecifierFlag flag, std::unique_ptr<Type> &&type = nullptr) {
            m_flag = flag;
            m_type = std::move(type);
        }

    private:
        TypeSpecifierFlag m_flag;
        std::unique_ptr<Type> m_type;
    };

    class StorageClassSpecifier {
        friend class DeclarationSpecifiers;

    public:
        enum StorageClassSpecifierFlag {TYPEDEF, EXTERN, STATIC, AUTO, REGISTER, NOT_SPECIFIED};
        void set(StorageClassSpecifierFlag flag) {
            m_flag = flag;
        }
    private:
        StorageClassSpecifierFlag m_flag;
    };

    class TypeQualifier {
        friend class DeclarationSpecifiers;
        friend class TypeQualifierList;

    public:
        enum TypeQualifierFlag {CONST, VOLATILE};
        void set(TypeQualifierFlag flag) {
            m_flag = flag;
        }

    private:
        TypeQualifierFlag m_flag;
    };

    class DeclarationSpecifiers {
        friend class Declaration;
    public:
        DeclarationSpecifiers():
            m_storage(StorageClassSpecifier::NOT_SPECIFIED),
            m_type_specifiers{0},
            m_const(false), m_volatile(false) {}

        void add(StorageClassSpecifier &&specifier);
        void add(TypeSpecifier &&specifier);
        void add(TypeQualifier &&qualifier);

        std::unique_ptr<Type> getType() const;
    private:
        StorageClassSpecifier::StorageClassSpecifierFlag m_storage;
        std::bitset<TypeSpecifier::MAX_TYPE_SPECIFIER_FLAG> m_type_specifiers;
        bool m_const, m_volatile;
        std::unique_ptr<Type> m_type;
    };

    class TypeQualifierList {
        friend class PointerDeclarator;
    public:
        void add(TypeQualifier &&qualifier);

    private:
        bool m_const, m_volatile;
    };

    class Initializer {
    public:
        ~Initializer() = default;
    };

    class ExpressionInitializer: public Initializer {
    public:
        explicit ExpressionInitializer(std::unique_ptr<Expression> &&expr): m_expr(std::move(expr)) {}
    private:
        std::unique_ptr<Expression> m_expr;
    };

    class InitializerList: public Initializer {
    public:
        void add(std::unique_ptr<Initializer> &&initializer) {
            m_initializers.push_back(std::move(initializer));
        }
    private:
        std::vector<std::unique_ptr<Initializer>> m_initializers;
    };

    class Declarator {
    public:
        virtual const std::string &identifier() const = 0;

        virtual std::unique_ptr<Type> getType(std::unique_ptr<Type> &&base_type) const = 0;
        virtual void setBase(std::unique_ptr<Declarator> &&base) = 0;
    };

    class PointerDeclarator: public Declarator {
    public:
        PointerDeclarator(): m_const(false), m_volatile(false) {}
        explicit PointerDeclarator(TypeQualifierList &&list): m_const(list.m_const), m_volatile(list.m_volatile) {}

        const std::string &identifier() const override;
        std::unique_ptr<Type> getType(std::unique_ptr<Type> &&base_type) const override;
        void setBase(std::unique_ptr<Declarator> &&base) override;
    private:
        std::unique_ptr<Declarator> m_base;
        bool m_const, m_volatile;
    };

    class IdentifierDeclarator: public Declarator {
    public:
        // Empty identifier means an abstract declarator
        explicit IdentifierDeclarator(std::string identifier = ""): m_identifier(std::move(identifier)) {}

        const std::string &identifier() const override {
            return m_identifier;
        }

        std::unique_ptr<Type> getType(std::unique_ptr<Type> &&base_type) const override {
            return std::move(base_type);
        }
        void setBase(std::unique_ptr<Declarator> &&base) override {
            assert(0);
        }

    private:
        std::string m_identifier;
    };

    class ArrayDeclarator: public Declarator {
    public:
        ArrayDeclarator(): m_num(0), m_incomplete(true) {}
        explicit ArrayDeclarator(std::unique_ptr<Expression> &&expression);

        const std::string &identifier() const override;
        std::unique_ptr<Type> getType(std::unique_ptr<Type> &&base_type) const override;
        void setBase(std::unique_ptr<Declarator> &&base) override;
    private:
        std::unique_ptr<Declarator> m_base;
        uint64_t m_num;
        bool m_incomplete;
    };

    class InitDeclarator {
        friend class Declaration;
    public:
        explicit InitDeclarator(std::unique_ptr<Declarator> &&declarator):
            m_declarator(std::move(declarator)) {}
        InitDeclarator(std::unique_ptr<Declarator> &&declarator, std::unique_ptr<Initializer> &&initializer):
            m_declarator(std::move(declarator)), m_initializer(std::move(initializer)) {}

    private:
        std::unique_ptr<Declarator> m_declarator;
        std::unique_ptr<Initializer> m_initializer;
    };

    class InitDeclaratorList {
        friend class Declaration;
    public:
        void add(std::unique_ptr<InitDeclarator> &&declarator) {
            m_declarators.push_back(std::move(declarator));
        }
    private:
        std::vector<std::unique_ptr<InitDeclarator>> m_declarators;
    };

    class Declaration {
    public:
        explicit Declaration(std::unique_ptr<DeclarationSpecifiers> &&specifier)
            : m_storage(specifier->m_storage), m_base_type(specifier->getType()) {}
        Declaration(std::unique_ptr<DeclarationSpecifiers> &&specifier,
                std::unique_ptr<InitDeclaratorList> &&declarators)
            : m_storage(specifier->m_storage), m_base_type(specifier->getType()),
            m_declarators(std::move(declarators->m_declarators)) {}

        void generate(Driver &driver);
    private:
        StorageClassSpecifier::StorageClassSpecifierFlag m_storage;
        std::unique_ptr<Type> m_base_type;
        std::vector<std::unique_ptr<InitDeclarator>> m_declarators;
    };

    class Value {
    public:
        virtual bool isDefinition() const { return true; }

        virtual llvm::Value *get(Driver &driver) = 0;

    };

    class ExternValue: public Value {
    public:
        ExternValue(): m_value(nullptr) {}
        bool isDefinition() const override { return false; }

        llvm::Value *get(Driver &driver) override;

    private:
        llvm::Value *m_value;
    };
}

#endif //MYC89COMPILER_DECLARATION_H
