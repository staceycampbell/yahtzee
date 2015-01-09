#include <curses.h>
#include <stdlib.h>
#include "yz_consts.h"
#include "yz_types.h"
#include "yz_funcs.h"
#include "yz_macros.h"

#define RULE_WIDTH 57
#define RULE_LENGTH (sizeof(Rules) / sizeof(Rules[0]))
#define RULE_WINDOW_LENGTH 18
#define RULE_WINDOW_WIDTH (RULE_WIDTH + 2)
#define TEXT_LENGTH (RULE_WINDOW_LENGTH - 2)
#define WINDOW_COUNT ((RULE_LENGTH / TEXT_LENGTH) + 1)

extern int OptUseColor;

static char *Rules[] = {
	"Rules of Yahtzee",
	"",
	"Object:",
	" The object of Yahtzee is to make the highest",
	" score of all players in each game.  This is",
	" achieved by rolling dice and placing the",
	" dice in the category likely to result in the",
	" highest possible score.",
	"",
	"Rolling the Dice:",
	" The dice are rolled three times per turn for",
	" each player.  Between rolls the player is able",
	" to hold dice so that those dice will not",
	" participate in the next roll.",
	"",
	"Selection of Best Category:",
	" When the dice roll has been completed the player",
	" must select a category for the final set of",
	" dice. The rules regarding scoring for each",
	" category are as follows:",
	"",
	" - one, two, ..., six: dice matching the",
	"     number of the category are summed, this",
	"     sum is the score (e.g. dice values of",
	"     1 4 4 5 6 will score 8 in the 'four'",
	"     category; 1 in the 'one' category etc.).",
	" - three/four of a kind: if at least three/four",
	"     of the dice are of one kind then the score",
	"     will be the summation of all dice, otherwise",
	"     the score is zero",
	" - full house: a full house is scored when there",
	"     exists a pair of dice AND a triplet of dice",
	"     (e.g. dice values 4 4 4 1 1 represent a full",
	"     house; 5 of a kind is also a full house).",
	" - small straight: a small straight is scored when",
	"     at least four dice have consecutive values",
	"     (e.g. 1 2 3 4 2 is a small straight).",
	" - large straight: a large straight is scored when",
	"     the five dice have consecutive values.",
	" - Yahtzee: a Yahtzee is scored when all dice have",
	"     the same value",
	" - chance: the score for chance is the summation",
	"     of all dice, it is impossible to score less",
	"     than 5 for chance.",
	"",
	" If the dice are placed in a category where they do",
	" not satisfy the requirements of that category, they",
	" will score zero. The player must select a category",
	" in every turn.",
	"",
	"63 Bonus:",
	"",
	" If the total score in categories one to six is greater",
	" than or equal to 63 then a bonus of 35 is awarded.",
	"",
	"Yahtzee Bonus:",
	"",
	" If a player scores 50 in the Yahtzee category, then all",
	" subsequent Yahtzees will result in a bonus of 100.",
	" Bonus Yahtzees will score 30 and 40 if they are placed",
	" in the small or large straight categories respectively;",
	" otherwise the bonus Yahtzee will score normally for the",
	" selected category.",
	"",
	"End of Game:",
	"",
	" When all players have completed their score cards",
	" the totals of their scores are calculated and the",
	" player with the highest score wins."
};


void
Rools(win)
     WINDOW *win;

{
	int i, j, y_move_increment, x_move_increment, y = 0, x = 0, text_counter = 0, window_up, window_demanded, tmp_y, tmp_x;
	WINDOW *rules_window[WINDOW_COUNT];
	char ch;

	getyx(win, tmp_y, tmp_x);
	wmove(win, 0, 0);
	wrefresh(win);
	x_move_increment = (COLS - RULE_WINDOW_WIDTH) / WINDOW_COUNT;
	y_move_increment = (LINES - RULE_WINDOW_LENGTH) / WINDOW_COUNT;

	for (i = 0; i < WINDOW_COUNT; ++i)
	{
		rules_window[i] = newwin(RULE_WINDOW_LENGTH, RULE_WINDOW_WIDTH, y, x);
		for (j = 1; j < TEXT_LENGTH && text_counter < RULE_LENGTH; ++j)
		{
			mvwaddstr(rules_window[i], j, 1, Rules[text_counter]);
			++text_counter;
		}
#ifdef SYS5_3_COLOR
		if (OptUseColor)
			wattron(rules_window[i], COLOR_PAIR(PAIR6));
		else
			wattron(rules_window[i], A_DIM);
#else
		wattron(rules_window[i], A_DIM);
#endif
		box(rules_window[i], 0, 0);
#ifdef SYS5_3_COLOR
		if (OptUseColor)
			wattroff(rules_window[i], COLOR_PAIR(PAIR6));
		else
			wattroff(rules_window[i], A_DIM);
#else
		wattroff(rules_window[i], A_DIM);
#endif
		y += y_move_increment;
		x += x_move_increment;
	}
	window_up = -1;
	window_demanded = 0;
	do
	{
		if (window_demanded != window_up)
		{
			touchwin(rules_window[window_demanded]);
			wrefresh(rules_window[window_demanded]);
			window_up = window_demanded;
		}
#ifdef SYS5_3_COLOR
		if (OptUseColor)
			wattron(rules_window[window_up], COLOR_PAIR(PAIR3));
#endif
		mvwaddstr(rules_window[window_up], RULE_WINDOW_LENGTH - 2, 1, "--(m)ore, (g)ame, (b)ack or ?--");
#ifdef SYS5_3_COLOR
		if (OptUseColor)
			wattroff(rules_window[window_up], COLOR_PAIR(PAIR3));
#endif
#ifndef SYS5_3_CURSES
		wrefresh(rules_window[window_up]);
#endif
		ch = wgetch(rules_window[window_up]);
		mvwaddstr(rules_window[window_up], RULE_WINDOW_LENGTH - 2, 1, "                               ");
		wmove(rules_window[window_up], RULE_WINDOW_LENGTH - 2, 1);
		wrefresh(rules_window[window_up]);
		switch (ch)
		{
		case 'm':
		case ' ':
			window_demanded = (window_demanded + 1) % WINDOW_COUNT;
			break;
		case 'b':
			--window_demanded;
			if (window_demanded < 0)
				window_demanded = WINDOW_COUNT - 1;
			break;
		case 'g':
			break;
		default:
			ExtraCheck(rules_window[window_up], ch, HLP_ROOLS);
			break;
		}
	}
	while (ch != 'g');
	for (i = 1; i < WINDOW_COUNT; ++i)
		delwin(rules_window[i]);
	touchwin(win);
	wmove(win, tmp_y, tmp_x);
	wrefresh(win);
}
