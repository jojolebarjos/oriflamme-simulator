
#ifndef ORIFLAMME_ACTION_H
#define ORIFLAMME_ACTION_H

#include "common.h"
#include "state.h"

typedef enum {
    EFFECT_NONE,
    EFFECT_PLACE,
    EFFECT_REVEAL,
    EFFECT_INCREASE,
    EFFECT_EARN,
    EFFECT_STEAL,
    EFFECT_KILL,
    EFFECT_SWAP,
    EFFECT_MIMIC,
    EFFECT_MAX,
} Effect;

extern char const* Effect_NAMES[EFFECT_MAX];

typedef struct {
    PyObject_HEAD
    int effect;
    int first;
    int second;
    StateObject* current_state;
    StateObject* next_state;
} ActionObject;

/**
 * Create new action.
 * Reference on `current_state` is incremented, i.e. not stolen.
 */
ActionObject* Action_New(int effect, int first, int second, StateObject* current_state);

extern PyTypeObject Action_Type;

#endif
