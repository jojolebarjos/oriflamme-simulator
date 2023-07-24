#include "common.h"
#include "kind.h"
#include "stack.h"
#include "state.h"
#include "action.h"

static PyTypeObject * types[] = {
    &Stack_Type,
    &State_Type,
    &Action_Type,
    NULL,
};

static int module_exec(PyObject * module) {

    // Initialize global constants for enums
    for (unsigned k = 0; k < MAX_KIND; ++k)
        if (PyModule_AddIntConstant(module, KIND_NAMES[k], k))
            goto fail;
    for (unsigned k = 0; k < MAX_PHASE; ++k)
        if (PyModule_AddIntConstant(module, PHASE_NAMES[k], k))
            goto fail;
    for (unsigned k = 0; k < MAX_ACT; ++k)
        if (PyModule_AddIntConstant(module, ACT_NAMES[k], k))
            goto fail;

    // Initialize types
    for (PyTypeObject ** t = types; *t; ++t)
        if (PyType_Ready(*t))
            goto fail;

    // Register types
    for (PyTypeObject ** t = types; *t; ++t) {
        Py_INCREF(*t);
        if (PyModule_AddObject(module, (*t)->tp_name + sizeof("oriflamme.") - 1, (PyObject *)*t)) {
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
    {Py_mod_exec, (void *)module_exec},
    {0, NULL},
};

static struct PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "oriflamme",
    .m_doc = NULL,
    .m_size = 0,
    .m_slots = slots,
};

PyMODINIT_FUNC PyInit_oriflamme() {
    return PyModuleDef_Init(&module);
}
