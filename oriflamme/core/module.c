
#include "common.h"
#include "card.h"
#include "board.h"
#include "state.h"
#include "action.h"

static PyMethodDef methods[] = {
    {NULL},
};

static PyTypeObject* types[] = {
    &Card_Type,
    &State_Type,
    &Action_Type,
    NULL,
};

static int module_exec(PyObject* module) {

    // Prepare custom types
    for (PyTypeObject** t = types; *t; ++t) {
        if (PyType_Ready(*t) < 0) {
            goto fail;
        }
    }

    // Register types
    for (PyTypeObject** t = types; *t; ++t) {
        Py_INCREF(*t);
        if (PyModule_AddObject(module, (*t)->tp_name + sizeof(PACKAGE_NAME ".") - 1, (PyObject*)*t) < 0) {
            Py_DECREF(*t);
            goto fail;
        }
    }

    return 0;

fail:
    Py_XDECREF(module);
    return -1;
}

static PyModuleDef_Slot slots[] = {
    {Py_mod_exec, module_exec},
    {0, NULL},
};

static struct PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    .m_name = PACKAGE_NAME,
    .m_methods = methods,
    .m_slots = slots,
};

PyMODINIT_FUNC PyInit_core() {
    return PyModuleDef_Init(&module);
}
