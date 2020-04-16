#include "Game.hpp"

int* Game::getCardsSelf() {
    return cardsSelf;
}

int* Game::getCardsOther() {
    return cardsOther;
}

void Game::setCardsSelf(const int *cards) {
    for (int i = 0; i < 8; i++) {
        this->cardsSelf[i] = cards[i];
    }
}

void Game::setCardsOther(const int *cards) {
    for (int i = 0; i < 8; i++) {
        this->cardsOther[i] = cards[i];
    }
}