
#ifndef ORIFLAMME_ACTION_H
#define ORIFLAMME_ACTION_H

#include "common.h"
#include "state.h"

typedef enum {
    EFFECT_NONE,
    EFFECT_PLACE,
    EFFECT_REVEAL,
    EFFECT_KILL,
    // ...
    EFFECT_MAX,
} Act;

typedef struct {
    PyObject_HEAD
    int effect;
    StateObject* current_state;
    StateObject* next_state;
} ActionObject;

ActionObject* Action_New(int effect, StateObject* current_state);

extern PyTypeObject Action_Type;

#endif
