
import random

from .core import *
from .constant import *


def create_deck():
    choices = [
        KIND_NONE,
        KIND_ARCHER,
        KIND_HEIR,
        KIND_LORD,
        KIND_SHAPESHIFTER,
        KIND_SOLDIER,
        KIND_SPY,
        KIND_AMBUSH,
        KIND_ASSASSINATION,
        KIND_CONSPIRACY,
        #KIND_ROYAL_DECREE,
    ]
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
