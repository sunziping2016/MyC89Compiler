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

        virtual bool isVoidType() const { return false; }
        virtual bool isSignedIntegerType() const { return false; }
        virtual bool isUnsignedIntegerType() const { return false; }
        virtual bool isIntegerType() const { return false; }
        virtual bool isFloatingType() const { return false; }

        bool isObjectType() const { return !isFunctionType(); }
        virtual bool isFunctionType() const { return false; }
        virtual bool isArrayType() const { return false; }

        virtual bool isIncompleteType() const { return false; }

        virtual bool equal(const Type &other) const {
            return m_const == other.m_const
                   && m_volatile == other.m_volatile
                   && typeid(*this) == typeid(other);
        }

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

        bool isVoidType() const override { return true; }
        bool isIncompleteType() const override { return true; }
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
        bool isFloatingType() const override {
            return m_type == FLOAT || m_type == DOUBLE || m_type == LONG_DOUBLE;
        }

        bool equal(const Type &other) const override {
            if (Type::equal(other)) {
                const auto &other_ref = static_cast<const BasicType &>(other); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
                return m_type == other_ref.m_type;
            }
            return false;
        }
    private:
        TypeFlag m_type;
    };

    class PointerType: public Type {
    public:
        explicit PointerType(std::unique_ptr<Type> &&element): m_element(std::move(element)) {}
        PointerType(const PointerType &other): Type(other), m_element(other.m_element->clone()) {}
        PointerType &operator = (const PointerType &other);

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
        friend class FunctionType;
    public:
        explicit ArrayType(std::unique_ptr<Type> &&element, uint64_t num = 0, bool incomplete = false):
                m_element(std::move(element)), m_num(num), m_incomplete(incomplete) {}
        ArrayType(const ArrayType &other):
            Type(other), m_element(other.m_element->clone()),
            m_num(other.m_num), m_incomplete(other.m_incomplete) {}
        ArrayType &operator = (const ArrayType &other);

        ArrayType *clone() const override {
            return new ArrayType(*this);
        }
        PointerType *toPointerType() const {
            return new PointerType(std::unique_ptr<Type>(m_element->clone()));
        }
        llvm::Type *generate(llvm::LLVMContext &context) const override {
            return llvm::ArrayType::get(m_element->generate(context), m_num);
        }

        bool isArrayType() const override { return true; }
        bool isIncompleteType() const override { return m_incomplete; }

        bool equal(const Type &other) const override {
            if (Type::equal(other)) {
                const auto &other_ref = static_cast<const ArrayType &>(other); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
                return m_num == other_ref.m_num && m_element->equal(*other_ref.m_element);
            }
            return false;
        }
    private:
        std::unique_ptr<Type> m_element;
        uint64_t m_num;
        bool m_incomplete;
    };

    class FunctionType: public Type {
    public:
        FunctionType(std::unique_ptr<Type> &&ret, std::vector<std::unique_ptr<Type>> &&args, bool var_arg):
                m_return(std::move(ret)), m_args(std::move(args)), m_var_arg(var_arg) {}

        FunctionType(const FunctionType &other):
                Type(other), m_return(std::unique_ptr<Type>(other.m_return->clone())), m_var_arg(other.m_var_arg) {
            for (const auto &arg: other.m_args)
                m_args.emplace_back(arg->clone());
        }
        FunctionType &operator = (const FunctionType &other);

        FunctionType *clone() const override {
            return new FunctionType(*this);
        }
        PointerType *toPointerType() const {
            return new PointerType(std::unique_ptr<Type>(clone()));
        }
        llvm::Type *generate(llvm::LLVMContext &context) const override;

        bool isFunctionType() const override { return true; }

        bool equal(const Type &other) const override;

        std::unique_ptr<Type> &returnType() {
            return m_return;
        }
    private:
        std::unique_ptr<Type> m_return;
        std::vector<std::unique_ptr<Type>> m_args;
        bool m_var_arg;
    };
}

#endif //MYC89COMPILER_TYPE_H
