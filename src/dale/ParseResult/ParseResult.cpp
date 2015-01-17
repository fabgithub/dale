#include "ParseResult.h"

namespace dale
{
ParseResult::ParseResult()
{
    block = NULL;
    type = NULL;
    value = NULL;
    address_of_value = NULL;
    type_of_address_of_value = NULL;
    treat_as_terminator = 0;
    do_not_destruct = 0;
    do_not_copy_with_setf = 0;
    freshly_copied = 0;
    value_is_lvalue = 0;

    retval = NULL;
    retval_type = NULL;
    retval_used = false;
}

ParseResult::ParseResult(
    llvm::BasicBlock *new_block,
    Type *new_type,
    llvm::Value *new_value
)
{
    treat_as_terminator = 0;
    do_not_destruct = 0;
    do_not_copy_with_setf = 0;
    freshly_copied = 0;
    value_is_lvalue = 0;
    block = new_block;
    type  = new_type;
    value = new_value;
    address_of_value = NULL;
    type_of_address_of_value = NULL;

    retval = NULL;
    retval_type = NULL;
    retval_used = false;
}

ParseResult::~ParseResult()
{
}

int ParseResult::copyTo(ParseResult *x)
{
    x->treat_as_terminator = treat_as_terminator;
    x->do_not_destruct = do_not_destruct;
    x->do_not_copy_with_setf =  do_not_copy_with_setf;
    x->block = block;
    x->type = type;
    x->value = value;
    x->address_of_value = address_of_value;
    x->freshly_copied = freshly_copied;
    x->value_is_lvalue = value_is_lvalue;
    x->type_of_address_of_value = type_of_address_of_value;
    x->retval_type = retval_type;
    x->retval = retval;
    x->retval_used = retval_used;
    return 1;
}

void ParseResult::set(llvm::BasicBlock *new_block,
                      Type *new_type,
                      llvm::Value *new_value) {
    block = new_block;
    type  = new_type;
    value = new_value;
}

bool ParseResult::setAddressOfValue(Context *ctx)
{
    if (address_of_value) {
        return true;
    }

    llvm::IRBuilder<> builder(block);
    llvm::Type *llvm_type = ctx->toLLVMType(type, NULL, false);
    if (!llvm_type) {
        return false;
    }
    address_of_value = builder.CreateAlloca(llvm_type);
    /* todo: if retval is present, that should probably be returned
     * instead. */
    builder.CreateStore(getValue(ctx), address_of_value);
    return true;
}

bool ParseResult::getAddressOfValue(Context *ctx, ParseResult *pr)
{
    bool res = setAddressOfValue(ctx);
    if (!res) {
        return false;
    }
    Type *new_type =
        (type_of_address_of_value)
            ? type_of_address_of_value
            : ctx->tr->getPointerType(type);

    pr->set(block, new_type, address_of_value);
    return true;
}

llvm::Value *ParseResult::getValue(Context *ctx)
{
    if (retval) {
        llvm::IRBuilder<> builder(block);
        return builder.CreateLoad(retval);
    }
    if (value) {
        return value;
    }
    fprintf(stderr, "No value in ParseResult.\n");
    abort();
}
}
