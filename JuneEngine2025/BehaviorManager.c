#include "BehaviorManager.h"

#define PAGE_SIZE 3
#define INIT_SPARSE 128
#define INIT_DENSE 16

BehaviorManager* BehaviorManager_Init(void) {
    BehaviorManager* manager = calloc(1, sizeof(BehaviorManager));

    if (!manager) {
        perror("calloc");
        return;
    }

    manager->set = behaviorSet_t_Init(PAGE_SIZE, INIT_SPARSE, INIT_DENSE);
    return manager;
}

void BehaviorManager_Shutdown(BehaviorManager* manager) {
    for (size_t i = 0; i < manager->set->denseCount; ++i) {
        BehaviorEntry* entry = manager->set->dense[i];

        for (size_t i = 0; i < entry->count; ++i) {
            if (entry->behaviors[i].exit) entry->behaviors[i].exit();
        }

        free(entry->behaviors);
    }

    behaviorSet_t_Free(manager->set);
    free(manager);
}

behavior_t* AddBehavior(BehaviorManager* manager, 
    const char* name,
    const size_t* ID,
    void* ctx,
    const BehaviorFunction enter,
    const BehaviorFunction update,
    const BehaviorFunction exit) {

    BehaviorEntry* entry;

    if (behaviorSet_t_Contains(manager->set, ID)) 
    {
        entry = behaviorSet_t_Get(manager->set, ID);
        if (entry->count >= entry->capacity) {
            size_t new_capacity = entry->capacity * 2;
            behavior_t* t = realloc(entry->behaviors, new_capacity * sizeof(behavior_t));
            if (!t) {
                perror("realloc");
                return NULL;
            }
            entry->capacity = new_capacity;
            entry->behaviors = t;
        }
    }
    else
    {
        entry = malloc(sizeof(BehaviorEntry));
        if (!entry) {
            perror("malloc");
            return NULL;
        }

        behavior_t* behaviors = calloc(1, sizeof(behavior_t));
        if (!behaviors) {
            perror("calloc");
            return NULL;
        }

        *entry = (BehaviorEntry){
            .ID = ID,
            .behaviors = behaviors,
            0,
            1,
        };

        behaviorSet_t_Insert(manager->set, entry, ID);
    }

    behavior_t behavior = {
        .name = name,
        .entityID = ID,
        .ctx = ctx,
        .enter = enter,
        .update = update,
        .exit = exit,
    };

    entry->behaviors[entry->count] = behavior;
    behavior_t* bRef = &entry->behaviors[entry->count++];

    if (bRef->enter != NULL) bRef->enter();

    return bRef;
}

behavior_t* GetBehavior(BehaviorManager* manager, const size_t ID, const char* name) {
    BehaviorEntry* entry = GetBehaviors(manager, ID);

    if (!entry) {
        return NULL;
    }

    for (size_t i = 0; i < entry->count; ++i) {
        if (strcmp(entry->behaviors[i].name, name) == 0) {
            return &entry->behaviors[i];
        }
    }

    return NULL;
}

BehaviorEntry* GetBehaviors(BehaviorManager* manager, const size_t ID) {
    return behaviorSet_t_Get(manager->set, ID);
}

void RemoveBehavior(BehaviorManager* manager, const size_t ID, const char* name) {
    BehaviorEntry* entry = GetBehaviors(manager, ID);

    if (!entry) {
        return;
    }

    for (size_t i = 0; i < entry->count; ++i) {
        if (strcmp(entry->behaviors[i].name, name) == 0) {
            if (entry->behaviors[i].exit) entry->behaviors[i].exit();

            for (size_t j = i; j < entry->count - 1; ++j) {
                entry->behaviors[j] = entry->behaviors[j + 1];
            }

            entry->count--;
            break;
        }
    }
}

void RemoveBehaviors(BehaviorManager* manager, const size_t ID) {
    BehaviorEntry* entry = GetBehaviors(manager, ID);

    if (!entry) {
        return;
    }

    for (size_t i = 0; i < entry->count; ++i) {
        if (entry->behaviors[i].exit) entry->behaviors[i].exit();
    }

    behaviorSet_t_Remove(manager->set, ID);

    free(entry->behaviors);
    free(entry);
}

void BehaviorManager_Update(BehaviorManager* manager) {
    for (size_t i = 0; i < manager->set->denseCount; ++i) {
        BehaviorEntry* entry = manager->set->dense[i];

        for (size_t j = 0; j < entry->count; ++j) {
            if (!entry->behaviors[j].update) continue;

            entry->behaviors[j].update();
        }
    }
}