#include "NMWException.h"
#include "MWCPlusLib.h"

DestructorChain* __global_destructor_chain;

void* __register_global_object(void* object, void* destructor, void* regmem) {
    ((DestructorChain*)regmem)->next = __global_destructor_chain;
    ((DestructorChain*)regmem)->destructor = destructor;
    ((DestructorChain*)regmem)->object = object;
    __global_destructor_chain = (DestructorChain*)regmem;
    return object;
}

void __destroy_global_chain(void) {
    DestructorChain* iter;
    while ((iter = __global_destructor_chain) != 0) {
        __global_destructor_chain = iter->next;
        DTORCALL_COMPLETE(iter->destructor, iter->object);
    }
}

void __init_cpp_exceptions(void) {}

void __fini_cpp_exceptions(void) {}

#pragma section ".ctors$10"
__declspec(section ".ctors$10")
extern void* const __init_cpp_exceptions_reference = __init_cpp_exceptions;

#pragma section ".dtors$10"
__declspec(section ".dtors$10")
extern void* const __destroy_global_chain_reference = __destroy_global_chain;

#pragma section ".dtors$15"
__declspec(section ".dtors$15")
extern void* const __fini_cpp_exceptions_reference = __fini_cpp_exceptions;
