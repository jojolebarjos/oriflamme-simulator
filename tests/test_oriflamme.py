import random

import pytest

from oriflamme import *


def _begin_with_hand(*hand, num_players=3):
    queue = []
    hands = [hand] * num_players
    tokens = [1] * num_players
    board = Board(queue, hands, tokens)
    state = PlaceState(board, 0)
    return state


# TODO clean way to select action deterministically in tests


def test_game_a():
    state = _begin_with_hand(Kind.SOLDIER, Kind.LORD, Kind.ARCHER)

    assert state.player == 0
    actions = state.actions
    assert len(actions) == 3

    action = actions[0]
    state = action.next_state
    assert state.board.queue == [
        Stack(Kind.SOLDIER, 0, 0),
    ]

    assert state.player == 1
    actions = state.actions
    assert len(actions) == 6

    action = actions[2]
    state = action.next_state
    assert state.board.queue == [
        Stack(Kind.ARCHER, 1, 0),
        Stack(Kind.SOLDIER, 0, 0),
    ]

    assert state.player == 2
    actions = state.actions
    assert len(actions) == 6

    action = actions[4]
    state = action.next_state
    assert state.board.queue == [
        Stack(Kind.ARCHER, 1, 0),
        Stack(Kind.SOLDIER, 0, 0),
        Stack(Kind.LORD, 2, 0),
    ]

    assert state.index == 0
    actions = state.actions
    assert len(actions) == 2

    action = actions[1]
    assert isinstance(action, AccumulateAction)
    state = action.next_state
    assert state.board.queue == [
        Stack(Kind.ARCHER, 1, 1),
        Stack(Kind.SOLDIER, 0, 0),
        Stack(Kind.LORD, 2, 0),
    ]

    assert state.index == 1
    actions = state.actions
    assert len(actions) == 2

    action = actions[0]
    assert isinstance(action, RevealAction)
    state = action.next_state
    assert state.board.queue == [
        Stack(Kind.ARCHER, 1, 1),
        Stack(Kind.SOLDIER, 0),
        Stack(Kind.LORD, 2, 0),
    ]

    assert state.index == 1
    actions = state.actions
    assert len(actions) == 2

    action = actions[0]
    assert isinstance(action, KillAction)
    assert action.index == 0
    state = action.next_state
    assert state.board.queue == [
        Stack(Kind.SOLDIER, 0),
        Stack(Kind.LORD, 2, 0),
    ]
    assert state.board.tokens == [2, 1, 1]

    assert state.index == 1
    actions = state.actions
    assert len(actions) == 2

    action = actions[0]
    assert isinstance(action, RevealAction)
    state = action.next_state
    assert state.board.queue == [
        Stack(Kind.SOLDIER, 0),
        Stack(Kind.LORD, 2),
    ]

    assert state.index == 1
    actions = state.actions
    assert len(actions) == 1

    action = actions[0]
    assert isinstance(action, GainAction)
    state = action.next_state
    assert state.board.tokens == [2, 1, 2]

    assert state.player == 0
    actions = state.actions
    assert len(actions) == 6

    action = actions[0]
    assert isinstance(action, InsertAction)
    state = action.next_state
    assert state.board.queue == [
        Stack(Kind.LORD, 0, 0),
        Stack(Kind.SOLDIER, 0),
        Stack(Kind.LORD, 2),
    ]

    assert state.player == 1
    actions = state.actions
    assert len(actions) == 4

    action = actions[2]
    assert isinstance(action, InsertAction)
    state = action.next_state
    assert state.board.queue == [
        Stack(Kind.LORD, 0, 0),
        Stack(Kind.SOLDIER, 0),
        Stack(Kind.LORD, 2),
        Stack(Kind.SOLDIER, 1, 0),
    ]

    assert state.player == 2
    actions = state.actions
    assert len(actions) == 6

    action = actions[2]
    assert isinstance(action, InsertAction)
    state = action.next_state
    assert state.board.queue == [
        Stack(Kind.LORD, 0, 0),
        Stack(Kind.SOLDIER, 0),
        Stack(Kind.LORD, 2),
        Stack(Kind.SOLDIER, 1, 0),
        Stack(Kind.SOLDIER, 2, 0),
    ]

    assert state.index == 0
    actions = state.actions
    assert len(actions) == 2

    action = actions[0]
    assert isinstance(action, RevealAction)
    state = action.next_state
    assert state.board.queue == [
        Stack(Kind.LORD, 0),
        Stack(Kind.SOLDIER, 0),
        Stack(Kind.LORD, 2),
        Stack(Kind.SOLDIER, 1, 0),
        Stack(Kind.SOLDIER, 2, 0),
    ]

    assert state.index == 0
    actions = state.actions
    assert len(actions) == 1

    action = actions[0]
    assert isinstance(action, GainAction)
    state = action.next_state
    assert state.board.tokens == [4, 1, 2]

    assert state.index == 1
    actions = state.actions
    assert len(actions) == 2

    action = actions[1]
    assert isinstance(action, KillAction)
    state = action.next_state
    assert state.board.queue == [
        Stack(Kind.LORD, 0),
        Stack(Kind.SOLDIER, 0),
        Stack(Kind.SOLDIER, 1, 0),
        Stack(Kind.SOLDIER, 2, 0),
    ]
    assert state.board.tokens == [5, 1, 2]

    assert state.index == 2
    actions = state.actions
    assert len(actions) == 2

    action = actions[0]
    assert isinstance(action, RevealAction)
    state = action.next_state
    assert state.board.queue == [
        Stack(Kind.LORD, 0),
        Stack(Kind.SOLDIER, 0),
        Stack(Kind.SOLDIER, 1),
        Stack(Kind.SOLDIER, 2, 0),
    ]

    assert state.index == 2
    actions = state.actions
    assert len(actions) == 2

    action = actions[0]
    assert isinstance(action, KillAction)
    state = action.next_state
    assert state.board.queue == [
        Stack(Kind.LORD, 0),
        Stack(Kind.SOLDIER, 1),
        Stack(Kind.SOLDIER, 2, 0),
    ]
    assert state.board.tokens == [5, 2, 2]

    assert state.index == 2
    actions = state.actions
    assert len(actions) == 2

    action = actions[0]
    assert isinstance(action, RevealAction)
    state = action.next_state
    assert state.board.queue == [
        Stack(Kind.LORD, 0),
        Stack(Kind.SOLDIER, 1),
        Stack(Kind.SOLDIER, 2),
    ]

    assert state.index == 2
    actions = state.actions
    assert len(actions) == 1

    action = actions[0]
    assert isinstance(action, KillAction)
    state = action.next_state
    assert state.board.queue == [
        Stack(Kind.LORD, 0),
        Stack(Kind.SOLDIER, 2),
    ]
    assert state.board.tokens == [5, 2, 3]

    assert isinstance(state, EndState)


def test_random():
    for i in range(100):
        state = _begin_with_hand(
            Kind.ARCHER,
            Kind.HEIR,
            Kind.LORD,
            Kind.SHAPESHIFTER,
            Kind.SOLDIER,
            Kind.SPY,
            Kind.ASSASSINATION,
        )

        while not state.has_ended:
            actions = state.actions
            action = random.choice(actions)
            state = action.next_state
