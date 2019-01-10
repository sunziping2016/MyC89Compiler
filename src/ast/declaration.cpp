#include <cassert>

#include "error.h"
#include "type.h"
#include "declaration.h"
#include "expression.h"
#include "driver.h"

void c89c::DeclarationSpecifiers::add(StorageClassSpecifier &&specifier) {
    if (m_storage_class == StorageClassSpecifier::NOT_SPECIFIED)
        m_storage_class = specifier.m_flag;
    else if (m_storage_class == specifier.m_flag)
        throw SemanticWarning("duplicated storage class specifier");
    else
        throw SemanticError("cannot combine with previous storage class specifier");
}

void c89c::DeclarationSpecifiers::add(TypeSpecifier &&specifier) {
    // No `long long' support
    switch (specifier.m_flag) {
        case TypeSpecifier::VOID:
        case TypeSpecifier::FLOAT:
        case TypeSpecifier::STRUCT_OR_UNION:
        case TypeSpecifier::ENUM:
        case TypeSpecifier::TYPENAME:
            if (m_type_specifiers.any())
                throw SemanticError("cannot combine with previous type specifier");
            break;
        case TypeSpecifier::CHAR:
        case TypeSpecifier::INT:
            if ((m_type_specifiers & ~std::bitset<TypeSpecifier::MAX_TYPE_SPECIFIER_FLAG>(
                    1 << TypeSpecifier::SIGNED | 1 << TypeSpecifier::UNSIGNED)).any())
                throw SemanticError("cannot combine with previous type specifier");
            break;
        case TypeSpecifier::SHORT:
            if ((m_type_specifiers & ~std::bitset<TypeSpecifier::MAX_TYPE_SPECIFIER_FLAG>(
                    1 << TypeSpecifier::SIGNED | 1 << TypeSpecifier::UNSIGNED |
                    1 << TypeSpecifier::INT)).any())
                throw SemanticError("cannot combine with previous type specifier");
            break;
        case TypeSpecifier::LONG:
            if ((m_type_specifiers & ~std::bitset<TypeSpecifier::MAX_TYPE_SPECIFIER_FLAG>(
                    1 << TypeSpecifier::SIGNED | 1 << TypeSpecifier::UNSIGNED |
                    1 << TypeSpecifier::INT | 1 << TypeSpecifier::DOUBLE)).any())
                throw SemanticError("cannot combine with previous type specifier");
            break;
        case TypeSpecifier::DOUBLE:
            if ((m_type_specifiers & ~std::bitset<TypeSpecifier::MAX_TYPE_SPECIFIER_FLAG>(
                    1 << TypeSpecifier::LONG)).any())
                throw SemanticError("cannot combine with previous type specifier");
            break;
        case TypeSpecifier::SIGNED:
        case TypeSpecifier::UNSIGNED:
            if ((m_type_specifiers & ~std::bitset<TypeSpecifier::MAX_TYPE_SPECIFIER_FLAG>(
                    1 << TypeSpecifier::CHAR | 1 << TypeSpecifier::INT |
                    1 << TypeSpecifier::SHORT | 1 << TypeSpecifier::LONG)).any())
                throw SemanticError("cannot combine with previous type specifier");
            break;
        default:
            assert(0);

    }
    m_type_specifiers.set(specifier.m_flag);
    if (specifier.m_flag == TypeSpecifier::STRUCT_OR_UNION ||
            specifier.m_flag == TypeSpecifier::ENUM ||
            specifier.m_flag == TypeSpecifier::TYPENAME)
        m_type = std::move(specifier.m_type);
}

void c89c::DeclarationSpecifiers::add(TypeQualifier &&qualifier) {
    switch (qualifier.m_flag) {
        case TypeQualifier::CONST:
            if (m_const)
                throw SemanticWarning("duplicated type qualifier");
            m_const = true;
            break;
        case TypeQualifier::VOLATILE:
            if (m_volatile)
                throw SemanticWarning("duplicated type qualifier");
            m_volatile = true;
            break;
        default:
            assert(0);
    }
}

std::unique_ptr<c89c::Type> c89c::DeclarationSpecifiers::getType() const {
    std::unique_ptr<Type> type;
    if (m_type)
        type.reset(m_type->clone());
    else if (m_type_specifiers[TypeSpecifier::UNSIGNED]) {
        if (m_type_specifiers[TypeSpecifier::CHAR])
            type = std::make_unique<BasicType>(BasicType::UNSIGNED_CHAR);
        else if (m_type_specifiers[TypeSpecifier::SHORT])
            type = std::make_unique<BasicType>(BasicType::UNSIGNED_SHORT);
        else if (m_type_specifiers[TypeSpecifier::LONG])
            type = std::make_unique<BasicType>(BasicType::UNSIGNED_LONG);
        else
            type = std::make_unique<BasicType>(BasicType::UNSIGNED_INT);
    } else if (m_type_specifiers[TypeSpecifier::CHAR]) {
        if (m_type_specifiers[TypeSpecifier::SIGNED])
            type = std::make_unique<BasicType>(BasicType::SIGNED_CHAR);
        else
            type = std::make_unique<BasicType>(BasicType::CHAR);
    } else if (m_type_specifiers[TypeSpecifier::SHORT])
        type = std::make_unique<BasicType>(BasicType::SHORT);
    else if (m_type_specifiers[TypeSpecifier::LONG])
        type = std::make_unique<BasicType>(BasicType::LONG);
    else if (m_type_specifiers[TypeSpecifier::FLOAT])
        type = std::make_unique<BasicType>(BasicType::FLOAT);
    else if (m_type_specifiers[TypeSpecifier::DOUBLE]) {
        if (m_type_specifiers[TypeSpecifier::LONG])
            type = std::make_unique<BasicType>(BasicType::LONG_DOUBLE);
        else
            type = std::make_unique<BasicType>(BasicType::DOUBLE);
    } else if (m_type_specifiers[TypeSpecifier::VOID])
        type = std::make_unique<VoidType>();
    else
        type = std::make_unique<BasicType>(BasicType::INT);
    type->setConst(m_const);
    type->setVolatile(m_volatile);
    return type;
}

void c89c::TypeQualifierList::add(TypeQualifier &&qualifier) {
    if (qualifier.m_flag == TypeQualifier::CONST) {
        if (m_const)
            throw SemanticWarning("duplicated type qualifier");
        m_const = true;
    } else {
        if (m_volatile)
            throw SemanticWarning("duplicated type qualifier");
        m_volatile = true;
    }
}


const std::string &c89c::PointerDeclarator::identifier() const {
    assert(m_base);
    return m_base->identifier();
}

std::unique_ptr<c89c::Type> c89c::PointerDeclarator::getType(std::unique_ptr<Type> &&base_type) const {
    assert(m_base);
    std::unique_ptr<Type> type = std::make_unique<PointerType>(std::move(base_type));
    type->setConst(m_const);
    type->setVolatile(m_volatile);
    return m_base->getType(std::move(type));
}

void c89c::PointerDeclarator::setBase(std::unique_ptr<Declarator> &&base) {
    if (m_base)
        m_base->setBase(std::move(base));
    else
        m_base = std::move(base);
}

c89c::ArrayDeclarator::ArrayDeclarator(std::unique_ptr<c89c::Expression> &&expression): m_incomplete(false) {
    if (expression->isIntegerConstantExpression()) {
        llvm::APInt value = expression->getIntegerConstantValue();
        if (!expression->getType()->isUnsignedIntegerType() && value.isSignBitSet())
            throw SemanticError("array size is negative");
        m_num = value.getLimitedValue();
    } else
        throw SemanticError("array size must be an integer constant expression");
}

const std::string &c89c::ArrayDeclarator::identifier() const {
    assert(m_base);
    return m_base->identifier();
}

std::unique_ptr<c89c::Type> c89c::ArrayDeclarator::getType(std::unique_ptr<c89c::Type> &&base_type) const {
    std::unique_ptr<Type> type = std::make_unique<ArrayType>(std::move(base_type), m_num, m_incomplete);
    return m_base->getType(std::move(type));
}

void c89c::ArrayDeclarator::setBase(std::unique_ptr<c89c::Declarator> &&base) {
    m_base = std::move(base);
}

void c89c::InitDeclarator::generate(const c89c::DeclarationSpecifiers &specifiers, c89c::Driver &driver) {
    // TODO: when inside the declaration list inside function definition
    auto &&type = m_declarator->getType(specifiers.getType());
    if(specifiers.m_storage_class == StorageClassSpecifier::TYPEDEF) {
        if (m_initializer)
            throw SemanticError("illegal initializer for typedef");
        auto iter = driver.topScope().find(m_declarator->identifier());
        if (iter == driver.topScope().end()) {
            driver.topScope().emplace(m_declarator->identifier(), std::move(type));
        } else {
            if (iter->second.index() == 1) {
                const auto &other_type = std::get<1>(iter->second);
                if (!type->equal(*other_type))
                    throw SemanticError("typedef redefinition with different types");
            } else
                throw SemanticError("redefinition of \'" + m_declarator->identifier() + "\'");
        }
    }
}
