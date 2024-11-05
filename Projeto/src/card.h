// card.h
#ifndef CARD_H
#define CARD_H

#include <SDL2/SDL.h>

typedef struct {
    int id;                // Identificador único para pares
    SDL_Rect rect;         // Retângulo para posição e tamanho
    SDL_Texture* texture;  // Textura da imagem da carta
    int flipped;           // Estado: 0 para virada para baixo, 1 para virada para cima
} Card;

void flip_card(Card* card);
int check_match(Card* card1, Card* card2);

#endif
