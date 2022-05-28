#include "consoleManipulation.h"

int color_enabled = 1;

/*declare HANDLEs*/
HANDLE hStdout, hStdin;

void setupStdHandles(){
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
}

int setVirtualMode(){
	DWORD dwMode = 0;
	if (!GetConsoleMode(hStdout, &dwMode)){
		return 1;
	}
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

	SetConsoleMode(hStdout, dwMode);

	return 0;
}


void setColor(const wchar_t * color){
	if(color_enabled)
		wprintf(color);
}

void setPos(int x, int y){
	COORD pos = {x, y};

	SetConsoleCursorPosition(hStdout, pos);
}

void hideCursor(){
	wprintf(L"\x1b[?25l");
}



INPUT_RECORD getInput(){
	INPUT_RECORD consoleEvent[2];
	DWORD numRead = 0;


	while(!numRead)
		ReadConsoleInput(hStdin, consoleEvent, 1, &numRead);

	return consoleEvent[0];
}

WORD getKeydown(){
	INPUT_RECORD consoleEvent;
	KEY_EVENT_RECORD keyEvent;
	do{
		consoleEvent = getInput();	
		keyEvent = consoleEvent.Event.KeyEvent;


	}while(!keyEvent.bKeyDown);

	return keyEvent.wVirtualKeyCode;
}

int getNumberKeydown(){
	while(TRUE){/* wait for number */
	WORD key = getKeydown();	
	if(key >= KEY_0 && key <= KEY_0+9)/* number key */
		return key-KEY_0;
	}
}

char *getString(int maxSize){
	char *str = malloc(maxSize+1 * sizeof(char));
	int stringSize = 0;
	WORD key;
	while((key = getKeydown()) != VK_RETURN){
		if(key == VK_BACK){
			if(stringSize){
				stringSize--;
				/*move cursor back one, print a space, and move cursor back again*/
				wprintf(MOVE_CURSOR_BACK);
				printf(" ");
				wprintf(MOVE_CURSOR_BACK);
			}
		}else{
		str[stringSize++] = key;
		printf("%c", key);
		if(stringSize == maxSize)
			break;
		}
	}
	str[stringSize] = '\0';/* truncate */

	return str;
}

char *prompt(char *message, int maxReplySize){
	printf("%s", message);
	char *str;
        do{
		str = getString(maxReplySize);
	}while(str[0] == '\0'); /* keep getting the string until it has a character in it */

	return str;
}

void setConsoleSize(int columns, int lines){
	char command[100];
	sprintf(command, "mode con: cols=%d lines=%d", columns, lines);
	system(command);
}

void pause(){
	getNumberKeydown();	
}
