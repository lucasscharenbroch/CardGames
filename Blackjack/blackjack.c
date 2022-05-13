#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define TRUE 1
#define FALSE 0

#define BUST 1
#define BLACKJACK 2
#define CONTINUE 3

char **newDeck(){
	char types[] = "A234567890JQK";
	char suits[] = "SHCD";

	char **deck = calloc(52, sizeof(char *));

	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 13; j++){
			char *card = malloc(3);
			card[0] = types[j]; 
			card[1] = suits[i]; 
			card[2] = '\0';
			deck[j + i*13] = card;
		} 
	}

	return deck;
}

void stringSwap(char **stringArray, int indexA, int indexB){
	char * temp = stringArray[indexA];
	stringArray[indexA] = stringArray[indexB];
	stringArray[indexB] = temp;
}

void shuffleDeck(char **deck){
	srand(time(NULL));
	for(int i = 0; i < 52; i++){
		/* swap deck[i] with another random card */
		int indexToSwap = rand() % 52;
		stringSwap(deck, i, indexToSwap);
	}
}

void dealCards(int numCards, char **deck, int *deckPosP, char **hand, int *handSizeP){
	for(int i = 0; i < numCards; i++)
		hand[(*handSizeP)++] = deck[(*deckPosP)++];
}	

int scoreCard(char cardType){
	if(cardType >= '2' && cardType <= '9')
		return cardType - '0';
	else
		return 10;
}

int scoreHand(char **hand, int handSize){
	int aces = 0;
	int score = 0;
	char type;

	for(int i = 0; i < handSize; i++){
		type = hand[i][0];
		if(type == 'A')
			aces++;
		else 
			score += scoreCard(type);
	}

	score += aces;
	while(aces > 0 && score+10 <= 21)
		aces--, score+=10;

	return score;
}

char prompt(char *promptString){
	printf("%s", promptString);
	int c = getchar();
	if(c == '\n')
		;
	else
		while(getchar() != '\n');

	return c;
}

void printHand(char **hand, int handSize){
	for(int i = 0; i < handSize; i++){
		printf("%s", hand[i]);
		if(i != handSize-1)
			printf(", ");
	}
	printf("\n");
}

void printMaskedHand(int size){
	for(int i = 0; i < size; i++){
		printf("??");	
		if(i != size-1)
			printf(", ");
	}
	printf("\n\n");
}

/* - - - declare vars - - - */

char **deck;
int deckPos;

char **playerHand, **opponentHand;
int pHandSize, oHandSize;
int playerStay, opponentStay;
int move;

/*declare scores*/
int playerScore, opponentScore;

/* - - - - - - - -*/

void printBothRevealedHands(){
	playerScore = scoreHand(playerHand, pHandSize);
	opponentScore = scoreHand(opponentHand, oHandSize);

	printf("Player hand: ");
	printHand(playerHand, pHandSize);
	printf("Player score: %d\n", playerScore);

	printf("Opponent hand: ");
	printHand(opponentHand, oHandSize);
	printf("Opponent score: %d\n", opponentScore);
}


int playerMove(){
	/*prompt for move*/ 
	move = prompt("(H)it or (S)tay?\n");

	if(tolower(move) == 'h'){
		printf("You hit.\n");
		dealCards(1, deck, &deckPos, playerHand, &pHandSize);
	}else{
		printf("You stay.\n");
		playerStay = TRUE;
	}

	/*announce hand and score*/
	printf("Player hand: ");
	printHand(playerHand, pHandSize);
	playerScore = scoreHand(playerHand, pHandSize);
	printf("Player Score: %d\n", playerScore);

	if(playerScore > 21){
		printf("You busted! Better luck next time.\n\n");
		printBothRevealedHands();
		return BUST;
	}else if(playerScore == 21){
		printf("Blackjack! You win!\n\n");
		printBothRevealedHands();
		return BLACKJACK;
	}

	return CONTINUE;
}

int opponentMove(){
	/*prompt for move*/ 
	move = (opponentScore < 17) ? 'h' : 's';

	if(move == 'h'){
		printf("Opponent hits.\n");
		dealCards(1, deck, &deckPos, opponentHand, &oHandSize);
	}else{
		printf("Opponent stays.\n");
		opponentStay = TRUE;
	}

	/*announce hand (masked)*/
	printf("\nOpponent hand: ");
	printMaskedHand(oHandSize);

	if(opponentScore > 21){
		printf("Opponent busted! You Win.\n\n");
		printBothRevealedHands();
		return BUST;
	}else if(opponentScore == 21){
		printf("Opponent got Blackjack! You lose!\n\n");
		printBothRevealedHands();
		return BLACKJACK;
	}

	return CONTINUE;
}


void blackjack(){
	/*get a new deck, shuffle it*/
	deck = newDeck();
	deckPos = 0;
	shuffleDeck(deck);

	/*init player and opponent hands*/
	playerHand = calloc(52, sizeof(char *));
	opponentHand = calloc(52, sizeof(char *));
	pHandSize = 0, oHandSize = 0;
	playerStay = FALSE, opponentStay = FALSE;

	/*init scores*/
	playerScore = 0, opponentScore = 0;

	/*Deal first two cards for player and opponent*/
	dealCards(2, deck, &deckPos, playerHand, &pHandSize);
	dealCards(2, deck, &deckPos, opponentHand, &oHandSize);

	/*print the hands and scores*/
	printf("\nThe game begins. Cards are dealt.\n");
	playerScore = scoreHand(playerHand, pHandSize);
	opponentScore = scoreHand(opponentHand, oHandSize);

	printf("Player hand: ");
	printHand(playerHand, pHandSize);
	printf("Player score: %d\n", playerScore);

	printf("\nOpponent hand: ");
	printMaskedHand(oHandSize);

	/*check for blackjack*/
	if(playerScore == 21 || opponentScore == 21){
		if(playerScore == opponentScore){
			printf("Player and Opponent both got blackjack. You tie.\n");
		}else if(playerScore ==21){
			printf("Blackjack! You win.\n");
		}else{
			printf("Opponent got blackjack! You lose.\n");
		}

		printBothRevealedHands();
		return;
	}


	/*main loop*/
	while(!playerStay || !opponentStay){
		printf("\n - - - - - - - - - - - - - - - - - - - - - - - - - - \n\n"); 

		/* give turns and check return conditions*/

		if(!playerStay)
			if(playerMove() != CONTINUE)
				return;
		

		if(!opponentStay)
			if(opponentMove() != CONTINUE)
				return;

	printBothRevealedHands();/**/
	}

	/*win/loss conditions*/
	if(playerScore == opponentScore){
		printf("You tied, %d to %d\n", playerScore, opponentScore);
	}else if(playerScore > opponentScore){
		printf("You won, %d to %d\n", playerScore, opponentScore);
	}else{
		printf("You lose, %d to %d\n", playerScore, opponentScore);
	}
}



int main(){

	blackjack();

	return 0;
}
