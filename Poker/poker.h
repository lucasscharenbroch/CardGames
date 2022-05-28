#include "consoleManipulation.h"
#include "cards.h"
#include <time.h>

#define END_BETTING 0
#define CONTINUE_BETTING 1

#define CHECK 0 
#define CALL 1
#define RAISE 2
#define ALL_IN 3
#define FOLD 4

#define BLUFF_DIE 25

struct gameInfo{
	int numOpponents;
	int startChips;
	int pot;
	int ante;
	int maskOpponentHand;

	/* betting */
	int minCall;
	int firstBet;
	int firstChecked;

	/* dealing */
	char *board[3];
	int cardsDealt;

};

struct playerInfo{
	char name[21];
	int chips;
	char *hand[2];
	int playing;
	int folded;
	int isAllIn;
};

struct opponentInfo{
	char name[21];
	int chips;
	char *hand[2];
	int playing;
	int folded;
	int isAllIn;
	int chipsAtRoundStart;
	int bluffScore;
};

/* printing.c */
void printCard(int, int, char *);
void printHand(char **cards, int x, int y, int isMasked);
void printPlayerInfo();
void printOpponentInfo();
void printMemoFrameBorders();
void printMemo(char *memo);
void printGameInfo();
void refreshAndPause();

/* betting.c */
int playerBet();
int opponentBet(int oId);
void bets();

/* scoring.c */
int scoreHand(char *hand[2], char *board[3], int cardsDealt);
char *scoreType(int score);
void showdown();

/* globals */
extern struct playerInfo player;
extern struct opponentInfo opponents[];
extern struct gameInfo game;

