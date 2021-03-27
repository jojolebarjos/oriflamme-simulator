
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

PyObject* Deck_SetDeck(PyObject* decks, Py_ssize_t index, PyObject* deck) {
    Py_ssize_t size = PyTuple_GET_SIZE(decks);
    if (index < 0 || index >= size) {
        Py_DECREF(deck);
        PyErr_Format(PyExc_ValueError, "deck index out of bounds (0 <= %d <= %d)", index, size);
        return NULL;
    }
    PyObject* new_decks = PyTuple_New(size);
    if (!new_decks) {
        Py_DECREF(deck);
        return NULL;
    }
    for (Py_ssize_t i = 0; i < size; ++i) {
        if (i != index) {
            PyObject* d = PyTuple_GET_ITEM(decks, i);
            Py_INCREF(d);
            PyTuple_SET_ITEM(new_decks, i, d);
        }
    }
    PyTuple_SET_ITEM(new_decks, index, deck);
    return new_decks;
}

PyObject* Deck_RemoveCard(PyObject* deck, int kind) {

    // Search for first card with desired kind
    Py_ssize_t size = PyTuple_GET_SIZE(deck);
    for (Py_ssize_t index = 0; index < size; ++index) {
        PyObject* integer = PyTuple_GET_ITEM(deck, index);
        if (PyLong_AsLong(integer) == kind) {

            // Create copy
            PyObject* new_deck = PyTuple_New(size);
            if (!new_deck) {
                return NULL;
            }
            for (Py_ssize_t i = 0; i < size - 1; ++i) {
                Py_ssize_t j = i < index ? i : i + 1;
                PyObject* c = PyTuple_GET_ITEM(deck, j);
                Py_INCREF(c);
                PyTuple_SET_ITEM(new_deck, i, c);
            }
            return new_deck;
        }
    }

    // If not found, raise error
    PyErr_Format(PyExc_ValueError, "deck does not contain any kind %d", kind);
    return NULL;
}
