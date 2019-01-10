#include <cassert>

#include "expression.h"
#include "type.h"

c89c::IntegerConstant::IntegerConstant(llvm::APInt value, std::unique_ptr<c89c::Type> &&type):
        Expression(std::move(type)), m_value(std::move(value)) {
    auto basic_type = dynamic_cast<BasicType *>(m_type.get());
    assert(basic_type);
    switch (basic_type->getTypeFlag()) {
        case BasicType::CHAR:
        case BasicType::UNSIGNED_CHAR:
        case BasicType::SIGNED_CHAR:
            assert(m_value.getBitWidth() == 8);
            break;
        case BasicType::SHORT:
        case BasicType::UNSIGNED_SHORT:
            assert(m_value.getBitWidth() == 16);
            break;
        case BasicType::INT:
        case BasicType::UNSIGNED_INT:
            assert(m_value.getBitWidth() == 32);
            break;
        case BasicType::LONG:
        case BasicType::UNSIGNED_LONG:
            assert(m_value.getBitWidth() == 64);
            break;
        default:
            assert(0);
    }
}

c89c::CharacterConstant::CharacterConstant(int value):
        IntegerConstant(llvm::APInt(32, static_cast<uint64_t>(value), true),
        std::unique_ptr<Type>(std::make_unique<BasicType>(BasicType::INT))) {}
