
#ifndef ORIFLAMME_ACTION_H
#define ORIFLAMME_ACTION_H


#include <cstdint>

#include "state.h"


enum Act : uint8_t {
    ACT_NONE,
    ACT_PLACE,
    ACT_REVEAL,
    ACT_ACCUMULATE,
    ACT_KILL,
    ACT_HEIR,
    ACT_LORD,
    ACT_STEAL,
    // TODO royal decree
};


struct Action {
    uint8_t act;
    uint8_t x;
    uint8_t y;
    uint8_t z;

    void apply(State & state) const {
        switch (act) {
        case ACT_PLACE:
            apply_place(state);
            break;
        case ACT_REVEAL:
            apply_reveal(state);
            break;
        case ACT_ACCUMULATE:
            apply_accumulate(state);
            break;
        case ACT_KILL:
            apply_kill(state);
            break;
        case ACT_HEIR:
            apply_heir(state);
            break;
        case ACT_LORD:
            apply_lord(state);
            break;
        case ACT_STEAL:
            apply_steal(state);
            break;
        // TODO royal decree
        }
    }

private:

    void apply_place(State & state) const {
        // TODO probably need two kind of placement?
    }

    void apply_reveal(State & state) const {
        Card & card = state.queue.stacks[state.index].head();

        // If this is a conspiracy, it brings more tokens
        if (card.kind == KIND_CONSPIRACY) {
            state.players[card.player].tokens += card.tokens * 2;
            post_evaluation(state, true);
        }

        // If this is an ambush, then it only brings one token
        else if (card.kind == KIND_AMBUSH) {
            state.players[card.player].tokens += 1;
            post_evaluation(state, true);
        }

        // Otherwise, reveal the card and collect the tokens
        // Note: no index is incremented, as the card will be evaluated
        else {
            state.players[card.player].tokens += card.tokens;
            card.tokens = -1;
        }
    }

    void apply_accumulate(State & state) const {

        // Add one token to the unrevealed card
        Card & card = state.queue.stacks[state.index].head();
        card.tokens += 1;

        // Done
        post_evaluation(state);
    }

    void apply_kill(State & state) const {
        Card & source = state.queue.stacks[state.index].head();
        Card & target = state.queue.stacks[x].head();

        // If target was an ambush, then it's a trap
        if (target.kind == KIND_AMBUSH) {
            
        }
        // TODO
        post_evaluation(state);
    }

    void apply_heir(State & state) const {

        // Search for another card with the same name
        // Note: we need to check our name, as we could be a shapeshifter as well
        Card & card = state.queue.stacks[state.index].head();
        bool found = false;
        for (unsigned i = 0; i < state.queue.size; ++i)
            if (i != state.index && state.queue.stacks[i].head().kind == card.kind) {
                found = true;
                break;
            }

        // If we are the only one, we get 2 tokens
        if (!found)
            state.players[card.player].tokens += 2;

        // Done
        post_evaluation(state);
    }

    void apply_lord(State & state) const {

        // Get one token
        Card & card = state.queue.stacks[state.index].head();
        unsigned count = 1;

        // Get another one, if left is of our family
        if (state.index > 0 && state.queue.stacks[state.index - 1].head().player == card.player)
            ++count;

        // Same, on the right
        if (state.index + 1 < state.queue.size && state.queue.stacks[state.index + 1].head().player == card.player)
            ++count;

        // Done
        state.players[card.player].tokens += count;
        post_evaluation(state);
    }

    void apply_steal(State & state) const {

        // Steal one token
        Card & card = state.queue.stacks[state.index].head();
        state.players[card.player].tokens += 1;
        state.players[x].tokens -= 1;

        // Done
        post_evaluation(state);
    }

    // TODO royal decree

    void post_evaluation(State & state, bool discard = false) const {

        // If card is discarded, pop from stack
        if (discard) {
            Stack & stack = state.queue.stacks[state.index];
            stack.pop();

            // If there is still a card there, we need to evaluate it
            if (stack.size > 0)
                return;

            // Otherwise, we move to the next card
            state.queue.remove_stack(state.index);
        }

        // Otherwise, move to next card
        else
            ++state.index;

        // If the whole queue was evaluated...
        if (state.index == state.queue.size) {
            state.phase = PHASE_PLACE;
            state.index = 0;
            ++state.turn;

            // Check for game completion
            if (state.turn >= 6)
                state.phase = PHASE_END;
        }
    }

public:

    static unsigned actions_for(State const & state, Action * actions) {
        switch (state.phase) {
        case PHASE_PLACE:
            return actions_for_place(state, actions);
        case PHASE_EVALUATE:
            return actions_for_evaluate(state, actions);
        default:
            return 0;
        }
    }

private:

    static unsigned actions_for_place(State const & state, Action * actions) {
        unsigned count = 0;

        // For each card in current player's deck
        for (uint8_t kind = 1; kind < MAX_KIND; ++kind)
            if (state.players[state.index].deck[kind]) {

                // Can be placed at front
                actions[count++] = {ACT_PLACE, kind, 0}; // TODO proper indices

                // Can be placed at back
                if (state.queue.size > 0)
                    actions[count++] = {ACT_PLACE, kind, state.queue.size};

                // Can be placed on top of any card/stack of current player's color
                for (uint8_t i = 0; i < state.queue.size; ++i)
                    if (state.queue.stacks[i].head().player == state.index)
                        actions[count++] = {ACT_PLACE, kind, i};
            }

        return count;
    }

    static unsigned actions_for_evaluate(State const & state, Action * actions) {

        // If card is revealed, then it must be evaluated
        Card const & card = state.queue.stacks[state.index].head();
        if (card.is_revealed())
            return actions_for_evaluate_revealed(state, actions, card.kind);

        // Otherwise, player must decides whether to reveal it or not
        actions[0] = {ACT_REVEAL};
        actions[1] = {ACT_ACCUMULATE};
        return 2;
    }

    static unsigned actions_for_evaluate_revealed(State const & state, Action * actions, unsigned kind) {
        switch (kind) {
        case KIND_ARCHER:
            return actions_for_evaluate_archer(state, actions);
        case KIND_HEIR:
            return actions_for_evaluate_heir(state, actions);
        case KIND_LORD:
            return actions_for_evaluate_lord(state, actions);
        case KIND_SHAPESHIFTER:
            return actions_for_evaluate_shapeshifter(state, actions);
        case KIND_SOLDIER:
            return actions_for_evaluate_soldier(state, actions);
        case KIND_SPY:
            return actions_for_evaluate_spy(state, actions);
        case KIND_ASSASSINATION:
            return actions_for_evaluate_assassination(state, actions);
        case KIND_ROYAL_DECREE:
            return actions_for_evaluate_royal_decree(state, actions);
        default:
            return 0;
        }
    }

    static unsigned actions_for_evaluate_archer(State const & state, Action * actions) {
        unsigned count = 0;

        // Can attack first card
        actions[count++] = {ACT_KILL, 0};

        // Can attack last card
        if (state.queue.size > 1)
            actions[count++] = {ACT_KILL, (uint8_t)(state.queue.size - 1)};

        return count;
    }

    static unsigned actions_for_evaluate_heir(State const & state, Action * actions) {
        actions[0] = {ACT_HEIR};
        return 1;
    }

    static unsigned actions_for_evaluate_lord(State const & state, Action * actions) {
        actions[0] = {ACT_LORD};
        return 1;
    }

    static unsigned actions_for_evaluate_shapeshifter(State const & state, Action * actions) {
        unsigned count = 0;

        // Copy left neighbor abilities
        unsigned first_kind = KIND_NONE;
        if (state.index > 0) {
            Card const & card = state.queue.stacks[state.index - 1].head();
            if (card.is_revealed() && card.kind != KIND_SHAPESHIFTER) {
                first_kind = card.kind;
                count += actions_for_evaluate_revealed(state, actions, card.kind);
            }
        }

        // Copy right neighbor abilities
        if (state.index < state.queue.size - 1) {
            Card const & card = state.queue.stacks[state.index + 1].head();
            if (card.is_revealed() && card.kind != KIND_SHAPESHIFTER && card.kind != first_kind)
                count += actions_for_evaluate_revealed(state, actions, card.kind);
        }

        return count;
    }

    static unsigned actions_for_evaluate_soldier(State const & state, Action * actions) {
        unsigned count = 0;

        // Can kill left neighbor
        if (state.index > 0)
            actions[count++] = {ACT_KILL, (uint8_t)(state.index - 1)};

        // Can kill right neighbor
        if (state.index < state.queue.size - 1)
            actions[count++] = {ACT_KILL, (uint8_t)(state.index + 1)};

        return count;
    }

    static unsigned actions_for_evaluate_spy(State const & state, Action * actions) {
        unsigned count = 0;
        Card const & card = state.queue.stacks[state.index].head();

        // Can steal from player on the left
        if (state.index > 0) {
            Card const & neighbor = state.queue.stacks[state.index - 1].head();
            if (neighbor.player != card.player && state.players[neighbor.player].tokens > 0)
                actions[count++] = {ACT_STEAL, neighbor.player};
        }

        // Can steal from player on the right
        if (state.index < state.queue.size - 1) {
            Card const & neighbor = state.queue.stacks[state.index + 1].head();
            if (neighbor.player != card.player && state.players[neighbor.player].tokens > 0 && (count == 0 || neighbor.player != actions[0].x))
                actions[count++] = {ACT_STEAL, neighbor.player};
        }

        return count;
    }

    static unsigned actions_for_evaluate_assassination(State const & state, Action * actions) {
        unsigned count = 0;

        // Can kill any of the top card
        for (uint8_t i = 0; i < state.queue.size; ++i)
            if (i != state.index)
                actions[count++] = {ACT_KILL, i};

        return count;
    }

    static unsigned actions_for_evaluate_royal_decree(State const & state, Action * actions) {
        unsigned count = 0;

        // Can move any top card
        for (unsigned i = 0; i < state.queue.size; ++i)
            if (i != state.index) {

                // Can be inserted at any location
                // ...

                // Can be placed on any card of the same family
                // ...
            }

        return count;
    }
};


#endif
