#include "ethnic_common.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

typedef struct ArchetypeHash {
    Component* components;
    unsigned int size, used;
} ArchetypeHash;

typedef struct ArchetypeSystem {
    ArchetypeHash* archetypes;
    unsigned int size, used;
} ArchetypeSystem;

static ArchetypeSystem ArchMod;

static void archetype_push(ArchetypeHash* archetype)
{
    if (ArchMod.used >= ArchMod.size) {
        ArchMod.size *= 2;
        ArchMod.archetypes = (ArchetypeHash*)realloc(ArchMod.archetypes, sizeof(ArchetypeHash) * ArchMod.size);
    }
    memcpy(ArchMod.archetypes + ArchMod.used++, archetype, sizeof(ArchetypeHash));
}

static void archetype_push_component(ArchetypeHash* archetype, Component component)
{
    if (archetype->used >= archetype->size) {
        archetype->size *= 2;
        archetype->components = (Component*)realloc(archetype->components, sizeof(Component) * archetype->size);
    }
    memcpy(archetype->components + archetype->used++, &component, sizeof(Component));
}

static void archetype_free_component(ArchetypeHash* archetype, Component component)
{
    Component* c = archetype->components + component;
    for (Component* end = archetype->components + archetype->used - 1; c != end; c++) {
        memcpy(c, c + 1, sizeof(unsigned int));
    }
    memset(c, 0, sizeof(Component));
    archetype->used--;
}

void archetype_system_init()
{
    ArchMod.archetypes = (ArchetypeHash*)malloc(sizeof(ArchetypeHash));
    ArchMod.size = 1;
    ArchMod.used = 0;
}

void archetype_system_destroy()
{
    for (unsigned int i = 0; i < ArchMod.used; i++) {
        free(ArchMod.archetypes[i].components);
    }
    free(ArchMod.archetypes);
}

Archetype archetype_create(unsigned int count, ...)
{
    ArchetypeHash archetype;
    archetype.components = (Component*)malloc(sizeof(Component));
    archetype.size = 1;
    archetype.used = 0;

    va_list args;
    va_start(args, count);
    for (unsigned int i = 0; i < count; i++) {
        archetype_push_component(&archetype, va_arg(args, Component));
    }
    va_end(args);

    archetype_push(&archetype);
    return ArchMod.used - 1;
}

void archetype_destroy(Archetype archetype)
{
    ArchetypeHash* a = ArchMod.archetypes + archetype;
    free(a->components);
    for (ArchetypeHash* end = ArchMod.archetypes + ArchMod.used - 1; a != end; a++) {
        memcpy(a, a + 1, sizeof(ArchetypeHash));
    }
    memset(a, 0, sizeof(ArchetypeHash));
    ArchMod.used--;
}

void archetype_add_component(Archetype archetype, Component component)
{
    ArchetypeHash* a = ArchMod.archetypes + archetype;
    archetype_push_component(a, component);
}

void archetype_remove_component(Archetype archetype, Component component)
{
    ArchetypeHash* a = ArchMod.archetypes + archetype;
    archetype_free_component(a, component);
}

Entity archetype_entity(Archetype archetype, unsigned int count, ...)
{
    Entity entity = entity_new();
    ArchetypeHash* a = ArchMod.archetypes + archetype;

    va_list args;
    va_start(args, count);
    for (unsigned int i = 0; i < count; i++) {
        entity_add_component(entity, a->components[i], va_arg(args, void*));
    }
    va_end(args);
    return entity;
}
