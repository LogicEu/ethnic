#include "ethnic_common.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

static void entity_push_key(ComponentHash* component)
{
    if (component->keys_used >= component->keys_size) {
        component->keys_size *= 2;
        component->keys = (unsigned int*)realloc(component->keys, component->keys_size * sizeof(unsigned int));
    }
    memset(component->keys + component->keys_used++, 0, sizeof(unsigned int));
}
 
static void entity_push_data(ComponentHash* component, Entity entity, void* data)
{
    if (component->data_used >= component->data_size) {
        component->data_size *= 2;
        component->data = (char*)realloc(component->data, component->data_size * component->data_bytes);
        component->entities = (Entity*)realloc(component->entities, component->data_size * sizeof(Entity));
    }
    memcpy((char*)component->data + component->data_bytes * component->data_used, data, component->data_bytes);
    memcpy(component->entities + component->data_used++, &entity, sizeof(Entity));
}

static void entity_remove_data(ComponentHash* component, const unsigned int index)
{
    const unsigned int bytes = component->data_bytes;
    char* ptr = (char*)component->data + index * bytes;
    Entity* e = component->entities + index;
    for (unsigned int i = index; i < component->data_used - 1; i++) {
        memcpy(ptr, ptr + bytes, bytes);
        memcpy(e, e + 1, sizeof(Entity));
        e++;
        ptr += bytes;
    }
    memset(ptr, 0, bytes);
    memset(e, 0, sizeof(Entity));
    component->data_used--;
}

Entity entity_new()
{
    ModuleHash* module = module_get_activated();
    if (module->destroyed_used > 0) return module_pop_destroyed_key();
    for (unsigned int i = 0; i < module->used; i++) {
        entity_push_key(&module->components[i]);
    }
    return module->entity_count++;
}

Entity entity_create(unsigned int component_count, ...)
{
    Entity entity = entity_new();
    Component component = 0;
    component_count += component_count;

    va_list args;
    va_start(args, component_count);
    for (unsigned int i = 0; i < component_count; i++) {
        if (i % 2 == 0) component = va_arg(args, Component);
        else entity_add_component(entity, component, va_arg(args, void*));
    }
    va_end(args);
    return entity;
}

unsigned int entity_count()
{
    return module_get_activated()->entity_count;
}

void entity_add_component(Entity entity, Component component, void* data)
{
    ComponentHash* cHash = component_get_hash(component);
    if (cHash->keys[entity]) return;
    const unsigned int key = cHash->data_used + 1;
    memcpy(cHash->keys + entity, &key, sizeof(unsigned int));
    entity_push_data(cHash, entity, data);
}

void entity_remove_component(Entity entity, Component component)
{
    ComponentHash* cHash = component_get_hash(component);
    unsigned int key = cHash->keys[entity];
    if (!key) return;
    entity_remove_data(cHash, --key);
    cHash->keys[entity] = 0;
}

unsigned int entity_has(Entity entity, Component component)
{
    return component_get_hash(component)->keys[entity];
}

void* entity_get(Entity entity, Component component)
{
    ComponentHash* cHash = component_get_hash(component);
    unsigned int key = cHash->keys[entity];
    if (!key) return NULL;
    return cHash->data + cHash->data_bytes * --key;
}

void entity_set(Entity entity, Component component, void* data)
{
    ComponentHash* cHash = component_get_hash(component);
    unsigned int key = cHash->keys[entity];
    if (!key) return;
    memcpy(cHash->data + cHash->data_bytes * --key, data, cHash->data_bytes);
}

static void entity_sleep(ComponentHash* component, unsigned int key)
{
    for (unsigned int e = 0; e < entity_count(); e++) {
        unsigned int k = component->keys[e];
        component->keys[e] = k - (k >= key);
    }
}

void entity_destroy(Entity entity)
{
    ModuleHash* module = module_get_activated();
    for (unsigned int i = 0; i < module->used; i++) {
        ComponentHash* component = module->components + i;
        unsigned int key = component->keys[entity];
        if (!key) continue;
        component->keys[entity] = 0;  
        entity_sleep(component, key); 
        entity_remove_data(component, --key);
    }
    module_push_destroyed_key(entity);
}

Entity entity_find(Component component, unsigned int component_index)
{
    ComponentHash* cHash = component_get_hash(component);
    if (cHash->data_used <= component_index) return 0;
    return *(cHash->entities + component_index);
}
