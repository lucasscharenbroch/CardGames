#include "poker.h"

/* console operations */

void setUpConsole(){
	setupStdHandles();
	setVirtualMode();
	hideCursor();
	setConsoleSize(150, 40);

	system("@echo off");
	system("title Poker");

	setColor(WHITE_GREEN);

	system("cls");
}

/* game info */

struct gameInfo game;

/* opponent info */

struct opponentInfo opponents[4];

char opponentNames[20][21] = {"Richard", "Troy", "Nancy", "Drew", "Eric", "Peter", "Pete", "Sam", "Doug", "Sim", "Amy", "Jeffery", "Kyle", "Samantha", "Kevin", "Brian", "Andy", "Molly", "Craig", "Gary"};


int randomComparison(const void *a, const void *b){
	return rand() % 2;	
}

void setupOpponents(){
	/* shuffle opponentNames */ 
	srand(time(NULL));
	qsort(opponentNames, 20, sizeof(char)*21, randomComparison);

	for(int i = 0; i < game.numOpponents; i++){
		strcpy(opponents[i].name, opponentNames[i]);/* randomize this later*/
		opponents[i].chips = game.startChips;
		opponents[i].playing = TRUE;
		opponents[i].folded = FALSE;
		opponents[i].isAllIn = FALSE;
	}	
}

/* player info */

struct playerInfo player;
char *playerName;

void setupPlayer(){
	strcpy(player.name, playerName);
	player.chips = game.startChips;
	player.folded = FALSE;
	player.playing = TRUE;
	player.isAllIn = FALSE;
}



/* game control */

void dealCards(){
	/* init & shuffle deck */
	char **deck = newDeck();
	shuffleDeck(deck);

	/* deal cards */
	int deckPos = 0;

	/* deal player */
	for(int i = 0; i < 2; i++){
		player.hand[i] = deck[deckPos++];
	}

	/* deal opponents */
	for(int o = 0; o < game.numOpponents; o++){
		for(int i = 0; i < 2; i++){
			opponents[o].hand[i] = deck[deckPos++];
		}
	}

	/* deal board */
	for(int i = 0; i < 3; i++){
		game.board[i] = deck[deckPos++];
	}
	game.cardsDealt = 0;
}

void setupNewGame(){
	/* prompt player for game setup */
	printf("Welcome to poker! Press a number key to begin game setup.\n");
	pause();
	playerName = prompt("Enter player name (Max 20 chars):", 20);
	game.numOpponents = prompt("\nNumber of opponents:", 5)[0] - '0'; /* first digit returned */
	game.numOpponents = game.numOpponents >= 1 && game.numOpponents <= 4 ? game.numOpponents : 4; /* default numOpponents to 4 */
	game.startChips = 500;

	/* setup opponents */
	setupOpponents();

	/* setup player */
	setupPlayer();
}

void setupNewRound(){
	/* rest game statistics */
	game.maskOpponentHand = TRUE;
	dealCards();

	/* reset betting flags (folded/ all in) */
	player.folded = FALSE;
	player.isAllIn = FALSE;

	srand(time(NULL));
	for(int i = 0; i < game.numOpponents; i++){
		if(opponents[i].playing){
			opponents[i].folded = FALSE;
			opponents[i].isAllIn = FALSE;
			opponents[i].chipsAtRoundStart = opponents[i].chips;

			/* roll the bluff dice */
			opponents[i].bluffScore = rand() % (BLUFF_DIE); 
			opponents[i].bluffScore += rand() % (BLUFF_DIE); 
		}
	}
}


void payAntes(){
	/* prompt for ante */
	if(player.chips == 0){
		player.playing = FALSE;
		return;
	}

	char memo[1000];
	sprintf(memo, "Pay %d-chip ante? (1/0)", game.ante);

	printMemo(memo);
	int playerChoice = getNumberKeydown();
	if(playerChoice == 0){
		player.playing = FALSE;
		return;
	}else{
		if(player.chips >= game.ante){
			player.chips -= game.ante;
			game.pot += game.ante;
		}else{
			game.pot += player.chips;
			player.chips = 0;
		}
	}

	/* make opponents pay ante */

	for(int o = 0; o < game.numOpponents; o++){
		if(opponents[o].playing){
			if(opponents[o].chips == 0)
				opponents[o].playing = FALSE;
			else if(opponents[o].chips < game.ante){
				game.pot += opponents[o].chips;
				opponents[o].chips = 0;
			}else{/* opponet has sufficient chips */
				game.pot += game.ante;
				opponents[o].chips -= game.ante;	
			}
		}
	}
}

int oneOrMoreOpponentsPlaying(){
	for(int i = 0; i < game.numOpponents; i++){
		if(opponents[i].playing)
			return TRUE;
	}	

	return FALSE;
}

void playRound(){
	/* pay antes */
	payAntes();
	if(!player.playing || !oneOrMoreOpponentsPlaying()){
		return;
	}

	/* round setup */
	setupNewRound();

	/* clear screen */
	system("cls");
	/* print info */
	printGameInfo(TRUE);

	/* first bets */
	bets();

	game.cardsDealt++; /* flop */
	printMemo("The flop.");
	refreshAndPause();

	bets();

	game.cardsDealt++; /* turn */
	printMemo("The turn.");
	refreshAndPause();

	bets();

	game.cardsDealt++; /*river */
	printMemo("The river.");
	refreshAndPause();

	bets();

	/* reveal opponent hands*/
	game.maskOpponentHand = FALSE; /* hands unmasked */
	printGameInfo();

	/* showdown */
	showdown();

	refreshAndPause();
}

void play(){


	setupNewGame();

	game.ante = 2;

	do{
		/* play a round */
		playRound();

		/* up the ante */
		game.ante += 2;

	}while(player.playing && oneOrMoreOpponentsPlaying());

	char memo[1000];
	sprintf(memo, "Game over. You ended with %d chips.", player.chips);
	printMemo(memo);
	

}

/* main */

int main(){
	setUpConsole();
	play();
	getNumberKeydown();



	/* clean up the console */

	system("cls");/* clear screen */
	system("color 07");/* restore system color*/

	return 0;
}
