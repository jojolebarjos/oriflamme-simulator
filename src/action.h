
#ifndef ORIFLAMME_ACTION_H
#define ORIFLAMME_ACTION_H

#include "common.h"
#include "state.h"

typedef struct {
    PyObject_HEAD
    StateObject* current_state;
    StateObject* next_state;
} ActionObject;

extern PyTypeObject Action_Type;

#endif
