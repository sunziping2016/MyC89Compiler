#include <cassert>

#include "error.h"
#include "type.h"
#include "declaration.h"

using namespace std;
using namespace c89c;

void DeclarationSpecifiers::add(StorageClassSpecifier &&specifier) {
    if (m_storage_class == StorageClassSpecifier::NOT_SPECIFIED)
        m_storage_class = specifier.m_flag;
    else if (m_storage_class == specifier.m_flag)
        throw SemanticWarning("duplicated storage class specifier");
    else
        throw SemanticError("cannot combine with previous storage class specifier");
}

void DeclarationSpecifiers::add(TypeSpecifier &&specifier) {
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
            if ((m_type_specifiers & ~bitset<TypeSpecifier::MAX_TYPE_SPECIFIER_FLAG>(
                    1 << TypeSpecifier::SIGNED | 1 << TypeSpecifier::UNSIGNED)).any())
                throw SemanticError("cannot combine with previous type specifier");
            break;
        case TypeSpecifier::SHORT:
            if ((m_type_specifiers & ~bitset<TypeSpecifier::MAX_TYPE_SPECIFIER_FLAG>(
                    1 << TypeSpecifier::SIGNED | 1 << TypeSpecifier::UNSIGNED |
                    1 << TypeSpecifier::INT)).any())
                throw SemanticError("cannot combine with previous type specifier");
            break;
        case TypeSpecifier::LONG:
            if ((m_type_specifiers & ~bitset<TypeSpecifier::MAX_TYPE_SPECIFIER_FLAG>(
                    1 << TypeSpecifier::SIGNED | 1 << TypeSpecifier::UNSIGNED |
                    1 << TypeSpecifier::INT | 1 << TypeSpecifier::DOUBLE)).any())
                throw SemanticError("cannot combine with previous type specifier");
            break;
        case TypeSpecifier::DOUBLE:
            if ((m_type_specifiers & ~bitset<TypeSpecifier::MAX_TYPE_SPECIFIER_FLAG>(
                    1 << TypeSpecifier::LONG)).any())
                throw SemanticError("cannot combine with previous type specifier");
            break;
        case TypeSpecifier::SIGNED:
        case TypeSpecifier::UNSIGNED:
            if ((m_type_specifiers & ~bitset<TypeSpecifier::MAX_TYPE_SPECIFIER_FLAG>(
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

void DeclarationSpecifiers::add(TypeQualifier &&qualifier) {
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
