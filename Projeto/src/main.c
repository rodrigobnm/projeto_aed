//
// gcc main.c card.c -o jogo -I/usr/include/SDL2 -lSDL2 -lSDL2_image -lSDL2_ttf
/// ./jogo
//
//
//
//
//
//
//



#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
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
TTF_Font* font = NULL;

int initialize();
void load_cards(Card cards[], int num_pairs);
void render_cards(Card cards[], int num_cards);
void render_menu();
void cleanup(Card cards[], int num_cards);

int main() {
    if (!initialize()) return -1;

    Card cards[NUM_CARDS * 2];  // Duas cartas por par
    load_cards(cards, NUM_CARDS);

    int running = 1;
    int in_menu = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int x = event.button.x;
                int y = event.button.y;

                if (in_menu) {
                    if (x >= 300 && x <= 500 && y >= 200 && y <= 250) {
                        in_menu = 0;  // Começar o jogo
                    } else if (x >= 300 && x <= 500 && y >= 300 && y <= 350) {
                        running = 0;  // Sair do jogo
                    }
                } else {
                    for (int i = 0; i < NUM_CARDS * 2; i++) {
                        if (cards[i].texture != NULL &&
                            x >= cards[i].rect.x && x <= (cards[i].rect.x + CARD_WIDTH) &&
                            y >= cards[i].rect.y && y <= (cards[i].rect.y + CARD_HEIGHT)) {
                            SDL_DestroyTexture(cards[i].texture);
                            cards[i].texture = NULL;
                        }
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (in_menu) {
            render_menu();
        } else {
            render_cards(cards, NUM_CARDS * 2);
        }

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

    if (TTF_Init() == -1) {
        printf("Erro ao inicializar SDL_ttf: %s\n", TTF_GetError());
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

    font = TTF_OpenFont("fonte_t.ttf", 24);
    if (!font) {
        printf("Erro ao carregar fonte: %s\n", TTF_GetError());
        return 0;
    }

    return 1;
}

void load_cards(Card cards[], int num_pairs) {
    srand(time(NULL));
    for (int i = 0; i < num_pairs * 2; i++) {
        cards[i].id = i / 2;
        cards[i].rect.w = CARD_WIDTH;
        cards[i].rect.h = CARD_HEIGHT;
        cards[i].rect.x = (i % 4) * (CARD_WIDTH + 10) + 50;
        cards[i].rect.y = (i / 4) * (CARD_HEIGHT + 10) + 50;
        cards[i].flipped = 0;
        cards[i].texture = IMG_LoadTexture(renderer, "card_back.png");
        if (!cards[i].texture) {
            printf("Erro ao carregar imagem da carta: %s\n", SDL_GetError());
        }
    }
}

void render_text(const char* text, int x, int y) {
    SDL_Color color = {0, 0, 0, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect textRect;
    textRect.x = x;
    textRect.y = y;
    textRect.w = surface->w;
    textRect.h = surface->h;

    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void render_menu() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_Rect startButton = {300, 200, 200, 50};
    SDL_Rect exitButton = {300, 300, 200, 50};

    SDL_RenderFillRect(renderer, &startButton);
    SDL_RenderFillRect(renderer, &exitButton);

    render_text("Iniciar", 350, 215);  // Texto do botão "Começar"
    render_text("Sair", 375, 315);     // Texto do botão "Sair"
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
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}
