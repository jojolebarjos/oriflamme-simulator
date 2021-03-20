
#include "state.h"

static int State_init(StateObject* self, PyObject* args, PyObject* kwargs) {
    static char* kwlist[] = {
        "phase",
        "board",
        "decks",
        "current_index",
        "current_family",
        NULL
    };
    self->board = NULL;
    self->decks = NULL;
    if (!PyArg_ParseTupleAndKeywords(
        args,
        kwargs,
        "iOOii:__init__",
        kwlist,
        &self->phase,
        &self->board,
        &self->decks,
        &self->index,
        &self->family
    )) {
        return -1;
    }
    if (!Board_Check(self->board) || !Deck_CheckMany(self->decks)) {
        return -1;
    }
    if (self->phase < 0 || self->phase >= PHASE_MAX) {
        PyErr_SetString(PyExc_ValueError, "invalid phase");
        return -1;
    }
    if (self->phase == PHASE_PLACE && self->index != -1) {
        PyErr_SetString(PyExc_ValueError, "index must be -1 for PHASE_PLACE");
        return -1;
    }
    if (self->phase != PHASE_PLACE && (self->index < 0 || self->index >= PyTuple_GET_SIZE(self->board))) {
        PyErr_SetString(PyExc_ValueError, "invalid index");
        return -1;
    }
    if (self->family < 0 || self->family >= PyTuple_GET_SIZE(self->decks)) {
        PyErr_SetString(PyExc_ValueError, "invalid family");
        return -1;
    }
    Py_INCREF(self->board);
    Py_INCREF(self->decks);
    self->actions = NULL;
    return 0;
}

static void State_dealloc(StateObject* self) {
    Py_XDECREF(self->board);
    Py_XDECREF(self->decks);
    Py_XDECREF(self->actions);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* State_get_actions(StateObject* self, void* closure) {
    if (!self->actions) {
        // TODO
        self->actions = Py_None;
        Py_INCREF(Py_None);
    }
    Py_INCREF(self->actions);
    return self->actions;
}

static PyMemberDef State_members[] = {
    {"phase", T_INT, offsetof(StateObject, phase), READONLY},
    {"board", T_OBJECT, offsetof(StateObject, board), READONLY},
    {"decks", T_OBJECT, offsetof(StateObject, decks), READONLY},
    {"current_index", T_INT, offsetof(StateObject, index), READONLY},
    {"current_family", T_INT, offsetof(StateObject, family), READONLY},
    {NULL},
};

static PyGetSetDef State_getset[] = {
    {"actions", (getter)State_get_actions, NULL, NULL, NULL},
    {NULL},
};

PyTypeObject State_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "oriflamme.State",
    .tp_basicsize = sizeof(StateObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc)State_init,
    .tp_dealloc = (destructor)State_dealloc,
    .tp_members = State_members,
    .tp_getset = State_getset,
};
