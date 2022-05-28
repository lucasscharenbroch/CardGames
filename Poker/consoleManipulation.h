#include <windows.h>
#include <wchar.h>
#include <stdio.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#define TRUE 1
#define FALSE 0

#define KEY_0 0x30

#define WHITE_GREEN L"\x1b[97;42m"
#define BLACK_GREEN L"\x1b[30;42m"
#define RED_GREEN L"\x1b[31;42m"

#define MOVE_CURSOR_BACK L"\x1b[1D"

void setupStdHandles();
int setVirtualMode();
void setColor(const wchar_t * color);
void setPos(int x, int y);
void hideCursor();
INPUT_RECORD getInput();
WORD getKeydown();
int getNumberKeydown();
void setConsoleSize(int columns, int lines);
void pause();
char *prompt(char *message, int maxReplySize);
