#ifndef ORIFLAMME_KIND_H
#define ORIFLAMME_KIND_H

#include "common.h"

typedef enum {
    KIND_ARCHER = 0,
    KIND_HEIR,
    KIND_LORD,
    KIND_SHAPESHIFTER,
    KIND_SOLDIER,
    KIND_SPY,
    KIND_AMBUSH,
    KIND_ASSASSINATION,
    KIND_CONSPIRACY,
    KIND_ROYAL_DECREE,
    MAX_KIND,
} Kind;

extern const char * KIND_NAMES[MAX_KIND];

#endif
