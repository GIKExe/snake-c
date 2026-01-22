#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define GRID_SIZE 20

#define WWL WINDOW_WIDTH / GRID_SIZE - 1
#define WHL WINDOW_HEIGHT / GRID_SIZE - 1

typedef struct {
	int x, y;
} Point;

void set_random_pos(Point *pos) {
	pos->x = rand() % (WINDOW_WIDTH / GRID_SIZE);
	pos->y = rand() % (WINDOW_HEIGHT / GRID_SIZE);
}

int main(int argc, char* argv[]) {
	// 0. Инициализация Рандома
	srand(time(NULL));

	// 1. Инициализация SDL3
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	
	// 2. Создание окна и рендерера (флаги изменились)
	SDL_Window* window = SDL_CreateWindow("Змейка", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
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
			if (event.type == SDL_EVENT_QUIT) {
				is_running = false;
			}
			if (event.type == SDL_EVENT_KEY_DOWN) {
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
		
		// Проверка съедения еды (упрощённо)
		if (snake[0].x == food.x && snake[0].y == food.y) {
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
		
		SDL_RenderPresent(renderer);
		SDL_Delay(100);
	}
	
	// 5. Очистка
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
	
	return 0;
}