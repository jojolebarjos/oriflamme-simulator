#ifndef ORIFLAMME_STATE_H
#define ORIFLAMME_STATE_H

#include "common.h"

typedef enum {
    PHASE_NONE,
    PHASE_PLACE,
    PHASE_EVALUATE,
    PHASE_END,
    MAX_PHASE,
} Phase;

extern const char * PHASE_NAMES[MAX_PHASE];

typedef struct {
    PyObject_HEAD
    PyObject * queue;
    PyObject * hands;
    PyObject * tokens;
    unsigned phase;
    unsigned index;
} StateObject;

StateObject * State_Build(
    PyObject * queue,
    PyObject * hands,
    PyObject * tokens,
    unsigned phase,
    unsigned index
);

extern PyTypeObject State_Type;

#endif
