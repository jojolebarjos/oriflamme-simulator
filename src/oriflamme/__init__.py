from __future__ import annotations

from dataclasses import dataclass, field
from enum import Enum
from typing import Sequence


# TODO use attrs frozen slotted dataclasses
# TODO use lazy properties in some cases?


class Kind(Enum):
    ARCHER = 0
    HEIR = 1
    LORD = 2
    SHAPESHIFTER = 3
    SOLDIER = 4
    SPY = 5
    AMBUSH = 6
    ASSASSINATION = 7
    CONSPIRACY = 8
    ROYAL_DECREE = 9
    # TODO more from extension


@dataclass
class Stack:
    kind: Kind
    player: int
    tokens: int | None = None
    tail: Stack | None = None

    @property
    def is_revealed(self) -> bool:
        return self.tokens is None


@dataclass
class Board:
    queue: Sequence[Stack]
    hands: Sequence[Sequence[Kind]]
    tokens: Sequence[int]
    # TODO keep track of discarded (revealed only) cards

    def replace(self, queue=None, hands=None, tokens=None) -> Board:
        if queue is None:
            queue = self.queue
        if hands is None:
            hands = self.hands
        if tokens is None:
            tokens = self.tokens
        return Board(queue, hands, tokens)


class State:
    board: Board
    actions: Sequence[Action]
    has_ended: bool = False

    @classmethod
    def begin(cls):
        # TODO how to give random generator?
        # TODO proper init, with options
        queue = []
        hand = [
            Kind.ARCHER,
            Kind.HEIR,
            Kind.LORD,
            # Kind.SHAPESHIFTER,
            Kind.SOLDIER,
            # Kind.SPY,
            # Kind.AMBUSH,
            # Kind.ASSASSINATION,
            # Kind.CONSPIRACY,
            # Kind.ROYAL_DECREE,
        ]
        hands = [hand] * 3
        tokens = [1] * 3
        board = Board(queue, hands, tokens)
        state = PlaceState(board, 0)
        return state


@dataclass
class PlaceState(State):
    board: Board
    player: int

    @property
    def actions(self) -> Sequence[Action]:
        hand = self.board.hands[self.player]
        assert hand
        actions = []
        # TODO deduplicate actions, if any kind is duplicated in hand

        # Insert card at start of queue
        for kind in hand:
            action = InsertAction(self, kind, self.player, 0)
            actions.append(action)

        # Insert card at end of queue
        length = len(self.board.queue)
        if length > 0:
            for kind in hand:
                action = InsertAction(self, kind, self.player, length)
                actions.append(action)

        # Stack card on top of another card
        for index in range(length):
            if self.board.queue[index].player == self.player:
                for kind in hand:
                    action = StackAction(self, kind, self.player, index)
                    actions.append(action)

        return actions


@dataclass
class EvaluateState(State):
    board: Board
    index: int

    @property
    def actions(self) -> Sequence[Action]:
        stack = self.board.queue[self.index]

        # If card is revealed, then player must evaluate it
        if stack.is_revealed:
            actions = self._actions_for(stack.kind)
            # TODO deduplicate actions?
            if not actions:
                actions.append(NoneAction(self))
            return actions

        # Otherwise, the player can either reveal it or add a token
        return [
            RevealAction(self),
            AccumulateAction(self),
        ]

    def _actions_for(self, kind: Kind) -> Sequence[Action]:
        match kind:
            # Archer has to kill either first or last card
            case Kind.ARCHER:
                player = self.board.queue[self.index].player
                actions = [KillAction(self, player, 0)]
                length = len(self.board.queue)
                if length > 1:
                    actions.append(KillAction(self, player, length - 1))
                return actions

            # Heir inherits tokens, if this is the only (revealed) card with this name
            case Kind.HEIR:
                player = self.board.queue[self.index].player
                name = self.board.queue[self.index].kind
                count = sum(name == stack.kind for stack in self.board.queue)
                if count == 1:
                    return [GainAction(self, player, 2)]
                return []

            # Lord collects tokens
            case Kind.LORD:
                player = self.board.queue[self.index].player
                length = len(self.board.queue)
                count = 1
                if self.index > 0 and self.board.queue[self.index - 1].player == player:
                    count += 1
                if (
                    self.index + 1 < length
                    and self.board.queue[self.index + 1].player == player
                ):
                    count += 1
                return [GainAction(self, player, count)]

            # Shapeshifter can copy the ability (but not the name) of adjacent card
            case Kind.SHAPESHIFTER:
                left_kind = None
                if self.index > 0:
                    stack = self.board.queue[self.index - 1]
                    if stack.is_revealed and stack.kind != Kind.SHAPESHIFTER:
                        left_kind = stack.kind
                right_kind = None
                length = len(self.board.queue)
                if self.index + 1 < length:
                    stack = self.board.queue[self.index + 1]
                    if stack.is_revealed and stack.kind != Kind.SHAPESHIFTER:
                        right_kind = stack.kind
                actions = []
                if left_kind is not None:
                    actions.extend(self._actions_for(left_kind))
                if right_kind is not None and right_kind != left_kind:
                    actions.extend(self._actions_for(right_kind))
                return actions

            # Soldier has to kill either previous or next card, if any
            case Kind.SOLDIER:
                player = self.board.queue[self.index].player
                length = len(self.board.queue)
                actions = []
                if self.index > 0:
                    actions.append(KillAction(self, player, self.index - 1))
                if self.index + 1 < length:
                    actions.append(KillAction(self, player, self.index + 1))
                return actions

            # Steal tokens from adjacent player
            case Kind.SPY:
                player = self.board.queue[self.index].player
                left_player = player
                if self.index > 0:
                    left_player = self.board.queue[self.index - 1].player
                right_player = player
                length = len(self.board.queue)
                if self.index + 1 < length:
                    right_player = self.board.queue[self.index + 1].player
                actions = []
                if left_player != player:
                    actions.append(TransferAction(self, left_player, player, 1))
                if right_player != player and right_player != left_player:
                    actions.append(TransferAction(self, right_player, player, 1))
                return actions

            # Ambush and conspiracy do nothing
            # Note: tokens have already been handled at reveal
            case Kind.AMBUSH | Kind.CONSPIRACY:
                return []

            # Assassinate any card
            case Kind.ASSASSINATION:
                player = self.board.queue[self.index].player
                length = len(self.board.queue)
                return [
                    KillAction(self, player, i)
                    for i in range(length)
                    if i != self.index
                ]

            # Move any card
            case Kind.ROYAL_DECREE:
                # TODO
                raise NotImplementedError

            # Unexpected kind
            case _:
                raise KeyError(kind)


@dataclass
class EndState(State):
    board: Board
    has_ended = True

    @property
    def actions(self) -> Sequence[Action]:
        return []


class Action:
    state: State
    next_state: State


@dataclass
class NoneAction(Action):
    state: State = field(repr=False)

    @property
    def next_state(self) -> State:
        return _advance_state(self.state)


@dataclass
class InsertAction(Action):
    state: PlaceState = field(repr=False)
    kind: Kind
    player: int
    index: int

    @property
    def next_state(self) -> State:
        stack = Stack(self.kind, self.player, 0)
        queue = _insert_at(self.state.board.queue, self.index, stack)
        hands = _unhand(self.state.board.hands, self.player, self.kind)
        return _advance_place_state(self.state, queue=queue, hands=hands)


@dataclass
class StackAction(Action):
    state: PlaceState = field(repr=False)
    kind: Kind
    player: int
    index: int

    @property
    def next_state(self) -> State:
        queue = _stack_at(
            self.state.board.queue,
            self.index,
            self.kind,
            self.player,
            0,
        )
        hands = _unhand(self.state.board.hands, self.player, self.kind)
        return _advance_place_state(self.state, queue=queue, hands=hands)


@dataclass
class RevealAction(Action):
    state: EvaluateState = field(repr=False)

    @property
    def next_state(self) -> State:
        stack = self.state.board.queue[self.state.index]
        # TODO handle AMBUSH and CONSPIRACY tokens
        queue = _replace_at(
            self.state.board.queue,
            self.state.index,
            stack.kind,
            stack.player,
        )
        return _advance_evaluation_state(
            self.state,
            index=self.state.index,
            queue=queue,
        )


@dataclass
class AccumulateAction(Action):
    state: EvaluateState = field(repr=False)

    @property
    def next_state(self) -> State:
        stack = self.state.board.queue[self.state.index]
        queue = _replace_at(
            self.state.board.queue,
            self.state.index,
            stack.kind,
            stack.player,
            stack.tokens + 1,
        )
        return _advance_evaluation_state(self.state, queue=queue)


@dataclass
class KillAction(Action):
    state: EvaluateState = field(repr=False)
    killer: int
    index: int

    @property
    def next_state(self) -> State:
        # Grant token for kill
        # TODO handle AMBUSH
        tokens = _increase(self.state.board.tokens, self.killer, 1)

        # Remove card
        old_queue = self.state.board.queue
        new_queue = _unstack_at(old_queue, self.index)

        # Careful with index update, if a stack was removed
        index = self.state.index + 1
        if len(old_queue) != len(new_queue) and self.index <= self.state.index:
            index -= 1

        return _advance_evaluation_state(
            self.state,
            index=index,
            queue=new_queue,
            tokens=tokens,
        )


@dataclass
class GainAction(Action):
    state: EvaluateState = field(repr=False)
    player: int
    amount: int

    @property
    def next_state(self) -> State:
        tokens = _increase(
            self.state.board.tokens,
            self.player,
            self.amount,
        )
        return _advance_evaluation_state(self.state, tokens=tokens)


@dataclass
class TransferAction(Action):
    state: EvaluateState = field(repr=False)
    from_player: int
    to_player: int
    amount: int

    @property
    def next_state(self) -> State:
        tokens = _transfer(
            self.state.board.tokens,
            self.from_player,
            self.to_player,
            self.amount,
        )
        return _advance_evaluation_state(self.state, tokens=tokens)


@dataclass
class MoveAction(Action):
    ...
    # TODO should probably do a two-steps action, to reduce complexity


def _advance_state(state, **kwargs):
    match state:
        case PlaceState():
            return _advance_place_state(state, **kwargs)
        case EvaluateState():
            return _advance_evaluation_state(state, **kwargs)
        case EndState():
            return state
        case _:
            raise TypeError


def _advance_place_state(state, *, board=None, **kwargs):
    if board is None:
        board = state.board.replace(**kwargs)

    # If there are still players that need to play a card, let them
    next_player = _next_player_for_placement(board, state.player + 1)
    if next_player is not None:
        return PlaceState(board, next_player)

    # Moving to evaluation phase
    return _advance_evaluation_state(state, board=board, index=0)


def _advance_evaluation_state(state, *, board=None, index=None, **kwargs):
    if board is None:
        board = state.board.replace(**kwargs)

    # If there are still cards to be evaluated, go for it
    if index is None:
        index = state.index + 1
    count = len(board.queue)
    if index < count:
        return EvaluateState(board, index)

    # If there are more card to play
    next_player = _next_player_for_placement(board, 0)
    if next_player is not None:
        return PlaceState(board, next_player)

    # Game is over, no player has more than one card left
    return EndState(board)


def _next_player_for_placement(board, next_player):
    count = len(board.hands)
    while next_player < count and len(board.hands[next_player]) <= 1:
        next_player += 1
    if next_player < count:
        return next_player
    return None


def _insert_at(sequence, index, value):
    result = list(sequence)
    result.insert(index, value)
    return result


def _set_at(sequence, index, value):
    result = list(sequence)
    result[index] = value
    return result


def _remove_at(sequence, index):
    result = list(sequence)
    del result[index]
    return result


def _remove_first(sequence, value):
    result = list(sequence)
    result.remove(value)
    return result


def _stack_at(queue, index, kind, player, tokens=None):
    stack = Stack(kind, player, tokens, queue[index])
    return _set_at(queue, index, stack)


def _unstack_at(queue, index):
    stack = queue[index]
    if stack.tail is None:
        return _remove_at(queue, index)
    return _set_at(queue, index, stack.tail)


def _replace_at(queue, index, kind, player, tokens=None):
    stack = Stack(kind, player, tokens, queue[index].tail)
    return _set_at(queue, index, stack)


def _unhand(hands, player, kind):
    old_hand = hands[player]
    new_hand = _remove_first(old_hand, kind)
    return _set_at(hands, player, new_hand)


def _increase(tokens, player, amount):
    tokens = list(tokens)
    tokens[player] = max(tokens[player] + amount, 0)
    return tokens


def _transfer(tokens, from_player, to_player, amount):
    assert amount >= 0
    amount = min(amount, tokens[from_player])
    if amount <= 0:
        return tokens
    tokens = list(tokens)
    tokens[from_player] -= amount
    tokens[to_player] += amount
    return tokens
