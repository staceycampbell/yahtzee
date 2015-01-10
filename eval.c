#include <curses.h>
#include <stdlib.h>
#include "yz_consts.h"
#include "yz_types.h"
#include "yz_macros.h"
#include "yz_funcs.h"

static int StraightScore[] = { SC_SMALL_STRAIGHT, SC_LARGE_STRAIGHT };

static void
EvOne2Six(dice_t dice[YZ_DICE], int *score, int cat)
{
	int i;
	int bump;

	bump = cat + 1;
	for (i = 0; i < YZ_DICE; ++i)
		if (dice[i].dice == cat)
			*score += bump;
}

static void
EvChance(dice_t dice[YZ_DICE], int *score, int cat)
{
	int i;

	for (i = 0; i < YZ_DICE; ++i)
		*score += dice[i].dice + 1;
}

static void
EvOaK(dice_t dice[YZ_DICE], int *score, int cat)
{
	int i;
	int reps[YZ_DICE_MAX];

	cat -= SC_OAK_NORMAL;
	for (i = 0; i < YZ_DICE_MAX; ++i)
		reps[i] = 0;
	for (i = 0; i < YZ_DICE; ++i)
		if (++reps[dice[i].dice] >= cat)
		{
			EvChance(dice, score, cat);
			return;
		}
}

static void
EvFullHouse(dice_t dice[YZ_DICE], int *score, int cat)
{
	int new_dice[YZ_DICE];
	int i;

	for (i = 0; i < YZ_DICE; ++i)
		new_dice[i] = dice[i].dice;
	DICE_SORT(new_dice);
	if ((new_dice[0] == new_dice[1] && new_dice[1] == new_dice[2] &&
	     new_dice[3] == new_dice[4]) || (new_dice[2] == new_dice[3] && new_dice[3] == new_dice[4] && new_dice[0] == new_dice[1]))
		*score = SC_FULL_HOUSE;
}

static void
EvStraight(dice_t dice[YZ_DICE], int *score, int cat)
{
	int new_dice[YZ_DICE];
	int i, j, run, conseq;
	int next;

	cat -= SC_STRAIGHT_NORMAL;
	for (i = 0; i < YZ_DICE; ++i)
		new_dice[i] = dice[i].dice;
	DICE_SORT(new_dice);
	for (i = 0; i < YZ_DICE - 1; i = j)
	{
		j = i;
		run = 0;
		do
		{
			next = j + 1;
			if ((conseq = new_dice[j] + 1 == new_dice[next]) != 0)
				++run;
			if (!conseq)
				conseq = new_dice[j] == new_dice[next];
			++j;
		}
		while (conseq && j < YZ_DICE - 1);
		if (run >= cat)
		{
			*score = StraightScore[cat - SC_STR_SCORE_NORM];
			return;
		}
	}
}

static void
EvYahtzee(dice_t dice[YZ_DICE], int *score, int cat)
{
	EvOaK(dice, score, YZ_DICE + SC_OAK_NORMAL);
	if (*score)
		*score = SC_YAHTZEE;
}

void
Eval(int cat, dice_t dice[YZ_DICE], int *score, int *bonus, int *bonus63, players_t players[YZ_PLAYERS_MAX], int player)
{
	int yz_score = 0;
	int total63;
	static void (*eval_funs[]) () = {EvOne2Six, EvOne2Six, EvOne2Six, EvOne2Six, EvOne2Six, EvOne2Six, EvOaK, EvOaK, EvFullHouse, EvStraight, EvStraight, EvYahtzee, EvChance};

	*bonus63 = *bonus = *score = 0;
	if (cat != YZ_CAT && players[player].score[YZ_CAT])
	{
		EvYahtzee(dice, &yz_score, cat);
		if (yz_score == SC_YAHTZEE)
		{
			*bonus = SC_YAHTZEE_BONUS;
			if (cat == YZ_LG_CAT || cat == YZ_SM_CAT)
			{
				cat -= SC_STR_SCORE_NORM + SC_STRAIGHT_NORMAL;
				*score = StraightScore[cat];
				return;
			}
		}
	}
	(*eval_funs[cat])(dice, score, cat);
	if (cat <= YZ_CAT63 && !players[player].bonus63)
	{
		total63 = players[player].total63 + *score;
		if (total63 >= YZ_63BONUS_LIMIT)
			*bonus63 = YZ_63BONUS_VAL;
	}
}
