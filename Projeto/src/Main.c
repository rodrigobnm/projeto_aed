#include <SDL2/SDL.h>
#include <stdbool.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define PLAYER_WIDTH 50
#define PLAYER_HEIGHT 50
#define PLAYER_SPEED 5

int main(int argc, char *argv[]) {
    // Inicialização do SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        printf("Erro ao inicializar o SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Criação da janela
    SDL_Window *window = SDL_CreateWindow("Jogo em C com SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) {
        printf("Erro ao criar a janela: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Criação do renderizador
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_DestroyWindow(window);
        printf("Erro ao criar o renderizador: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Definição das variáveis do jogador
    SDL_Rect player = {WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, PLAYER_WIDTH, PLAYER_HEIGHT};
    int playerSpeed = PLAYER_SPEED;

    bool running = true;
    SDL_Event event;

    // Loop principal do jogo
    while (running) {
        // Tratamento de eventos
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Captura das teclas pressionadas para movimentação
        const Uint8 *keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_UP]) player.y -= playerSpeed;
        if (keystate[SDL_SCANCODE_DOWN]) player.y += playerSpeed;
        if (keystate[SDL_SCANCODE_LEFT]) player.x -= playerSpeed;
        if (keystate[SDL_SCANCODE_RIGHT]) player.x += playerSpeed;

        // Impedir que o jogador saia da tela
        if (player.x < 0) player.x = 0;
        if (player.y < 0) player.y = 0;
        if (player.x > WINDOW_WIDTH - PLAYER_WIDTH) player.x = WINDOW_WIDTH - PLAYER_WIDTH;
        if (player.y > WINDOW_HEIGHT - PLAYER_HEIGHT) player.y = WINDOW_HEIGHT - PLAYER_HEIGHT;

        // Renderização
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Fundo preto
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Jogador vermelho
        SDL_RenderFillRect(renderer, &player);

        SDL_RenderPresent(renderer);
    }

    // Limpeza dos recursos
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
