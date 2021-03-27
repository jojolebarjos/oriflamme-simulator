
#include "state.h"
#include "action.h"

char const* Phase_NAMES[PHASE_MAX] = {
    "NONE",
    "PLACE",
    "REVEAL",
    "ACT",
};

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

static PyObject* State_CreateActionsPlace(StateObject* self) {
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
            ActionObject* action = Action_New(EFFECT_PLACE, kind, (int)board_size, self);
            if (!action) {
                Py_DECREF(actions);
                return NULL;
            }
            PyTuple_SET_ITEM(actions, card_count + i, (PyObject*)action);
        }
    }
    return actions;
}

static PyObject* State_CreateActionsReveal(StateObject* self) {

    // Create "reveal" action
    ActionObject* reveal_action = Action_New(
        EFFECT_REVEAL,
        -1, -1,
        self
    );
    if (!reveal_action) {
        return NULL;
    }

    // Create "increase" action
    ActionObject* increase_action = Action_New(
        EFFECT_INCREASE,
        -1, -1,
        self
    );
    if (!increase_action) {
        Py_DECREF(reveal_action);
        return NULL;
    }

    // Pack actions
    PyObject* actions = PyTuple_Pack(2, reveal_action, increase_action);
    Py_DECREF(reveal_action);
    Py_DECREF(increase_action);
    return actions;
}

static PyObject* State_CreateActionsAct(StateObject* self) {
    CardObject* card = State_CURRENT_CARD(self);
    switch (card->kind) {

    // Archer kills first or last
    case KIND_ARCHER:
        // TODO archer
        PyErr_SetString(PyExc_NotImplementedError, "archer action not implemented");
        return NULL;

    // Heir yields 2 tokens, if alone
    case KIND_HEIR:
        // TODO heir
        PyErr_SetString(PyExc_NotImplementedError, "heir action not implemented");
        return NULL;

    // Lord yield n+1 tokens, n being the number of adjacent cards of the same family
    case KIND_LORD:
        // TODO lord
        PyErr_SetString(PyExc_NotImplementedError, "lord action not implemented");
        return NULL;

    // Shapeshifter copy the role of a neighboring (revealed) card
    case KIND_SHAPESHIFTER:
        // TODO shapeshifter
        PyErr_SetString(PyExc_NotImplementedError, "shapeshifter action not implemented");
        return NULL;

    // Soldier kills previous or next
    case KIND_SOLDIER:
        // TODO soldier
        PyErr_SetString(PyExc_NotImplementedError, "soldier action not implemented");
        return NULL;

    // Spy steals from neighboring family
    case KIND_SPY:
        // TODO spy
        PyErr_SetString(PyExc_NotImplementedError, "spy action not implemented");
        return NULL;

    // Ambush does nothing, if revealed voluntarily
    case KIND_AMBUSH:
        // TODO ambush
        PyErr_SetString(PyExc_NotImplementedError, "ambush action not implemented");
        return NULL;

    // Assassination kills one card
    case KIND_ASSASSINATION:
        // TODO assassination
        PyErr_SetString(PyExc_NotImplementedError, "assassination action not implemented");
        return NULL;

    // Conspiracy does nothing
    case KIND_CONSPIRACY:
        // TODO conspiracy
        PyErr_SetString(PyExc_NotImplementedError, "conspiracy action not implemented");
        return NULL;

    // Royal decree swap two cards
    case KIND_ROYAL_DECREE:
        // TODO royal decree
        PyErr_SetString(PyExc_NotImplementedError, "royal decree action not implemented");
        return NULL;

    // Unexpected card kind means no action
    default:
        PyErr_SetString(PyExc_NotImplementedError, "card action not implemented");
        return NULL;
    }
}

static PyObject* State_CreateActions(StateObject* self) {
    switch (self->phase) {
    case PHASE_NONE:
        return PyTuple_New(0);
    case PHASE_PLACE:
        return State_CreateActionsPlace(self);
    case PHASE_REVEAL:
        return State_CreateActionsReveal(self);
    case PHASE_ACT:
        return State_CreateActionsAct(self);
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

static PyObject* State_get_card(StateObject* self, void* closure) {
    PyObject* card;
    if (self->phase == PHASE_REVEAL || self->phase == PHASE_ACT) {
        card = (PyObject*)State_CURRENT_CARD(self);
    } else {
        card = Py_None;
    }
    Py_INCREF(card);
    return card;
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
    {"card", (getter)State_get_card, NULL, NULL, NULL},
    // TODO is_finished?
    // TODO winning_family?
    {NULL},
};

static PyObject* State_repr(StateObject* self) {
    switch (self->phase) {
    case PHASE_NONE:
        return PyUnicode_FromFormat(
            "State(%s)",
            Phase_NAMES[self->phase]
        );
    case PHASE_PLACE:
        return PyUnicode_FromFormat(
            "State(%s, family=%d)",
            Phase_NAMES[self->phase],
            self->index
        );
    case PHASE_REVEAL:
    case PHASE_ACT:
        return PyUnicode_FromFormat(
            "State(%s, %R@%d)",
            Phase_NAMES[self->phase],
            State_CURRENT_CARD(self),
            self->index
        );
    default:
        PyErr_SetString(PyExc_NotImplementedError, "phase not implemented");
        return NULL;
    }
}

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
    .tp_repr = (reprfunc)State_repr,
};
