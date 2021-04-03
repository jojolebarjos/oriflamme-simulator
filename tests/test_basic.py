
import pytest

from oriflamme import *

def test_end():
    board = ()
    decks = ((), (), (), ())
    scores = (0, 0, 0, 0)
    state = State(PHASE_NONE, board, decks, scores, -1)

    # We are done
    assert state.actions == ()

def test_place_empty():
    board = ()
    decks = ((KIND_SOLDIER,), (KIND_ARCHER,))
    scores = (0, 0)
    state = State(PHASE_PLACE, board, decks, scores, 0)

    # Can only place soldier at one location
    assert len(state.actions) == 1
    action = state.actions[0]
    assert action.effect == EFFECT_PLACE

    # Next board will only have the soldier
    print(action.next_state.board)
    assert action.next_state.board == (Card(KIND_SOLDIER, 0, 0),)

    # Next family also has two actions (place before or after)
    assert len(action.next_state.actions) == 2

def test_soldier():
    board = (
        Card(KIND_SPY, 0, -1),
        Card(KIND_SOLDIER, 1, -1),
        Card(KIND_LORD, 0, 2),
    )
    decks = ((), ())
    scores = (0, 0)
    state = State(PHASE_ACT, board, decks, scores, 1)

    # Can kill both
    assert len(state.actions) == 2
    assert state.actions[0].next_state.board == board[1:]
    assert state.actions[1].next_state.board == board[:-1]
