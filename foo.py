
import random

from oriflamme import *

state = initial_state()

for _ in range(len(state.decks)):
    action = random.choice(state.actions)
    state = action.next_state

a = state.actions[0]
