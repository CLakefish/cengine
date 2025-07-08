#include "StateMachine.h"

#define SM_ASSERT(val) assert(val)

void sm_Init(stateMachine_t* sM, const state_t states[], int size) {
	if (size <= 0) {
		printf("Size was less than or equal to 0 in stateMachine initialization!");
		return;
	}

	SM_ASSERT(sM);

	state_t* s = (state_t*)malloc(size * sizeof(state_t));
	SM_ASSERT(s);

	sM->current = NULL;
	sM->previous = NULL;
	sM->count = size;
	sM->capacity = size;
	sM->time = 0;

	for (int i = 0; i < size; ++i) {
		s[i] = states[i];
	}

	sM->states = s;
}

void sm_CheckExpansion(stateMachine_t* sM) {
	if (sM->count >= sM->capacity) {
		sM->capacity *= 2;

		state_t* temp = (state_t*)realloc(sM->states, sM->capacity * sizeof(state_t));
		SM_ASSERT(temp);
		sM->states = temp;
	}
}

inline state_t* sm_Create(stateMachine_t* sM, const sm_st enter, const sm_st update, const sm_st exit, void* ctx) {
	SM_ASSERT(sM);

	sM->count++;
	sm_CheckExpansion(sM);

	sM->states[sM->count].enter = enter;
	sM->states[sM->count].update = update;
	sM->states[sM->count].exit = exit;
	sM->states[sM->count].context = ctx;

	return &sM->states[sM->count];
}

inline void sm_Add(stateMachine_t* sM, const state_t* state) {
	SM_ASSERT(sM);
	SM_ASSERT(state != NULL);

	sm_CheckExpansion(sM);

	sM->states[sM->count] = *state;
	sM->count++;
}

inline int sm_Change(stateMachine_t* sM, state_t* state) {
	SM_ASSERT(sM);
	if (state == NULL || state == sM->current) return 1;

	if (sM->current != NULL && sM->current->exit != NULL) {
		sM->current->exit(sM->current->context);
	}

	sM->previous = sM->current;
	sM->current = state;
	sM->time = 0;

	if (sM->current != NULL && sM->current->enter != NULL) {
		sM->current->enter(sM->current->context);
	}

	return 0;
}

inline int sm_ChangeIndex(stateMachine_t* sM, const int idx) {
	SM_ASSERT(sM);
	if (idx <= -1 || idx > sM->count) return 1;

	if (sM->current != NULL && sM->current->exit != NULL) {
		sM->current->exit(sM->current->context);
	}

	sM->previous = sM->current;
	sM->current = &sM->states[idx];
	sM->time = 0;

	if (sM->current != NULL && sM->current->enter != NULL) {
		sM->current->enter(sM->current->context);
	}

	return 0;
}

inline void sm_Update(stateMachine_t* sM, float dT) {
	if (sM->current != NULL && sM->current->update != NULL) {
		sM->current->update(sM->current->context);
	}

	sM->time += dT;
}

inline void sm_Free(stateMachine_t* sM) {
	if (sM->current != NULL && sM->current->exit != NULL) {
		sM->current->exit(sM->current->context);
	}

	free(sM->states);

	sM->states = NULL;
	sM->current = NULL;
	sM->previous = NULL;
	sM->count = 0;
	sM->capacity = 0;
	sM->time = 0;
}