
# Простая змейка на языке C
для сборки на windows используйте (-mwindows для отключения консоли):
```sh
gcc -o snake.exe snake.c -I raylib/include -L raylib/lib -l raylib -l opengl32 -l gdi32 -l winmm
```