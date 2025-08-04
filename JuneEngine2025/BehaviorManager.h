#ifndef BEHAVIOR_MANAGER
#define BEHAVIOR_MANAGER

#include "Behavior.h"
#include "SparseSet.h"
#include "Entity.h"

typedef struct BehaviorEntry {
    size_t ID;
    behavior_t* behaviors;
    size_t count, capacity;
} BehaviorEntry;

GEN_SPARSE_SET(behaviorSet_t, BehaviorEntry)

typedef struct BehaviorManager {
    behaviorSet_t* set;
} BehaviorManager;

BehaviorManager* BehaviorManager_Init(void);
void BehaviorManager_Shutdown(BehaviorManager* behaviors);

behavior_t* AddBehavior(BehaviorManager* behaviors, 
    const char* name, 
    const size_t* ID,
    void* ctx,
    const BehaviorFunction enter, 
    const BehaviorFunction update, 
    const BehaviorFunction exit);

behavior_t* GetBehavior(BehaviorManager* behaviors, 
    const size_t ID, 
    const char* name);

BehaviorEntry* GetBehaviors(BehaviorManager* behaviors,
    const size_t ID);

void RemoveBehavior(BehaviorManager* behaviors, 
    const size_t ID, 
    const char* name);

void RemoveBehaviors(BehaviorManager* behaviors,
    const size_t ID);

void BehaviorManager_Update(BehaviorManager* manager);

#endif