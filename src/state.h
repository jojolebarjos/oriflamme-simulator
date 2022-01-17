
#ifndef ORIFLAMME_STATE_H
#define ORIFLAMME_STATE_H


#include <cstdint>


enum Kind : uint8_t{
    KIND_NONE = 0,
    KIND_ARCHER,
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
};


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


struct Card {
    uint8_t kind;
    uint8_t player;
    int8_t tokens;

    inline bool is_revealed() const {
        return tokens < 0;
    }
};


constexpr unsigned num_players = 4;
constexpr unsigned num_starting_cards = 7;
constexpr unsigned max_stack_size = num_starting_cards;
constexpr unsigned max_queue_size = num_players * num_starting_cards;


struct Stack {
    Card cards[max_stack_size];
    uint8_t size;

    inline Card & head() {
        return cards[size - 1];
    }

    inline Card const & head() const {
        return cards[size - 1];
    }

    inline void push(Card card) {
        if (size >= max_stack_size)
            throw 42;
        cards[size++] = card;
    }

    inline void pop() {
        if (size == 0)
            throw 42;
        --size;
    }

    inline void set(Card card) {
        cards[0] = card;
        size = 1;
    }
};


struct Queue {
    Stack stacks[max_queue_size];
    uint8_t size;


    inline void place_before(Card card, unsigned i) {
        if (size >= max_queue_size || i >= size)
            throw 42;
        for (unsigned j = size; j > i; --j)
            stacks[j] = stacks[j - 1];
        stacks[i].set(card);
    }

    inline void place_front(Card card) {
        place_before(card, 0);
    }

    inline void place_back(Card card) {
        place_before(card, size);
    }

    inline void place_on(Card card, unsigned i) {
        if (i >= size || stacks[i].head().player != card.player)
            throw 42;
        stacks[i].push(card);
    }

    inline void remove_stack(unsigned i) {
        for (unsigned j = i + 1; j < size; ++j)
            stacks[j - 1] = stacks[j];
        --size;
    }
};


struct Player {
    char deck[MAX_KIND];
    uint8_t tokens;
};


enum Phase {
    PHASE_NONE,
    PHASE_PLACE,
    PHASE_EVALUATE,
    PHASE_END,
    MAX_PHASE,
};


struct Action;


struct State {
    Queue queue;
    Player players[num_players];
    uint8_t phase;
    uint8_t index;
    uint8_t turn;

    unsigned enumerate(Action * actions) const;

private:

    unsigned enumerate_place(Action * actions) const;
    unsigned enumerate_evaluate(Action * actions) const;
    unsigned enumerate_evaluate_revealed(Action * actions, unsigned kind) const;
    unsigned enumerate_evaluate_archer(Action * actions) const;
    unsigned enumerate_evaluate_heir(Action * actions) const;
    unsigned enumerate_evaluate_lord(Action * actions) const;
    unsigned enumerate_evaluate_shapeshifter(Action * actions) const;
    unsigned enumerate_evaluate_soldier(Action * actions) const;
    unsigned enumerate_evaluate_spy(Action * actions) const;
    unsigned enumerate_evaluate_assassination(Action * actions) const;
    unsigned enumerate_evaluate_royal_decree(Action * actions) const;
};


#endif
