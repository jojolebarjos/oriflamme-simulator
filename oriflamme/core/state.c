
#include "state.h"
#include "action.h"

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
        &self->current_index,
        &self->current_family
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
    if (self->phase == PHASE_PLACE && self->current_index != -1) {
        PyErr_SetString(PyExc_ValueError, "index must be -1 for PHASE_PLACE");
        return -1;
    }
    if (self->phase != PHASE_PLACE && (self->current_index < 0 || self->current_index >= PyTuple_GET_SIZE(self->board))) {
        PyErr_SetString(PyExc_ValueError, "invalid index");
        return -1;
    }
    if (self->current_family < 0 || self->current_family >= PyTuple_GET_SIZE(self->decks)) {
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

static PyObject* State_CreatePlaceActions(StateObject* self) {

    // For convenience
    int family = self->current_family;
    PyObject* board = self->board;
    Py_ssize_t board_size = PyTuple_GET_SIZE(board);
    PyObject* deck = PyTuple_GET_ITEM(self->decks, family);
    Py_ssize_t card_count = PyTuple_GET_SIZE(deck);

    // Compute how many options are available
    // TODO change that when stacking will be allowed
    Py_ssize_t action_count = board_size == 0 ? card_count : card_count * 2;

    // Allocate output tuple
    PyObject* actions = PyTuple_New(action_count);
    if (!actions) {
        return NULL;
    }

    // Build actions
    for (Py_ssize_t i = 0; i < card_count; ++i) {
        // TODO proper action

        // Place front
        ActionObject* action = Action_New(EFFECT_PLACE, self);
        if (!action) {
            Py_DECREF(actions);
            return NULL;
        }
        PyTuple_SET_ITEM(actions, i, (PyObject*)action);

        // Place back
        if (board_size > 0) {
            ActionObject* action = Action_New(EFFECT_PLACE, self);
            if (!action) {
                Py_DECREF(actions);
                return NULL;
            }
            PyTuple_SET_ITEM(actions, card_count + i, (PyObject*)action);
        }
    }
    return actions;
}

static PyObject* State_CreateRevealActions(StateObject* self) {
    PyErr_SetString(PyExc_NotImplementedError, "reveal phase not implemented");
    return NULL;
}

static PyObject* State_CreateActions(StateObject* self) {
    switch (self->phase) {
    case PHASE_PLACE:
        return State_CreatePlaceActions(self);
    case PHASE_REVEAL:
        return State_CreateRevealActions(self);
    default:
        PyErr_SetString(PyExc_NotImplementedError, "phase not implemented");
        return NULL;
    }
}

static PyObject* State_get_actions(StateObject* self, void* closure) {
    if (!self->actions) {
        self->actions = State_CreateActions(self);
        if (!self->actions) {
            return NULL;
        }
    }
    Py_INCREF(self->actions);
    return self->actions;
}

static PyMemberDef State_members[] = {
    {"phase", T_INT, offsetof(StateObject, phase), READONLY},
    {"board", T_OBJECT, offsetof(StateObject, board), READONLY},
    {"decks", T_OBJECT, offsetof(StateObject, decks), READONLY},
    {"current_index", T_INT, offsetof(StateObject, current_index), READONLY},
    {"current_family", T_INT, offsetof(StateObject, current_family), READONLY},
    {NULL},
};

static PyGetSetDef State_getset[] = {
    {"actions", (getter)State_get_actions, NULL, NULL, NULL},
    {NULL},
};

PyTypeObject State_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = PACKAGE_NAME ".State",
    .tp_basicsize = sizeof(StateObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc)State_init,
    .tp_dealloc = (destructor)State_dealloc,
    .tp_members = State_members,
    .tp_getset = State_getset,
};
