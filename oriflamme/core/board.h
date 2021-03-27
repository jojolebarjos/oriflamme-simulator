
#ifndef ORIFLAMME_BOARD_H
#define ORIFLAMME_BOARD_H

#include "common.h"
#include "card.h"

/**
 * Make sure this is a board, i.e. a tuple of cards.
 */
int Board_Check(PyObject* board);

/**
 * Copy board, with specified card at given index.
 * Reference on `card` is stolen, even on error.
 */
PyObject* Board_SetCard(PyObject* board, Py_ssize_t index, PyObject* card);

/**
 * Copy board, with one card removed.
 */
PyObject* Board_RemoveCard(PyObject* board, Py_ssize_t index);

/**
 * Copy board, with specified card inserted.
 * Reference on `card` is stolen, even on error.
 */
PyObject* Board_InsertCard(PyObject* board, Py_ssize_t index, PyObject* card);

/**
 * Copy board, with specified cards swapped.
 */
PyObject* Board_SwapCards(PyObject* board, Py_ssize_t first, Py_ssize_t second);

#endif
