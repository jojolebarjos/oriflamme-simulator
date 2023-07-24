#ifndef ORIFLAMME_STACK_H
#define ORIFLAMME_STACK_H

#include "common.h"

typedef struct {
    PyObject_HEAD
    unsigned kind;
    unsigned player;
    int tokens;
    PyObject * tail;
} StackObject;

StackObject * Stack_Build(
    unsigned kind,
    unsigned player,
    int tokens,
    StackObject * tail
);

extern PyTypeObject Stack_Type;

#endif
