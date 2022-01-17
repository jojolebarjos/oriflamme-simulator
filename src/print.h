
#ifndef ORIFLAMME_PRINT_H
#define ORIFLAMME_PRINT_H


#include <cstdint>
#include <cstdlib>

#include "action.h"
#include "state.h"


constexpr char const * KIND_NAMES[] = {
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


inline void pretty_print(Action const & action) {
    switch (action.act) {
    case ACT_NONE:
        printf("no action\n");
        break;
    case ACT_PLACE:
        printf("place %s before %u\n", KIND_NAMES[action.x], action.y);
        break;
    case ACT_REVEAL:
        printf("reveal\n");
        break;
    case ACT_ACCUMULATE:
        printf("accumulate\n");
        break;
    case ACT_KILL:
        printf("kill at %u\n", KIND_NAMES[action.x]);
        break;
    case ACT_HEIR:
        printf("heir");
        break;
    case ACT_LORD:
        printf("lord");
        break;
    case ACT_STEAL:
        printf("steal from %u\n", action.x);
        break;
    }
}


inline void pretty_print(State const & state) {

    // Show decks
    for (unsigned p = 0; p < 4; ++p) {
        printf("player #%u (%u tokens):", p, state.players[p].tokens);
        for (unsigned i = 0; i < MAX_KIND; ++i)
            if (state.players[p].deck[i])
                printf(" %s", KIND_NAMES[i]);
        printf("\n");
    }

    // Show board
    for (unsigned i = 0; i < state.queue.size; ++i) {
        Stack const & stack = state.queue.stacks[i];
        printf("%u. #%u", i, stack.head().player);
        for (unsigned j = 0; j < stack.size; ++j) {
            Card const & card = stack.cards[j];
            if (card.is_revealed())
                printf(" %s", KIND_NAMES[card.kind]);
            else
                printf(" (%s=%u)", KIND_NAMES[card.kind], card.tokens);
        }
        printf("\n");
    }
}


#endif
