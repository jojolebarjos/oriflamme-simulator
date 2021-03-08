
from dataclasses import dataclass

from typing import Tuple

import random


class lazy_property:
    def __init__(self, fget):
        self.fget = fget

    def __get__(self, instance, owner):
        value = self.fget(instance)
        setattr(instance, self.fget.__name__, value)
        return value


(
    KIND_NONE,
    KIND_ARCHER,
    KIND_SOLDIER,
    KIND_SPY,
    KIND_HEIR,
    KIND_ASSASSINATION,
    KIND_ROYAL_DECREE,
    KIND_LORD,
    KIND_SHAPESHIFTER,
    KIND_AMBUSH,
    KIND_CONSPIRACY,
) = range(11)


(
    PHASE_NONE,
    PHASE_PLACEMENT,
    PHASE_REVEAL,
    PHASE_ACT,
) = range(4)


def tuple_insert(t, i, o):
    if i < 0 or i > len(t):
        raise IndexError
    t = (*t[:i], o, *t[i:])
    return t


def tuple_set(t, i, o):
    if i < 0 or i >= len(t):
        raise IndexError
    t = (*t[:i], o, *t[i+1:])
    return t


@dataclass
class Card:
    kind: int
    family: int
    tokens: int

    @property
    def is_revealed(self):
        return self.tokens < 0


@dataclass
class Deck:
    kinds: Tuple[int] = ()

    def add(self, kind):
        kinds = (*self.kinds, kind)
        return Deck(kinds)

    def remove(self, kind):
        index = self.kinds.index(kind)
        kinds = (*self.kinds[:index], *self.kinds[index+1:])
        return Deck(kinds)


@dataclass
class Board:
    queue: Tuple[Card]
    decks: Tuple[Deck]


def initial_state(num_families):
    assert num_families > 1
    decks = []
    cards = list(range(1, 11))
    for f in range(num_families):
        random.shuffle(cards)
        deck = Deck(tuple(cards[:7]))
        decks.append(deck)
    board = Board((), tuple(decks))
    state = PlaceState(board, 0)
    return state


@dataclass
class PlaceState:
    board: Board
    current_family: int

    @lazy_property
    def actions(self):
        assert self.current_family < len(self.board.decks)
        result = []
        n = len(self.board.queue)
        for k in self.board.decks[self.current_family].kinds:
            a = PlaceAction(self, k, 0)
            result.append(a)
            if n > 0:
                a = PlaceAction(self, k, n)
                result.append(a)
        return tuple(result)


@dataclass
class PlayState:
    board: Board
    current_index: int

    @lazy_property
    def actions(self):
        ...


@dataclass
class PlaceAction:
    state: PlaceState
    kind: int
    index: int
    
    @lazy_property
    def next_state(self):
        kind = self.kind
        family = self.state.current_family
        card = Card(kind, family, 0)
        queue = tuple_insert(self.state.board.queue, self.index, card)
        old_decks = self.state.board.decks
        deck = old_decks[family].remove(kind)
        decks = tuple_set(old_decks, family, deck)
        board = Board(queue, decks)
        next_family = family + 1
        if next_family >= len(decks):
            next_state = PlayState(board, 0)
        else:
            next_state = PlaceState(board, next_family)
        return next_state





