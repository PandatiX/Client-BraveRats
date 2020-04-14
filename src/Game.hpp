#ifndef CLIENTBRAVERATS_GAME_HPP
#define CLIENTBRAVERATS_GAME_HPP

class Game {
private:
    int cardsSelf[8]{};
    int cardsOther[8]{};

public:
    Game() = default;

    int* getCardsSelf();
    int* getCardsOther();

    void setCardsSelf(int*);
    void setCardsOther(int*);

    ~Game() = default;
};

#endif //CLIENTBRAVERATS_GAME_HPP