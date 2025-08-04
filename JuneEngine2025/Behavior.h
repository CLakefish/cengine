#ifndef BEHAVIOR
#define BEHAVIOR

typedef void (*BehaviorFunction)(void);

typedef struct behavior_t {
    char* name;
    void* ctx;
    size_t entityID;
    BehaviorFunction enter;
    BehaviorFunction update;
    BehaviorFunction exit;
} behavior_t;

#endif