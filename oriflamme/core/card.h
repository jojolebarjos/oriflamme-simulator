
#ifndef ORIFLAMME_CARD_H
#define ORIFLAMME_CARD_H

#include "common.h"

typedef enum {
    KIND_NONE,
    KIND_ARCHER,
    KIND_HEIR,
    KIND_LORD,
    KIND_SHAPESHIFTER,
    KIND_SOLDIER,
    KIND_SPY,
    KIND_AMBUSH,
    KIND_ASSASSINATION,
    KIND_CONSPIRACY,
    KIND_ROYAL_DECREE,
    KIND_MAX,
} Kind;

extern char const* Kind_NAMES[KIND_MAX];

#define Kind_IS_CHARACTER(kind) ((kind) >= KIND_ARCHER && (kind) <= KIND_SPY)
#define Kind_IS_INTRIGUE(kind) ((kind) >= KIND_AMBUSH && (kind) <= KIND_ROYAL_DECREE)

typedef struct {
    PyObject_HEAD
    Kind kind;
    int family;
    int tokens; // -1 means revealed
} CardObject;

CardObject* Card_New(int kind, int family, int tokens);

extern PyTypeObject Card_Type;

#endif
