#include <stdio.h>
#include <string.h>
#include "EntityManager.h"

EntityManager entityManager;

static void EntityManager_CheckShrinkQueue(void) {
	int capacity = entityManager.queueCapacity / 2;

	if (entityManager.queueCount > capacity) return;
	if (capacity == 0) return;

	size_t* t = (size_t*)realloc(entityManager.nextQueue, sizeof(size_t) * capacity);

	if (!t) {
		perror("realloc");
		return;
	}

	entityManager.nextQueue = t;
	entityManager.queueCapacity = capacity;
}

static void EntityManager_CheckExpandQueue(void) {
	if (entityManager.queueCount < entityManager.queueCapacity) return;
	entityManager.queueCapacity *= 2;
	size_t* t = (size_t*)realloc(entityManager.nextQueue, sizeof(size_t) * entityManager.queueCapacity);

	if (!t) {
		perror("realloc");
		return;
	}

	entityManager.nextQueue = t;
}

static void EntityManager_CheckExpandData(void) {
	if (entityManager.dataCount < entityManager.dataCapacity) return;
	entityManager.dataCapacity *= 2;
	entity_t* t = (entity_t*)realloc(entityManager.data, sizeof(entity_t) * entityManager.dataCapacity);

	if (!t) {
		perror("realloc");
		return;
	}

	entityManager.data = t;
}

static void EntityManager_CheckExpandCallback(void) {
	if (entityManager.callbackCount < entityManager.callbackCapacity) return;
	entityManager.callbackCapacity *= 2;
	entityCallback* t = (entityCallback*)realloc(entityManager.onDestroy, sizeof(entityCallback) * entityManager.callbackCapacity);
	
	if (!t) {
		perror("realloc");
		return;
	}

	entityManager.onDestroy = t;
}

void EntityManager_Init(void) {
	entityManager = (EntityManager){ 0 };

	entityManager.queueCapacity    = INIT_ALLOC;
	entityManager.dataCapacity	   = INIT_ALLOC;
	entityManager.callbackCapacity = INIT_ALLOC;

	size_t* q = (size_t*)calloc(entityManager.queueCapacity, sizeof(size_t));
	if (!q) {
		perror("calloc");
		return;
	}
	entityManager.nextQueue = q;

	entity_t* d = (entity_t*)calloc(entityManager.dataCapacity, sizeof(entity_t));
	if (!d) {
		perror("calloc");
		return;
	}
	entityManager.data = d;

	entityCallback* c = (entityCallback*)calloc(entityManager.callbackCapacity, sizeof(entityCallback));
	if (!c) {
		perror("calloc");
		return;
	}
	entityManager.onDestroy = c;
}

void EntityManager_AddDestroyCallback(entityCallback* callback) {
	EntityManager_CheckExpandCallback();

	entityManager.onDestroy[entityManager.callbackCount] = callback;
	entityManager.callbackCount++;
}

void EntityManager_Debug(void) {
	printf("Entity Manager queue: %zu\n", entityManager.queueCount);
	for (size_t i = 0; i < entityManager.queueCount; ++i) {
		printf("%zu, ", entityManager.nextQueue[i]);
	}
}

void EntityManager_Shutdown(void) {
	free(entityManager.data);
	free(entityManager.nextQueue);
	free(entityManager.onDestroy);
}

entity_t* Entity_Instantiate(void) {
	size_t id;
	if (entityManager.queueCount > 0) {
		id = entityManager.nextQueue[--entityManager.queueCount];
		EntityManager_CheckShrinkQueue();
	}
	else {
		EntityManager_CheckExpandData();
		id = entityManager.dataCount++;
	}

	entity_t* e = &entityManager.data[id];
	*e = (entity_t){ id, "" };
	return e;
}

entity_t* Entity_GetFromID(const size_t id) {
	for (size_t i = 0; i < entityManager.dataCount; ++i) {
		if (entityManager.data[i].ID == id) {
			return &entityManager.data[i];
		}
	}

	return NULL;
}

entity_t* Entity_GetFromName(const char* name) {
	for (size_t i = 0; i < entityManager.dataCount; ++i) {
		if (strcmp(entityManager.data[i].name, name) == 0) {
			return &entityManager.data[i];
		}
	}

	return NULL;
}

/// <summary>
/// WARNING: Stack allocated entity removal will NOT function. If you try it, it will fail, and that is on you. Only destroy something which has been instantiated or call Entity_GetFromID();
/// </summary>
/// <param name="e">The entity to delete</param>
void Entity_Destroy(entity_t* e) {
	if (e == NULL) {
		return;
	}

	// Check if its already been queued for deletion
	if (e->ID == (size_t)(-1)) {
		printf("ID of type %zu is invalid/dead.\n", e->ID);
		return;
	}

	for (size_t i = 0; i < entityManager.callbackCount; ++i) {
		entityManager.onDestroy[i](e);
	}

	EntityManager_CheckExpandQueue();
	entityManager.nextQueue[entityManager.queueCount++] = e->ID;
	e->ID = -1;
}