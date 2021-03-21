
#include "deck.h"

int Deck_Check(PyObject* object) {

    // Ensure it is a tuple
    if (!object || !PyTuple_Check(object)) {
        PyErr_SetString(PyExc_ValueError, "deck must be a tuple");
        return 0;
    }

    // Ensure content is only integers (i.e. kinds)
    Py_ssize_t size = PyTuple_GET_SIZE(object);
    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject* item = PyTuple_GET_ITEM(object, i);
        if (!PyLong_Check(item)) {
            PyErr_SetString(PyExc_ValueError, "deck tuple must contain only integers");
            return 0;
        }
        long kind = PyLong_AsLong(item);
        if (kind <= 0 || kind >= KIND_MAX) {
            PyErr_Clear();
            PyErr_SetString(PyExc_ValueError, "deck item must be a valid integer kind");
            return 0;
        }
    }
    return 1;
}

int Deck_CheckMany(PyObject* object) {

    // Ensure it is a tuple
    if (!object || !PyTuple_Check(object)) {
        PyErr_SetString(PyExc_ValueError, "decks must be a tuple");
        return 0;
    }

    // At least two families
    Py_ssize_t size = PyTuple_GET_SIZE(object);
    if (size < 2) {
        PyErr_SetString(PyExc_ValueError, "decks must have at least two families");
        return 0;
    }

    // Ensure content is only decks
    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject* item = PyTuple_GET_ITEM(object, i);
        if (!Deck_Check(item)) {
            return 0;
        }
    }
    return 1;
}
