
#include <algorithm>
#include <cstdlib>
#include <random>
#include <vector>

#include "state.h"
#include "action.h"
#include "game.h"
#include "print.h"


std::random_device rd;
std::mt19937 g(rd());


void init_player(Player & player) {
    std::vector<unsigned> choices;
    for (unsigned i = 1; i < MAX_KIND; ++i)
        choices.push_back(i);
    std::shuffle(choices.begin(), choices.end(), g);
    for (unsigned i = 0; i < 7; ++i)
        player.deck[choices[i]] = 1;
    player.tokens = 1;
}


int main(int argc, char* argv[]) {
    
    State state = {};
    for (unsigned i = 0; i < 4; ++i) {
        init_player(state.players[i]);
    }
    state.phase = PHASE_PLACE;
    
    pretty_print(state);
    
    Action actions[64];
    unsigned count = Action::actions_for(state, actions);
    printf("actions:\n");
    for (unsigned i = 0; i < count; ++i) {
        printf("%u. ", i);
        pretty_print(actions[i]);
    }
    
    return 0;
}
