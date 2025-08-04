#include "TransformManager.h"

#define INIT_ALLOC 128

TransformManager* TransformManager_Init(void) {
    TransformManager* manager = calloc(1, sizeof(TransformManager));

    if (!manager) {
        perror("calloc");
        return NULL;
    }

    manager->set = transformSet_t_Init(5, 100, 100);

    manager->capacity = INIT_ALLOC;
    manager->count = 0;
    manager->destroyList = calloc(manager->capacity, sizeof(size_t));

    if (!manager->destroyList) {
        perror("calloc");
        transformSet_t_Free(manager->set);
        free(manager);
        return NULL;
    }

    return manager;
}

void TransformManager_Shutdown(TransformManager* manager) {
    transformSet_t_Free(manager->set);
    free(manager->destroyList);
    free(manager);
}

void TransformManager_Serialize(TransformManager* manager) {

}

void TransformManager_Deserialize(TransformManager* manager) {

}

void TransformManager_HandleDeletions(TransformManager* manager) {
    for (size_t i = 0; i < manager->count; ++i) {
        transformSet_t_Remove(manager->set, manager->destroyList[i]);
    }

    // Shrink list
    manager->capacity = INIT_ALLOC;
    manager->count = 0;

    size_t* t = realloc(manager->destroyList, manager->capacity);
    if (!t) {
        perror("realloc");
        return;
    }

    manager->destroyList = t;
}

transform_t* AddTransform(TransformManager* manager, const vec3_t position, const vec3_t rotation, const vec3_t scale, const size_t* ID) {
    transform_t* newTransform = malloc(sizeof(transform_t));

    if (!newTransform) {
        perror("malloc");
        return;
    }

    *newTransform = (transform_t){ position, scale, rotation };

    transformSet_t_Insert(manager->set, newTransform, ID);

    return newTransform;
}

transform_t* GetTransform(TransformManager* manager, const size_t* ID) {
    return transformSet_t_Get(manager->set, ID);
}

void RemoveTransform(TransformManager* manager, const size_t* ID) {
    if (!GetTransform(manager, ID)) return;

    if (manager->count >= manager->capacity) {
        manager->capacity *= 2;

        size_t* t = realloc(manager->destroyList, manager->capacity * sizeof(size_t));
        if (!t) {
            perror("realloc");
            return;
        }

        manager->destroyList = t;
    }

    manager->destroyList[manager->count++] = ID;
}