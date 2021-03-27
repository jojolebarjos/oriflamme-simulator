
#ifndef ORIFLAMME_SCORE_H
#define ORIFLAMME_SCORE_H

#include "common.h"

/**
 * Make sure this is a score collection, i.e. a tuple of integer.
 */
int Score_CheckMany(PyObject* scores);

/**
 * Copy score collection, with specified score at given index.
 */
PyObject* Score_Add(PyObject* scores, Py_ssize_t family, int value);

#endif
