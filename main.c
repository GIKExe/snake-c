#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "consola.c"

int window_widht = 800;
int window_height = 600;
#define GRID_SIZE 20

#define WWL window_widht / GRID_SIZE - 1
#define WHL window_height / GRID_SIZE - 1

typedef   signed char i8;
typedef unsigned char u8;
typedef   signed short i16;
typedef unsigned short u16;
typedef   signed int i32;
typedef unsigned int u32;
typedef   signed long i64;
typedef unsigned long u64;
typedef float f32;
typedef double f64;

typedef struct {
	i16 x, y;
} Point;

typedef struct {
	u8 r, g, b;
} Color;

typedef struct {
	u8 x, y;
} Pos;

typedef struct SnakeElement SnakeElement;
typedef struct SnakeElement {
	Pos pos;
	SnakeElement *pred;
} SnakeElement;

typedef struct {
	Pos pos;
	Color *color;
} Apple;

void set_random_pos(Pos *pos) {
	pos->x = rand() % (window_widht / GRID_SIZE);
	pos->y = rand() % (window_height / GRID_SIZE);
}

void draw_text(SDL_Renderer *renderer, TTF_Font *font, char *text, u8 x, u8 y, Color *color, bool w_centered, bool h_centered) {
	SDL_Color _color = {color->r, color->g, color->b, 255};
	SDL_Surface *surface = TTF_RenderText_Blended(font, text, strlen(text), _color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_FRect srcrect = {0, 0, surface->w, surface->h};
	SDL_FRect dstrect = {x, y, surface->w, surface->h};
	if (w_centered) dstrect.x = window_widht / 2 - surface->w / 2 + x;
	if (h_centered) dstrect.y = window_height / 2 - surface->h / 2 + y;

	SDL_RenderTexture(renderer, texture, &srcrect, &dstrect);
	SDL_DestroySurface(surface);
	SDL_DestroyTexture(texture);
}

bool is_collide(Pos *a, Pos *b) {
	return a->x == b->x && a->y == b->y;
}

typedef struct {
	TTF_Font *font16;
	TTF_Font *font20;
	TTF_Font *font48;
	SDL_Window *window;
	SDL_Renderer *renderer;
} Core;

bool exit_flag = false;
u16 count = 0;

// #colors
Color ColorRed = {255, 0, 0};
Color ColorGreen = {0, 255, 0};
Color ColorGold = {255, 182, 79};
Color ColorPurple = {153, 76, 255};

SnakeElement *new_element(SnakeElement *end_of_snake) {
	SnakeElement *element = malloc(sizeof(SnakeElement));
	element->pos.x = end_of_snake->pos.x;
	element->pos.y = end_of_snake->pos.y;
	element->pred = end_of_snake;
	return element;
}

// #deathmenu
void death_menu(Core *core) {
	bool is_running = true;
	SDL_Event event;

	while (is_running) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT:
					is_running = false;
					exit_flag = true;
					break;
				case SDL_EVENT_KEY_DOWN:
					switch (event.key.key) {
						case SDLK_ESCAPE:
							exit_flag = true;
						case SDLK_SPACE:
							is_running = false;
							break;
					}
					break;
			}
		}

		SDL_SetRenderDrawColor(core->renderer, 0, 0, 0, 255);
		SDL_RenderClear(core->renderer);

		char msg1[] = "Змейка сдохла!";
		draw_text(core->renderer, core->font48, (char *) &msg1, 0, 0, &ColorRed, true, true);

		char msg2[23];
		snprintf(msg2, sizeof(msg2), "Ваш счёт: %hd", count);
		draw_text(core->renderer, core->font20, (char *) &msg2, 0, 48, &ColorRed, true, true);

		char msg3[] = "нажмите пробел чтобы начать";
		draw_text(core->renderer, core->font20, (char *) &msg3, 0, 68, &ColorRed, true, true);

		SDL_RenderPresent(core->renderer);
		SDL_Delay(100);
	}
}

// #gamemenu
void game_menu(Core *core) {
	bool is_running = true;
	SDL_Event event;
	
	bool walls_is_death = false;
	SnakeElement *snake = malloc(sizeof(SnakeElement));
	SnakeElement *end_of_snake = snake;
	SnakeElement *element;
	SnakeElement *element_pred;
	snake->pred = NULL;
	set_random_pos(&snake->pos);
	int direction = 0; // 0=right, 1=down, 2=left, 3=up
	Apple food;
	food.color = &ColorRed;
	set_random_pos(&food.pos);
	
	while (is_running) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT:
					exit_flag = true;
					is_running = false;
					break;
				case SDL_EVENT_KEY_DOWN:
					switch(event.key.key) {
						case SDLK_ESCAPE:
							exit_flag = true;
							is_running = false;
							break;
						case SDLK_W:
						case SDLK_UP:
							if (direction != 1) direction = 3;
							break;
						case SDLK_S:
						case SDLK_DOWN:
							if (direction != 3) direction = 1;
							break;
						case SDLK_A:
						case SDLK_LEFT:
							if (direction != 0) direction = 2;
							break;
						case SDLK_D:
						case SDLK_RIGHT:
							if (direction != 2) direction = 0;
							break;
					}
					break;
				case SDL_EVENT_WINDOW_RESIZED:
					window_widht = event.window.data1;
					window_height = event.window.data2;
					break;
			}
		}

		// Проверка съедения еды
		if (is_collide(&snake->pos, &food.pos)) {
			end_of_snake = new_element(end_of_snake);
			walls_is_death = food.color == &ColorPurple;
			if (food.color == &ColorGold) {
				end_of_snake = new_element(end_of_snake);
			}
			
			set_random_pos(&food.pos);
			switch (rand() % 100) {
				case 0 ... 9:
					food.color = &ColorGold;
					break;
				case 10 ... 39:
					food.color = &ColorPurple;
					break;
				default:
					food.color = &ColorRed;
			}
		}
		
		// Логика движения змейки (с конца)
		element = end_of_snake;
		element_pred = element->pred;
		while (element_pred != NULL) {
			element->pos.x = element_pred->pos.x;
			element->pos.y = element_pred->pos.y;
			element = element_pred;
			element_pred = element->pred;
		}

		// Перемещение головы
		switch(direction) {
			case 0: snake->pos.x++; break; // right
			case 1: snake->pos.y++; break; // down
			case 2: snake->pos.x--; break; // left
			case 3: snake->pos.y--; break; // up
		}

		if (snake->pos.x == 255) {
			if (walls_is_death) is_running = false; else snake->pos.x = WWL;
		}
		if (snake->pos.x > WWL) {
			if (walls_is_death) is_running = false; else snake->pos.x = 0;
		}
		if (snake->pos.y == 255) {
			if (walls_is_death) is_running = false; else snake->pos.y = WHL;
		}
		if (snake->pos.y > WHL) {
			if (walls_is_death) is_running = false; else snake->pos.y = 0;
		}
		
		// обработка сталкивания 
		element = end_of_snake;
		while (element != NULL && element != snake) {
			if (is_collide(&snake->pos, &element->pos)) {
				is_running = false;
				break;
			}
			element = element->pred;
		}
		
		// Отрисовка
		SDL_SetRenderDrawColor(core->renderer, 0, 0, 0, 255);
		SDL_RenderClear(core->renderer);

		// Отрисовка еды
		SDL_SetRenderDrawColor(core->renderer, food.color->r, food.color->g, food.color->b, 255);
		SDL_FRect food_rect = {
			food.pos.x * GRID_SIZE,
			food.pos.y * GRID_SIZE,
			GRID_SIZE - 1,
			GRID_SIZE - 1
		};
		SDL_RenderFillRect(core->renderer, &food_rect);
		
		// Отрисовка змейки
		SDL_SetRenderDrawColor(core->renderer, 0, 255, 0, 255);
		element = end_of_snake;
		while (element != NULL) {
			SDL_FRect rect = {
				element->pos.x * GRID_SIZE,
				element->pos.y * GRID_SIZE,
				GRID_SIZE - 1,
				GRID_SIZE - 1
			};
			SDL_RenderFillRect(core->renderer, &rect);
			element = element->pred;
		}

		char msg1[25];
		snprintf(msg1, sizeof(msg1), "Позиция: %hhd, %hhd", snake->pos.x, snake->pos.y);
		draw_text(core->renderer, core->font16, (char *) &msg1, 5, 5, &ColorRed, false, false);

		count = 0;
		element = end_of_snake;
		while (element != NULL) {
			count++;
			element = element->pred;
		}

		char msg2[16];
		snprintf(msg2, sizeof(msg2), "Счёт: %hd", count);
		draw_text(core->renderer, core->font16, (char *) &msg2, 5, 21, &ColorRed, false, false);

		SDL_RenderPresent(core->renderer);
		SDL_Delay(100);
	}

	// освобождение памяти
	element = end_of_snake;
	while (element != NULL) {
		element_pred = element->pred;
		free(element);
		element = element_pred;
	}
}

int main(int argc, char* argv[]) {
	// Инициализация Рандома
	srand(time(NULL));

	// Инициализация SDL3
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();

	// Сбор данных для расшаривания
	Core core;
	SDL_IOStream *io = SDL_IOFromConstMem(consola_ttf, sizeof(consola_ttf));
	core.font16 = TTF_OpenFontIO(io, false, 16);
	core.font20 = TTF_OpenFontIO(io, false, 20);
	core.font48 = TTF_OpenFontIO(io, false, 48);
	
	// Создание окна и рендерера
	core.window = SDL_CreateWindow("Змейка", window_widht, window_height, SDL_WINDOW_RESIZABLE);
	core.renderer = SDL_CreateRenderer(core.window, NULL);

	while (true) {
		game_menu(&core);
		if (exit_flag) break;
		death_menu(&core);
		if (exit_flag) break;
	}
	
	// Очистка
	SDL_DestroyRenderer(core.renderer);
	SDL_DestroyWindow(core.window);
	TTF_CloseFont(core.font16);
	TTF_CloseFont(core.font20);
	TTF_CloseFont(core.font48);
	SDL_CloseIO(io);
	TTF_Quit();
	SDL_Quit();
	
	return 0;
}