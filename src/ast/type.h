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
        virtual bool equal(const Type &other) const {
            return m_const == other.m_const
                && m_volatile == other.m_volatile
                && typeid(*this) == typeid(other);
        }

        virtual llvm::Type *generate(llvm::LLVMContext &context) const = 0;

        bool isConst() const { return m_const; }
        bool isVolatile() const { return m_volatile; }

        void setConst(bool c) { m_const = c; }
        void setVolatile(bool v) { m_volatile = v; }

    private:
        bool m_const, m_volatile;
    };

    class BasicType: public Type {
    public:
        enum TypeFlag {VOID, CHAR, SIGNED_CHAR, SHORT, INT, LONG, UNSIGNED_CHAR,
                UNSIGNED_SHORT, UNSIGNED_INT, UNSIGNED_LONG,
                FLOAT, DOUBLE, LONG_DOUBLE};

        explicit BasicType(TypeFlag type): type(type) {}

        bool equal(const Type &other) const override {
            if (Type::equal(other)) {
                const auto &other_ref = static_cast<const BasicType &>(other); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
                return type == other_ref.type;
            }
            return false;
        }

        llvm::Type *generate(llvm::LLVMContext &context) const override {
            switch (type) {
                case VOID:
                    return llvm::Type::getVoidTy(context);
                case CHAR: case SIGNED_CHAR: case UNSIGNED_CHAR:
                    return llvm::Type::getInt8Ty(context);
                case SHORT: case UNSIGNED_SHORT:
                    return llvm::Type::getInt16Ty(context);
                case INT: case UNSIGNED_INT:
                    return llvm::Type::getInt32Ty(context);
                case LONG: case UNSIGNED_LONG:
                    return llvm::Type::getInt64Ty(context);
                case FLOAT:
                    return llvm::Type::getFloatTy(context);
                case DOUBLE:
                    return llvm::Type::getDoubleTy(context);
                case LONG_DOUBLE:
                    return llvm::Type::getFP128Ty(context);
                default:
                    assert(0);
            }
        }

    private:
        TypeFlag type;
    };

    class ArrayType: public Type {
    public:
        ArrayType(std::unique_ptr<Type> &&element, uint64_t num):
            element(std::move(element)), num(num) {}

        bool equal(const Type &other) const override {
            if (Type::equal(other)) {
                const auto &other_ref = static_cast<const ArrayType &>(other); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
                return num == other_ref.num && element->equal(*other_ref.element);
            }
            return false;
        }

        llvm::Type *generate(llvm::LLVMContext &context) const override {
            return llvm::ArrayType::get(element->generate(context), num);
        }

    private:
        std::unique_ptr<Type> element;
        uint64_t num;
    };
}

#endif //MYC89COMPILER_TYPE_H
