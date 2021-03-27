
#include "action.h"

ActionObject* Action_New(int effect, int first, int second, StateObject* current_state) {
    ActionObject* action = PyObject_New(ActionObject, &Action_Type);
    if (!action) {
        return NULL;
    }
    action->effect = effect;
    action->first = first;
    action->second = second;
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

static StateObject* Action_CreateState_PostAction(
    StateObject* current_state,
    PyObject* new_board,
    PyObject* new_decks,
    PyObject* new_scores
) {

    // Use existing values as default
    if (!new_board) {
        new_board = current_state->board;
        Py_INCREF(new_board);
    }
    if (!new_decks) {
        new_decks = current_state->decks;
        Py_INCREF(new_decks);
    }
    if (!new_scores) {
        new_scores = current_state->scores;
        Py_INCREF(new_scores);
    }

    // If it was the last card in the queue, start a new turn
    int new_index = current_state->index + 1;
    if (new_index >= PyTuple_GET_SIZE(new_board)) {
        return State_New(
            PHASE_PLACE,
            new_board,
            new_decks,
            new_scores,
            0
        );
    }

    // If next card is not yet revealed, ask for reveal
    CardObject* next_card = (CardObject*)(PyTuple_GET_ITEM(new_board, new_index));
    if (next_card->tokens >= 0) {
        return State_New(
            PHASE_REVEAL,
            new_board,
            new_decks,
            new_scores,
            new_index
        );
    }

    // Otherwise, ask for action
    return State_New(
        PHASE_ACT,
        new_board,
        new_decks,
        new_scores,
        new_index
    );
}

static StateObject* Action_CreateStatePlace(ActionObject* self) {

    // Get parameters
    StateObject* current_state = self->current_state;
    PyObject* board = current_state->board;
    int family = current_state->index;
    PyObject* decks = current_state->decks;
    Py_ssize_t num_families = PyTuple_GET_SIZE(decks);
    PyObject* deck = PyTuple_GET_ITEM(decks, family);
    int kind = self->first;
    int index = self->second;

    // Create new deck, with one card less
    PyObject* new_deck = Deck_RemoveCard(deck, kind);
    if (!new_deck) {
        return NULL;
    }

    // Create new deck collection
    PyObject* new_decks = Deck_SetDeck(decks, family, new_deck);
    if (!new_decks) {
        return NULL;
    }

    // Create new card
    CardObject* new_card = Card_New(kind, family, 0);
    if (!new_card) {
        Py_DECREF(new_decks);
        return NULL;
    }

    // Create new board
    PyObject* new_board = Board_InsertCard(board, index, (PyObject*)new_card);
    if (!new_board) {
        Py_DECREF(new_decks);
        return NULL;
    }

    // Create new state
    StateObject* next_state;
    Py_INCREF(current_state->scores);
    if (family + 1 >= num_families) {
        CardObject* card = (CardObject*)(PyTuple_GET_ITEM(new_board, 0));
        int phase = card->tokens >= 0 ? PHASE_REVEAL : PHASE_ACT;
        next_state = State_New(
            PHASE_REVEAL,
            new_board,
            new_decks,
            current_state->scores,
            0
        );
    } else {
        next_state = State_New(
            PHASE_PLACE,
            new_board,
            new_decks,
            current_state->scores,
            family + 1
        );
    }
    return next_state;
}

static StateObject* Action_CreateStateReveal(ActionObject* self) {

    // Get parameters
    StateObject* current_state = self->current_state;
    CardObject* current_card = State_CURRENT_CARD(current_state);
    PyObject* current_scores = current_state->scores;

    // Create new card
    CardObject* new_card = Card_New(current_card->kind, current_card->family, 0);
    if (!new_card) {
        return NULL;
    }

    // Create new board
    PyObject* new_board = Board_SetCard(
        current_state->board,
        current_state->index,
        (PyObject*)new_card
    );
    if (!new_board) {
        return NULL;
    }

    // Compute score
    int increment = current_card->tokens;
    switch (current_card->kind) {
    case KIND_AMBUSH:
        increment = 0; // TODO +1 when act
        break;
    case KIND_CONSPIRACY:
        increment *= 2;
        break;
    }

    // Create new scores
    PyObject* new_scores;
    if (increment == 0) {
        new_scores = current_scores;
        Py_INCREF(current_scores);
    } else {
        new_scores = Score_Add(current_scores, current_card->family, increment);
        if (!new_scores) {
            Py_DECREF(new_board);
            return NULL;
        }
    }

    // Create new state
    Py_INCREF(current_state->decks);
    StateObject* next_state = State_New(
        PHASE_ACT,
        new_board,
        current_state->decks,
        new_scores,
        current_state->index
    );
    return next_state;
}

static StateObject* Action_CreateStateIncrease(ActionObject* self) {

    // Get parameters
    StateObject* current_state = self->current_state;
    CardObject* current_card = State_CURRENT_CARD(current_state);

    // Create new card
    CardObject* new_card = Card_New(
        current_card->kind,
        current_card->family,
        current_card->tokens + 1
    );
    if (!new_card) {
        return NULL;
    }

    // Create new board
    PyObject* new_board = Board_SetCard(
        current_state->board,
        current_state->index,
        (PyObject*)new_card
    );
    if (!new_board) {
        return NULL;
    }

    // Create new state
    return Action_CreateState_PostAction(
        current_state,
        new_board,
        NULL,
        NULL
    );
}

static StateObject* Action_CreateState(ActionObject* self) {
    switch (self->effect) {
    case EFFECT_PLACE:
        return Action_CreateStatePlace(self);
    case EFFECT_REVEAL:
        return Action_CreateStateReveal(self);
    case EFFECT_INCREASE:
        return Action_CreateStateIncrease(self);
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
    {"effect", T_INT, offsetof(ActionObject, effect), READONLY},
    {"first", T_INT, offsetof(ActionObject, first), READONLY},
    {"second", T_INT, offsetof(ActionObject, second), READONLY},
    {"current_state", T_OBJECT, offsetof(ActionObject, current_state), READONLY},
    {NULL},
};

static PyGetSetDef Action_getset[] = {
    {"next_state", (getter)Action_get_next_state, NULL, NULL, NULL},
    // TODO better names for indices, cards...
    {NULL},
};

static PyObject* Action_repr(ActionObject* self) {
    switch (self->effect) {
    case EFFECT_NONE:
        return PyUnicode_FromFormat(
            "Action(NONE)"
        );
    case EFFECT_PLACE:
        return PyUnicode_FromFormat(
            "Action(PLACE, %s, index=%d)",
            Kind_NAMES[self->first],
            self->second
        );
    case EFFECT_REVEAL:
        return PyUnicode_FromFormat(
            "Action(REVEAL, %R@%d)",
            State_CURRENT_CARD(self->current_state),
            self->current_state->index
        );
    case EFFECT_INCREASE:
        return PyUnicode_FromFormat(
            "Action(INCREASE, %R@%d)",
            State_CURRENT_CARD(self->current_state),
            self->current_state->index
        );
    default:
        return PyUnicode_FromFormat(
            "Action(UNKNOWN)"
        );
    }
}

PyTypeObject Action_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = PACKAGE_NAME ".Action",
    .tp_basicsize = sizeof(ActionObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    // TODO maybe expose new/init?
    .tp_dealloc = (destructor)Action_dealloc,
    .tp_members = Action_members,
    .tp_getset = Action_getset,
    .tp_repr = (reprfunc)Action_repr,
};
