#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int window_widht = 800;
int window_height = 600;
#define GRID_SIZE 20

#define WWL window_widht / GRID_SIZE - 1
#define WHL window_height / GRID_SIZE - 1

#define FONT_PATH "ttf/consola.ttf"

typedef unsigned char u8;

typedef struct {
	int x, y;
} Point;

void set_random_pos(Point *pos) {
	pos->x = rand() % (window_widht / GRID_SIZE);
	pos->y = rand() % (window_height / GRID_SIZE);
}

void draw_text(SDL_Renderer *renderer, TTF_Font *font, char *text, u8 x, u8 y, u8 r, u8 g, u8 b) {
	SDL_Color color = {r, g, b, 255};
	SDL_Surface *surface = TTF_RenderText_Blended(font, text, strlen(text), color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

	SDL_FRect srcrect = {0, 0, surface->w, surface->h};
	SDL_FRect dstrect = {x, y, surface->w, surface->h};

	SDL_RenderTexture(renderer, texture, &srcrect, &dstrect);
	SDL_DestroySurface(surface);
	SDL_DestroyTexture(texture);
}

int main(int argc, char* argv[]) {
	// 0. Инициализация Рандома
	srand(time(NULL));

	// 1. Инициализация SDL3
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	TTF_Font *font = TTF_OpenFont(FONT_PATH, 16);
	if (font == NULL) {
		printf("Не удалось загрузить шрифт: ", FONT_PATH);
		exit(0);
	}
	
	// 2. Создание окна и рендерера
	SDL_Window* window = SDL_CreateWindow("Змейка", window_widht, window_height, SDL_WINDOW_RESIZABLE);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
	
	// 3. Основные переменные
	bool is_running = true;
	SDL_Event event;
	
	// Пример: инициализация змейки
	Point snake[100];
	set_random_pos(&snake[0]);
	int snake_length = 1;
	int direction = 0; // 0=right, 1=down, 2=left, 3=up
	Point food;
	set_random_pos(&food);
	
	while (is_running) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_QUIT:
					is_running = false;
					break;
				case SDL_EVENT_KEY_DOWN:
					switch(event.key.key) {
						case SDLK_ESCAPE:
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
		
		// Логика движения змейки
		for (int i = snake_length - 1; i > 0; i--) {
			snake[i] = snake[i-1];
		}
		
		// Перемещение головы
		switch(direction) {
			case 0: snake[0].x++; break; // right
			case 1: snake[0].y++; break; // down
			case 2: snake[0].x--; break; // left
			case 3: snake[0].y--; break; // up
		}

		if (snake[0].x > WWL) snake[0].x = 0;
		if (snake[0].x < 0) snake[0].x = WWL;
		if (snake[0].y > WHL) snake[0].y = 0;
		if (snake[0].y < 0) snake[0].y = WHL;
		
		// Проверка съедения еды
		if (snake[0].x == food.x && snake[0].y == food.y) {
			snake[snake_length] = snake[snake_length - 1];
			snake_length++;
			set_random_pos(&food);
		}
		
		// Отрисовка
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		
		// Отрисовка змейки
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		for (int i = 0; i < snake_length; i++) {
			SDL_FRect rect = {
				snake[i].x * GRID_SIZE,
				snake[i].y * GRID_SIZE,
				GRID_SIZE,
				GRID_SIZE
			};
			SDL_RenderFillRect(renderer, &rect);
		}
		
		// Отрисовка еды
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_FRect food_rect = {
			food.x * GRID_SIZE,
			food.y * GRID_SIZE,
			GRID_SIZE - 1,
			GRID_SIZE - 1
		};
		SDL_RenderFillRect(renderer, &food_rect);

		char result[29];
		snprintf(result, sizeof(result), "Позиция: %hd, %hd", snake[0].x, snake[0].y);
		draw_text(renderer, font, (char *) &result, 5, 5, 255, 0, 0);

		SDL_RenderPresent(renderer);
		SDL_Delay(100);
	}
	
	// 5. Очистка
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();
	
	return 0;
}