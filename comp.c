#include <curses.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include "yz_consts.h"
#include "yz_types.h"
#include "yz_macros.h"
#include "yz_funcs.h"
#include "yz_costs.h"

extern int OptCheat;
static float PCostList[YZ_PLAYERS_MAX][YZ_CAT_COUNT][SC_YAHTZEE + 1];
static int LastWinner = -1;

static void
HoldOne2Six(int cat, dice_t dice[YZ_DICE])
{
	int i;

	for (i = 0; i < YZ_DICE; ++i)
		if (dice[i].dice == cat)
			dice[i].hold = 1;
}

static void
HoldOaK(int cat, dice_t dice[YZ_DICE])
{
	int i;
	int reps[YZ_DICE_MAX];
	int best_die[YZ_DICE_MAX];
	int best_i = 0;
	int new_dice[YZ_DICE];

	for (i = 0; i < YZ_DICE_MAX; ++i)
		reps[i] = best_die[i] = 0;
	for (i = 0; i < YZ_DICE; ++i)
		new_dice[i] = dice[i].dice;
	RDICE_SORT(new_dice);
	for (i = 0; i < YZ_DICE; ++i)
		if (++reps[new_dice[i]] > best_die[best_i])
		{
			best_i = new_dice[i];
			best_die[best_i] = reps[new_dice[i]];
		}
	HoldOne2Six(best_i, dice);
}

static void
HoldFullHouse(int cat, dice_t dice[YZ_DICE])
{
	int i;
	int reps[YZ_DICE_MAX];

	for (i = 0; i < YZ_DICE_MAX; ++i)
		reps[i] = 0;
	for (i = 0; i < YZ_DICE; ++i)
		++reps[dice[i].dice];
	for (i = 0; i < YZ_DICE; ++i)
		dice[i].hold = reps[dice[i].dice] > 1;
}

static void
HoldStraight(int cat, dice_t dice[YZ_DICE])
{
	int i, j;
	int tmp;
	int best_run = 0, run;
	int start;
	int gap, sub_gap;
	int new_dice[YZ_DICE];
	int gone[YZ_DICE_MAX];
	int old_positions[YZ_DICE];
	int hold[YZ_DICE], best_hold[YZ_DICE];

	gap = cat - SC_STRAIGHT_NORMAL;
	sub_gap = gap + 1;
	for (i = 0; i < YZ_DICE; ++i)
	{
		new_dice[i] = dice[i].dice;
		old_positions[i] = i;
	}
	for (i = 0; i < YZ_DICE; ++i)
		for (j = i; j < YZ_DICE; ++j)
			if (new_dice[i] < new_dice[j])
			{
				SWAP(new_dice[i], new_dice[j], tmp);
				SWAP(old_positions[i], old_positions[j], tmp);
			}
	for (i = 0; i + gap < YZ_DICE; ++i)
	{
		for (j = 0; j < YZ_DICE_MAX; ++j)
			gone[j] = 0;
		for (j = 0; j < YZ_DICE; ++j)
			hold[j] = 0;
		run = 0;
		start = new_dice[i];
		for (j = i; j < YZ_DICE && start - new_dice[j] < sub_gap; ++j)
			if (!gone[new_dice[j]])
			{
				gone[new_dice[j]] = 1;
				hold[old_positions[j]] = 1;
				++run;
			}
		if (run > best_run)
		{
			best_run = run;
			for (j = 0; j < YZ_DICE; ++j)
				best_hold[j] = hold[j];
		}
	}
	for (i = 0; i < YZ_DICE; ++i)
		dice[i].hold = best_hold[i];
}

static void
HoldYahtzee(int cat, dice_t dice[YZ_DICE])
{
	HoldOaK(cat, dice);
}

static void
HoldChance(int cat, dice_t dice[YZ_DICE])
{
	int i;

	for (i = 0; i < YZ_DICE; ++i)
		if (dice[i].dice > YZ_DICE_MAX / 2)
			dice[i].hold = 1;
}

void
InitCosts(void)
{
	int i, j, p;

	for (i = 0; i < YZ_CAT_COUNT; ++i)
		for (j = 0; j < SC_YAHTZEE + 1; ++j)
			for (p = 0; p < YZ_PLAYERS_MAX; ++p)
				PCostList[p][i][j] = CostList[i][j];
}

void
AdjustCosts(int winner, int player_count)
{
	int player, i, j;
	float var;
	extern double drand48();
	extern long lrand48();
	extern double sqrt();

	for (player = 0; player < player_count; ++player)
		if (player != winner)
			for (i = 0; i < YZ_CAT_COUNT; ++i)
				for (j = 0; j < SC_YAHTZEE + 1; ++j)
				{
					var = sqrt(Que * (j + 0.1) * drand48());
					if (lrand48() & 1)
						var = -var;
					PCostList[player][i][j] = PCostList[winner][i][j] + var;
				}
	LastWinner = winner;
	Que = Que * 0.9999;
	DumpCosts();
}

void
DumpCosts(void)
{
	int i, j;
	int wrap_5;
	FILE *fp;

	if (LastWinner == -1)
		return;
	(void)unlink("oldtmp.h");
	(void)link("tmp.h", "oldtmp.h");
	(void)unlink("tmp.h");
	if ((fp = fopen("tmp.h", "w")) == NULL)
	{
		fprintf(stderr, "Cannot open cost file\n");
		return;
	}
	fprintf(fp, "static float Que = %f;\n\n", Que);
	fprintf(fp, "static float CostList[YZ_CAT_COUNT][SC_YAHTZEE + 1] = {{\n\t");
	wrap_5 = 0;
	for (i = 0; i < YZ_CAT_COUNT; ++i)
	{
		for (j = 0; j < SC_YAHTZEE + 1; ++j)
		{
			fprintf(fp, "%f", PCostList[LastWinner][i][j]);
			if (wrap_5 < 4)
			{
				if (j != SC_YAHTZEE)
				{
					fprintf(fp, ", ");
					++wrap_5;
				}
			}
			else
			{
				wrap_5 = 0;
				fprintf(fp, ",\n\t");
			}
		}
		if (i != YZ_CAT_COUNT - 1)
			fprintf(fp, "},\n /* %.3d */ {", i);
	}
	fprintf(fp, "}};\n");
	fclose(fp);
}

static void
FindHold(int cat, dice_t dice[YZ_DICE])
{
	static void (*hold_funs[]) () =
	{
	HoldOne2Six, HoldOne2Six,
			HoldOne2Six, HoldOne2Six, HoldOne2Six, HoldOne2Six, HoldOaK, HoldOaK, HoldFullHouse, HoldStraight, HoldStraight, HoldYahtzee, HoldChance};

	CLEAR_HOLD(dice);
	(*hold_funs[cat]) (cat, dice);
}

int
ComputerHold(players_t players[YZ_PLAYERS_MAX], dice_t dice[YZ_DICE], int player)
{
	int cat;
	int i;
	int score, bonus, bonus63;
	float huer, best_huer = -1000.00;
	dice_t new_dice[YZ_DICE];
	int held = 0;

	for (cat = 0; cat < YZ_CAT_COUNT; ++cat)
		if (!players[player].used[cat])
		{
			for (i = 0; i < YZ_DICE; ++i)
				new_dice[i] = dice[i];
			FindHold(cat, new_dice);
			huer = 0.0;
			for (i = 0; i < HUER_SAMPLE_SIZE; ++i)
			{
				ROLL_DICE(new_dice);
				Eval(cat, new_dice, &score, &bonus, &bonus63, players, player);
				huer += score + PCostList[player][cat][score] + bonus + bonus63;
			}
			if (huer > best_huer)
			{
				best_huer = huer;
				held = 0;
				for (i = 0; i < YZ_DICE; ++i)
					if ((dice[i].hold = new_dice[i].hold) != 0)
						++held;
			}
		}

	return held;
}

int
ComputerSelect(WINDOW *old_win, players_t players[YZ_PLAYERS_MAX], dice_t dice[YZ_DICE], int player)
{
	WINDOW *win = 0;
	int cheat, col;
	int cat, best_cat = 0;
	int score, bonus, bonus63;
	float huer, best_huer = -1000.0;

	cheat = OptCheat && !players[player].computer;
	if (cheat)
	{
		col = YZ_PLAYER_COLS * player + YZ_PLAYER_PAD;
		win = GrabWindow(old_win, YZ_CAT_COUNT + 2, 15, YZ_TOP - 2, col, 0);
	}
	for (cat = 0; cat < YZ_CAT_COUNT; ++cat)
		if (!players[player].used[cat])
		{
			Eval(cat, dice, &score, &bonus, &bonus63, players, player);
			huer = score + PCostList[player][cat][score] + bonus + bonus63;
			if (cheat)
			{
				wmove(win, cat + 1, 1);
				wprintw(win, "%f", huer);
			}
			if (huer > best_huer)
			{
				best_huer = huer;
				best_cat = cat;
			}
		}
	if (cheat)
	{
		while (wgetch(win) != ' ');
		DropWindow(old_win, win);
	}

	return best_cat;
}
