
#include "action.h"

ActionObject* Action_New(int effect, StateObject* current_state) {
    ActionObject* action = PyObject_New(ActionObject, &Action_Type);
    if (!action) {
        return NULL;
    }
    action->effect = effect;
    action->current_state = current_state;
    action->next_state = NULL;
    Py_INCREF(current_state);
    return action;
}

static void Action_dealloc(ActionObject* self) {
    Py_XDECREF(self->current_state);
    Py_XDECREF(self->next_state);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static StateObject* Action_CreateState(ActionObject* self) {
    switch (self->effect) {
    // TODO
    default:
        PyErr_SetString(PyExc_NotImplementedError, "action not implemented");
        return NULL;
    }
}

static PyObject* Action_get_next_state(ActionObject* self, void* closure) {
    if (!self->next_state) {
        self->next_state = Action_CreateState(self);
        if (!self->next_state) {
            return NULL;
        }
    }
    Py_INCREF(self->next_state);
    return (PyObject*)self->next_state;
}

static PyMemberDef Action_members[] = {
    {"current_state", T_OBJECT, offsetof(ActionObject, current_state), READONLY},
    {NULL},
};

static PyGetSetDef Action_getset[] = {
    {"next_state", (getter)Action_get_next_state, NULL, NULL, NULL},
    {NULL},
};

PyTypeObject Action_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = PACKAGE_NAME ".Action",
    .tp_basicsize = sizeof(ActionObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    // TODO maybe expose new/init?
    .tp_dealloc = (destructor)Action_dealloc,
    .tp_members = Action_members,
    .tp_getset = Action_getset,
};
