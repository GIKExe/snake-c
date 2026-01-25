
# Простая змейка на языке C
для сборки на windows используйте (-mwindows для отключения консоли):
```sh
gcc -o snake.exe snake.c -I raylib-5.5_win64_mingw-w64/include -L raylib-5.5_win64_mingw-w64/lib -l raylib -l opengl32 -l gdi32 -l winmm
```