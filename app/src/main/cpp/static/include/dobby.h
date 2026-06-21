// dobby.h - Dobby Hook Library Header
// Source: https://github.com/jmpews/Dobby

#ifndef DOBBY_H
#define DOBBY_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Dobby hook function - intercepts function calls
void *DobbyHook(void *target, void *replace, void **origin);

// Dobby destruct hook
void *DobbyDestructHook(void *target, void *replace, void **origin);

// Dobby code patch
void DobbyCodePatch(void *address, uint8_t *buffer, uint32_t buffer_size);

// Dobby memory operation
void DobbyMemOperation(void *address, void *buffer, uint32_t buffer_size, bool is_patch);

// Dobby symbol resolution
void *DobbySymbolResolver(const char *image_name, const char *symbol_name);

// Dobby VM operations (for VMT hooks)
void **DobbyVMTResolveVirtualFunction(void **vtable, size_t index);
void *DobbyVMTGetWrapper(void *vtable, void *method, void *replace_method);
void DobbyVMTHookVirtualFunction(void *vtable, void *method, void *replace_method, void **origin_method);
void DobbyVMTHookAll(void *vtable, void *replace_method, void **origin_method);
void DobbyVMTDisableHookVirtualFunction(void *vtable, void *method, void *origin_method);
void DobbyVMTResetVirtualFunction(void *vtable, void *method, void *origin_method);

// Dobby import table hook
void *DobbyHookImport(const char *image_name, const char *symbol_name, void *replace, void **origin);

// Dobby near branch trampoline
void *DobbyNearBranchTrampoline(void *address, void *replace, void **origin);

// Dobby fast trampoline
void *DobbyFastTrampoline(void *target, void *replace, void **origin);

// Dobby context operations
typedef struct _DobbyRegisterContext {
    uint64_t general[31];
    uint64_t sp;
    uint64_t pc;
    uint64_t fp;
    uint64_t lr;
    uint64_t x0, x1, x2, x3, x4, x5, x6, x7;
    uint64_t d0, d1, d2, d3, d4, d5, d6, d7;
    uint64_t q0, q1, q2, q3, q4, q5, q6, q7;
    void *float_pointer;
} DobbyRegisterContext;

typedef void (*dobby_hook_func_t)(DobbyRegisterContext *ctx, void *arg);

void *DobbyHookWithArg(void *target, dobby_hook_func_t replace, void **origin);

#ifdef __cplusplus
}
#endif

#endif // DOBBY_H