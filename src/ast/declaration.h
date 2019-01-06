#ifndef MYC89COMPILER_DECLARATION_H
#define MYC89COMPILER_DECLARATION_H

#include <bitset>
#include <memory>

namespace c89c {
    class Type;

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

    public:
        enum TypeQualifierFlag {CONST, VOLATILE};
        void set(TypeQualifierFlag flag) {
            m_flag = flag;
        }

    private:
        TypeQualifierFlag m_flag;
    };

    class DeclarationSpecifiers {
    public:
        DeclarationSpecifiers():
            m_storage_class(StorageClassSpecifier::NOT_SPECIFIED),
            m_type_specifiers{0},
            m_const(false), m_volatile(false) {}
        DeclarationSpecifiers(DeclarationSpecifiers &&other) = default;
        DeclarationSpecifiers &operator = (DeclarationSpecifiers &&other) = default;

        void add(StorageClassSpecifier &&specifier);
        void add(TypeSpecifier &&specifier);
        void add(TypeQualifier &&qualifier);

    private:
        StorageClassSpecifier::StorageClassSpecifierFlag m_storage_class;
        std::bitset<TypeSpecifier::MAX_TYPE_SPECIFIER_FLAG> m_type_specifiers;
        bool m_const, m_volatile;
        std::unique_ptr<Type> m_type;
    };
}

#endif //MYC89COMPILER_DECLARATION_H
