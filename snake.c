
#include "raylib.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "consolas.c"

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

#define SUPERDARKGRAY  CLITERAL(Color){ 30, 30, 30, 255 }

int screenWidth = 800;
int screenHeight = 600;
int gridSize = 20;
#define WWL screenWidth / gridSize - 1
#define WHL screenHeight / gridSize - 1
Font font;

typedef struct SnakeElement SnakeElement;
typedef struct Food Food;
typedef struct Pos Pos;

struct Pos {
	u8 x, y;
};

struct SnakeElement {
	Pos pos;
	SnakeElement *el;
};

struct Food {
	Pos pos;
	Color color;
};


SnakeElement *new_element(SnakeElement *end_of_snake) {
	SnakeElement *el = malloc(sizeof(SnakeElement));
	el->pos.x = end_of_snake->pos.x;
	el->pos.y = end_of_snake->pos.y;
	el->el = end_of_snake;
	return el;
}


void deathMenu(bool *exitFlag, int *count) {
	const char *msg1 = "Змейка сдохла!";
	const char *msg2 = TextFormat("Ваш счёт: %i", *count);
	const char *msg3 = "Нашмите пробел чтобы продолжить";

	bool running = true;
	while (running) {
		if (WindowShouldClose()) {
			*exitFlag = true;
			running = false;
		}
		if (IsKeyPressed(KEY_SPACE)) {
			running = false;
		}

		Vector2 pos1 = {
			screenWidth/2 - MeasureTextEx(font, msg1, 48, 1).x/2,
			screenHeight/2 - 48
		};

		Vector2 pos2 = {
			screenWidth/2 - MeasureTextEx(font, msg2, 20, 1).x/2,
			screenHeight/2
		};

		Vector2 pos3 = {
			screenWidth/2 - MeasureTextEx(font, msg3, 20, 1).x/2,
			screenHeight/2 + 20
		};

		BeginDrawing();
			ClearBackground(SUPERDARKGRAY);
			DrawTextEx(font, msg1, pos1, 48, 1, RED);
			DrawTextEx(font, msg2, pos2, 20, 1, RED);
			DrawTextEx(font, msg3, pos3, 20, 1, RED);
		EndDrawing();
	}
}


void gameMenu(bool *exitFlag, int *count) {
	(*count) = 1;
	u8 direction = 0; // 0=right, 1=down, 2=left, 3=up
	SnakeElement *snake = malloc(sizeof(SnakeElement));
	snake->pos = (Pos) {10, 10};
	snake->el = NULL;
	SnakeElement *end_of_snake = snake;
	SnakeElement *el;
	SnakeElement *elo;
	bool walls_is_death = false;
	Food food = {(Pos) {5, 5}, RED};

	bool running = true;
	while (running) {
		if (WindowShouldClose()) {
			*exitFlag = true;
			running = false;
		}

		if (IsWindowResized()) {
			screenWidth = GetScreenWidth();
			screenHeight = GetScreenHeight();
		}

		int key = GetKeyPressed();
		switch (key) {
			case KEY_W:
			case KEY_UP:
				if (direction != 1) direction = 3;
				break;
			case KEY_S:
			case KEY_DOWN:
				if (direction != 3) direction = 1;
				break;
			case KEY_A:
			case KEY_LEFT:
				if (direction != 0) direction = 2;
				break;
			case KEY_D:
			case KEY_RIGHT:
				if (direction != 2) direction = 0;
				break;
		}

		// Проверка съедения еды
		if (snake->pos.x == food.pos.x && snake->pos.y == food.pos.y) {
			(*count)++;
			end_of_snake = new_element(end_of_snake);
			walls_is_death = food.color.g == 122; // PURPLE
			if (food.color.g == 203) { 						// GOLD
				end_of_snake = new_element(end_of_snake);
				(*count)++;
			}
			
			while (true) {
				bool flag = true;
				food.pos.x = rand() % (screenWidth / gridSize);
				food.pos.y = rand() % (screenHeight / gridSize);
				el = end_of_snake;
				while (el != NULL) {
					if (food.pos.x == el->pos.x && food.pos.y == el->pos.y) {
						flag = false;
						break;
					}
					el = el->el;
				}
				if (flag) break;
			}
			
			switch (rand() % 100) {
				case 0 ... 9:
					food.color = GOLD;
					break;
				case 10 ... 39:
					food.color = PURPLE;
					break;
				default:
					food.color = RED;
			}
		}
		
		// Логика движения змейки (с конца)
		el = end_of_snake;
		elo = el->el;
		while (elo != NULL) {
			el->pos.x = elo->pos.x;
			el->pos.y = elo->pos.y;
			el = elo;
			elo = el->el;
		}

		// Перемещение головы
		switch(direction) {
			case 0: snake->pos.x++; break; // right
			case 1: snake->pos.y++; break; // down
			case 2: snake->pos.x--; break; // left
			case 3: snake->pos.y--; break; // up
		}

		if (snake->pos.x == 255) {
			if (walls_is_death) running = false; else snake->pos.x = WWL;
		}
		if (snake->pos.x > WWL) {
			if (walls_is_death) running = false; else snake->pos.x = 0;
		}
		if (snake->pos.y == 255) {
			if (walls_is_death) running = false; else snake->pos.y = WHL;
		}
		if (snake->pos.y > WHL) {
			if (walls_is_death) running = false; else snake->pos.y = 0;
		}
		
		// обработка сталкивания 
		el = end_of_snake;
		while (el != NULL && el != snake) {
			if (snake->pos.x == el->pos.x && snake->pos.y == el->pos.y) {
				running = false;
				break;
			}
			el = el->el;
		}

		BeginDrawing();
			ClearBackground(SUPERDARKGRAY);
			DrawRectangle(food.pos.x*gridSize, food.pos.y*gridSize, gridSize, gridSize, food.color);
			el = end_of_snake;
			while (el != NULL) {
				DrawRectangle(el->pos.x*gridSize, el->pos.y*gridSize, gridSize, gridSize, GREEN);
				el = el->el;
			}
			DrawTextEx(font, TextFormat("Счёт: %i", *count), (Vector2) {5, 5}, 16, 1, RED);
		EndDrawing();
	}

	el = snake;
	while (el!=NULL) {
		elo = el->el;
		free(el);
		el = elo;
	}
}


int main(void) {
	// Рандом
	srand(time(NULL));
	
	// Окно
	InitWindow(screenWidth, screenHeight, "snake-c");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(10);

	// Шрифт
	int codePoints[161] = {0x401, 0x451}, i, j;
	for (i=2, j=32; j < 127; i++, j++) codePoints[i] = j;
	for (j=0x410; j < 0x450; i++, j++) codePoints[i] = j;
	font = LoadFontFromMemory(".ttf", consolas_ttf, consolas_ttf_len, 128, codePoints, 161);
	SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

	// Меню
	bool exitFlag = false;
	u32 count = 0;

	while (true) {
		gameMenu(&exitFlag, &count);
		if (exitFlag) break;
		deathMenu(&exitFlag, &count);
		if (exitFlag) break;
	}

	// Выгрузка
	UnloadFont(font);
	CloseWindow();
	return 0;
}