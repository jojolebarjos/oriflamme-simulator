#include "kind.h"
#include "stack.h"

StackObject * Stack_Build(
    unsigned kind,
    unsigned player,
    int tokens,
    StackObject * tail
) {
    StackObject * stack = PyObject_New(StackObject, &Stack_Type);
    if (stack) {
        stack->kind = kind;
        stack->player = player;
        stack->tokens = tokens;
        stack->tail = (PyObject *)tail;
        Py_XINCREF(tail);
    }
    return stack;
}

static int Stack_init(StackObject * self, PyObject * args, PyObject * kwargs) {
    static char * kwlist[] = {"kind", "player", "tokens", "tail", NULL};
    self->tokens = -1;
    self->tail = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "II|iO!:__init__", kwlist, &self->kind, &self->player, &self->tokens, &Stack_Type, &self->tail))
        return -1;
    Py_XINCREF(self->tail);
    // TODO check kind and player
    return 0;
}

static void Stack_dealloc(StackObject * self) {
    Py_XDECREF(self->tail);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject * Stack_repr(StackObject * self) {
    return PyUnicode_FromFormat("(%s, %u, %i, %R)", KIND_NAMES[self->kind], self->player, self->tokens, self->tail);
}

static PyObject * Stack_is_revealed(StackObject * self, void * closure) {
    return PyBool_FromLong(self->tokens < 0);
}

static PyMemberDef Stack_members[] = {
    {"kind", T_UINT, offsetof(StackObject, kind), READONLY, NULL},
    {"player", T_UINT, offsetof(StackObject, player), READONLY, NULL},
    {"tokens", T_INT, offsetof(StackObject, tokens), READONLY, NULL},
    {"tail", T_OBJECT, offsetof(StackObject, tail), READONLY, NULL},
    {NULL}
};

static PyGetSetDef Stack_getsets[] = {
    {"is_revealed", (getter)Stack_is_revealed, NULL, NULL, NULL},
    {NULL}
};

PyTypeObject Stack_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "oriflamme.Stack",
    .tp_basicsize = sizeof(StackObject),
    .tp_dealloc = (destructor)Stack_dealloc,
    .tp_repr = (reprfunc)Stack_repr,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_members = Stack_members,
    .tp_getset = Stack_getsets,
    .tp_init = Stack_init,
    .tp_new = PyType_GenericNew,
};
