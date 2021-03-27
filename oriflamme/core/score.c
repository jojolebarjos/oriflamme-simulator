
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

PyObject* Score_Transfer(PyObject* scores, Py_ssize_t src, Py_ssize_t dst, int value) {

    // Check indices
    Py_ssize_t size = PyTuple_GET_SIZE(scores);
    if (src < 0 || src >= size) {
        PyErr_Format(PyExc_ValueError, "source family index out of bounds (0 <= %d <= %d)", src, size);
        return NULL;
    }
    if (dst < 0 || dst >= size) {
        PyErr_Format(PyExc_ValueError, "destination family index out of bounds (0 <= %d <= %d)", dst, size);
        return NULL;
    }

    // Re-use, if possible
    if (value == 0 || src == dst) {
        Py_INCREF(scores);
        return scores;
    }

    // Create new tuple
    PyObject* new_scores = PyTuple_New(size);
    if (!new_scores) {
        return NULL;
    }

    // Create new score integers
    long old_src_score = PyLong_AsLong(PyTuple_GET_ITEM(scores, src));
    long old_dst_score = PyLong_AsLong(PyTuple_GET_ITEM(scores, dst));
    PyObject* new_src_score = PyLong_FromLong(old_src_score - value);
    if (!new_src_score) {
        Py_DECREF(new_scores);
        return NULL;
    }
    PyObject* new_dst_score = PyLong_FromLong(old_dst_score + value);
    if (!new_dst_score) {
        Py_DECREF(new_scores);
        Py_DECREF(new_src_score);
        return NULL;
    }

    // Fill new tuple
    for (Py_ssize_t i = 0; i < size; ++i) {
        if (i != src && i != dst) {
            PyObject* s = PyTuple_GET_ITEM(scores, i);
            Py_INCREF(s);
            PyTuple_SET_ITEM(new_scores, i, s);
        }
    }
    PyTuple_SET_ITEM(new_scores, src, new_src_score);
    PyTuple_SET_ITEM(new_scores, dst, new_dst_score);
    return new_scores;
}
