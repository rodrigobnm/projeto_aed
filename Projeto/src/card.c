// card.c
#include "card.h"

void flip_card(Card* card) {
    card->flipped = !card->flipped;
}

int check_match(Card* card1, Card* card2) {
    return card1->id == card2->id;
}
