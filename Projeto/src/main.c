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
#define NUM_CARDS 3


typedef struct Card {
    char name[30];
    int birth_year;
    char image_path[50];
    SDL_Texture* texture;
    SDL_Texture* background_texture;
    struct Card* next;
    struct Card* prev;
    int x, y;
    int slot_index;
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
SDL_Rect slots[NUM_CARDS];
int check_order_button_pressed = 0;  // Variável global para verificar o clique do botão


// Declarações das funções
void add_card(CardList* list, const char* name, int birth_year, const char* image_path);
void shuffle_cards(CardList* list);
void check_order();
int is_sorted(CardList* list);
Card* get_card_at(int x, int y);
int get_slot_index(int x, int y);
void render_feedback(int slot_index, int correct);
int initialize();
void render_text(const char* text, int x, int y, SDL_Color color);
SDL_Texture* load_texture(const char* path);
SDL_Texture* load_background_texture(const char* character_name);
void render_menu();
void render_game();
void cleanup();
void render_button(SDL_Rect rect, const char* text);

int main() {
    if (!initialize()) return -1;
    
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

    // Ajuste diretamente no código onde você define os slots
    for (int i = 0; i < NUM_CARDS; i++) {
    // Calcular a posição inicial X para centralizar os slots
    int total_width = (CARD_WIDTH + 10) * NUM_CARDS - 10;  // Largura total das cartas com o espaçamento
    int starting_x = (WINDOW_WIDTH - total_width) / 2;      // Espaço à esquerda para centralizar

    // Posição de cada slot
    slots[i].x = starting_x + (CARD_WIDTH + 10) * i;  // Ajusta X com base no cálculo de centralização
    slots[i].y = 400;  // Posição fixa no eixo Y para os slots
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
                if (x >= (WINDOW_WIDTH - 200) / 2 && x <= (WINDOW_WIDTH + 200) / 2 && y >= 250 && y <= 300)  {
                    in_menu = 0;
                } else if (x >= (WINDOW_WIDTH - 200) / 2 && x <= (WINDOW_WIDTH + 200) / 2 && y >= 350 && y <= 400) {
                    running = 0;
                }
            } else {
                // Verificar se o botão "Voltar ao Menu" foi clicado
                if (x >= 10 && x <= 160 && y >= 10 && y <= 50) {
                    in_menu = 1;  // Volta para o menu
                    lives = 3;  // Reseta o número de vidas
                    // Resetar outras variáveis se necessário (como o estado do jogo)
                }

                Card* card = get_card_at(x, y);
                if (card) {
                    dragged_card = card;
                    offset_x = x - card->x;
                    offset_y = y - card->y;
                }

                int button_x = (WINDOW_WIDTH - 200) / 2;  // Centraliza o botão horizontalmente
                int button_y = 600;  // Posição fixa do botão no eixo Y
                if (x >= button_x && x <= button_x + 200 && y >= button_y && y <= button_y + 50) {
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
        
        // Defina a posição das mensagens de vitória e game over
        int message_y_position = 100 + CARD_HEIGHT + 20;  // Ajuste a posição para ficar acima das cartas e abaixo do texto superior
        
        if (is_sorted(&cardList)) {
            render_text("Parabéns! Você venceu!", WINDOW_WIDTH / 2, message_y_position, (SDL_Color){0, 255, 0, 255});
        } else if (lives <= 0) {
            render_text("Game Over! Você perdeu todas as vidas.", WINDOW_WIDTH / 2, message_y_position, (SDL_Color){255, 0, 0, 255});
        }
    }

    SDL_RenderPresent(renderer);
}


    cleanup();
    return 0;
}

// Função para adicionar uma carta à lista
// Função para adicionar uma carta à lista
void add_card(CardList* list, const char* name, int birth_year, const char* image_path) {
    Card* new_card = (Card*)malloc(sizeof(Card));
    strcpy(new_card->name, name);
    new_card->birth_year = birth_year;
    strcpy(new_card->image_path, image_path);
    new_card->texture = load_texture(image_path);
    new_card->background_texture = load_background_texture(name);
    new_card->next = NULL;
    new_card->prev = list->tail;

    if (list->tail) {
        list->tail->next = new_card;
    } else {
        list->head = new_card;
    }

    list->tail = new_card;

    // Atualizar as posições das cartas após a adição
    int total_width = (CARD_WIDTH + 10) * NUM_CARDS - 10;  // Largura total das cartas com espaçamento
    int starting_x = (WINDOW_WIDTH - total_width) / 2;      // Espaço à esquerda para centralizar
    int index = 0;

    // Recalcular a posição de todas as cartas
    Card* current = list->head;
    while (current) {
        current->x = starting_x + (CARD_WIDTH + 10) * index;
        current->y = 200;  // Posição fixa no eixo Y
        current->slot_index = -1;
        current = current->next;
        index++;
    }
}

// Função para embaralhar as cartas
void shuffle_cards(CardList* list) {
    srand(time(NULL));

    // Copiar todas as cartas da lista para um array
    Card* cards[9];  // Array para as 9 cartas originais
    Card* current = list->head;
    int index = 0;

    // Copiar as cartas para o array
    while (current && index < 9) {
        cards[index++] = current;
        current = current->next;
    }

    // Embaralhar as 9 cartas
    for (int i = 8; i > 0; i--) {
        int j = rand() % (i + 1);
        Card* temp = cards[i];
        cards[i] = cards[j];
        cards[j] = temp;
    }

    // Atualizar a lista de cartas com apenas 3 cartas selecionadas
    list->head = cards[0];
    list->tail = cards[NUM_CARDS - 1];

    // Atualizar as ligações entre as cartas na lista
    for (int i = 0; i < NUM_CARDS; i++) {
        cards[i]->prev = (i > 0) ? cards[i - 1] : NULL;
        cards[i]->next = (i < NUM_CARDS - 1) ? cards[i + 1] : NULL;
    }

    // Recalcular as posições das cartas para manter a centralização
    int total_width = (CARD_WIDTH + 10) * NUM_CARDS - 10;
    int starting_x = (WINDOW_WIDTH - total_width) / 2;

    // Atualizar as posições das cartas
    for (int i = 0; i < NUM_CARDS; i++) {
        cards[i]->x = starting_x + (CARD_WIDTH + 10) * i;
        cards[i]->y = 200;
        cards[i]->slot_index = -1;
    }
}


// Função para verificar se as cartas estão ordenadas
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

// Função para pegar uma carta na posição x, y
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

// Função para verificar o índice do encaixe na posição x, y
int get_slot_index(int x, int y) {
    for (int i = 0; i < NUM_CARDS; i++) {
        if (x >= slots[i].x && x <= slots[i].x + slots[i].w && y >= slots[i].y && y <= slots[i].y + slots[i].h) {
            return i;
        }
    }
    return -1;
}

// Função para carregar a textura de fundo de acordo com o personagem
SDL_Texture* load_background_texture(const char* character_name) {
    char background_path[60];
    snprintf(background_path, sizeof(background_path), "%s_fundo.png", character_name);
    SDL_Texture* background_texture = load_texture(background_path);

    if (!background_texture) {
        background_texture = load_texture("default_fundo.png");
    }
    return background_texture;
}

// Função para exibir o menu
void render_menu() {
    render_text("Menu Principal", WINDOW_WIDTH / 2, 100, (SDL_Color){255, 255, 255, 255});
    SDL_Rect start_button = {WINDOW_WIDTH / 2 - 100, 250, 200, 50};
    SDL_Rect quit_button = {WINDOW_WIDTH / 2 - 100, 350, 200, 50};
    render_button(start_button, "Iniciar Jogo");
    render_button(quit_button, "Sair");
}


// Função para renderizar o jogo
void render_game() {
    render_text("Arraste as cartas para os encaixes na ordem correta", WINDOW_WIDTH / 2, 50, (SDL_Color){255, 255, 255, 255});

    // Ajusta a posição do botão (movendo-o para a direita)
    SDL_Rect back_button = {40, 30, 150, 40};  

    // Calcula a posição centralizada do texto dentro do botão
    int text_x = back_button.x + back_button.w / 2;
    int text_y = back_button.y + back_button.h / 2;

    // Renderiza o texto na nova posição
    render_text("Voltar ao Menu", text_x, text_y, (SDL_Color){255, 255, 255, 255});


    // Resto do código que renderiza as cartas e slots
    Card* current = cardList.head;
    while (current) {
        if (current->background_texture) {
            SDL_RenderCopy(renderer, current->background_texture, NULL, NULL);
        }

        SDL_Rect rect = {current->x, current->y, CARD_WIDTH, CARD_HEIGHT};

        if (dragged_card == current) {
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 100);
            SDL_Rect shadow_rect = {current->x + 5, current->y + 5, CARD_WIDTH, CARD_HEIGHT};
            SDL_RenderFillRect(renderer, &shadow_rect);
        }

        SDL_RenderCopy(renderer, current->texture, NULL, &rect);
        current = current->next;
    }

    for (int i = 0; i < NUM_CARDS; i++) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &slots[i]);
    }

    SDL_Rect check_button = {WINDOW_WIDTH / 2 - 100, 600, 200, 50};
    render_button(check_button, "Checar Ordem");

    // Exibe a mensagem de erro abaixo do título, acima da área das cartas
    if (check_order_button_pressed) {
        int all_cards_in_slots = 1;  // Verifica se todas as cartas estão em slots válidos
        Card* current = cardList.head;
        while (current) {
            if (current->slot_index == -1) {
                all_cards_in_slots = 0;
                break;
            }
            current = current->next;
        }

        // Se houver cartas fora dos slots, exibe a mensagem logo abaixo do texto inicial
        if (!all_cards_in_slots) {
            int message_y_position = 100; // Ajuste da posição Y para exibir abaixo do título
            render_text("Todas as cartas devem ser inseridas!", WINDOW_WIDTH / 2, message_y_position, (SDL_Color){255, 0, 0, 255});
        }
    }
}




// Função para checar se a ordem está correta
void check_order() {
    check_order_button_pressed = 1;  // Marca que o botão "Checar Ordem" foi pressionado

    // Verifica se todas as cartas estão em slots válidos
    int all_cards_in_slots = 1;  // Assume que todas as cartas estão nos slots
    Card* current = cardList.head;
    while (current) {
        if (current->slot_index == -1) {  // Se a carta não estiver em um slot válido
            all_cards_in_slots = 0;  // Alguma carta não está no slot
            break;
        }
        current = current->next;
    }

    // Se houver cartas fora dos slots, exibe a mensagem
    if (!all_cards_in_slots) {
        render_text("Todas as cartas devem ser inseridas!", WINDOW_WIDTH / 2, 550, (SDL_Color){255, 0, 0, 255});
        return;  // Não faz mais nada se as cartas não estiverem nos slots
    }

    // Caso todas as cartas estejam no lugar correto, verifica se a ordem está correta
    int correct_order = 1;
    current = cardList.head;
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
                render_feedback(i, 0);  // Se a ordem estiver errada
                correct_order = 0;
            } else {
                render_feedback(i, 1);  // Se a ordem estiver correta
            }
        }
    }

    // Se a ordem estiver correta, mostra uma mensagem de vitória
    if (correct_order) {
        printf("Você venceu!\n");
    } else {
        printf("Ordem incorreta. Tente novamente!\n");
        lives--;
    }
}


// Função para renderizar feedback visual
void render_feedback(int slot_index, int correct) {
    SDL_Color color = correct ? (SDL_Color){0, 255, 0, 255} : (SDL_Color){255, 0, 0, 255};
    render_text(correct ? "V" : "X", slots[slot_index].x + CARD_WIDTH / 2, slots[slot_index].y - 20, color);
}

// Função para inicializar SDL
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

// Função para renderizar texto
void render_text(const char* text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {x - surface->w / 2, y - surface->h / 2, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Função para carregar uma textura a partir de um caminho de arquivo
SDL_Texture* load_texture(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

// Função para renderizar um botão
void render_button(SDL_Rect rect, const char* text) {
    rect.x = (WINDOW_WIDTH - rect.w) / 2;  // Centralizar na largura da janela
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
    render_text(text, rect.x + rect.w / 2, rect.y + rect.h / 2, (SDL_Color){255, 255, 255, 255});
}


// Função de limpeza para liberar recursos
void cleanup() {
    Card* current = cardList.head;
    while (current) {
        SDL_DestroyTexture(current->texture);
        SDL_DestroyTexture(current->background_texture);
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
