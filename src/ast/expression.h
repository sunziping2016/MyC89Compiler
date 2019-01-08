#include <utility>

#ifndef MYC89COMPILER_EXPRESSION_H
#define MYC89COMPILER_EXPRESSION_H

#include <memory>

#include <llvm/ADT/APInt.h>

namespace c89c {
    class Type;

    class Expression {
    public:
        Expression(): m_lvalue(false) {}
        explicit Expression(std::unique_ptr<Type> &&type, bool lvalue = false):
            m_type(std::move(type)), m_lvalue(lvalue) {}

        Type *getType() const {
            return m_type.get();
        }
        bool isLvalue() const {
            return m_lvalue;
        }

        virtual bool isIntegerConstantExpression() const {
            return false;
        }
        virtual llvm::APInt getIntegerConstantValue() const {
            return llvm::APInt::getNullValue(32);
        }
        virtual ~Expression() = default;
    protected:
        std::unique_ptr<Type> m_type;
        bool m_lvalue;
    };

    class TodoExpression: public Expression {};

    class IntegerConstant: public Expression {
    public:
        IntegerConstant(llvm::APInt value, std::unique_ptr<Type> &&type);

        bool isIntegerConstantExpression() const override {
            return true;
        }
        llvm::APInt getIntegerConstantValue() const override {
            return m_value;
        }
    private:
        llvm::APInt m_value;
    };

    class CharacterConstant: public IntegerConstant {
    public:
        explicit CharacterConstant(int value);
    };
}

#endif //MYC89COMPILER_EXPRESSION_H
