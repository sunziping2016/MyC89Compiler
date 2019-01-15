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

c89c::ParameterDeclaration::ParameterDeclaration(std::unique_ptr<c89c::DeclarationSpecifiers> &&specifiers,
                                                 std::unique_ptr<c89c::Declarator> &&declarator) {
    if (specifiers->m_storage_class == StorageClassSpecifier::REGISTER)
        m_register = true;
    else if (specifiers->m_storage_class == StorageClassSpecifier::NOT_SPECIFIED)
        m_register = false;
    else
        throw SemanticError("invalid storage class specifier in function declarator");
    if (declarator) {
        m_type = declarator->getType(specifiers->getType());
        m_identifier = declarator->identifier();
    } else
        m_type = specifiers->getType();
    if (auto array_type = dynamic_cast<ArrayType *>(m_type.get()))
        m_type.reset(array_type->decay());
    else if (auto function_type = dynamic_cast<FunctionType *>(m_type.get()))
        m_type.reset(function_type->decay());
    if (m_type->isVoidType() && !m_identifier.empty())
        throw SemanticError("argument may not have \'void\' type");
}

void c89c::ParameterList::add(std::unique_ptr<c89c::ParameterDeclaration> &&parameter) {
    if (!m_parameters.empty() && parameter->m_type->isVoidType())
        throw SemanticError("argument may not have \'void\' type");
    m_parameters.push_back(std::move(parameter));
}

c89c::FunctionDeclarator::FunctionDeclarator(std::unique_ptr<c89c::ParameterList> &&parameters):
        m_var_args(parameters->isVarArgs()) {
    if (!parameters->isVoid())
        m_parameters = std::move(parameters->m_parameters);
}

const std::string &c89c::FunctionDeclarator::identifier() const {
    assert(m_base);
    return m_base->identifier();
}

std::unique_ptr<c89c::Type> c89c::FunctionDeclarator::getType(std::unique_ptr<c89c::Type> &&base_type) const {
    std::vector<std::unique_ptr<Type>> args;
    for (const auto &param: m_parameters)
        args.push_back(std::unique_ptr<Type>(param->m_type->clone()));
    std::unique_ptr<Type> type = std::make_unique<FunctionType>(std::move(base_type), std::move(args),
            m_var_args, args.empty() && m_var_args);
    return m_base->getType(std::move(type));
}
static int a();

void c89c::FunctionDeclarator::setBase(std::unique_ptr<c89c::Declarator> &&base) {
    m_base = std::move(base);
}

void c89c::InitDeclarator::generate(const c89c::DeclarationSpecifiers &specifiers, c89c::Driver &driver) {
    // TODO: when inside the declaration list inside function definition
    auto &&type = m_declarator->getType(specifiers.getType());
    if (specifiers.m_storage_class == StorageClassSpecifier::TYPEDEF) {
        // Typedef
        if (m_initializer)
            throw SemanticError("illegal initializer for typedef");
        auto iter = driver.topScope().names.find(m_declarator->identifier());
        if (iter == driver.topScope().names.end()) {
            driver.topScope().names.emplace(m_declarator->identifier(), std::move(type));
        } else {
            if (iter->second.index() == 1) {
                const auto &other_type = std::get<1>(iter->second);
                if (!type->equal(*other_type))
                    throw SemanticError("typedef redefinition with different types");
            } else
                throw SemanticError("redefinition of \'" + m_declarator->identifier() + "\' as different kind of symbol");
        }
    } else if (auto function_type = dynamic_cast<FunctionType *>(type.get())) {
        // Function
        if (specifiers.m_storage_class == StorageClassSpecifier::AUTO ||
            specifiers.m_storage_class == StorageClassSpecifier::REGISTER)
            throw SemanticError("illegal storage class on function");
        if (specifiers.m_storage_class == StorageClassSpecifier::STATIC && !driver.isGlobal())
            throw SemanticError("function declared in block scope cannot have \'static\' storage class");
        if (m_initializer)
            throw SemanticError("illegal initializer (only variables can be initialized)");
        const auto &return_type = function_type->returnType();
        if (return_type->isArrayType())
            throw SemanticError("function cannot return array type");
        if (return_type->isFunctionType())
            throw SemanticError("function cannot return function type");
        if (return_type->isIncompleteType() && !return_type->isVoidType())
            throw SemanticError("incomplete result type in function definition");
        auto linkage = specifiers.m_storage_class == StorageClassSpecifier::STATIC ?
                Driver::FunctionPrototypeItem::INTERNAL : Driver::FunctionPrototypeItem::EXTERNAL;
        auto function_iter = driver.findInAllFunctions(m_declarator->identifier());
        if (function_iter != driver.allFunctionsEnd()) {
            if (!function_iter->second.type->compatible(*function_type))
                throw SemanticError("conflicting types for \'" + m_declarator->identifier() + "\'");
            if (function_iter->second.linkage == Driver::FunctionPrototypeItem::EXTERNAL &&
                    linkage == Driver::FunctionPrototypeItem::INTERNAL)
                throw SemanticError("static declaration of \'" + m_declarator->identifier() + "\' follows non-static declaration");
            if (function_iter->second.linkage == Driver::FunctionPrototypeItem::INTERNAL)
                linkage = Driver::FunctionPrototypeItem::INTERNAL;
            if (function_iter->second.type->isOldStyle() && !function_iter->second.type->isOldStyleDefinition()) {
                driver.addToAllFunctions(m_declarator->identifier(),
                                         {linkage, std::unique_ptr<FunctionType>(function_type->clone())});
            }
        } else
            driver.addToAllFunctions(m_declarator->identifier(),
                    {linkage, std::unique_ptr<FunctionType>(function_type->clone())});
        auto scope_iter = driver.topScope().names.find(m_declarator->identifier());
        if (scope_iter == driver.topScope().names.end())
            driver.topScope().names.emplace(m_declarator->identifier(), std::make_unique<FunctionValue>(
                    m_declarator->identifier(), std::unique_ptr<Type>(function_type->clone()),
                    specifiers.m_storage_class == StorageClassSpecifier::STATIC ? FunctionValue::INTERNAL :
                    FunctionValue::EXTERNAL));
        else {
            if (scope_iter->second.index() == 0) {
                const auto &value = std::get<0>(scope_iter->second);
                if (!value->type()->isFunctionType())
                    throw SemanticError("redefinition of \'" + m_declarator->identifier() + "\' as different kind of symbol");
            } else
                throw SemanticError("redefinition of \'" + m_declarator->identifier() + "\' as different kind of symbol");
        }
    }
}

llvm::Function *c89c::FunctionValue::get(Driver &driver) {
    if (m_value)
        return m_value;
    m_value = llvm::Function::Create(static_cast<llvm::FunctionType *>(m_type->generate(driver.context())), // NOLINT(cppcoreguidelines-pro-type-static-cast-downcast)
            m_linkage == EXTERNAL ? llvm::GlobalValue::ExternalLinkage : llvm::GlobalValue::InternalLinkage,
            m_name, &driver.module());
    return m_value;
}
