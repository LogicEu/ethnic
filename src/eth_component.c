#include "ethnic_common.h"
#include <stdlib.h>
#include <string.h>

void component_push(ComponentHash* component)
{
    ModuleHash* module = module_get_activated();
    if (module->used >= module->size) {
        module->size *= 2;
        module->components = (ComponentHash*)realloc(module->components, module->size * sizeof(ComponentHash));
    }
    memcpy(module->components + module->used++, component, sizeof(ComponentHash));
}

void module_push_destroyed_key(unsigned int key)
{
    ModuleHash* module = module_get_activated();
    if (module->destroyed_used >= module->destroyed_size) {
        module->destroyed_size *= 2;
        module->destroyed_keys = (unsigned int*)realloc(module->destroyed_keys, module->destroyed_size * sizeof(unsigned int));
    }
    memcpy(module->destroyed_keys + module->destroyed_used++, &key, sizeof(unsigned int));
}

unsigned int module_pop_destroyed_key()
{
    ModuleHash* module = module_get_activated();
    unsigned int* val = module->destroyed_keys + --module->destroyed_used;
    unsigned int ret = *val;
    memset(val, 0, sizeof(unsigned int));
    return ret;
}

static ComponentHash component_new(unsigned int bytes)
{
    ComponentHash component;
    component.keys_used = component.data_used = 0;
    component.keys_size = component.data_size = ETH_RESERVE;

    while (entity_count() > component.keys_size) {
        component.keys_size *= 2;
    }

    component.data_bytes = bytes;
    component.keys = (unsigned int*)malloc(sizeof(unsigned int) * component.keys_size);
    component.entities = (Entity*)malloc(sizeof(Entity) * component.data_size);
    component.data = (char*)malloc(component.data_bytes * component.data_size);
    memset(component.keys, 0, sizeof(unsigned int) * component.keys_size);
    memset(component.entities, 0, sizeof(Entity) * component.data_size);
    return component;
}

void component_free(ComponentHash* component)
{
    free(component->data);
    free(component->keys);
    free(component->entities);
}

Component component_create(unsigned int bytes)
{
    ComponentHash component = component_new(bytes);
    component_push(&component);
    return component_count() - 1;
}

void component_destroy(Component component)
{
    ModuleHash* module = module_get_activated();
    ComponentHash* c = component_get_hash(component);
    component_free(c);
    for (ComponentHash* end = module->components + module->used - 1; c != end; c++) {
        memcpy(c, c + 1, sizeof(unsigned int));
    }
    memset(c, 0, sizeof(unsigned int));
    module->used--;
}

unsigned int component_count()
{
    return module_get_activated()->used;
}

unsigned int component_get_size(Component component)
{
    return component_get_hash(component)->data_bytes;
}

ComponentHash* component_get_hash(Component component)
{
    return module_get_activated()->components + component;
}

unsigned int component_entity_count(Component component)
{
    return component_get_hash(component)->data_used;
}

void* component_get(Component component)
{
    return component_get_hash(component)->data;
}
