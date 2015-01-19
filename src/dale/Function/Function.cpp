#include "Function.h"

#include "../STLUtils/STLUtils.h"

namespace dale
{
Function::Function()
{
    is_macro      = 0;
    always_inline = 0;
    internal_name = new std::string;
    cto           = 0;
    is_destructor = 0;
    is_setf_fn    = 0;
    defgotos      = new std::vector<DeferredGoto *>;
    labels        = new std::map<std::string, Label *>;
    serialise     = true;
}

Function::Function(
    Type *return_type,
    std::vector<Variable *> *parameter_types,
    llvm::Function *llvm_function,
    int is_macro,
    std::string *internal_name,
    int always_inline)
{
    this->return_type     = return_type;
    this->parameter_types = parameter_types;
    this->llvm_function   = llvm_function;
    this->is_macro        = is_macro;
    this->internal_name   = new std::string(*internal_name);
    this->always_inline   = always_inline;
    cto = 0;
    is_destructor = 0;
    is_setf_fn    = 0;
    serialise     = true;

    defgotos      = new std::vector<DeferredGoto *>;
    labels        = new std::map<std::string, Label *>;
}

Function::~Function()
{
    delete internal_name;
    delete defgotos;
    delete labels;
    if (parameter_types) {
        stl::deleteElements(parameter_types);
        delete parameter_types;
    }
}

Label *Function::getLabel(const char *str)
{
    std::map<std::string, Label*>::iterator b = labels->find(str);
    if (b == labels->end()) {
        return NULL;
    }
    return b->second;
}

bool Function::addLabel(const char *str, Label *label)
{
    labels->insert(std::pair<std::string, Label *>(str, label));
    return true;
}

int Function::isVarArgs(void)
{
    if (parameter_types->size() == 0) {
        return 0;
    }

    Variable *back = parameter_types->back();

    return (back->type->base_type == BaseType::VarArgs) ? 1 : 0;
}

unsigned int Function::numberOfRequiredArgs(void)
{
    if (parameter_types->size() == 0) {
        return 0;
    }

    unsigned int num_of_args = parameter_types->size();

    if (isVarArgs()) {
        num_of_args -= 1;
    }

    return num_of_args;
}

int Function::isEqualTo(Function *other_fn)
{
    if (!return_type->isEqualTo(other_fn->return_type)) {
        return 0;
    }

    return dale::stl::isEqualTo(
               parameter_types,
               other_fn->parameter_types
           );
}

int Function::attrsAreEqual(Function *other_fn)
{
    if (always_inline ^ other_fn->always_inline) {
        return 0;
    }

    return 1;
}

bool Function::isDeclaration(void)
{
    return (!llvm_function || (llvm_function->size() == 0));
}

bool Function::hasRetval(void)
{
    return (return_type->is_retval != 0);
}
}