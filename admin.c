#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "yz_consts.h"
#include "yz_types.h"
#include "yz_macros.h"
#include "yz_funcs.h"

extern int OptLearn;
extern int OptCheat;
extern int OptFast;
extern int OptUseColor;

static void
Outline(WINDOW *win, players_t players[YZ_PLAYERS_MAX], int player, int player_count)
{
	int last;
	char *name;

	last = player - 1;
	if (last < 0)
		last = player_count - 1;
	name = players[last].name;
	mvwaddstr(win, YZ_TOP - 2, YZ_PLAYER_COLS * last + YZ_PLAYER_PAD - strlen(name), name);
	name = players[player].name;
#ifdef SYS5_3_COLOR
	if (OptUseColor)
		wattron(win, COLOR_PAIR(PAIR5));
	else
		wstandout(win);
#else
	wstandout(win);
#endif
	mvwaddstr(win, YZ_TOP - 2, YZ_PLAYER_COLS * player + YZ_PLAYER_PAD - strlen(name), name);
#ifdef SYS5_3_COLOR
	if (OptUseColor)
		wattroff(win, COLOR_PAIR(PAIR5));
	else
		wstandend(win);
#else
	wstandend(win);
#endif
}

int
DetermineWinner(WINDOW *win, players_t players[YZ_PLAYERS_MAX], int player_count)
{
	int best_total = 0;
	int best_player = 0;
	int player;
	char *name;
	int last;

	for (player = 0; player < player_count; ++player)
		if (players[player].total > best_total)
		{
			best_total = players[player].total;
			best_player = player;
		}
	last = player_count - 1;
	name = players[last].name;
	mvwaddstr(win, YZ_TOP - 2, YZ_PLAYER_COLS * last + YZ_PLAYER_PAD - strlen(name), name);
	name = players[best_player].name;
	wstandout(win);
#ifdef SYS5_3_CURSES
	wattron(win, A_BLINK | COLOR_PAIR(PAIR2));
#endif
	mvwaddstr(win, YZ_TOP - 2, YZ_PLAYER_COLS * best_player + YZ_PLAYER_PAD - strlen(name), name);
	wstandend(win);
#ifdef SYS5_3_CURSES
	wattroff(win, A_BLINK | COLOR_PAIR(PAIR2));
#endif
	wrefresh(win);

	return best_player;
}

static int
HumanHold(WINDOW *win, players_t players[YZ_PLAYERS_MAX], dice_t dice[YZ_DICE], int player)
{
	int ch;
	int die = 0;
	int i, held = 0;

#ifndef SYS5_3_CURSES
	wrefresh(win);
#endif
	while ((ch = wgetch(win)) != YZ_CR)
		switch (ch)
		{
		case 'h':
			--die;
			if (die < 0)
				die = YZ_DICE - 1;
			DieCentre(win, die);
			break;
		case 'l':
			die = (die + 1) % YZ_DICE;
			DieCentre(win, die);
			break;
		case ' ':
			dice[die].hold = !dice[die].hold;
			DisplayDie(win, dice, die);
			die = (die + 1) % YZ_DICE;
			DieCentre(win, die);
			break;
		case 'r':
			(void)ComputerHold(players, dice, player);
			DisplayDice(win, dice);
			DieCentre(win, die);
			break;
		case 'a':
			for (i = 0; i < YZ_DICE; ++i)
				dice[i].hold = 1;
			DisplayDice(win, dice);
			DieCentre(win, die);
			break;
		case 't':
			for (i = 0; i < YZ_DICE; ++i)
				dice[i].hold = !dice[i].hold;
			DisplayDice(win, dice);
			DieCentre(win, die);
			break;
		case 'o':
			DSTRUCT_SORT(dice);
			DisplayDice(win, dice);
			DisplayDie(win, dice, die);
			break;
		default:
			if (ch >= '1' && ch < '1' + YZ_DICE)
			{
				die = ch - '1';
				dice[die].hold = !dice[die].hold;
				DisplayDie(win, dice, die);
			}
			else
				ExtraCheck(win, ch, HLP_HOLD);
			break;
		}
	for (i = 0; i < YZ_DICE; ++i)
		if (dice[i].hold)
			++held;

	return held;
}

static int
SetDice(WINDOW *old_win, players_t players[YZ_PLAYERS_MAX], dice_t dice[YZ_DICE], int player)
{
	WINDOW *win;
	int i, bogus;

	echo();
	do
	{
		win = GrabWindow(old_win, 3, 30, 12, 20, 0);
		mvwaddstr(win, 1, 1, "Five dice: ");
		wrefresh(win);
		wscanw(win, "%d %d %d %d %d", &dice[0].dice, &dice[1].dice, &dice[2].dice, &dice[3].dice, &dice[4].dice);
		bogus = 0;
		for (i = 0; i < YZ_DICE; ++i)
			if (--dice[i].dice < 0 || dice[i].dice >= YZ_DICE_MAX)
			{
				bogus = 1;
				flash();
			}
		DropWindow(old_win, win);
	}
	while (bogus);
	noecho();
	for (i = 0; i < YZ_DICE; ++i)
		dice[i].hold = 1;

	return YZ_DICE;
}

static int
GetHold(WINDOW *win, players_t players[YZ_PLAYERS_MAX], dice_t dice[YZ_DICE], int player)
{
	int held;

	DisplayDice(win, dice);
	if (players[player].computer)
	{
		held = ComputerHold(players, dice, player);
		DisplayDice(win, dice);
	}
	else if (!OptCheat)
		held = HumanHold(win, players, dice, player);
	else
		held = SetDice(win, players, dice, player);

	wrefresh(win);
	if (!OptLearn && players[player].computer && !OptFast)
		(void)sleep((unsigned int)0);

	return held;
}

static int
HumanSelect(WINDOW *win, players_t players[YZ_PLAYERS_MAX], dice_t dice[YZ_DICE], int player)
{
	int ch;
	int cat = 0;

	while (players[player].used[cat])
		cat = (cat + 1) % YZ_CAT_COUNT;
	do
	{
		wmove(win, YZ_TOP - 1 + cat, sizeof(YZ_LG_STRAIGHT));
#ifndef SYS5_3_CURSES
		wrefresh(win);
#endif
		switch (ch = wgetch(win))
		{
		case 'j':
			do
			{
				cat = (cat + 1) % YZ_CAT_COUNT;
			}
			while (players[player].used[cat]);
			break;
		case 'k':
			do
			{
				if (--cat < 0)
					cat = YZ_CAT_COUNT - 1;
			}
			while (players[player].used[cat]);
			break;
		case 'r':
			cat = ComputerSelect(win, players, dice, player);
			break;
		default:
			if (ch != YZ_CR)
				ExtraCheck(win, ch, HLP_SELECT);
			break;
		}
	}
	while (ch != YZ_CR);

	return cat;
}

static void
CalcScore(WINDOW *win, int cat, players_t players[YZ_PLAYERS_MAX], dice_t dice[YZ_DICE], int player)
{
	int yz_bonus = 0;
	int bonus63 = 0;
	players_t *player_p;
	int col;
	int display;

	player_p = &players[player];
	display = player_p->computer && !OptLearn;
	col = YZ_PLAYER_COLS * player + YZ_PLAYER_PAD;
	Eval(cat, dice, &player_p->score[cat], &yz_bonus, &bonus63, players, player);
	player_p->used[cat] = 1;
	player_p->total += player_p->score[cat];
	if (display)
		wstandout(win);
	PutNumber(win, cat + YZ_TOP - 1, col, player_p->score[cat]);
	if (display)
		wstandend(win);
	if (cat <= YZ_CAT63)
	{
		player_p->total63 += player_p->score[cat];
		PutNumber(win, YZ_63TOTAL_ROW, col, player_p->total63);
		if (bonus63)
		{
			player_p->bonus63 = bonus63;
			player_p->total += bonus63;
			PutNumber(win, YZ_63BONUS_ROW, col, bonus63);
		}
	}
	if (yz_bonus)
	{
		player_p->bonus += yz_bonus;
		player_p->total += yz_bonus;
		PutNumber(win, YZ_BONUS_ROW, col, player_p->bonus);
	}
	PutNumber(win, YZ_TOTAL_ROW, col, player_p->total);
	if (display)
	{
		DieCentre(win, 0);
		if (!OptFast)
			(void)sleep((unsigned int)0);
		PutNumber(win, cat + YZ_TOP - 1, col, player_p->score[cat]);
		wrefresh(win);
	}
	else
		wrefresh(win);
}

static void
SelectCategory(WINDOW *win, players_t players[YZ_PLAYERS_MAX], dice_t dice[YZ_DICE], int player)
{
	int cat;

	DisplayDice(win, dice);
	if (players[player].computer || (!players[player].computer && OptCheat))
		cat = ComputerSelect(win, players, dice, player);
	else
		cat = HumanSelect(win, players, dice, player);
	CalcScore(win, cat, players, dice, player);
}

void
PlayGame(WINDOW *win, players_t players[YZ_PLAYERS_MAX], dice_t dice[YZ_DICE], int player_count)
{
	int cat;
	int player;
	int held;

	for (cat = 0; cat < YZ_CAT_COUNT; ++cat)
		for (player = 0; player < player_count; ++player)
		{
			Outline(win, players, player, player_count);
			held = GetHold(win, players, dice, player);
			if (held < YZ_DICE && !OptCheat)
			{
				ROLL_DICE(dice);
				held = GetHold(win, players, dice, player);
				if (held < YZ_DICE)
					ROLL_DICE(dice);
			}
			CLEAR_HOLD(dice);
			DisplayDice(win, dice);
			SelectCategory(win, players, dice, player);
			ROLL_DICE(dice);
			flushinp();
		}
}
