#include "ethnic_common.h"
#include <stdlib.h>
#include <stdarg.h>

Module module_create(unsigned int count, ...)
{   
    ModuleHash module;
    module.size = module.destroyed_size = ETH_RESERVE;
    module.used = module.entity_count = module.destroyed_used = 0;
    module.components = (ComponentHash*)malloc(sizeof(ComponentHash) * module.size);
    module.destroyed_keys = (unsigned int*)malloc(sizeof(unsigned int) * module.destroyed_size);
    module_push(&module);

    va_list args;
    va_start(args, count);
    for (unsigned int i = 0; i < count; i++) {
        component_create(va_arg(args, unsigned int));
    }
    va_end(args);

    return module_count() - 1;
}

void module_free(ModuleHash* module)
{
    for (unsigned int i = 0; i < module->used; i++) {
        component_free(module->components + i);
    }
    free(module->components);
    free(module->destroyed_keys);
}
