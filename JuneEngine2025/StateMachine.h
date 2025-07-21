#ifndef STATEMACHINE
#define STATEMACHINE

// PLEASE NOTE
// This is a (notably amateurish) framework for FSMs, there is no Hierarchical Support at the moment!

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef void (*sm_st)(void*);

typedef struct state_t {
	sm_st enter;
	sm_st update;
	sm_st exit;
	void* context;
} state_t;

typedef struct stateMachine_t {
	state_t* states;
	state_t* current;
	state_t* previous;

	float time;

	int count;
	int capacity;
} stateMachine_t;

void			sm_Init(stateMachine_t* sM, const state_t states[], int size);
void			sm_CheckExpansion(stateMachine_t* sM);
inline state_t* sm_Create(stateMachine_t* sM, const sm_st enter, const sm_st update, const sm_st exit, void* ctx);
inline void		sm_Add(stateMachine_t* sM, const state_t* state);
inline int		sm_Change(stateMachine_t* sM, state_t* state);
inline int		sm_ChangeIndex(stateMachine_t* sM, const int idx);
inline void		sm_Update(stateMachine_t* sM, float dT);
inline void		sm_Free(stateMachine_t* sM);

#endif