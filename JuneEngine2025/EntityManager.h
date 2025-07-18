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

	// Data
	entity_t* data;
	size_t dataCount, dataCapacity;

	// Destroy callbacks
	entityCallback* onDestroy;
	size_t callbackCount, callbackCapacity;
} EntityManager;

extern EntityManager entityManager;

void	 EntityManager_Init(void);
void	 EntityManager_Shutdown(void);
void	 EntityManager_AddDestroyCallback(entityCallback* callback);
void	 EntityManager_Debug(void);

entity_t* Entity_Instantiate(void);
entity_t* Entity_GetFromID(const size_t id);
entity_t* Entity_GetFromName(const char* name);
void	  Entity_Destroy(entity_t* e);

#endif // !ENTITY_MANAGER
