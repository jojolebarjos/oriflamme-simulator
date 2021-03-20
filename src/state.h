
#ifndef ORIFLAMME_STATE_H
#define ORIFLAMME_STATE_H

#include "common.h"
#include "card.h"
#include "board.h"
#include "deck.h"

typedef enum {
    PHASE_PLACE,
    PHASE_REVEAL,
    PHASE_ACT_ARCHER,
    PHASE_ACT_SOLDIER,
    PHASE_ACT_SPY,
    PHASE_ACT_HEIR,
    PHASE_ACT_ASSASSINATION,
    PHASE_ACT_ROYAL_DECREE,
    PHASE_ACT_LORD,
    PHASE_ACT_SHAPESHIFTER,
    PHASE_ACT_AMBUSH,
    PHASE_ACT_CONSPIRACY,
    PHASE_MAX,
} Phase;

typedef struct {
    PyObject_HEAD
    int phase;
    PyObject* board;
    PyObject* decks;
    int index;
    int family;
    PyObject* actions;
} StateObject;

extern PyTypeObject State_Type;

#endif
