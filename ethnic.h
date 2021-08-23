#ifndef ETHNIC_ECS_H
#define ETHNIC_ECS_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************
========= e t h n i c =========
MODULAR ENTITY COMPONENT SYSTEM
===============================
******************************/

#ifndef ETH_RESERVE
    #define ETH_RESERVE 32
#endif

typedef unsigned int Archetype;
typedef unsigned int Entity;
typedef unsigned int Component;
typedef unsigned int Module;

void ethnicInit();
void ethnicExit();

Module module_create(unsigned int count, ...);
Module module_current();
void module_destroy(Module module);
void module_activate(Module module);

void module_save(const char* path, Module module);
Module module_load(const char* path);

Component component_create(unsigned int bytes);
void component_destroy(Component component);
void* component_get(Component component);
unsigned int component_count();
unsigned int component_get_size(Component component);
unsigned int component_entity_count(Component component);

Entity archetype_entity(Archetype archetype, unsigned int count, ...);
Archetype archetype_create(unsigned int count, ...);
void archetype_destroy(Archetype archetype);
void archetype_add_component(Archetype archetype, Component component);
void archetype_remove_component(Archetype archetype, Component component);

Entity entity_new();
Entity entity_create(unsigned int component_count, ...);
Entity entity_find(Component component, unsigned int component_index);
unsigned int entity_count();
unsigned int entity_has(Entity entity, Component component);
void entity_add_component(Entity entity, Component component, void* data);
void* entity_get(Entity entity, Component component);
void entity_destroy(Entity entity);
void entity_set(Entity entity, Component component, void* data);
void entity_remove_component(Entity entity, Component component);

#ifdef __cplusplus
}
#endif
#endif
