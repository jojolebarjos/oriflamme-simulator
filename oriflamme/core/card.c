
#include "card.h"

char const* Kind_NAMES[KIND_MAX] = {
    "NONE",
    "ARCHER",
    "HEIR",
    "LORD",
    "SHAPESHIFTER",
    "SOLDIER",
    "SPY",
    "AMBUSH",
    "ASSASSINATION",
    "CONSPIRACY",
    "ROYAL_DECREE",
};

CardObject* Card_New(int kind, int family, int tokens) {
    CardObject* new_card = PyObject_New(CardObject, &Card_Type);
    if (!new_card) {
        return NULL;
    }
    new_card->kind = kind;
    new_card->family = family;
    new_card->tokens = tokens;
    return new_card;
}

// TODO methods to get revealed/corrupted card?

static int Card_init(CardObject* self, PyObject* args, PyObject* kwargs) {
    static char* kwlist[] = {"kind", "family", "tokens", NULL};
    self->tokens = -1;
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "ii|i:__init__", kwlist, &self->kind, &self->family, &self->tokens)) {
        return -1;
    }
    // TODO check kind
    // TODO how to check family?
    // TODO check tokens
    return 0;
}

static void Card_dealloc(CardObject* self) {
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* Card_get_is_revealed(CardObject* self, void* closure) {
    return PyBool_FromLong(self->tokens >= 0);
}

static PyMemberDef Card_members[] = {
    {"kind", T_INT, offsetof(CardObject, kind), READONLY},
    {"family", T_INT, offsetof(CardObject, family), READONLY},
    {"tokens", T_INT, offsetof(CardObject, tokens), READONLY},
    {NULL},
};

static PyGetSetDef Card_getset[] = {
    {"is_revealed", (getter)Card_get_is_revealed, NULL, NULL, NULL},
    {NULL},
};

static PyObject* Card_repr(CardObject* self) {
    if (self->tokens < 0) {
        return PyUnicode_FromFormat(
            "Card(%s, family=%d, revealed)",
            Kind_NAMES[self->kind],
            self->family
        );
    }
    return PyUnicode_FromFormat(
        "Card(%s, family=%d, tokens=%d)",
        Kind_NAMES[self->kind],
        self->family,
        self->tokens
    );
}

PyTypeObject Card_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = PACKAGE_NAME ".Card",
    .tp_basicsize = sizeof(CardObject),
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc)Card_init,
    .tp_dealloc = (destructor)Card_dealloc,
    .tp_members = Card_members,
    .tp_getset = Card_getset,
    .tp_repr = (reprfunc)Card_repr,
};
