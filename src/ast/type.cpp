#include <vector>

#include "type.h"

llvm::Type *c89c::BasicType::generate(llvm::LLVMContext &context) const {
    switch (m_type) {
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

c89c::PointerType &c89c::PointerType::operator = (const c89c::PointerType &other) {
    if (&other != this) {
        Type::operator=(other);
        m_element.reset(other.m_element->clone());
    }
    return *this;
}

c89c::ArrayType &c89c::ArrayType::operator = (const c89c::ArrayType &other) {
    if (&other != this) {
        Type::operator=(other);
        m_element.reset(other.m_element->clone());
        m_num = other.m_num;
    }
    return *this;
}

c89c::FunctionType &c89c::FunctionType::operator = (const c89c::FunctionType &other) {
    if (&other != this) {
        m_return.reset(other.m_return->clone());
        m_args.clear();
        for (const auto &arg: other.m_args)
            m_args.emplace_back(arg->clone());
        m_var_arg = other.m_var_arg;
    }
    return *this;
}

llvm::Type *c89c::FunctionType::generate(llvm::LLVMContext &context) const {
    std::vector<llvm::Type *> args;
    for (const auto &arg: m_args)
        args.push_back(arg->generate(context));
    return llvm::FunctionType::get(m_return->generate(context), args, m_var_arg);
}
