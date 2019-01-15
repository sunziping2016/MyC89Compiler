#include <vector>

#include "type.h"

bool c89c::Type::equal(const c89c::Type &other) const {
    return m_const == other.m_const
           && m_volatile == other.m_volatile
           && typeid(*this) == typeid(other);
}

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

bool c89c::BasicType::equal(const c89c::Type &other) const {
    if (Type::equal(other)) {
        const auto &other_ref = static_cast<const BasicType &>(other); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
        return m_type == other_ref.m_type;
    }
    return false;
}

bool c89c::BasicType::compatible(const c89c::Type &other) const {
    if (Type::compatible(other)) {
        const auto &other_ref = static_cast<const BasicType &>(other); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
        return m_type == other_ref.m_type;
    }
    return false;
}

c89c::PointerType &c89c::PointerType::operator = (const c89c::PointerType &other) {
    if (&other != this) {
        Type::operator=(other);
        m_element.reset(other.m_element->clone());
    }
    return *this;
}

bool c89c::PointerType::compatible(const c89c::Type &other) const {
    if (Type::compatible(other)) {
        const auto &other_ref = static_cast<const PointerType &>(other); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
        return m_element->compatible(*other_ref.m_element);
    }
    return false;
}

c89c::ArrayType &c89c::ArrayType::operator = (const c89c::ArrayType &other) {
    if (&other != this) {
        Type::operator=(other);
        m_element.reset(other.m_element->clone());
        m_num = other.m_num;
    }
    return *this;
}

bool c89c::ArrayType::equal(const c89c::Type &other) const {
    if (Type::equal(other)) {
        const auto &other_ref = static_cast<const ArrayType &>(other); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
        return m_incomplete == other_ref.m_incomplete && m_num == other_ref.m_num && m_element->equal(*other_ref.m_element);
    }
    return false;
}

bool c89c::ArrayType::compatible(const c89c::Type &other) const {
    if (Type::compatible(other)) {
        const auto &other_ref = static_cast<const ArrayType &>(other); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
        return (m_incomplete || other_ref.m_incomplete || m_num == other_ref.m_num) && m_element->equal(*other_ref.m_element);
    }
    return false;
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

bool c89c::FunctionType::equal(const c89c::Type &other) const {
    if (Type::equal(other)) {
        const auto &other_ref = static_cast<const FunctionType &>(other); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
        if (m_var_arg != other_ref.m_var_arg ||
            m_args.size() != other_ref.m_args.size() ||
            !m_return->equal(*other_ref.m_return))
            return false;
        for (decltype(m_args.size()) i = 0; i < m_args.size(); ++i)
            if (!m_args[i]->equal(*other_ref.m_args[i]))
                return false;
        return true;
    }
    return false;
}

bool c89c::FunctionType::oldStyleCompatibleWithNewStyle(const c89c::FunctionType &oldStyle,
        const c89c::FunctionType &newStyle) {
    assert(oldStyle.m_old_style && !newStyle.m_old_style);
    if (newStyle.m_var_arg)
        return false;
    if (!oldStyle.isOldStyleDefinition()) {
        // declaration
        for (const auto &arg: newStyle.m_args)
            if (arg->affectedByPromotion())
                return false;
        return true;
    } else {
        // definition
        if (oldStyle.m_args.size() != newStyle.m_args.size())
            return false;
        for (decltype(oldStyle.m_args.size()) i = 0; i < oldStyle.m_args.size(); ++i)
            if (!std::unique_ptr<Type>(oldStyle.m_args[i]->promote())->compatible(*newStyle.m_args[i]))
                return false;
        return true;
    }
}

// https://en.cppreference.com/w/c/language/type # Compatible types
bool c89c::FunctionType::compatible(const c89c::Type &other) const {
    if (Type::compatible(other)) {
        const auto &other_ref = static_cast<const FunctionType &>(other); // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
        if (!m_return->compatible(*other_ref.m_return))
            return false;
        if (!m_old_style && !other_ref.m_old_style) {
            // !m_old_style && !other_ref.m_old_style
            if (m_args.size() != other_ref.m_args.size() ||
                    m_var_arg != other_ref.m_var_arg)
                return false;
            for (decltype(m_args.size()) i = 0; i < m_args.size(); ++i)
                if (!std::unique_ptr<Type>(m_args[i]->decay())->compatible(
                        *std::unique_ptr<Type>(other_ref.m_args[i]->decay())))
                    return false;
            return true;
        } else if (!other_ref.m_old_style)
            // m_old_style && !other_ref.m_old_style
            return oldStyleCompatibleWithNewStyle(*this, other_ref);
        else if (!m_old_style)
            // !m_old_style && other_ref.m_old_style
            return oldStyleCompatibleWithNewStyle(other_ref, *this);
        else
            // m_old_style && other_ref.m_old_style
            return true;
    }
    return false;
}
