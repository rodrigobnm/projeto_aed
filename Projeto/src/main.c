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
TTF_Font* font_for_characters = NULL;
SDL_Texture* heart_texture = NULL;
SDL_Texture* background_texture = NULL;  
SDL_Texture* background_game_texture = NULL;  



CardList cardList;
int lives = 2;  
Card* dragged_card = NULL;
int offset_x = 0, offset_y = 0;
SDL_Rect slots[NUM_CARDS];
int check_order_button_pressed = 0;  


void add_card(CardList* list, const char* name, int birth_year, const char* image_path);
void shuffle_cards(CardList* list);
int check_order();
Card* get_card_at(int x, int y);
int get_slot_index(int x, int y);
int initialize();
void render_text(const char* text, int x, int y, SDL_Color color);
SDL_Texture* load_texture(const char* path);
void render_menu();
void render_game();
void cleanup();
void render_button(SDL_Rect rect, const char* text);
void insertion_sort_cards(CardList* list);
void render_text_with_font(const char* text, int x, int y, SDL_Color color, TTF_Font* custom_font);
void render_lives(); 


int main() {
    int ok = 1;
    while(ok){
        cleanup();
        if (!initialize()) return -1;
        
        add_card(&cardList, "Alceu Valenca", 1946, "img/alceu_valenca.png");
        add_card(&cardList, "Ariano Suassuna", 1927, "img/ariano_suassuna.png");
        add_card(&cardList, "Chico Science", 1966, "img/chico_science.png");
        add_card(&cardList, "Gilberto Freyre", 1900, "img/gilberto_freyre.png");
        add_card(&cardList, "J Borges", 1935, "img/j_borges.png");
        add_card(&cardList, "Lampiao", 1898, "img/lampiao.png");
        add_card(&cardList, "Luiz Gonzaga", 1912, "img/luiz_gonzaga.png");
        add_card(&cardList, "Mestre Vitalino", 1909, "img/mestre_vitalino.png");
        add_card(&cardList, "Ila de Itamaraca", 1943, "img/ila_de_itamaraca.png");
        add_card(&cardList, "Torre de cristal", 2000, "img/torre_cristal.png");
        add_card(&cardList, "Marco zero", 1938, "img/marco_zero.png");
        add_card(&cardList, "Ladeiras de Olinda", 1537, "img/olinda_ladeiras.png");
        add_card(&cardList, "Caranguejo do Manguebeat", 1992, "img/caranguejo.png");
        add_card(&cardList, "Museu do cangaco", 1957, "img/lampiao_fundo.png");

        for (int i = 0; i < NUM_CARDS; i++) {
            slots[i].x = (WINDOW_WIDTH - ((CARD_WIDTH + 10) * NUM_CARDS - 10)) / 2 + (CARD_WIDTH + 10) * i;
            slots[i].y = 400;
            slots[i].w = CARD_WIDTH;
            slots[i].h = CARD_HEIGHT;
        }
        
        shuffle_cards(&cardList);
        lives = 2;
        check_order_button_pressed = 0;
        dragged_card = NULL;

        for (int i = 0; i < NUM_CARDS; i++) {
            int total_width = (CARD_WIDTH + 10) * NUM_CARDS - 10;
            int starting_x = (WINDOW_WIDTH - total_width) / 2;

            slots[i].x = starting_x + (CARD_WIDTH + 10) * i;
            slots[i].y = 400;
            slots[i].w = CARD_WIDTH;
            slots[i].h = CARD_HEIGHT;
        }

        int running = 1;
        int in_menu = 1;
        int order_checked = 0;
        int order_check_result = 0;
        SDL_Event event;

        while (running) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = 0;
                    ok = 0;
                } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                    int x = event.button.x;
                    int y = event.button.y;

                    if (in_menu) {
                        if (x >= (WINDOW_WIDTH - 200) / 2 && x <= (WINDOW_WIDTH + 200) / 2 && y >= 250 && y <= 300)  {
                            in_menu = 0;
                        } else if (x >= (WINDOW_WIDTH - 200) / 2 && x <= (WINDOW_WIDTH + 200) / 2 && y >= 350 && y <= 400) {
                            running = 0;
                            ok = 0;
                        }
                    } else {
                        if (x >= 10 && x <= 160 && y >= 10 && y <= 70) {
                            in_menu = 1;
                            order_checked = 0;
                        }

                        Card* card = get_card_at(x, y);
                        if (card) {
                            dragged_card = card;
                            offset_x = x - card->x;
                            offset_y = y - card->y;
                        }

                        int button_x = (WINDOW_WIDTH - 200) / 2;
                        int button_y = 600;
                        if (x >= button_x && x <= button_x + 200 && y >= button_y && y <= button_y + 50) {

                            if (check_order() == 1) {
                                order_check_result = 1;
                            } else {
                                order_check_result = 0;
                            }
                            order_checked = 1;

                            if (order_check_result == 1 || lives <= 0) {
                                running = 0;
                            }
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
                
                int message_y_position = 100 + CARD_WIDTH + 20;

                if (order_checked) {
                    if (order_check_result == 1) { 
                        render_text("Voce venceu!", WINDOW_WIDTH / 2, message_y_position, (SDL_Color){0, 0, 0, 255});
                        SDL_RenderPresent(renderer);
                        SDL_Delay(2000);  
                        running = 0;     
                    } else if (lives <= 0) {  
                        render_text("Game Over!", WINDOW_WIDTH / 2, message_y_position, (SDL_Color){255, 0, 0, 255});
                        render_text("Ordem Correta: ", WINDOW_WIDTH / 2, message_y_position + 80, (SDL_Color){0, 255, 0, 255});
                        SDL_RenderPresent(renderer);
                        SDL_Delay(2000);  
                        running = 0;     
                    }
                }
            }

            SDL_RenderPresent(renderer);
        }

    }
        TTF_CloseFont(font);
        TTF_CloseFont(font_for_characters);

        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
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

    if (list->tail) {
        list->tail->next = new_card;
    } else {
        list->head = new_card;
    }

    list->tail = new_card;

    
    int total_width = (CARD_WIDTH + 10) * NUM_CARDS - 10;  
    int starting_x = (WINDOW_WIDTH - total_width) / 2;      
    int index = 0;

    
    Card* current = list->head;
    while (current) {
        current->x = starting_x + (CARD_WIDTH + 10) * index;
        current->y = 200;  
        current->slot_index = -1;
        current = current->next;
        index++;
    }
}

void shuffle_cards(CardList* list) {
    srand(time(NULL));

    
    Card* cards[14];  
    Card* current = list->head;
    int index = 0;

    
    while (current && index < 14) {
        cards[index++] = current;
        current = current->next;
    }

    
    for (int i = 13; i > 0; i--) {
        int j = rand() % (i + 1);
        Card* temp = cards[i];
        cards[i] = cards[j];
        cards[j] = temp;
    }

   
    list->head = cards[0];
    list->tail = cards[NUM_CARDS - 1];

    
    for (int i = 0; i < NUM_CARDS; i++) {
        cards[i]->prev = (i > 0) ? cards[i - 1] : NULL;
        cards[i]->next = (i < NUM_CARDS - 1) ? cards[i + 1] : NULL;
    }

    int total_width = (CARD_WIDTH + 10) * NUM_CARDS - 10;
    int starting_x = (WINDOW_WIDTH - total_width) / 2;

    for (int i = 0; i < NUM_CARDS; i++) {
        cards[i]->x = starting_x + (CARD_WIDTH + 10) * i;
        cards[i]->y = 200;
        cards[i]->slot_index = -1;
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

void render_text_with_font(const char* text, int x, int y, SDL_Color color, TTF_Font* custom_font) {
    SDL_Surface* surface = TTF_RenderText_Blended(custom_font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {x - surface->w / 2, y - surface->h / 2, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
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
    
    SDL_Rect background_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderCopy(renderer, background_texture, NULL, &background_rect);

   
    render_text("Menu Principal", WINDOW_WIDTH / 2, 100, (SDL_Color){0, 0, 0, 0});
    
    SDL_Rect start_button = {WINDOW_WIDTH / 2 - 100, 250, 200, 50};
    SDL_Rect quit_button = {WINDOW_WIDTH / 2 - 100, 350, 200, 50};
    
    render_button(start_button, "Iniciar Jogo");
    render_button(quit_button, "Sair");
}

void render_lives() {
    int heart_width = 50;
    int heart_height = 50;
    int spacing = 10;

    for (int i = 0; i < lives; i++) {
        
        SDL_Rect heart_rect = {
            WINDOW_WIDTH - (heart_width + spacing) * (i + 1),  
            WINDOW_HEIGHT - heart_height - 10,                 
            heart_width,
            heart_height
        };
        SDL_RenderCopy(renderer, heart_texture, NULL, &heart_rect);
    }
}

void render_game() {
    
    SDL_Rect background_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderCopy(renderer, background_game_texture, NULL, &background_rect);

    render_text("Arraste as cartas para os encaixes na ordem de idade.", WINDOW_WIDTH / 2, 50, (SDL_Color){0, 0, 0, 255});

    int mouse_x, mouse_y;

    
    SDL_GetMouseState(&mouse_x, &mouse_y);
    
    SDL_Rect back_button = {40, 30, 150, 40};
    int text_x = back_button.x + back_button.w / 2;
    int text_y = back_button.y + back_button.h / 2;
    render_text("<--", text_x, text_y, (SDL_Color){255, 255, 255, 255});

    
    Card* current = cardList.head;
    while (current) {
        
        SDL_Rect rect = {current->x, current->y, CARD_WIDTH, CARD_HEIGHT};
        if (dragged_card == current) {
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 100);
            SDL_Rect shadow_rect = {current->x + 5, current->y + 5, CARD_WIDTH, CARD_HEIGHT};
            SDL_RenderFillRect(renderer, &shadow_rect);
        }

        SDL_RenderCopy(renderer, current->texture, NULL, &rect);

        
        int name_y_position = current->y + CARD_HEIGHT + 10;  
        if (mouse_x >= current->x && mouse_x <= current->x + CARD_WIDTH &&
            mouse_y >= current->y && mouse_y <= current->y + CARD_HEIGHT) {
           
            render_text_with_font(current->name, current->x + CARD_WIDTH / 2, name_y_position - 20, (SDL_Color){255, 255, 255, 255}, font_for_characters);
        }
        current = current->next;
    }

    for (int i = 0; i < NUM_CARDS; i++) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &slots[i]);
    }

    
    SDL_Rect check_button = {WINDOW_WIDTH / 2 - 100, 600, 200, 50};
    render_button(check_button, "Checar Ordem");

    
    render_lives();

    if (check_order_button_pressed) {
        int all_cards_in_slots = 1;
        Card* current = cardList.head;
        while (current) {
            if (current->slot_index == -1) {
                all_cards_in_slots = 0;
                break;
            }
            current = current->next;
        }

        if (!all_cards_in_slots ) {
            int message_y_position = 100;
            render_text("Todas as cartas devem ser inseridas!", WINDOW_WIDTH / 2, message_y_position, (SDL_Color){255, 0, 0, 255});
        }
    }
}

int check_order() {
    check_order_button_pressed = 1;

    int all_cards_in_slots = 1;
    Card* current = cardList.head;
    while (current) {
        if (current->slot_index == -1) {
            all_cards_in_slots = 0;
            break;
        }
        current = current->next;
    }

    if (all_cards_in_slots) {
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

            if (card1 && card2 && card1->birth_year > card2->birth_year) {
                correct_order = 0;
                break;
            }
        }

        if (!correct_order) {
            lives--;
            printf("A ordem está errada. Voce perdeu uma vida!\n");
            if (lives <= 0) {
                printf("Voce perdeu todas as vidas. Fim de jogo!\n");

                
                insertion_sort_cards(&cardList);

                return -1;
            }
        }

        if (correct_order) {
            printf("Parabéns! Voce acertou a ordem das cartas!\n");
            return 1;
        }

        return 0;
    } else {
        return 0;
    }
}

int initialize() {
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) {
        printf("Erro: %s\n", SDL_GetError());
        return 0;
    }

    
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Erro ao inicializar SDL_image: %s\n", IMG_GetError());
        return 0;
    }

   
    window = SDL_CreateWindow("Jogo de Ordenação", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);


    if (!window || !renderer) {
        printf("Erro ao criar janela ou renderizador: %s\n", SDL_GetError());
        return 0;
    }


    font = TTF_OpenFont("fonts/fonte_t.ttf", 32);
    font_for_characters = TTF_OpenFont("fonts/font_t_2.ttf", 18);  


    if (!font || !font_for_characters) {
        printf("Erro ao carregar fontes: %s\n", TTF_GetError());
        return 0;
    }


    heart_texture = load_texture("img/coracao.png");
    if (!heart_texture) {
        printf("Erro ao carregar a imagem de coracao.png\n");
        return 0;
    }

   
    background_texture = load_texture("img/background.png");
    if (!background_texture) {
        printf("Erro ao carregar a imagem de background.png\n");
        return 0;
    }


    background_game_texture = load_texture("img/backgroundJogo.png");
    if (!background_game_texture) {
        printf("Erro ao carregar a imagem de backgroundJogo.png\n");
        return 0;
    }

    return 1; 
}



void insertion_sort_cards(CardList* list) {
    if (!list->head) return;

    Card* sorted = NULL;


    Card* current = list->head;
    while (current) {
        Card* next = current->next;
        current->prev = current->next = NULL;

        
        if (!sorted || current->birth_year < sorted->birth_year) {
            current->next = sorted;
            if (sorted) sorted->prev = current;
            sorted = current;
        } else {
            Card* temp = sorted;
            while (temp->next && temp->next->birth_year < current->birth_year) {
                temp = temp->next;
            }
            current->next = temp->next;
            if (temp->next) temp->next->prev = current;
            temp->next = current;
            current->prev = temp;
        }
        current = next;
    }

   
    list->head = sorted;

    
    list->tail = sorted;
    while (list->tail && list->tail->next) {
        list->tail = list->tail->next;
    }


    Card* card = list->head;
    for (int i = 0; i < NUM_CARDS; i++) {
        if (card) {
            card->x = slots[i].x;
            card->y = slots[i].y;
            card->slot_index = i; 
            card = card->next;
        }
    }
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
    rect.x = (WINDOW_WIDTH - rect.w) / 2;  
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
    render_text(text, rect.x + rect.w / 2, rect.y + rect.h / 2, (SDL_Color){255, 255, 255, 255});
}


void cleanup() {
if (heart_texture) {
    SDL_DestroyTexture(heart_texture);
    heart_texture = NULL;
}

Card* current = cardList.head;
while (current) {
    if (current->texture) {
        SDL_DestroyTexture(current->texture);
        current->texture = NULL;
    }
    Card* temp = current;
    current = current->next;
    free(temp); 
}

cardList.head = NULL;
cardList.tail = NULL;

if (renderer) {
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
}

if (window) {
    SDL_DestroyWindow(window);
    window = NULL;
}
}
