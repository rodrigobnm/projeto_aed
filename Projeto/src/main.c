#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "card.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define CARD_WIDTH 100
#define CARD_HEIGHT 150
#define NUM_CARDS 8  // Número de pares (16 cartas no total)

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int initialize();
void load_cards(Card cards[], int num_pairs);
void render_cards(Card cards[], int num_cards);
void cleanup(Card cards[], int num_cards);

int main() {
    if (!initialize()) return -1;

    Card cards[NUM_CARDS * 2];  // Duas cartas por par
    load_cards(cards, NUM_CARDS);

    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x;
                int y = event.button.y;

                for (int i = 0; i < NUM_CARDS * 2; i++) {
                    if (cards[i].texture != NULL &&
                        x >= cards[i].rect.x && x <= (cards[i].rect.x + CARD_WIDTH) &&
                        y >= cards[i].rect.y && y <= (cards[i].rect.y + CARD_HEIGHT)) {
                        // Remove a textura da carta, fazendo-a "sumir"
                        SDL_DestroyTexture(cards[i].texture);
                        cards[i].texture = NULL;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        render_cards(cards, NUM_CARDS * 2);

        SDL_RenderPresent(renderer);
    }

    cleanup(cards, NUM_CARDS * 2);
    return 0;
}

int initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
        return 0;
    }

    window = SDL_CreateWindow("Jogo de Cartas", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erro ao criar janela: %s\n", SDL_GetError());
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erro ao criar renderizador: %s\n", SDL_GetError());
        return 0;
    }

    return 1;
}

void load_cards(Card cards[], int num_pairs) {
    srand(time(NULL));
    for (int i = 0; i < num_pairs * 2; i++) {
        cards[i].id = i / 2;  // Define um ID para cada par
        cards[i].rect.w = CARD_WIDTH;
        cards[i].rect.h = CARD_HEIGHT;
        cards[i].rect.x = (i % 4) * (CARD_WIDTH + 10) + 50;
        cards[i].rect.y = (i / 4) * (CARD_HEIGHT + 10) + 50;
        cards[i].flipped = 0;

        // Carrega uma textura da carta (substituir "card_back.png" e "card_front.png" por imagens reais)
        cards[i].texture = IMG_LoadTexture(renderer, "card_back.png");  // Carregar a imagem da carta virada para baixo
        if (!cards[i].texture) {
            printf("Erro ao carregar imagem da carta: %s\n", SDL_GetError());
        }
    }
}

void render_cards(Card cards[], int num_cards) {
    for (int i = 0; i < num_cards; i++) {
        if (cards[i].texture != NULL) {
            SDL_RenderCopy(renderer, cards[i].texture, NULL, &cards[i].rect);
        }
    }
}

void cleanup(Card cards[], int num_cards) {
    for (int i = 0; i < num_cards; i++) {
        if (cards[i].texture) SDL_DestroyTexture(cards[i].texture);
    }
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}
