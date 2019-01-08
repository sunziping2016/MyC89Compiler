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
