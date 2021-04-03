
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

    // Fetch arguments
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

    // Check that structured arguments have a valid format
    if (!Board_Check(self->board) || !Deck_CheckMany(self->decks) || !Score_CheckMany(self->scores)) {
        return -1;
    }

    // At least one family is required
    Py_ssize_t num_families = PyTuple_GET_SIZE(self->decks);
    if (num_families < 1) {
        PyErr_SetString(PyExc_ValueError, "at least one family is needed");
        return -1;
    }

    // Check that number of families if consistent
    if (PyTuple_GET_SIZE(self->decks) != PyTuple_GET_SIZE(self->scores)) {
        PyErr_Format(
            PyExc_ValueError,
            "decks and scores count mismatch (%d != %d)",
            PyTuple_GET_SIZE(self->decks),
            PyTuple_GET_SIZE(self->scores)
        );
        return -1;
    }

    // Check that phase type is valid
    if (self->phase < 0 || self->phase >= PHASE_MAX) {
        PyErr_SetString(PyExc_ValueError, "invalid phase");
        return -1;
    }

    // None phase (i.e. game has ended) has no index
    if (self->phase == PHASE_NONE) {

        // Index must be -1
        if (self->index != -1) {
            PyErr_SetString(PyExc_ValueError, "index must be -1 at end game");
            return -1;
        }

        // TODO check that decks are empty
    }

    // Otherwise, the game is on-going
    else {

        // Placement phase indexes a family
        if (self->phase == PHASE_PLACE) {
            if (self->index < 0 || self->index >= num_families) {
                PyErr_Format(PyExc_ValueError, "family index out of bounds (0 <= %d < %d)", self->index, num_families);
                return -1;
            }
        }

        // Other phases index a location
        else {
            Py_ssize_t board_size = PyTuple_GET_SIZE(self->board);
            if (self->index < 0 || self->index >= board_size) {
                PyErr_Format(PyExc_ValueError, "card index out of bounds (0 <= %d <= %d)", self->index, board_size);
                return -1;
            }
        }

        // Placement phase requires non-empty decks
        Py_ssize_t remaining = PyTuple_GET_SIZE(PyTuple_GET_ITEM(self->decks, num_families - 1));
        if (self->phase == PHASE_PLACE && remaining < 1) {
            PyErr_SetString(PyExc_ValueError, "placement phase requires non-empty decks (for families that did not place yet)");
            return -1;
        }

        // Check that decks sizes are consistent
        for (Py_ssize_t i = 0; i < num_families - 1; ++i) {
            Py_ssize_t expected = remaining;
            if (self->phase == PHASE_PLACE && i < self->index) {
                expected -= 1;
            }
            Py_ssize_t actual = PyTuple_GET_SIZE(PyTuple_GET_ITEM(self->decks, i));
            if (expected != actual) {
                PyErr_Format(PyExc_ValueError, "family %d has invalid deck size (%d instead of %d)", i, actual, expected);
                return -1;
            }
        }
    }

    // Parameters are acceptable, proceed
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

static PyObject* State_PackActions(StateObject* self, Py_ssize_t count, ActionObject** actions) {

    // Count actual actions
    Py_ssize_t actual = 0;
    for (Py_ssize_t i = 0; i < count; ++i) {
        if (actions[i]) {
            ++actual;
        }
    }

    // If there is no action, set to "none"
    ActionObject* no_action = NULL;
    if (actual == 0) {
        no_action = Action_New(EFFECT_NONE, -1, -1, self);
        if (!no_action) {
            return NULL;
        }
        actual = count = 1;
        actions = &no_action;
    }

    // Create tuple
    PyObject* result = PyTuple_New(actual);
    if (!result) {
        for (Py_ssize_t i = 0; i < count; ++i) {
            Py_XDECREF(actions[i]);
        }
        return NULL;
    }

    // Fill tuple
    Py_ssize_t j = 0;
    for (Py_ssize_t i = 0; i < count; ++i) {
        if (actions[i]) {
            PyTuple_SET_ITEM(result, j, (PyObject*)actions[i]);
            ++j;
        }
    }
    return result;
}

static PyObject* State_CreateActionsPlace(StateObject* self) {
    // TODO check for duplicated kinds (i.e. merge actions), typically fixing tuple with _PyTuple_Resize

    // Get parameters
    int family = self->index;
    PyObject* board = self->board;
    Py_ssize_t board_size = PyTuple_GET_SIZE(board);
    PyObject* deck = PyTuple_GET_ITEM(self->decks, family);
    Py_ssize_t card_count = PyTuple_GET_SIZE(deck);

    // Compute how many options are available
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
    ActionObject* actions[2];

    // Create "reveal" action
    actions[0] = Action_New(
        EFFECT_REVEAL,
        -1, -1,
        self
    );
    if (!actions[0]) {
        return NULL;
    }

    // Create "increase" action
    actions[1] = Action_New(
        EFFECT_INCREASE,
        -1, -1,
        self
    );
    if (!actions[1]) {
        Py_DECREF(actions[0]);
        return NULL;
    }

    // Pack actions
    return State_PackActions(self, 2, actions);
}

static PyObject* State_CreateActionsArcher(StateObject* self) {
    ActionObject* actions[2];

    // Get properties
    PyObject* board = self->board;
    Py_ssize_t board_size = PyTuple_GET_SIZE(board);

    // First card of the queue
    actions[0] = Action_New(EFFECT_KILL, 0, -1, self);
    if (!actions[0]) {
        return NULL;
    }

    // If there is only one card (i.e. the archer itself), it is the only action
    if (board_size < 2) {
        actions[1] = NULL;
    }

    // Otherwise, last card of the queue
    else {
        actions[1] = Action_New(EFFECT_KILL, (int)(board_size - 1), -1, self);
        if (!actions[1]) {
            Py_DECREF(actions[1]);
            return NULL;
        }
    }

    // Pack as tuple
    return State_PackActions(self, 2, actions);
}

static PyObject* State_CreateActionsHeir(StateObject* self) {

    // Get properties
    PyObject* board = self->board;
    Py_ssize_t board_size = PyTuple_GET_SIZE(board);

    // Check if there is any other revealed heir
    ActionObject* action = NULL;
    for (Py_ssize_t i = 0; i < board_size; ++i) {
        if (i != self->index) {
            CardObject* card = (CardObject*)PyTuple_GET_ITEM(board, i);
            // TODO should not check KIND_HEIR, but actually the kind of the current card
            if (card->kind == KIND_HEIR && card->tokens < 0) {

                // If yes, then the heir has no effect
                action = Action_New(EFFECT_NONE, -1, -1, self);
                if (!action) {
                    return NULL;
                }
                break;
            }
        }
    }

    // If no, then we can earn some points
    if (!action) {
        action = Action_New(EFFECT_EARN, 2, -1, self);
    }

    // Pack as tuple
    return State_PackActions(self, 1, &action);
}

static PyObject* State_CreateActionsLord(StateObject* self) {

    // Get properties
    PyObject* board = self->board;
    Py_ssize_t board_size = PyTuple_GET_SIZE(board);
    Py_ssize_t index = self->index;
    CardObject* card = (CardObject*)PyTuple_GET_ITEM(board, index);

    // At least one token
    int increment = 1;

    // Check left
    if (index > 0) {
        CardObject* left_card = (CardObject*)PyTuple_GET_ITEM(board, index - 1);
        if (left_card->family == card->family) {
            increment += 1;
        }
    }

    // Check right
    if (index + 1 < board_size) {
        CardObject* right_card = (CardObject*)PyTuple_GET_ITEM(board, index + 1);
        if (right_card->family == card->family) {
            increment += 1;
        }
    }

    // Create action
    ActionObject* action = Action_New(EFFECT_EARN, increment, -1, self);
    if (!action) {
        return NULL;
    }

    // Pack as tuple
    return State_PackActions(self, 1, &action);
}

static PyObject* State_CreateActionsShapeshifter(StateObject* self) {
    // TODO shapeshifter
    PyErr_SetString(PyExc_NotImplementedError, "shapeshifter action not implemented");
    return NULL;
}

static PyObject* State_CreateActionsSoldier(StateObject* self) {
    ActionObject* actions[2];

    // Get properties
    PyObject* board = self->board;
    Py_ssize_t board_size = PyTuple_GET_SIZE(board);
    Py_ssize_t index = self->index;

    // Left card
    actions[0] = NULL;
    if (index > 0) {
        actions[0] = Action_New(EFFECT_KILL, (int)(index - 1), -1, self);
        if (!actions[0]) {
            return NULL;
        }
    }

    // Right card
    actions[1] = NULL;
    if (index + 1 < board_size) {
        actions[1] = Action_New(EFFECT_KILL, (int)(index + 1), -1, self);
        if (!actions[1]) {
            Py_XDECREF(actions[0]);
            return NULL;
        }
    }

    // Pack as tuple
    return State_PackActions(self, 2, actions);
}

static PyObject* State_CreateActionsSpy(StateObject* self) {
    // TODO spy
    PyErr_SetString(PyExc_NotImplementedError, "spy action not implemented");
    return NULL;
}

static PyObject* State_CreateActionsAmbush(StateObject* self) {

    // Do nothing (i.e. +1 was already applied during reveal)
    return State_PackActions(self, 0, NULL);
}

static PyObject* State_CreateActionsAssassination(StateObject* self) {

    // Get properties
    PyObject* board = self->board;
    Py_ssize_t board_size = PyTuple_GET_SIZE(board);
    Py_ssize_t index = self->index;

    // If there is no other card, then we cannot do anything
    if (board_size < 2) {
        return State_PackActions(self, 0, NULL);
    }

    // Otherwise, each card can be killed
    PyObject* actions = PyTuple_New(board_size - 1);
    for (Py_ssize_t i = 0; i < board_size - 1; ++i) {
        Py_ssize_t j = i >= index ? i + 1 : i;
        ActionObject* action = Action_New(EFFECT_KILL, (int)j, -1, self);
        if (!action) {
            Py_DECREF(actions);
            return NULL;
        }
        PyTuple_SET_ITEM(actions, i, (PyObject*)action);
    }
    return actions;
}

static PyObject* State_CreateActionsConspiracy(StateObject* self) {

    // Do nothing (i.e. double was already applied during reveal)
    return State_PackActions(self, 0, NULL);
}

static PyObject* State_CreateActionsRoyalDecree(StateObject* self) {
    // TODO royal decree
    PyErr_SetString(PyExc_NotImplementedError, "royal decree action not implemented");
    return NULL;
}

static PyObject* State_CreateActionsAct(StateObject* self) {
    CardObject* card = State_CURRENT_CARD(self);
    switch (card->kind) {

    // Archer kills first or last
    case KIND_ARCHER:
        return State_CreateActionsArcher(self);

    // Heir yields 2 tokens, if alone
    case KIND_HEIR:
        return State_CreateActionsHeir(self);

    // Lord yield n+1 tokens, n being the number of adjacent cards of the same family
    case KIND_LORD:
        return State_CreateActionsLord(self);

    // Shapeshifter copy the role of a neighboring (revealed) card
    case KIND_SHAPESHIFTER:
        return State_CreateActionsShapeshifter(self);

    // Soldier kills previous or next
    case KIND_SOLDIER:
        return State_CreateActionsSoldier(self);

    // Spy steals from neighboring family
    case KIND_SPY:
        return State_CreateActionsSpy(self);

    // Ambush does nothing, if revealed voluntarily
    case KIND_AMBUSH:
        return State_CreateActionsAmbush(self);

    // Assassination kills one card
    case KIND_ASSASSINATION:
        return State_CreateActionsAssassination(self);

    // Conspiracy does nothing
    case KIND_CONSPIRACY:
        return State_CreateActionsConspiracy(self);

    // Royal decree swap two cards
    case KIND_ROYAL_DECREE:
        return State_CreateActionsRoyalDecree(self);

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
