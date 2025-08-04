#ifndef ENTITY_MANAGER
#define ENTITY_MANAGER

#include <stdlib.h>
#include "Entity.h"

#define INIT_ALLOC 256

typedef void (*entityCallback)(const entity_t*);

// Create entities
// Callbacks for when an entity is destroyed
typedef struct EntityManager {
	// ID Recycling (not needed but for funsies)
	size_t* nextQueue;
	size_t queueCount, queueCapacity;

	//// Destruction Queue (cant do in the middle of an iteration?)
	//size_t* destroyQueue;
	//size_t destroyCount, destroyCapacity;

	// Data
	entity_t* data;
	size_t dataCount, dataCapacity;

	// Destroy callbacks
	entityCallback* onDestroy;
	size_t callbackCount, callbackCapacity;
} EntityManager;

EntityManager* EntityManager_Init(void);
void EntityManager_Shutdown(EntityManager* manager);
void EntityManager_AddDestroyCallback(EntityManager* manager, entityCallback* callback);
void EntityManager_Debug(EntityManager* manager);

entity_t* Entity_Instantiate(EntityManager* manager);
void Entity_Destroy(EntityManager* manager, entity_t* e);
entity_t* Entity_GetFromID(EntityManager* manager, const size_t id);
entity_t* Entity_GetFromName(EntityManager* manager, const char* name);

#endif // !ENTITY_MANAGER
