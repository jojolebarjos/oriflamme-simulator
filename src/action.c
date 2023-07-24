#include "action.h"
#include "stack.h"
#include "state.h"

const char * ACT_NAMES[MAX_ACT] = {
    "ACT_NONE",
    "ACT_INSERT",
    "ACT_STACK",
    "ACT_REVEAL",
    "ACT_ACCUMULATE",
    "ACT_KILL",
    "ACT_GAIN",
    "ACT_STEAL",
    "ACT_MOVE",
};

ActionObject * Action_Build(
    StateObject * state,
    unsigned act,
    unsigned kind,
    unsigned i,
    unsigned j
) {
    ActionObject * action = PyObject_New(ActionObject, &Action_Type);
    if (action) {
        action->state = state;
        action->act = act;
        action->kind = kind;
        action->i = i;
        action->j = j;
        Py_INCREF(state);
    }
    return action;
}

static void Action_dealloc(ActionObject * self) {
    Py_XDECREF(self->state);
    Py_XDECREF(self->next_state);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject * Action_repr(ActionObject * self) {
    // TODO better repr
    return PyUnicode_FromFormat("(%s, ...)", ACT_NAMES[self->act]);
}

static StateObject * Action_CreateNextStateForNone(ActionObject * self) {
    // TODO
    Py_RETURN_NONE;
}

static StateObject * Action_CreateNextStateForInsert(ActionObject * self) {
    // TODO
    Py_RETURN_NONE;
}

static StateObject * Action_CreateNextStateForStack(ActionObject * self) {
    // TODO
    Py_RETURN_NONE;
}

static StateObject * Action_CreateNextStateForReveal(ActionObject * self) {
    // TODO
    Py_RETURN_NONE;
}

static StateObject * Action_CreateNextStateForAccumulate(ActionObject * self) {
    // TODO
    Py_RETURN_NONE;
}

static StateObject * Action_CreateNextStateForKill(ActionObject * self) {
    // TODO
    Py_RETURN_NONE;
}

static StateObject * Action_CreateNextStateForGain(ActionObject * self) {
    // TODO
    Py_RETURN_NONE;
}

static StateObject * Action_CreateNextStateForSteal(ActionObject * self) {
    // TODO
    Py_RETURN_NONE;
}

static StateObject * Action_CreateNextStateForMove(ActionObject * self) {
    // TODO
    Py_RETURN_NONE;
}

static StateObject * Action_CreateNextState(ActionObject * self) {
    switch (self->act) {
    case ACT_INSERT:
        return Action_CreateNextStateForInsert(self);
    case ACT_STACK:
        return Action_CreateNextStateForStack(self);
    case ACT_REVEAL:
        return Action_CreateNextStateForReveal(self);
    case ACT_ACCUMULATE:
        return Action_CreateNextStateForAccumulate(self);
    case ACT_KILL:
        return Action_CreateNextStateForKill(self);
    case ACT_GAIN:
        return Action_CreateNextStateForGain(self);
    case ACT_STEAL:
        return Action_CreateNextStateForSteal(self);
    case ACT_MOVE:
        return Action_CreateNextStateForMove(self);
    default:
        return Action_CreateNextStateForNone(self);
    }
}

static PyObject * Action_next_state(ActionObject * self, void * closure) {
    if (!self->next_state)
        self->next_state = Action_CreateNextState(self);
    Py_XINCREF(self->next_state);
    return (PyObject *)self->next_state;
}

static PyMemberDef Action_members[] = {
    {"state", T_OBJECT_EX, offsetof(ActionObject, state), READONLY, NULL},
    // TODO ...
    {NULL}
};

static PyGetSetDef Action_getsets[] = {
    {"next_state", (getter)Action_next_state, NULL, NULL, NULL},
    {NULL}
};

PyTypeObject Action_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "oriflamme.Action",
    .tp_basicsize = sizeof(ActionObject),
    .tp_dealloc = (destructor)Action_dealloc,
    .tp_repr = (reprfunc)Action_repr,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_members = Action_members,
    .tp_getset = Action_getsets,
    .tp_new = PyType_GenericNew,
};
