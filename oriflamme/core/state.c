
#include "state.h"
#include "action.h"

StateObject* State_New(int phase, PyObject* board, PyObject* decks, PyObject* scores, int index) {
    StateObject* new_state = PyObject_New(StateObject, &State_Type);
    if (!new_state) {
        Py_DECREF(board);
        Py_DECREF(decks);
        Py_DECREF(scores);
        return NULL;
    }
    new_state->phase = phase;
    new_state->board = board;
    new_state->decks = decks;
    new_state->scores = scores;
    new_state->index = index;
    new_state->actions = NULL;
    return new_state;
}

StateObject* State_Copy(StateObject* state) {
    StateObject* new_state = PyObject_New(StateObject, &State_Type);
    if (!new_state) {
        return NULL;
    }
    new_state->phase = state->phase;
    new_state->board = state->board;
    new_state->decks = state->decks;
    new_state->scores = state->scores;
    new_state->index = state->index;
    new_state->actions = NULL;
    Py_INCREF(new_state->board);
    Py_INCREF(new_state->decks);
    Py_INCREF(new_state->scores);
    return new_state;
}

static int State_init(StateObject* self, PyObject* args, PyObject* kwargs) {
    static char* kwlist[] = {
        "phase",
        "board",
        "decks",
        "scores",
        "index",
        NULL
    };
    self->board = NULL;
    self->decks = NULL;
    self->scores = NULL;
    if (!PyArg_ParseTupleAndKeywords(
        args,
        kwargs,
        "iOOOi:__init__",
        kwlist,
        &self->phase,
        &self->board,
        &self->decks,
        &self->scores,
        &self->index
    )) {
        return -1;
    }
    if (!Board_Check(self->board) || !Deck_CheckMany(self->decks) || !Score_CheckMany(self->scores)) {
        return -1;
    }
    if (PyTuple_GET_SIZE(self->decks) != PyTuple_GET_SIZE(self->scores)) {
        PyErr_Format(
            PyExc_ValueError,
            "decks and scores count mismatch (%d != %d)",
            PyTuple_GET_SIZE(self->decks),
            PyTuple_GET_SIZE(self->scores)
        );
        return -1;
    }
    if (self->phase < 0 || self->phase >= PHASE_MAX) {
        PyErr_SetString(PyExc_ValueError, "invalid phase");
        return -1;
    }
    if (self->phase == PHASE_PLACE) {
        Py_ssize_t num_families = PyTuple_GET_SIZE(self->decks);
        if (self->index < 0 || self->index >= num_families) {
            PyErr_Format(PyExc_ValueError, "family index out of bounds (0 <= %d <= %d)", self->index, num_families);
            return -1;
        }
    } else {
        Py_ssize_t board_size = PyTuple_GET_SIZE(self->board);
        if (self->index < 0 || self->index >= board_size) {
            PyErr_Format(PyExc_ValueError, "card index out of bounds (0 <= %d <= %d)", self->index, board_size);
            return -1;
        }
    }
    Py_INCREF(self->board);
    Py_INCREF(self->decks);
    Py_INCREF(self->scores);
    self->actions = NULL;
    return 0;
}

static void State_dealloc(StateObject* self) {
    Py_XDECREF(self->board);
    Py_XDECREF(self->decks);
    Py_XDECREF(self->scores);
    Py_XDECREF(self->actions);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* State_CreatePlaceActions(StateObject* self) {
    // TODO check for duplicated kinds (i.e. merge actions), typically fixing tuple with _PyTuple_Resize

    // For convenience
    int family = self->index;
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
        int kind = PyLong_AsLong(PyTuple_GET_ITEM(deck, i));

        // Place front
        ActionObject* action = Action_New(EFFECT_PLACE, kind, 0, self);
        if (!action) {
            Py_DECREF(actions);
            return NULL;
        }
        PyTuple_SET_ITEM(actions, i, (PyObject*)action);

        // Place back
        if (board_size > 0) {
            ActionObject* action = Action_New(EFFECT_PLACE, kind, (int)card_count, self);
            if (!action) {
                Py_DECREF(actions);
                return NULL;
            }
            PyTuple_SET_ITEM(actions, board_size + i, (PyObject*)action);
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
    {"scores", T_OBJECT, offsetof(StateObject, scores), READONLY},
    {"index", T_INT, offsetof(StateObject, index), READONLY},
    {NULL},
};

static PyGetSetDef State_getset[] = {
    {"actions", (getter)State_get_actions, NULL, NULL, NULL},
    // TODO current_family
    // TODO current_card
    // TODO is_finished
    // TODO winning_family?
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
