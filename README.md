#windows gcc compile code
gcc src/main.c include/sha256.c -Iinclude -o main.exe

#macos using clang
clang src/main.c include/sha256.c -Iinclude -o main

#and execute the program by using
./main


on macOS replace:
- system("pause") with new method
void pauseConsole(){
    printf("Press any keys to continue...);
    getchar();
}
- system("cls") with system("clear")

-_stricmp() with strcasecmp()
