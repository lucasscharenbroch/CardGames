#include "poker.h"


int areAllPlayersAllIn(){
	if(player.playing && !player.isAllIn)
		return FALSE;
	for(int i = 0; i < game.numOpponents; i++){
		if(opponents[i].playing && !opponents[i].isAllIn)
			return FALSE;
	}

	return TRUE;
}

int numPlayersNotFolded(){
	int num = 0;
	if(player.playing && !player.folded)
		num++;
	for(int i = 0; i < game.numOpponents; i++){
		if(opponents[i].playing && !opponents[i].folded)
			num++;
	}

	return num;
}

int playerBet(){
	/* update game.firstBet and game.firstChecked */
	if(game.firstBet == 0)
		game.firstBet = -1;	
	if(game.firstChecked == 0)
		return END_BETTING;

	/* prompt for choice */
	char memo[1000];

	sprintf(memo, "Choose one: \n %s1.) Call : %d\n 2.) Raise\n 3.) All in : %d\n 4.) Fold", (game.firstBet == -1) ? "0.) Check\n " : "", game.minCall, player.chips);	
	printMemo(memo);

	int playerChoice = getNumberKeydown(); 
	int betAmount; 

	if(playerChoice == CHECK && (game.firstBet == -1)){
		betAmount = 0;
		printMemo("You check.");

		if(game.firstChecked == -1)
			game.firstChecked = 0;
	}else if(playerChoice == CALL && player.chips >= game.minCall){
		sprintf(memo, "You call (%d chips).\n", game.minCall);
		printMemo(memo);
		
		betAmount = game.minCall;

	}else if(playerChoice == RAISE && player.chips >= game.minCall * 2){
		/* get bet from player, check that it's valid*/
		betAmount = game.minCall * 2;/* raise */

		sprintf(memo, "You raise (%d chips).\n", betAmount);
		printMemo(memo);
	}else if(playerChoice == ALL_IN){
		sprintf(memo, "All in! (%d chips).\n", player.chips);
		printMemo(memo);

		betAmount = player.chips;

		player.isAllIn = TRUE;
	}else if(playerChoice == FOLD){
		betAmount = 0;
		printMemo("You fold.");
		player.folded = TRUE;

	}else{
		/* invalid entry, ask again */
		return playerBet();
	}

	/* check for all in */
	if(player.chips == betAmount)
		player.isAllIn = TRUE;

	/* make chip transaction */
	player.chips -= betAmount;
	game.pot += betAmount;

	/* clear game.firstChecked if bet is made*/
	if(betAmount > 0)
		game.firstChecked = -1;

	/* update first bet */
	if(betAmount > 0 && game.firstBet == -1)
		game.firstBet = 0;

	/* if raised : set first and min bet*/
	if(betAmount > game.minCall){
	       game.firstBet = 0;	
	       game.minCall = betAmount;
	}

	return CONTINUE_BETTING;
}

int getOpponentChoice(int oId){
	const float maxPossibleScore = 174.0f; /*royal flush */
	const float currentScore = (float) scoreHand(opponents[oId].hand, game.board, game.cardsDealt);
	int chips = opponents[oId].chips;

	int lowestTolerableScore = opponents[oId].chipsAtRoundStart * (1 - currentScore/maxPossibleScore); /* fraction of chipsAtRoundStart, based on score */
	int bluffScore = opponents[oId].bluffScore * game.cardsDealt;
	lowestTolerableScore -= bluffScore;

	int raiseMargin = 5 * game.minCall * 2; /* the amount (over lowestTolerableScore) of chips that the opponent needs before they will raise */
	int callMargin = 5 * game.minCall;

	int canCheck = (game.firstBet == -1);
	int canCall = chips > game.minCall; 
	int canRaise = chips > game.minCall * 2; 

	if(lowestTolerableScore < 50 || (lowestTolerableScore < 100 && game.minCall >= chips))
		return ALL_IN;
	if((chips - raiseMargin > lowestTolerableScore) && canRaise)
		return RAISE;
	if(canCheck)
		return CHECK;
	if((chips - game.minCall > lowestTolerableScore) && canCall)
		return CALL;
	return FOLD;

}

int opponentBet(int oId){
	/* update game.firstBet and game.firstChecked */
	if(game.firstBet == oId + 1){
		setPos(5,5);
		game.firstBet = -1;	
	}if(game.firstChecked == oId + 1)
		return END_BETTING;

	/* get opponent choice */
	int opponentChoice = getOpponentChoice(oId);
	int betAmount;


	/* make chip transcation, announce choice */
	char memo[1000];
	switch(opponentChoice){
		case CHECK:
			betAmount = 0;
			sprintf(memo, "%s checks.", opponents[oId].name);	

			if(game.firstChecked == -1)
				game.firstChecked = oId + 1;
			break;
		case CALL:
			betAmount = game.minCall;

			sprintf(memo, "%s calls (%d chips).", opponents[oId].name, betAmount);	
			break;
		case RAISE:
			betAmount = 2 * game.minCall;/* raise */

			sprintf(memo, "%s raises (%d chips).", opponents[oId].name, betAmount);	
			break;
		case ALL_IN:
			betAmount = opponents[oId].chips; 
			opponents[oId].isAllIn = TRUE;

			sprintf(memo, "%s is all in. (%d chips).", opponents[oId].name, betAmount);	
			break;
		case FOLD:
			opponents[oId].folded = TRUE;
			betAmount = 0;

			sprintf(memo, "%s folds.", opponents[oId].name);	
			break;
	}
	/* print memo */
	printMemo(memo);

	/* make transaction */
	game.pot += betAmount;
	opponents[oId].chips -= betAmount;	

	/* clear game.firstChecked if bet is made */
	if(betAmount > 0)
		game.firstChecked = -1;

	/* update first bet*/
	if(betAmount > 0 && game.firstBet == -1){
		game.firstBet = oId + 1;
	}

	/* update first bet and min bet*/
	if(betAmount > game.minCall){
		game.firstBet = oId+1;
		game.minCall = betAmount;
	}

	return CONTINUE_BETTING;
}

void bets(){
	game.minCall = 2;
	game.firstBet = -1;
	game.firstChecked = -1;

	while(!areAllPlayersAllIn() && numPlayersNotFolded() > 1){
		/* player */
		if(!player.folded && !player.isAllIn){
			/* ask player to bet */
			if(playerBet() == END_BETTING){
				return;
			}else{
				refreshAndPause();
			}
		}else{
			if(game.firstBet == 0){
				game.firstBet = -1;
			}
		}


		/* opponents */
		for(int i = 0; i < game.numOpponents; i++){
			/* skip if folded */
			if(opponents[i].folded || opponents[i].isAllIn){
				if(game.firstBet == i+1){
					game.firstBet = -1;
				}
				continue;
			}

			/* bet */
			if(opponentBet(i) == END_BETTING){
				return;
			}else{
				refreshAndPause();
			}
		}
	}
}
