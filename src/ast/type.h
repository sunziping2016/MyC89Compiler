#ifndef MYC89COMPILER_TYPE_H
#define MYC89COMPILER_TYPE_H

#include <memory>
#include <typeinfo>
#include <cassert>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>

namespace c89c {
    class Type {
    public:
        Type(): m_const(false), m_volatile(false) {}

        virtual ~Type() = default;

        virtual Type *clone() const = 0;
        virtual llvm::Type *generate(llvm::LLVMContext &context) const = 0;

        bool isConst() const { return m_const; }
        bool isVolatile() const { return m_volatile; }

        void setConst(bool c) { m_const = c; }
        void setVolatile(bool v) { m_volatile = v; }

        virtual bool isSignedIntegerType() const { return false; }
        virtual bool isUnsignedIntegerType() const { return false; }
        virtual bool isIntegerType() const { return false; }

    private:
        bool m_const, m_volatile;
    };

    class VoidType: public Type {
    public:
        VoidType *clone() const override {
            return new VoidType(*this);
        }
        llvm::Type *generate(llvm::LLVMContext &context) const override {
            return llvm::Type::getVoidTy(context);
        }
    };

    class BasicType: public Type {
    public:
        enum TypeFlag {CHAR, SIGNED_CHAR, SHORT, INT, LONG, UNSIGNED_CHAR,
                UNSIGNED_SHORT, UNSIGNED_INT, UNSIGNED_LONG,
                FLOAT, DOUBLE, LONG_DOUBLE};

        explicit BasicType(TypeFlag type): m_type(type) {}

        TypeFlag getTypeFlag() {
            return m_type;
        }
        BasicType *clone() const override {
            return new BasicType(*this);
        }
        llvm::Type *generate(llvm::LLVMContext &context) const override;

        bool isSignedIntegerType() const override {
            return m_type == SIGNED_CHAR || m_type == SHORT ||
                m_type == INT || m_type == LONG;
        }
        bool isUnsignedIntegerType() const override {
            return m_type == UNSIGNED_CHAR || m_type == UNSIGNED_SHORT ||
                m_type == UNSIGNED_INT || m_type == UNSIGNED_LONG;
        }
        bool isIntegerType() const override {
            return m_type == CHAR || isSignedIntegerType() || isUnsignedIntegerType();
        }

    private:
        TypeFlag m_type;
    };

    class PointerType: public Type {
    public:
        explicit PointerType(std::unique_ptr<Type> &&element): m_element(std::move(element)) {}
        PointerType(const PointerType &other): Type(other), m_element(other.m_element->clone()) {}
        PointerType &operator = (const PointerType &other) {
            Type::operator = (other);
            m_element.reset(other.m_element->clone());
            return *this;
        }

        PointerType *clone() const override {
            return new PointerType(*this);
        }
        llvm::Type *generate(llvm::LLVMContext &context) const override {
            return llvm::PointerType::getUnqual(m_element->generate(context));
        }
    private:
        std::unique_ptr<Type> m_element;
    };

    class ArrayType: public Type {
    public:
        explicit ArrayType(std::unique_ptr<Type> &&element, uint64_t num = 0, bool incomplete = false):
                m_element(std::move(element)), m_num(num), m_incomplete(incomplete) {}
        ArrayType(const ArrayType &other):
            Type(other), m_element(other.m_element->clone()),
            m_num(other.m_num), m_incomplete(other.m_incomplete) {}
        ArrayType &operator = (const ArrayType &other) {
            Type::operator = (other);
            m_element.reset(other.m_element->clone());
            m_num = other.m_num;
            return *this;
        }

        ArrayType *clone() const override {
            return new ArrayType(*this);
        }
        llvm::Type *generate(llvm::LLVMContext &context) const override {
            return llvm::ArrayType::get(m_element->generate(context), m_num);
        }

    private:
        std::unique_ptr<Type> m_element;
        uint64_t m_num;
        bool m_incomplete;
    };
}

#endif //MYC89COMPILER_TYPE_H
