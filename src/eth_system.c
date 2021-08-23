#include "ethnic_common.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

static ModularSystem SysMod;

void ethnicInit() 
{
    SysMod.modules = (ModuleHash*)malloc(sizeof(ModuleHash));
    SysMod.size = 1;
    SysMod.used = SysMod.activated = 0;
    archetype_system_init();
}

void ethnicExit()
{
    archetype_system_destroy();
    for (unsigned int i = 0; i < SysMod.used; i++) {
        module_free(module_get(i));
    }
    free(SysMod.modules);
}

void module_activate(Module module)
{
    SysMod.activated = module;
}

Module module_current()
{
    return SysMod.activated;
}

void module_destroy(Module module)
{
    ModuleHash* m = SysMod.modules + module;
    module_free(m);
    for (ModuleHash* end = SysMod.modules + SysMod.used - 1; m != end; m++) {
        memcpy(m, m + 1, sizeof(ModuleHash));
    }
    memset(m, 0, sizeof(ModuleHash));
    SysMod.used--;
}

void module_push(ModuleHash* module)
{
    if (SysMod.used >= SysMod.size) {
        SysMod.size *= 2;
        SysMod.modules = (ModuleHash*)realloc(SysMod.modules, sizeof(ModuleHash) * SysMod.size);
    }
    SysMod.activated = SysMod.used;
    memcpy(SysMod.modules + SysMod.used++, module, sizeof(ModuleHash));
}

ModuleHash* module_get(Module module)
{
    return SysMod.modules + module;
}

ModuleHash* module_get_activated()
{
    return SysMod.modules + SysMod.activated;
}

unsigned int module_count()
{
    return SysMod.used;
}
