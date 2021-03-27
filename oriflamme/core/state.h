
#ifndef ORIFLAMME_STATE_H
#define ORIFLAMME_STATE_H

#include "common.h"
#include "card.h"
#include "board.h"
#include "deck.h"
#include "score.h"

typedef enum {
    PHASE_PLACE,
    PHASE_REVEAL,
    PHASE_ACT,
    PHASE_MAX,
} Phase;

typedef struct {
    PyObject_HEAD
    int phase;
    PyObject* board;
    PyObject* decks;
    PyObject* scores;
    int index;
    PyObject* actions;
} StateObject;

/**
 * Create new state.
 * References on `board`, `decks` and `scores` are stolen, even on error.
 */
StateObject* State_New(int phase, PyObject* board, PyObject* decks, PyObject* scores, int index);

/**
 * Copy state object.
 * Note that cached actions are not copied, as it is likely to be modified.
 */
StateObject* State_Copy(StateObject* state);

/**
 * Get current card. Shall not be called in PHASE_PLACE.
 * Return borrowed reference.
 */
#define State_CURRENT_CARD(state) ((CardObject*)(PyTuple_GET_ITEM((state)->board, (state)->index)))

extern PyTypeObject State_Type;

#endif
