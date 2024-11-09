#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define WINDOW_WIDTH 1300
#define WINDOW_HEIGHT 800
#define CARD_WIDTH 100
#define CARD_HEIGHT 150
#define NUM_CARDS 9  // Atualizado para incluir duas novas cartas

typedef struct Card {
    char name[30];
    int birth_year;
    char image_path[50];
    SDL_Texture* texture;
    struct Card* next;
    struct Card* prev;
    int x, y;       // Coordenadas da posição da carta
    int slot_index; // Índice do encaixe onde a carta foi solta
} Card;

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
SDL_Rect slots[NUM_CARDS]; // Encaixes para as cartas

// Funções de gerenciamento da lista encadeada
void add_card(CardList* list, const char* name, int birth_year, const char* image_path);
void shuffle_cards(CardList* list);
void check_order();
int is_sorted(CardList* list); // Adicionada a declaração da função is_sorted
Card* get_card_at(int x, int y);
int get_slot_index(int x, int y);
void render_feedback(int slot_index, int correct);

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

    // Inicializar lista de cartas com as novas cartas
    add_card(&cardList, "Alceu Valenca", 1946, "alceu_valenca.png");
    add_card(&cardList, "Ariano Suassuna", 1927, "ariano_suassuna.png");
    add_card(&cardList, "Chico Science", 1966, "chico_science.png");
    add_card(&cardList, "Gilberto Freyre", 1900, "gilberto_freyre.png");
    add_card(&cardList, "J Borges", 1935, "j_borges.png");
    add_card(&cardList, "Lampiao", 1898, "lampiao.png");
    add_card(&cardList, "Luiz Gonzaga", 1912, "luiz_gonzaga.png");
    add_card(&cardList, "Mestre Vitalino", 1909, "mestre_vitalino.png");
    add_card(&cardList, "Ila de Itamaraca", 1943, "ila_de_itamaraca.png");

    shuffle_cards(&cardList);

    // Inicializar posições dos encaixes para as cartas
    for (int i = 0; i < NUM_CARDS; i++) {
        slots[i].x = 50 + (CARD_WIDTH + 10) * i;
        slots[i].y = 400;
        slots[i].w = CARD_WIDTH;
        slots[i].h = CARD_HEIGHT;
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
                    if (x >= 300 && x <= 500 && y >= 250 && y <= 300) {
                        in_menu = 0;
                    } else if (x >= 300 && x <= 500 && y >= 350 && y <= 400) {
                        running = 0;
                    }
                } else {
                    Card* card = get_card_at(x, y);
                    if (card) {
                        dragged_card = card;
                        offset_x = x - card->x;
                        offset_y = y - card->y;
                    }
                    if (x >= 400 && x <= 600 && y >= 600 && y <= 650) {
                        check_order();
                    }
                }
            } else if (event.type == SDL_MOUSEMOTION && dragged_card) {
                dragged_card->x = event.motion.x - offset_x;
                dragged_card->y = event.motion.y - offset_y;
            } else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT && dragged_card) {
                int slot_index = get_slot_index(dragged_card->x, dragged_card->y);
                if (slot_index != -1) {
                    dragged_card->x = slots[slot_index].x;
                    dragged_card->y = slots[slot_index].y;
                    dragged_card->slot_index = slot_index;
                }
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
    new_card->x = 50 + (CARD_WIDTH + 10) * (list->tail ? NUM_CARDS - 1 : 0);
    new_card->y = 200;
    new_card->slot_index = -1;
}

int is_sorted(CardList* list) {
    Card* current = list->head;
    while (current && current->next) {
        if (current->birth_year > current->next->birth_year) {
            return 0;  // Não está em ordem
        }
        current = current->next;
    }
    return 1;  // Está em ordem
}

void check_order() {
    Card* current = cardList.head;
    int correct_order = 1;

    for (int i = 0; i < NUM_CARDS - 1; i++) {
        Card* card1 = NULL;
        Card* card2 = NULL;

        Card* temp = cardList.head;
        while (temp) {
            if (temp->slot_index == i) card1 = temp;
            if (temp->slot_index == i + 1) card2 = temp;
            temp = temp->next;
        }

        if (card1 && card2) {
            if (card1->birth_year > card2->birth_year) {
                render_feedback(i, 0);
                correct_order = 0;
            } else {
                render_feedback(i, 1);
            }
        }
    }

    if (correct_order) {
        printf("Você venceu!\n");
    } else {
        printf("Ordem incorreta. Tente novamente!\n");
        lives--;
    }
}

void render_feedback(int slot_index, int correct) {
    SDL_Color color = correct ? (SDL_Color){0, 255, 0, 255} : (SDL_Color){255, 0, 0, 255};
    render_text(correct ? "V" : "X", slots[slot_index].x + CARD_WIDTH / 2, slots[slot_index].y - 20, color);
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

        cards[i]->x = 50 + (CARD_WIDTH + 10) * i;
        cards[i]->y = 200;
    }
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

int get_slot_index(int x, int y) {
    for (int i = 0; i < NUM_CARDS; i++) {
        if (x >= slots[i].x && x <= slots[i].x + slots[i].w && y >= slots[i].y && y <= slots[i].y + slots[i].h) {
            return i;
        }
    }
    return -1;
}

void render_menu() {
    render_text("Menu Principal", WINDOW_WIDTH / 2, 100, (SDL_Color){255, 255, 255, 255});
    SDL_Rect start_button = {300, 250, 200, 50};
    SDL_Rect quit_button = {300, 350, 200, 50};
    render_button(start_button, "Iniciar Jogo");
    render_button(quit_button, "Sair");
}

void render_game() {
    render_text("Arraste as cartas para os encaixes na ordem correta", WINDOW_WIDTH / 2, 50, (SDL_Color){255, 255, 255, 255});
    
    Card* current = cardList.head;
    while (current) {
        SDL_Rect rect = {current->x, current->y, CARD_WIDTH, CARD_HEIGHT};
        SDL_RenderCopy(renderer, current->texture, NULL, &rect);
        current = current->next;
    }

    for (int i = 0; i < NUM_CARDS; i++) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &slots[i]);
    }

    SDL_Rect check_button = {400, 600, 200, 50};
    render_button(check_button, "Checar Ordem");
}

void render_button(SDL_Rect rect, const char* text) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
    render_text(text, rect.x + rect.w / 2, rect.y + rect.h / 2, (SDL_Color){255, 255, 255, 255});
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
