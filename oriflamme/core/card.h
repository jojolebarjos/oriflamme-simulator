
#ifndef ORIFLAMME_CARD_H
#define ORIFLAMME_CARD_H

#include "common.h"

typedef enum {
    KIND_NONE,
    KIND_ARCHER,
    KIND_SOLDIER,
    KIND_SPY,
    KIND_HEIR,
    KIND_ASSASSINATION,
    KIND_ROYAL_DECREE,
    KIND_LORD,
    KIND_SHAPESHIFTER,
    KIND_AMBUSH,
    KIND_CONSPIRACY,
    KIND_MAX,
} Kind;

typedef struct {
    PyObject_HEAD
    Kind kind;
    int family;
    int tokens; // -1 means revealed
} CardObject;

CardObject* Card_New(int kind, int family, int tokens);

extern PyTypeObject Card_Type;

#endif
