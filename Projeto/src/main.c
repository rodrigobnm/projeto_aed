#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800
#define CARD_WIDTH 100
#define CARD_HEIGHT 150
#define NUM_CARDS 8

// Estrutura para representar cada carta na lista
typedef struct Card {
    char name[30];
    int birth_year;
    char image_path[50];
    SDL_Texture* texture;
    struct Card* next;
    struct Card* prev;
    int x, y;  // Adiciona coordenadas para a posição da carta
} Card;

// Estrutura para representar a lista de cartas
typedef struct {
    Card* head;
    Card* tail;
} CardList;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;

CardList cardList;
int lives = 3;
Card* dragged_card = NULL;
int offset_x = 0, offset_y = 0;

// Funções para gerenciamento da lista encadeada
void add_card(CardList* list, const char* name, int birth_year, const char* image_path);
void shuffle_cards(CardList* list);
int is_sorted(CardList* list);
Card* get_card_at(int x, int y);
void rearrange_cards(Card* dropped_card);

// Funções SDL e de renderização
int initialize();
void render_text(const char* text, int x, int y, SDL_Color color);
SDL_Texture* load_texture(const char* path);
void render_menu();
void render_game();
void cleanup();
void render_button(SDL_Rect rect, const char* text);

int main() {
    if (!initialize()) return -1;

    // Inicializar lista de cartas
    add_card(&cardList, "Alceu Valenca", 1946, "alceu_valenca.png");
    add_card(&cardList, "Ariano Suassuna", 1927, "ariano_suassuna.png");
    add_card(&cardList, "Chico Science", 1966, "chico_science.png");
    add_card(&cardList, "Gilberto Freyre", 1900, "gilberto_freyre.png");
    add_card(&cardList, "J Borges", 1935, "j_borges.png");
    add_card(&cardList, "Lampiao", 1898, "lampiao.png");
    add_card(&cardList, "Lia de Itamaraca", 1944, "lia_de_itamaraca.png");
    add_card(&cardList, "Luiz Gonzaga", 1912, "luiz_gonzaga.png");

    shuffle_cards(&cardList);

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
                    if (x >= 300 && x <= 500 && y >= 250 && y <= 300) {
                        in_menu = 0;
                    } else if (x >= 300 && x <= 500 && y >= 350 && y <= 400) {
                        running = 0;
                    }
                } else {
                    // Detectar clique na carta para arrastar
                    Card* card = get_card_at(x, y);
                    if (card) {
                        dragged_card = card;
                        offset_x = x - card->x;
                        offset_y = y - card->y;
                    }
                }
            } else if (event.type == SDL_MOUSEMOTION && dragged_card) {
                // Atualizar posição da carta arrastada
                dragged_card->x = event.motion.x - offset_x;
                dragged_card->y = event.motion.y - offset_y;
            } else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT && dragged_card) {
                // Soltar a carta e reorganizar a lista
                rearrange_cards(dragged_card);
                dragged_card = NULL;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (in_menu) {
            render_menu();
        } else {
            render_game();
            if (is_sorted(&cardList)) {
                render_text("Parabéns! Você venceu!", WINDOW_WIDTH / 2, 50, (SDL_Color){0, 255, 0, 255});
            } else if (lives <= 0) {
                render_text("Game Over! Você perdeu todas as vidas.", WINDOW_WIDTH / 2, 50, (SDL_Color){255, 0, 0, 255});
            }
        }

        SDL_RenderPresent(renderer);
    }

    cleanup();
    return 0;
}

void add_card(CardList* list, const char* name, int birth_year, const char* image_path) {
    Card* new_card = (Card*)malloc(sizeof(Card));
    strcpy(new_card->name, name);
    new_card->birth_year = birth_year;
    strcpy(new_card->image_path, image_path);
    new_card->texture = load_texture(image_path);

    new_card->next = NULL;
    new_card->prev = list->tail;

    if (list->tail) list->tail->next = new_card;
    else list->head = new_card;

    list->tail = new_card;

    // Define a posição inicial da carta
    new_card->x = 50 + (CARD_WIDTH + 10) * (list->tail ? NUM_CARDS - 1 : 0);
    new_card->y = 200;
}

void shuffle_cards(CardList* list) {
    srand(time(NULL));
    Card* cards[NUM_CARDS];
    Card* current = list->head;

    for (int i = 0; i < NUM_CARDS && current; i++) {
        cards[i] = current;
        current = current->next;
    }

    for (int i = NUM_CARDS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card* temp = cards[i];
        cards[i] = cards[j];
        cards[j] = temp;
    }

    list->head = cards[0];
    list->tail = cards[NUM_CARDS - 1];

    for (int i = 0; i < NUM_CARDS; i++) {
        if (i > 0) cards[i]->prev = cards[i - 1];
        else cards[i]->prev = NULL;

        if (i < NUM_CARDS - 1) cards[i]->next = cards[i + 1];
        else cards[i]->next = NULL;

        // Define a posição das cartas após o embaralhamento
        cards[i]->x = 50 + (CARD_WIDTH + 10) * i;
        cards[i]->y = 200;
    }
}

int is_sorted(CardList* list) {
    Card* current = list->head;
    while (current && current->next) {
        if (current->birth_year > current->next->birth_year) {
            return 0;
        }
        current = current->next;
    }
    return 1;
}

void render_menu() {
    SDL_Color titleColor = {0, 0, 0, 255}; // Preto
    render_text("Jogo de Ordenação - Artistas de Pernambuco", WINDOW_WIDTH / 2, 100, titleColor);

    SDL_Rect startButton = {300, 250, 200, 50};
    render_button(startButton, "Iniciar");

    SDL_Rect exitButton = {300, 350, 200, 50};
    render_button(exitButton, "Sair");
}

void render_game() {
    Card* current = cardList.head;

    while (current) {
        SDL_Rect card_rect = {current->x, current->y, CARD_WIDTH, CARD_HEIGHT};
        SDL_RenderCopy(renderer, current->texture, NULL, &card_rect);
        current = current->next;
    }

    render_text("Arraste as cartas para ordenar", WINDOW_WIDTH / 2, 100, (SDL_Color){255, 255, 255, 255});
}

Card* get_card_at(int x, int y) {
    Card* current = cardList.head;
    while (current) {
        if (x >= current->x && x <= current->x + CARD_WIDTH && y >= current->y && y <= current->y + CARD_HEIGHT) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void rearrange_cards(Card* dropped_card) {
    // Ajustar lógica de reorganização se necessário
}

int initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) {
        printf("Erro: %s\n", SDL_GetError());
        return 0;
    }

    window = SDL_CreateWindow("Jogo de Ordenação", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    font = TTF_OpenFont("fonte_t.ttf", 32);
    return font != NULL && window && renderer;
}

void render_text(const char* text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {x - surface->w / 2, y - surface->h / 2, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

SDL_Texture* load_texture(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void render_button(SDL_Rect rect, const char* text) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
    render_text(text, rect.x + rect.w / 2, rect.y + rect.h / 2, (SDL_Color){255, 255, 255, 255});
}

void cleanup() {
    Card* current = cardList.head;
    while (current) {
        SDL_DestroyTexture(current->texture);
        Card* temp = current;
        current = current->next;
        free(temp);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}
