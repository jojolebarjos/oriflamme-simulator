#include "kind.h"
#include "stack.h"
#include "state.h"

const char * PHASE_NAMES[MAX_PHASE] = {
    "PHASE_NONE",
    "PHASE_PLACE",
    "PHASE_EVALUATE",
    "PHASE_END",
};

StateObject * State_Build(
    PyObject * queue,
    PyObject * hands,
    PyObject * tokens,
    unsigned phase,
    unsigned index
) {
    StateObject * state = PyObject_New(StateObject, &State_Type);
    if (state) {
        state->queue = queue;
        state->hands = hands;
        state->tokens = tokens;
        state->phase = phase;
        state->index = index;
        Py_INCREF(queue);
        Py_INCREF(hands);
        Py_INCREF(tokens);
    }
    return state;
}

static int State_init(StateObject * self, PyObject * args, PyObject * kwargs) {
    //static char * kwlist[] = {"kind", "player", "tokens", NULL};
    //if (!PyArg_ParseTupleAndKeywords(args, kwargs, "IIi:__init__", kwlist, &self->kind, &self->player, &self->tokens))
    //    return -1;
    return 0;
}

static void State_dealloc(StateObject * self) {
    Py_XDECREF(self->queue);
    Py_XDECREF(self->hands);
    Py_XDECREF(self->tokens);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject * State_repr(StateObject * self) {
    // TODO better repr
    return PyUnicode_FromFormat("(%s, ...)", PHASE_NAMES[self->phase]);
}

static PyObject * State_CreatePlaceActions(StateObject * self) {
    
    // For each card in current player's deck
    // Can be placed at front
    // Can be placed at back
    // Can be placed on top of any card/stack of current player's color
    
    Py_RETURN_NONE;
}

static PyObject * State_CreateActionsForArcher(StateObject * self) {
    // TODO
    Py_RETURN_NONE;
}

static PyObject * State_CreateActionsForHeir(StateObject * self) {
    // TODO
    Py_RETURN_NONE;
}

static PyObject * State_CreateActionsForLord(StateObject * self) {
    // TODO
    Py_RETURN_NONE;
}

static PyObject * State_CreateActionsForShapeshifter(StateObject * self) {
    // TODO
    Py_RETURN_NONE;
}

static PyObject * State_CreateActionsForSoldier(StateObject * self) {
    // TODO
    Py_RETURN_NONE;
}

static PyObject * State_CreateActionsForSpy(StateObject * self) {
    // TODO
    Py_RETURN_NONE;
}

static PyObject * State_CreateActionsForAmbush(StateObject * self) {
    // TODO
    Py_RETURN_NONE;
}

static PyObject * State_CreateActionsForAssassination(StateObject * self) {
    // TODO
    Py_RETURN_NONE;
}

static PyObject * State_CreateActionsForConspiracy(StateObject * self) {
    // TODO
    Py_RETURN_NONE;
}

static PyObject * State_CreateActionsForRoyalDecree(StateObject * self) {
    // TODO
    Py_RETURN_NONE;
}

static PyObject * State_CreateActionsForKind(StateObject * self, unsigned kind) {
    switch (kind) {
    case KIND_ARCHER:
        return State_CreateActionsForArcher(self);
    case KIND_HEIR:
        return State_CreateActionsForHeir(self);
    case KIND_LORD:
        return State_CreateActionsForLord(self);
    case KIND_SHAPESHIFTER:
        return State_CreateActionsForShapeshifter(self);
    case KIND_SOLDIER:
        return State_CreateActionsForSoldier(self);
    case KIND_SPY:
        return State_CreateActionsForSpy(self);
    case KIND_AMBUSH:
        return State_CreateActionsForAmbush(self);
    case KIND_ASSASSINATION:
        return State_CreateActionsForAssassination(self);
    case KIND_CONSPIRACY:
        return State_CreateActionsForConspiracy(self);
    case KIND_ROYAL_DECREE:
        return State_CreateActionsForRoyalDecree(self);
    default:
        // TODO empty action
        Py_RETURN_NONE;
    }
}

static PyObject * State_CreateEvaluateActions(StateObject * self) {

    // Get card on top of the current stack
    StackObject * stack = (StackObject *)PyTuple_GET_ITEM(self->queue, self->index);

    // If it is not yet revealed, the player either reveal it or place a token
    if (stack->tokens < 0) {
        // TODO accumulate
        // TODO reveal
        Py_RETURN_NONE;
    }

    // Otherwise, the player has to evaluate the card
    return State_CreateActionsForKind(stack->kind);
}

static PyObject * State_CreateActions(StateObject * self) {
    switch (self->phase) {
    case PHASE_PLACE:
        return State_CreatePlaceActions(self);
    case PHASE_EVALUATE:
        return State_CreateEvaluateActions(self);
    default:
        // TODO empty action
        Py_RETURN_NONE;
    }
}

static PyObject * State_actions(StateObject * self, void * closure) {
    // TODO should maybe keep actions as a weakref, so that we do not necessarily rebuild them?
    // https://docs.python.org/3/c-api/weakref.html
    return State_CreateActions(self);
}

static PyObject * State_create(PyObject * cls, PyObject * args, PyObject * kwargs) {
    // TODO PyArg_ParseTupleAndKeywords
    Py_RETURN_NONE;
}

static PyMemberDef State_members[] = {
    {"queue", T_OBJECT_EX, offsetof(StateObject, queue), READONLY, NULL},
    {"hands", T_OBJECT_EX, offsetof(StateObject, hands), READONLY, NULL},
    {"tokens", T_OBJECT_EX, offsetof(StateObject, tokens), READONLY, NULL},
    // TODO T_UINT, T_INT, for phase and ...
    {NULL}
};

static PyGetSetDef State_getsets[] = {
    {"actions", (getter)State_actions, NULL, NULL, NULL},
    {NULL}
};

static PyMethodDef State_methods[] = {
    {"create", (PyCFunction)State_create, METH_VARARGS | METH_KEYWORDS | METH_CLASS, NULL},
    {NULL}
};

PyTypeObject State_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "oriflamme.State",
    .tp_basicsize = sizeof(StateObject),
    .tp_dealloc = (destructor)State_dealloc,
    .tp_repr = (reprfunc)State_repr,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_members = State_members,
    .tp_methods = State_methods,
    .tp_getset = State_getsets,
    .tp_init = State_init,
    .tp_new = PyType_GenericNew,
};
