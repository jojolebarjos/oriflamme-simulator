
#include "action.h"

static void Action_dealloc(ActionObject* self) {
    Py_XDECREF(self->current_state);
    Py_XDECREF(self->next_state);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* Action_get_next_state(ActionObject* self, void* closure) {
    if (!self->next_state) {
        // TODO
        self->next_state = (StateObject*)Py_None;
        Py_INCREF(Py_None);
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
    .tp_name = "oriflamme.Action",
    .tp_basicsize = sizeof(ActionObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    // TODO maybe expose new/init?
    .tp_dealloc = (destructor)Action_dealloc,
    .tp_members = Action_members,
    .tp_getset = Action_getset,
};
