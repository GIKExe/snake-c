
# Простая змейка на языке C
для сборки приложения используйте:
```sh
gcc main.c -o main.exe -I SDL3/include -L SDL3/lib -l SDL3 -l SDL3_ttf 
```

или с флагом для собрки без консоли:
```sh
gcc main.c -o main.exe -I SDL3/include -L SDL3/lib -l SDL3 -l SDL3_ttf -mwindows
```

динамические библиотеки:
- [SDL](https://github.com/libsdl-org/SDL/releases/tag/release-3.4.0)
- [SDL_ttf](https://github.com/libsdl-org/SDL_ttf/releases/tag/release-3.2.2)
