
#include "score.h"

int Score_CheckMany(PyObject* scores) {

    // Ensure it is a tuple
    if (!scores || !PyTuple_Check(scores)) {
        PyErr_SetString(PyExc_ValueError, "scores must be a tuple");
        return 0;
    }

    // Ensure content is only integers
    Py_ssize_t size = PyTuple_GET_SIZE(scores);
    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject* score = PyTuple_GET_ITEM(scores, i);
        if (!PyLong_Check(score)) {
            PyErr_SetString(PyExc_ValueError, "score tuple must contain only integers");
            return 0;
        }
    }
    return 1;
}

PyObject* Score_Add(PyObject* scores, Py_ssize_t family, int value) {

    // Check index
    Py_ssize_t size = PyTuple_GET_SIZE(scores);
    if (family < 0 || family >= size) {
        PyErr_Format(PyExc_ValueError, "family index out of bounds (0 <= %d <= %d)", family, size);
        return NULL;
    }

    // Create new tuple
    PyObject* new_scores = PyTuple_New(size);
    if (!new_scores) {
        return NULL;
    }

    // Create new score integer
    long old_score = PyLong_AsLong(PyTuple_GET_ITEM(scores, family));
    PyObject* new_score = PyLong_FromLong(old_score + value);
    if (!new_score) {
        Py_DECREF(new_scores);
        return NULL;
    }

    // Fill new tuple
    for (Py_ssize_t i = 0; i < size; ++i) {
        if (i != family) {
            PyObject* s = PyTuple_GET_ITEM(scores, i);
            Py_INCREF(s);
            PyTuple_SET_ITEM(new_scores, i, s);
        }
    }
    PyTuple_SET_ITEM(new_scores, family, new_score);
    return new_scores;
}
