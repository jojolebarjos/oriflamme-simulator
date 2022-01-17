
#ifndef ORIFLAMME_ACTION_H
#define ORIFLAMME_ACTION_H


#include <cstdint>


struct State;


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

    void apply(State & state) const;

private:

    void apply_place(State & state) const;
    void apply_reveal(State & state) const;
    void apply_accumulate(State & state) const;
    void apply_kill(State & state) const;
    void apply_heir(State & state) const;
    void apply_lord(State & state) const;
    void apply_steal(State & state) const;
    // TODO royal decree

    void post_evaluation(State & state, bool discard = false) const;
};


#endif
