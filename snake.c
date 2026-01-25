
#include "raylib.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


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
Font font;

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
			(*count)++;
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


int main(void) {
	// Рандом
	srand(time(NULL));
	
	// Окно
	InitWindow(screenWidth, screenHeight, "snake-c");
	SetTargetFPS(60);

	// Шрифт
	int codePoints[161] = {0x401, 0x451}, i, j;
	for (i=2, j=32; j < 127; i++, j++) codePoints[i] = j;
	for (j=0x410; j < 0x450; i++, j++) codePoints[i] = j;
	font = LoadFontEx("consolas.ttf", 128, codePoints, 161);
	SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

	// Меню
	bool exitFlag = false;
	u32 count = 0;

	while (true) {
		deathMenu(&exitFlag, &count);
		if (exitFlag) break;
	}

	// Выгрузка
	UnloadFont(font);
	CloseWindow();
	return 0;
}