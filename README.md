# A simulator for Oriflamme

This project is a minimalistic implementation of Oriflamme, a board game by Adrien and Axel Hesling, published by Studio H in 2019.
It is not meant as an interactive playable version; rather, this is designed for reinforcement learning algorithms.


## Getting started

...

```
pip install -e .
```

...

```py
import random
from oriflamme import State, Action

state = State.begin()
while not state.has_ended:
    actions = state.actions
    action = random.choice(actions)
    state = action.next_state
```


## Game rules

The complete ruleset can be found [here](https://cdn.1j1ju.com/medias/3f/80/86-oriflamme-rulebook.pdf).


### Setup

 * Select 10 card types; players all play with this subset. This is relevant if there are more than 10 types, which is the case if extensions are used.
 * Each player discards at random 3 cards; other players must not know which ones you discarded.
 * Each player takes 1 token from the global supply.


### Placing a card

 * A card must be placed in the first phase of each turn.
 * It can be placed either at the front or the back, or on top of any card of your own family.
 * Covering a card (and the associated tokens) effectively creates a stack.


### Evaluating a card

 * Each card in the queue (at the top of its stack) must be evaluated.
 * An unrevealed card must either be revealed (and then evaluated) or kept hidden (and add a token on top of it).
 * Revealing a card gived the accumulated tokens to the player (see conspiracy and ambush for caveats).
 * By design, revealing a card the same turn as it was placed does not bring any token.
 * As long as a card is in the game (and not covered), it will be evaluated once each turn.


### End of game

 * The last card is not played.
 * Therefore, the game has exactly 6 rounds, since 3 cards are discarded at the beginning.
 * The player with the most tokens wins.
 * Note that tokens that are still on unrevealed cards do not contribute to the score.
 * If there is a tie, the player with the more cards in the queue wins, regardless of whether they are unrevealed or covered.
 * If there is still a tie, then there are multiple winners.


### Killing a card

 * Any kill implies that the card is revealed (i.e. known by players).


### Cards

#### Archer

 * Kill either the first or the last card of the queue, regardless of whether it is revealed or not.
 * If the target is a stack, only the top card is affected.
 * The archer can target itself.
 * You have to kill a card, even if you are forced to kill your own family (or even the archer itself).
 * You get 1 token from the general supply.


#### Heir

 * If there is no other revealed card with the same **name**, get 2 tokens from the general supply.
 * Only consider cards on top of stacks (i.e. another revealed heir that is covered does not prevent you from getting a token).
 * The check is done when evaluating the card itself (i.e. the condition does not need to hold for the whole turn).


#### Lord

 * Get 1 token, plus 1 for each adjacent card of your family, regardless of whether they are revealed or not.
 * Only the cards at the top of stacks count (i.e. at most 3 tokens can be earned).
 * Take the tokens from the general supply.


#### Shapeshifter

 * Copy the **ability** (i.e. not the name) of an adjacent revealed card, regardless of its family.
 * If the target is a stack, only the top card can be copied.
 * Since the name is not copied, this means that a shapeshifter copying an heir gets 2 tokens only if there is no other revealed (and uncovered) shapeshifter in the queue.


#### Soldier

 * Kill an adjacent card, regardless of whether it is revealed or not.
 * If the target is a stack, only the top card is affected.
 * You have to kill a card, even if you are forced to kill your own family.


#### Spy

 * Steal 1 token from a player who has an adjacent card, regardless of whether it is revealed or not.
 * It has no effect if it on yourself (or if the spy is alone in the queue).
 * The token is taken from the player's supply, not from the adjacent card.


#### Ambush

 * If revealed (i.e. killed) by an opponent, the opponent is discarded (i.e. killed) and you get 4 tokens from the general supply.
 * If you reveal it (i.e. either by killing it or deciding to reveal it) yourself, you 1 token from the general supply.
 * Any token that was collected on the ambush is discarded.
 * For instance, if an ennemy soldier attacks your ambush, the soldier's owner gets 1 token, you get 4 tokens, and both cards are discarded.
 * Conversely, if your own solder attacks your ambush, you get 1 + 1 = 2 tokens and discards both cards.


#### Assassination

 * Kill any card in the queue, regardless of whether it is revealed or not.
 * If the target is a stack, only the top card can be eliminated.
 * You have to kill a card, even if this from your own family.
 * The assassination card itself cannot be targeted.


#### Conspiracy

 * Get double the accumulated tokens when revealed.
 * It has no other effect.
 * Therefore, a conspiracy that is killed does not provide anything to its owner.


#### Royal decree

 * Move a card to any location in the queue, and then discard the decree.
 * The moved card may be revealed or not and must be on top.
 * "any location" means either
   * inserting before or after any existing card/stack;
   * placing on top of a card of its own family.
 * Placing the moved card on top of the decree does not prevent the loss of the decree; the decree is discarded, and then the moved card is executed, as any newly uncovered card.
 * Note that the moved card can therefore be evaluated twice in this turn.
 