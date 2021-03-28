
import random

from oriflamme import *

#state = initial_state()
#
#for _ in range(len(state.decks)):
#    action = random.choice(state.actions)
#    state = action.next_state
#
#a = state.actions[0]


state = initial_state(starting_deck=(KIND_ARCHER,))

for _ in range(4):
    action = state.actions[-1]
    state = action.next_state

state = state.actions[0].next_state
