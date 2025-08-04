#ifndef TRANSFORM_SYSTEM
#define TRANSFORM_SYSTEM

#include "Entity.h"
#include "transform.h"
#include "SparseSet.h"

GEN_SPARSE_SET(transformSet_t, transform_t)

typedef struct TransformManager {
    transformSet_t* set;

    size_t* destroyList;
    size_t count, capacity;
} TransformManager;

TransformManager* TransformManager_Init(void);
void TransformManager_Shutdown(TransformManager* manager);
void TransformManager_Serialize(TransformManager* manager);
void TransformManager_Deserialize(TransformManager* manager);

void TransformManager_HandleDeletions(TransformManager* manager);

transform_t* AddTransform(TransformManager* manager, const vec3_t position, const vec3_t rotation, const vec3_t scale, const size_t* ID);
transform_t* GetTransform(TransformManager* manager, const size_t* ID);

void RemoveTransform(TransformManager* manager, const size_t* ID);

#endif