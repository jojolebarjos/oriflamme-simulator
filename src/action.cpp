
#include "action.h"
#include "state.h"


void Action::apply(State & state) const {
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


void Action::apply_place(State & state) const {
    // TODO probably need two kind of placement?
}


void Action::apply_reveal(State & state) const {
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


void Action::apply_accumulate(State & state) const {

    // Add one token to the unrevealed card
    Card & card = state.queue.stacks[state.index].head();
    card.tokens += 1;

    // Done
    post_evaluation(state);
}


void Action::apply_kill(State & state) const {
    Card & source = state.queue.stacks[state.index].head();
    Card & target = state.queue.stacks[x].head();

    // If target was an ambush, then it's a trap
    if (target.kind == KIND_AMBUSH) {
        
    }
    // TODO
    post_evaluation(state);
}


void Action::apply_heir(State & state) const {

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


void Action::apply_lord(State & state) const {

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


void Action::apply_steal(State & state) const {

    // Steal one token
    Card & card = state.queue.stacks[state.index].head();
    state.players[card.player].tokens += 1;
    state.players[x].tokens -= 1;

    // Done
    post_evaluation(state);
}


// TODO royal decree


void Action::post_evaluation(State & state, bool discard) const {

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
