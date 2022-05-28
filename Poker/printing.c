#include "poker.h"

/* printing */

void printCard(int x, int y, char *name){
	setPos(x,y);
	printf("+------+");
	setPos(x,y+1);
	printf("|      |");
	setPos(x,y+2);
	printf("|  ");

	if(name[1] == 'S' || name[1] == 'C')
		setColor(BLACK_GREEN);
	else if(name[1] == 'H' || name[1] == 'D')
		setColor(RED_GREEN);


	printf("%s", name);
	setColor(WHITE_GREEN);
	printf("  |");
	setPos(x,y+3);
	printf("|      |");
	setPos(x,y+4);
	printf("+------+");
}

void printHand(char **cards, int x, int y, int isMasked){
	printCard(x, y, isMasked ? "??" : cards[0]);
	printCard(x+10, y, isMasked ? "??" : cards[1]);
}


void printPlayerInfo(){
	COORD playerFrame = {0, 32};

	setPos(playerFrame.X, playerFrame.Y-1);
	printf("%s's Hand:", player.name);
	
	setPos(playerFrame.X, playerFrame.Y);
	printf("Chips: %5d", player.chips);

	if(player.folded){
		setPos(playerFrame.X, playerFrame.Y+1);
		printf("(Folded)");
	}

	printHand(player.hand, playerFrame.X, playerFrame.Y+2, FALSE);
}

void printOpponentInfo(){
	/* hands */
	COORD opponentFrames[4] = {{0,0}, {40, 0}, {80, 0}, {120, 0}};

	for(int i = 0; i < game.numOpponents; i++){
		if(!opponents[i].playing)
			continue;

		/* hand */
		printHand(opponents[i].hand, opponentFrames[i].X, opponentFrames[i].Y, game.maskOpponentHand);

		/* name */
		setPos(opponentFrames[i].X, opponentFrames[i].Y+6);
		printf("%s", opponents[i].name);

		/* chips */
		setPos(opponentFrames[i].X, opponentFrames[i].Y+7);
		printf("Chips: %5d", opponents[i].chips);

		/* fold status */
		if(opponents[i].folded){
			setPos(opponentFrames[i].X, opponentFrames[i].Y+8);
			printf("(Folded)");
		}

	}
	
}

void printBoard(){
	COORD boardFrame = {50, 15};
	for(int i = 0; i < game.cardsDealt; i++){
		printCard(boardFrame.X + i*15, boardFrame.Y, game.board[i]);
	}
}


void printMemoFrameBorders(){
	COORD memoFrame = {30, 25};
	int frameLength = 80;
	int frameHeight = 10;
	int x, y;

	/* top */
	y = memoFrame.Y-2;
	for(x = memoFrame.X-1; x <= memoFrame.X+frameLength+1; x++)
		setPos(x, y), printf("_");

	/* bottom */
	y = memoFrame.Y+frameHeight+1;
	for(x = memoFrame.X-1; x <= memoFrame.X+frameLength+1; x++)
		setPos(x, y), printf("_");

	/* left */
	x = memoFrame.X-1;
	for(y = memoFrame.Y-1; y <= memoFrame.Y+frameHeight+1; y++)
		setPos(x,y), printf("|");

	/* right */
	x = memoFrame.X+frameLength+1;
	for(y = memoFrame.Y-1; y <= memoFrame.Y+frameHeight+1; y++)
		setPos(x,y), printf("|");
}

void printMemo(char *memo){
	COORD memoFrame = {30, 25};
	int frameLength = 80;
	int frameHeight = 10;

	/* clear frame */
	for(int x = memoFrame.X; x <= memoFrame.X + frameLength; x++)
		for(int y = memoFrame.Y; y <= memoFrame.Y + frameHeight; y++)
			setPos(x, y), printf(" ");


	/* write borders */
	printMemoFrameBorders();

	/* write memo */

	int xPos = memoFrame.X;
	int yPos = memoFrame.Y;

	for(int c = 0; memo[c] != '\0'; c++){
		setPos(xPos, yPos);
		printf("%c", memo[c]);
		xPos++;

		if(xPos == memoFrame.X + frameLength || memo[c] == '\n'){
			xPos = memoFrame.X;
			yPos++;
		}
	}
}

void printPot(int pot){
	COORD potFrame = {130, 30};

	setPos(potFrame.X, potFrame.Y);
	printf("POT: %5d", pot);
}

void printGameInfo(){
	printOpponentInfo();
	printPlayerInfo();
	printBoard();
	printPot(game.pot);
}

void refreshAndPause(){
	printGameInfo();
	pause();
}
