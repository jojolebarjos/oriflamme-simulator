
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

    // Re-use, if possible
    if (value == 0) {
        Py_INCREF(scores);
        return scores;
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

PyObject* Score_Add2(PyObject* scores, Py_ssize_t family1, int value1, Py_ssize_t family2, int value2) {

    // Corner cases
    if (family1 == family2) {
        return Score_Add(scores, family1, value1 + value2);
    }
    if (value1 == 0) {
        return Score_Add(scores, family2, value2);
    }
    if (value2 == 0) {
        return Score_Add(scores, family1, value1);
    }

    // Check indices
    Py_ssize_t size = PyTuple_GET_SIZE(scores);
    if (family1 < 0 || family1 >= size) {
        PyErr_Format(PyExc_ValueError, "first family index out of bounds (0 <= %d <= %d)", family1, size);
        return NULL;
    }
    if (family2 < 0 || family2 >= size) {
        PyErr_Format(PyExc_ValueError, "second family index out of bounds (0 <= %d <= %d)", family2, size);
        return NULL;
    }

    // Re-use, if possible
    if (value1 == 0 && value2 == 0) {
        Py_INCREF(scores);
        return scores;
    }

    // Create new tuple
    PyObject* new_scores = PyTuple_New(size);
    if (!new_scores) {
        return NULL;
    }

    // Create new score integers
    long old_score1 = PyLong_AsLong(PyTuple_GET_ITEM(scores, family1));
    PyObject* new_score1 = PyLong_FromLong(old_score1 + value1);
    if (!new_score1) {
        Py_DECREF(new_scores);
        return NULL;
    }
    long old_score2 = PyLong_AsLong(PyTuple_GET_ITEM(scores, family2));
    PyObject* new_score2 = PyLong_FromLong(old_score2 + value2);
    if (!new_score2) {
        Py_DECREF(new_scores);
        Py_DECREF(new_score1);
        return NULL;
    }

    // Fill new tuple
    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject* s;
        if (i == family1) {
            s = new_score1;
        } else if (i == family2) {
            s = new_score2;
        } else {
            s = PyTuple_GET_ITEM(scores, i);
            Py_INCREF(s);
        }
        PyTuple_SET_ITEM(new_scores, i, s);
    }
    return new_scores;
}
