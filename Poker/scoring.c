#include "poker.h"

#define ROYAL_FLUSH 160 + 14
#define STRAIGHT_FLUSH 160
#define FOUR_OF_A_KIND 140
#define FULL_HOUSE 120
#define FLUSH 100
#define STRAIGHT 80
#define THREE_OF_A_KIND 60
#define TWO_PAIRS 40
#define PAIR 20


int cardValue(char cardType){
	if(cardType >= '2' && cardType <= '9')/* number values */
		return cardType - '0';
	else
		switch(cardType){
			case '0':
				return 10;
				break;
			case 'J':
				return 11;
				break;
			case 'Q':
				return 12;
				break;
			case 'K':
				return 13;
				break;
			case 'A':
				return 14;
				break;
		}
}

int cardCompare(const void *card1, const void *card2){
	char *sCard1 = *(char **)card1;
	char *sCard2 = *(char **)card2;

	return (cardValue(sCard1[0]) > cardValue(sCard2[0]));
}

int findHighestCardValue(char *cards[], int numCards){
	int maxValue = 0, currentValue;
	for(int i = 0; i < numCards; i++){
		currentValue = cardValue(cards[i][0]);
		maxValue = maxValue > currentValue ? maxValue : currentValue;
	}
	return maxValue;
}

int scoreHand(char *hand[2], char *board[3], int cardsDealt){
	int numCards = 2 + cardsDealt;
	char *cards[numCards];
	
	/* copy all cards being scored into cards */
	cards[0] = hand[0];
	cards[1] = hand[1];
	
	for(int i = 0; i < cardsDealt; i++){
		cards[i+2] = board[i];
	}

	/* sort cards by value */
	qsort(cards, numCards, sizeof(char *), cardCompare);


	/* declare possible bools */
	int flush = FALSE;
	int straight = FALSE;
	int maxSame = FALSE;/* highst number of cards of the same type E.G. 2 kings, 3 aces => 3 */
	int fullHouse = FALSE;
	int twoPairs = FALSE;

	/* check for combinations that require all 5 cards (flush and straight)*/
	if(numCards == 5){
		/* check for flush */
		char suit = cards[0][1];
		int i;
		for(i = 0; i < numCards; i++){
			if(cards[i][1] != suit)
				break;	
		}
		if(i == 5)
			flush = TRUE;

		/* check for straight */
		
		int previousValue = cardValue(cards[0][0]);
		int currentValue;
		for(i = 1; i < numCards; i++){
			currentValue = cardValue(cards[i][0]);	
			if(currentValue != previousValue+1){
				break;
			}

			previousValue = currentValue;
		}
		if(i == 5)
			straight = TRUE;
	}

	/* check for pairs/ groups */

	int currentGroup = 0;
	int cardsPerType[15];

	/*initialize cardsPerType to zero */
	for(int i = 0; i < 15; i++){
		cardsPerType[i] = 0;
	}

	/* tally cards per type */
	for(int i = 0; i < numCards; i++){
		cardsPerType[cardValue(cards[i][0])]++;
	}


	int highCardScore = findHighestCardValue(cards, numCards); 
	int scoreOfMaxSame;
	/* search for maxSame, and update highCardScore accordingly */
	for(int i = 2; i < 15; i++){
		if(cardsPerType[i] == 2){
			if(maxSame == 2){
				twoPairs = TRUE;
				highCardScore = (scoreOfMaxSame + i) / 2; /* average of the value of each pair */
			}else if(maxSame == 3){
				fullHouse = TRUE;
				highCardScore = (scoreOfMaxSame + i) / 2; /* average of the value of each group */
			}else{
				/* pair */
				highCardScore = i;
			}
		}else if(cardsPerType[i] == 3){
			if(maxSame == 2){
				fullHouse = TRUE;
				highCardScore = (scoreOfMaxSame + i) / 2; /* average of the value of each group */
			}else{
				/* three of a kind */
				highCardScore = i;
			}	
		}else if(cardsPerType[i] >= 4){
			/* four of a kind */
			highCardScore = i;
		}

		/* update maxSame and scoreOfMaxSame */
		if(cardsPerType[i] > maxSame){
			maxSame = cardsPerType[i];
			scoreOfMaxSame = i;	
		}
	}


	int bonus = 0;

	if(straight && flush)
		bonus = STRAIGHT_FLUSH;
	else if(maxSame == 4)
		bonus = FOUR_OF_A_KIND;
	else if(fullHouse)
		bonus = FULL_HOUSE;
	else if(flush)
		bonus = FLUSH;
	else if(straight)
		bonus = STRAIGHT;
	else if(maxSame == 3)
		bonus = THREE_OF_A_KIND;
	else if(twoPairs)
		bonus = TWO_PAIRS;
	else if(maxSame == 2)
		bonus = PAIR;
	else
		bonus = 0;


	return bonus + highCardScore;
}

char *scoreType(int score){
	if(score == ROYAL_FLUSH){
		return "Royal Flush";
	}else if(score >= STRAIGHT_FLUSH){ 
		return "Straight Flush";
	}else if(score >= FOUR_OF_A_KIND){
		return "Four of a Kind";
	}else if(score >= FULL_HOUSE){
		return "Full House";
	}else if(score >= FLUSH){
		return "Flush";
	}else if(score >= STRAIGHT){
		return "Straight";
	}else if(score >= THREE_OF_A_KIND){
		return "Three of a Kind";
	}else if(score >= TWO_PAIRS){
		return "Two Pairs";
	}else if(score >= PAIR){
		return "Pair";
	}else{
		return "High Card";
	}
}

char *getName(int id){
	char *name = malloc(20 * sizeof(char));

	if(id == 0)
		strcpy(name, player.name);
	else
		strcpy(name, opponents[id-1].name);

	return name;
}

void showdown(){
	/* put all scores into array */
	int scores[1 + game.numOpponents];
	scores[0] = player.folded ? 0 : scoreHand(player.hand, game.board, 3); /* player score (if player didn't fold) */
	int maxScore = scores[0];

	for(int i = 0; i < game.numOpponents; i++){
		scores[i+1] = opponents[i].folded ? 0 : scoreHand(opponents[i].hand, game.board, 3);
		maxScore = maxScore > scores[i+1] ? maxScore : scores[i+1]; 
	}

	/* find winners */
	int winners[1 + game.numOpponents];
	int numWinners = 0;

	for(int i = 0; i < game.numOpponents+1; i++){
		if(scores[i] == maxScore){
			winners[numWinners++] = i;
		}
	}

	/* devide the pot */
	game.pot /= numWinners;
	
	for(int i = 0; i < numWinners; i++){
		if(winners[i] == 0)
			player.chips += game.pot;
		else
			opponents[winners[i]-1].chips += game.pot;
	}


	/* announce */
	char memo[1000];
	char winner[20];
	if(numWinners == 1){
		/* get winner name */
		char *name = getName(winners[0]);
		sprintf(memo, "Showdown!\n%s wins the pot of %d! \n(%s)", name, game.pot, scoreType(maxScore));
	}else{
		char *names[numWinners];
		for(int i = 0; i < numWinners; i++){
			names[i] = getName(winners[i]);
		}

		/* write to memo (use offset to continue writing in multiple sprintf statements) */
		int memoOffset = sprintf(memo, "Showdown!\n%d-way tie: pot of %d is devided equally among\n", numWinners, game.pot * numWinners);

		/*print all names*/
		for(int i = 0; i < numWinners; i++){
			/* name */
			memoOffset += sprintf(memo+memoOffset, "%s", names[i]); 

			/* comma */
			if(i != numWinners-1)
				memoOffset += sprintf(memo+memoOffset, ", ");
		}

		memoOffset += sprintf(memo+memoOffset, "\n(%s)", scoreType(maxScore));
	}
	
	/* reset pot */
	game.pot = 0;

	/* print message */
	printMemo(memo);
}
