
import random

from .core import *
from .constant import *


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
    board = ()
    decks = (starting_deck,) * num_families
    scores = (0,) * num_families
    state = State(PHASE_PLACE, board, decks, scores, 0)
    return state
