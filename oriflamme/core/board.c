
#include "board.h"

int Board_Check(PyObject* board) {

    // Ensure it is a tuple
    if (!board || !PyTuple_Check(board)) {
        PyErr_SetString(PyExc_ValueError, "board must be a tuple");
        return 0;
    }

    // Ensure content is only cards
    Py_ssize_t size = PyTuple_GET_SIZE(board);
    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject* item = PyTuple_GET_ITEM(board, i);
        if (!PyObject_TypeCheck(item, &Card_Type)) {
            PyErr_SetString(PyExc_ValueError, "board tuple must contain only card objects");
            return 0;
        }
    }
    return 1;
}

PyObject* Board_SetCard(PyObject* board, Py_ssize_t index, PyObject* card) {

    // Check indices
    Py_ssize_t size = PyTuple_GET_SIZE(board);
    if (index < 0 || index >= size) {
        Py_DECREF(card);
        PyErr_Format(PyExc_ValueError, "card index out of bounds (0 <= %d <= %d)", index, size);
        return NULL;
    }

    // Create board tuple
    PyObject* new_board = PyTuple_New(size);
    if (!new_board) {
        Py_DECREF(card);
        return NULL;
    }

    // Copy cards
    for (Py_ssize_t i = 0; i < size; ++i) {
        if (i != index) {
            PyObject* c = PyTuple_GET_ITEM(board, i);
            Py_INCREF(c);
            PyTuple_SET_ITEM(new_board, i, c);
        }
    }
    PyTuple_SET_ITEM(new_board, index, card);
    return new_board;
}

PyObject* Board_RemoveCard(PyObject* board, Py_ssize_t index) {

    // Check index
    Py_ssize_t size = PyTuple_GET_SIZE(board);
    if (index < 0 || index >= size) {
        PyErr_Format(PyExc_ValueError, "card index out of bounds (0 <= %d < %d)", index, size);
        return NULL;
    }

    // Create board tuple
    PyObject* new_board = PyTuple_New(size - 1);
    if (!new_board) {
        return NULL;
    }

    // Copy remaining cards
    Py_ssize_t j = 0;
    for (Py_ssize_t i = 0; i < size; ++i) {
        if (i != index) {
            PyObject* c = PyTuple_GET_ITEM(board, i);
            Py_INCREF(c);
            PyTuple_SET_ITEM(new_board, j, c);
            ++j;
        }
    }
    return new_board;
}

PyObject* Board_RemoveCard2(PyObject* board, Py_ssize_t index1, Py_ssize_t index2) {

    // Corner case
    if (index1 == index2) {
        return Board_RemoveCard(board, index1);
    }

    // Check indices
    Py_ssize_t size = PyTuple_GET_SIZE(board);
    if (index1 < 0 || index1 >= size) {
        PyErr_Format(PyExc_ValueError, "first card index out of bounds (0 <= %d < %d)", index1, size);
        return NULL;
    }
    if (index2 < 0 || index1 >= size) {
        PyErr_Format(PyExc_ValueError, "second card index out of bounds (0 <= %d < %d)", index2, size);
        return NULL;
    }

    // Create board tuple
    PyObject* new_board = PyTuple_New(size - 2);
    if (!new_board) {
        return NULL;
    }

    // Copy remaining cards
    Py_ssize_t j = 0;
    for (Py_ssize_t i = 0; i < size; ++i) {
        if (i != index1 && i != index2) {
            PyObject* c = PyTuple_GET_ITEM(board, i);
            Py_INCREF(c);
            PyTuple_SET_ITEM(new_board, j, c);
            ++j;
        }
    }
    return new_board;
}

PyObject* Board_InsertCard(PyObject* board, Py_ssize_t index, PyObject* card) {

    // Check index
    Py_ssize_t size = PyTuple_GET_SIZE(board);
    if (index < 0 || index > size) {
        Py_DECREF(card);
        PyErr_Format(PyExc_ValueError, "card index out of bounds (0 <= %d <= %d)", index, size);
        return NULL;
    }

    // Create board tuple
    PyObject* new_board = PyTuple_New(size + 1);
    if (!new_board) {
        Py_DECREF(card);
        return NULL;
    }

    // Copy cards
    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject* c = PyTuple_GET_ITEM(board, i);
        Py_INCREF(c);
        Py_ssize_t j = i < index ? i : i + 1;
        PyTuple_SET_ITEM(new_board, j, c);
    }
    PyTuple_SET_ITEM(new_board, index, card);
    return new_board;
}

PyObject* Board_SwapCards(PyObject* board, Py_ssize_t first, Py_ssize_t second) {

    // Check indices
    Py_ssize_t size = PyTuple_GET_SIZE(board);
    if (first < 0 || first >= size) {
        PyErr_Format(PyExc_ValueError, "card index out of bounds (0 <= %d <= %d)", first, size);
        return NULL;
    }
    if (second < 0 || second >= size) {
        PyErr_Format(PyExc_ValueError, "card index out of bounds (0 <= %d <= %d)", second, size);
        return NULL;
    }

    // Return current board, if possible
    if (first == second) {
        Py_INCREF(board);
        return board;
    }

    // Create board tuple
    PyObject* new_board = PyTuple_New(size);
    if (!new_board) {
        return NULL;
    }

    // Copy cards
    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject* c = PyTuple_GET_ITEM(board, i);
        Py_INCREF(c);
        Py_ssize_t j = i == first ? second : i == second ? first : i;
        PyTuple_SET_ITEM(new_board, j, c);
    }
    return new_board;
}
