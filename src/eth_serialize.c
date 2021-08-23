#include "ethnic_common.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// SERIALIZE //

void module_save(const char* path, Module module)
{
    FILE* file = fopen(path, "wb");
    if (!file) {
        printf("Could not open file '%s'\n", path);
        return;
    }

    ModuleHash* m = module_get(module);
    fwrite(m, sizeof(ModuleHash), 1, file);
    fwrite(m->destroyed_keys, sizeof(unsigned int), m->destroyed_used, file);
    
    for (unsigned int i = 0; i < m->used; i++) {
        ComponentHash* c = m->components + i;
        fwrite(c, sizeof(ComponentHash), 1, file);
        fwrite(c->entities, sizeof(Entity), c->data_used, file);
        fwrite(c->data, c->data_bytes, c->data_used, file);
        fwrite(c->keys, sizeof(unsigned int), c->keys_used, file);
    }
    fclose(file);
    printf("Succesfully saved file '%s'\n", path);
}

// DESERIALIZE //

static char* modular_file_read(const char* path)
{
    FILE* file = fopen(path, "rb");
    if (!file) {
        printf("Could not open file '%s'\n", path);
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* buffer = (char*)malloc(length);
    fread(buffer, sizeof(char), length, file);
    fclose(file);
    return buffer;
}

static ModuleHash modular_file_parse_module(char* buffer)
{
    unsigned int k = 0;
    ModuleHash m;
    memcpy(&m, &buffer[k], sizeof(ModuleHash));
    k += sizeof(ModuleHash);

    m.destroyed_keys = (unsigned int*)malloc(sizeof(unsigned int) * m.destroyed_size);
    memcpy(m.destroyed_keys, &buffer[k], sizeof(unsigned int) * m.destroyed_used);
    k += sizeof(unsigned int) * m.destroyed_used;

    m.components = (ComponentHash*)malloc(sizeof(ComponentHash) * m.size);
    for (unsigned int i = 0; i < m.used; i++) {
        ComponentHash* c = m.components + i;
        memcpy(c, &buffer[k], sizeof(ComponentHash));
        k += sizeof(ComponentHash);

        c->entities = (Entity*)malloc(sizeof(Entity) * c->data_size);
        memcpy(c->entities, &buffer[k], c->data_used * sizeof(Entity));
        k += sizeof(Entity) * c->data_used;

        c->data = malloc(c->data_bytes * c->data_size);
        memcpy(c->data, &buffer[k], c->data_used * c->data_bytes);
        k += c->data_bytes * c->data_used;

        c->keys = (unsigned int*)malloc(sizeof(unsigned int) * c->keys_size);
        memcpy(c->keys, &buffer[k], c->keys_used * sizeof(unsigned int));
        k += c->keys_used * sizeof(unsigned int);
    }
    return m;
}

Module module_load(const char* path)
{
    char* buffer = modular_file_read(path);
    if (buffer == NULL) return module_current();

    ModuleHash m = modular_file_parse_module(buffer);
    module_push(&m);
    free(buffer);
    printf("Succesfully loaded file '%s'\n", path);
    return module_current();
}