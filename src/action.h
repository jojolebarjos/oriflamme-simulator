#ifndef ORIFLAMME_ACTION_H
#define ORIFLAMME_ACTION_H

#include "common.h"
#include "state.h"

typedef enum {
    ACT_NONE,
    ACT_INSERT,
    ACT_STACK,
    ACT_REVEAL,
    ACT_ACCUMULATE,
    ACT_KILL,
    ACT_GAIN,
    ACT_STEAL,
    ACT_MOVE,
    MAX_ACT,
} Act;

extern const char * ACT_NAMES[MAX_ACT];

typedef struct {
    PyObject_HEAD
    StateObject * state;
    StateObject * next_state;
    unsigned act;
    unsigned kind;
    unsigned i;
    unsigned j;
} ActionObject;

ActionObject * Action_Build(
    StateObject * state,
    unsigned act,
    unsigned kind,
    unsigned i,
    unsigned j
);

extern PyTypeObject Action_Type;

#endif
