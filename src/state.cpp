
#include "state.h"
#include "action.h"


unsigned State::enumerate(Action * actions) const {
    switch (phase) {
    case PHASE_PLACE:
        return enumerate_place(actions);
    case PHASE_EVALUATE:
        return enumerate_evaluate(actions);
    default:
        return 0;
    }
}


unsigned State::enumerate_place(Action * actions) const {
    unsigned count = 0;

    // For each card in current player's deck
    for (uint8_t kind = 1; kind < MAX_KIND; ++kind)
        if (players[index].deck[kind]) {

            // Can be placed at front
            actions[count++] = {ACT_PLACE, kind, 0}; // TODO proper indices

            // Can be placed at back
            if (queue.size > 0)
                actions[count++] = {ACT_PLACE, kind, queue.size};

            // Can be placed on top of any card/stack of current player's color
            for (uint8_t i = 0; i < queue.size; ++i)
                if (queue.stacks[i].head().player == index)
                    actions[count++] = {ACT_PLACE, kind, i};
        }

    return count;
}


unsigned State::enumerate_evaluate(Action * actions) const {

    // If card is revealed, then it must be evaluated
    Card const & card = queue.stacks[index].head();
    if (card.is_revealed())
        return enumerate_evaluate_revealed(actions, card.kind);

    // Otherwise, player must decides whether to reveal it or not
    actions[0] = {ACT_REVEAL};
    actions[1] = {ACT_ACCUMULATE};
    return 2;
}


unsigned State::enumerate_evaluate_revealed(Action * actions, unsigned kind) const {
    switch (kind) {
    case KIND_ARCHER:
        return enumerate_evaluate_archer(actions);
    case KIND_HEIR:
        return enumerate_evaluate_heir(actions);
    case KIND_LORD:
        return enumerate_evaluate_lord(actions);
    case KIND_SHAPESHIFTER:
        return enumerate_evaluate_shapeshifter(actions);
    case KIND_SOLDIER:
        return enumerate_evaluate_soldier(actions);
    case KIND_SPY:
        return enumerate_evaluate_spy(actions);
    case KIND_ASSASSINATION:
        return enumerate_evaluate_assassination(actions);
    case KIND_ROYAL_DECREE:
        return enumerate_evaluate_royal_decree(actions);
    default:
        // Note: consipracy and ambush behavior is implemented during reveal
        return 0;
    }
}


unsigned State::enumerate_evaluate_archer(Action * actions) const {
    unsigned count = 0;

    // Can attack first card
    actions[count++] = {ACT_KILL, 0};

    // Can attack last card
    if (queue.size > 1)
        actions[count++] = {ACT_KILL, (uint8_t)(queue.size - 1)};

    return count;
}


unsigned State::enumerate_evaluate_heir(Action * actions) const {
    actions[0] = {ACT_HEIR};
    return 1;
}


unsigned State::enumerate_evaluate_lord(Action * actions) const {
    actions[0] = {ACT_LORD};
    return 1;
}


unsigned State::enumerate_evaluate_shapeshifter(Action * actions) const {
    unsigned count = 0;

    // Copy left neighbor abilities
    unsigned first_kind = KIND_NONE;
    if (index > 0) {
        Card const & card = queue.stacks[index - 1].head();
        if (card.is_revealed() && card.kind != KIND_SHAPESHIFTER) {
            first_kind = card.kind;
            count += enumerate_evaluate_revealed(actions, card.kind);
        }
    }

    // Copy right neighbor abilities
    if (index < queue.size - 1) {
        Card const & card = queue.stacks[index + 1].head();
        if (card.is_revealed() && card.kind != KIND_SHAPESHIFTER && card.kind != first_kind)
            count += enumerate_evaluate_revealed(actions, card.kind);
    }

    return count;
}


unsigned State::enumerate_evaluate_soldier(Action * actions) const {
    unsigned count = 0;

    // Can kill left neighbor
    if (index > 0)
        actions[count++] = {ACT_KILL, (uint8_t)(index - 1)};

    // Can kill right neighbor
    if (index < queue.size - 1)
        actions[count++] = {ACT_KILL, (uint8_t)(index + 1)};

    return count;
}


unsigned State::enumerate_evaluate_spy(Action * actions) const {
    unsigned count = 0;
    Card const & card = queue.stacks[index].head();

    // Can steal from player on the left
    if (index > 0) {
        Card const & neighbor = queue.stacks[index - 1].head();
        if (neighbor.player != card.player && players[neighbor.player].tokens > 0)
            actions[count++] = {ACT_STEAL, neighbor.player};
    }

    // Can steal from player on the right
    if (index < queue.size - 1) {
        Card const & neighbor = queue.stacks[index + 1].head();
        if (neighbor.player != card.player && players[neighbor.player].tokens > 0 && (count == 0 || neighbor.player != actions[0].x))
            actions[count++] = {ACT_STEAL, neighbor.player};
    }

    return count;
}


unsigned State::enumerate_evaluate_assassination(Action * actions) const {
    unsigned count = 0;

    // Can kill any of the top card
    for (uint8_t i = 0; i < queue.size; ++i)
        if (i != index)
            actions[count++] = {ACT_KILL, i};

    return count;
}


unsigned State::enumerate_evaluate_royal_decree(Action * actions) const {
    unsigned count = 0;

    // Can move any top card
    for (unsigned i = 0; i < queue.size; ++i)
        if (i != index) {

            // Can be inserted at any location
            // ...

            // Can be placed on any card of the same family
            // ...
        }

    return count;
}
