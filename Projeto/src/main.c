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
SDL_Texture* blurredTexture = NULL;
SDL_Texture* load_texture(const char* path);
SDL_Texture* create_blurred_texture(SDL_Texture* texture);
TTF_Font* font = NULL;

int initialize();
void render_text(const char* text, int x, int y, SDL_Color color);
void render_button(SDL_Rect rect, const char* text);
void render_menu();
void cleanup();

int main() {
    if (!initialize()) return -1;

    // Carregar a imagem de fundo
    backgroundTexture = load_texture("imagemdefundomenu.png");
    if (!backgroundTexture) {
        printf("Erro: Não foi possível carregar a imagem de fundo.\n");
        cleanup();
        return -1;
    }

    // Criar a textura borrada a partir da imagem de fundo
    blurredTexture = create_blurred_texture(backgroundTexture);
    if (!blurredTexture) {
        printf("Erro: Não foi possível criar a textura borrada.\n");
        cleanup();
        return -1;
    }

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

        // Renderizar a imagem de fundo borrada apenas no menu
        if (in_menu) {
            SDL_RenderCopy(renderer, blurredTexture, NULL, NULL);
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

SDL_Texture* load_texture(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        printf("Erro ao carregar imagem %s: %s\n", path, IMG_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        printf("Erro ao criar textura a partir da imagem %s: %s\n", path, SDL_GetError());
    }

    return texture;
}

SDL_Texture* create_blurred_texture(SDL_Texture* texture) {
    int width, height;
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);

    SDL_Texture* blurred_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    if (!blurred_texture) {
        printf("Erro ao criar textura borrada: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_SetTextureBlendMode(blurred_texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, blurred_texture);

    for (int offset_x = -2; offset_x <= 2; offset_x++) {
        for (int offset_y = -2; offset_y <= 2; offset_y++) {
            SDL_Rect dest_rect = { offset_x, offset_y, width, height };
            SDL_RenderCopy(renderer, texture, NULL, &dest_rect);
        }
    }

    SDL_SetRenderTarget(renderer, NULL);
    return blurred_texture;
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
    SDL_Color textColor = {0, 0, 0, 255};  // Preto

    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderDrawRect(renderer, &rect);

    render_text(text, rect.x + rect.w / 2, rect.y + rect.h / 2, textColor);
}

void render_menu() {
    SDL_Color titleColor = {0, 0, 0, 255}; // Preto

    render_text("Menu do Jogo", WINDOW_WIDTH / 2, 100, titleColor);

    SDL_Rect startButton = {300, 250, 200, 50};
    render_button(startButton, "Iniciar");

    SDL_Rect exitButton = {300, 350, 200, 50};
    render_button(exitButton, "Sair");
}

void cleanup() {
    if (backgroundTexture) SDL_DestroyTexture(backgroundTexture);
    if (blurredTexture) SDL_DestroyTexture(blurredTexture);
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}
