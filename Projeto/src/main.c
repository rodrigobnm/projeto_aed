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
SDL_Texture* backgroundTexture = NULL;
TTF_Font* font = NULL;

int initialize();
void render_text(const char* text, int x, int y, SDL_Color color);
void render_button(SDL_Rect rect, const char* text);
void render_menu();
void cleanup();

int main() {
    if (!initialize()) return -1;

    backgroundTexture = load_texture("background.jpg"); // Substitua "background.jpg" pelo caminho da sua imagem
    
    // Aplicar desfoque na imagem
    SDL_Texture* blurredTexture = create_blurred_texture(backgroundTexture);

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
                    // Coordenadas do botão "Iniciar"
                    if (x >= 300 && x <= 500 && y >= 250 && y <= 300) {
                        in_menu = 0;  // Começar o jogo
                    }
                    // Coordenadas do botão "Sair"
                    else if (x >= 300 && x <= 500 && y >= 350 && y <= 400) {
                        running = 0;  // Sair do jogo
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Tela de fundo preta
        SDL_RenderClear(renderer);

        if (in_menu) {
            render_menu();
        }

        SDL_RenderPresent(renderer);
    }

    cleanup();
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

    window = SDL_CreateWindow("Menu Estilo Minecraft", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erro ao criar janela: %s\n", SDL_GetError());
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erro ao criar renderizador: %s\n", SDL_GetError());
        return 0;
    }

    // Carregar a fonte estilo "Minecraft" (substitua pelo caminho da fonte no seu sistema)
    font = TTF_OpenFont("fonte_t.ttf", 32);
    if (!font) {
        printf("Erro ao carregar fonte: %s\n", TTF_GetError());
        return 0;
    }

    return 1;
}

void render_text(const char* text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect textRect;
    textRect.x = x - surface->w / 2;
    textRect.y = y - surface->h / 2;
    textRect.w = surface->w;
    textRect.h = surface->h;

    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void render_button(SDL_Rect rect, const char* text) {
    SDL_Color borderColor = {25, 25, 25, 255};  // Borda escura
    SDL_Color backgroundColor = {150, 150, 150, 255};  // Cinza claro
    SDL_Color textColor = {255, 255, 255, 255};  // Branco

    // Desenhar fundo do botão
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderFillRect(renderer, &rect);

    // Desenhar borda do botão
    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderDrawRect(renderer, &rect);

    // Renderizar texto do botão centralizado
    render_text(text, rect.x + rect.w / 2, rect.y + rect.h / 2, textColor);
}

void render_menu() {
    SDL_Color titleColor = {255, 255, 255, 255}; // Branco

    // Renderizar título
    render_text("Menu do Jogo", WINDOW_WIDTH / 2, 100, titleColor);

    // Botão "Iniciar"
    SDL_Rect startButton = {300, 250, 200, 50};
    render_button(startButton, "Iniciar");

    // Botão "Sair"
    SDL_Rect exitButton = {300, 350, 200, 50};
    render_button(exitButton, "Sair");
}

void cleanup() {
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}