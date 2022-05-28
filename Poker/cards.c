#include "cards.h"

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
