
#ifndef ORIFLAMME_DECK_H
#define ORIFLAMME_DECK_H

#include "common.h"
#include "card.h"

/**
 * Make sure this is a deck, i.e. a tuple of integers (0 <= k < KIND_MAX)
 */
int Deck_Check(PyObject* object);

/**
 * Make sure this is a deck collection, i.e. a tuple of decks
 */
int Deck_CheckMany(PyObject* object);

/**
 * Copy deck collection, with specified deck at given index.
 * Reference on `deck` is stolen, even on error.
 */
PyObject* Deck_SetDeck(PyObject* decks, Py_ssize_t index, PyObject* deck);

/**
 * Copy deck, with a single card removed.
 */
PyObject* Deck_RemoveCard(PyObject* deck, int kind);

#endif
