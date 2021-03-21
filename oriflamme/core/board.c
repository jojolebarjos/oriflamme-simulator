
#include "board.h"

int Board_Check(PyObject* object) {

    // Ensure it is a tuple
    if (!object || !PyTuple_Check(object)) {
        PyErr_SetString(PyExc_ValueError, "board must be a tuple");
        return 0;
    }

    // Ensure content is only cards
    Py_ssize_t size = PyTuple_GET_SIZE(object);
    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject* item = PyTuple_GET_ITEM(object, i);
        if (!PyObject_TypeCheck(item, &Card_Type)) {
            PyErr_SetString(PyExc_ValueError, "board tuple must contain only card objects");
            return 0;
        }
    }
    return 1;
}
