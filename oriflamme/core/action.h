
#ifndef ORIFLAMME_ACTION_H
#define ORIFLAMME_ACTION_H

#include "common.h"
#include "state.h"

typedef enum {
    EFFECT_NONE,
    EFFECT_PLACE,
    EFFECT_REVEAL,
    EFFECT_INCREASE,
    //EFFECT_ACT,
    /*
        KILL(index, do_suicide)
        SWAP(first, second, do_suicide)
        STEAL(family, do_suicide)
        EARN(tokens, do_suicide)
        MORPH(kind)
    */
    // ...
    EFFECT_MAX,
} Act;

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
