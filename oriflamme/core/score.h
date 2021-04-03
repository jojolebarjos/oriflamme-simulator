
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

/**
 * Copy score collection, with specified scores at given indices.
 */
PyObject* Score_Add2(PyObject* scores, Py_ssize_t family1, int value1, Py_ssize_t family2, int value2);

#endif
