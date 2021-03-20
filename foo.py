
import random

from oriflamme import *

PHASE_PLACE = 0

def create_deck():
    choices = list(range(1, 11))
    random.shuffle(choices)
    deck = choices[:7]
    deck.sort()
    deck = tuple(deck)
    return deck

def initial_state(num_families=4, starting_deck=None):
    if starting_deck is None:
        starting_deck = create_deck()
    decks = (starting_deck,) * num_families
    state = State(PHASE_PLACE, (), decks, -1, 0)
    return state

state = initial_state()
