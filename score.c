#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <curses.h>
#include <unistd.h>
#include <string.h>
#include "yz_consts.h"
#include "yz_types.h"
#include "yz_funcs.h"
#include "yz_macros.h"

static char *GrabName();
static int NCmp();
extern int OptCheat;
extern int OptNonStop;
extern int OptUseColor;
extern int OptFast;

void
DisplayScore(old_win, prompt)
     WINDOW *old_win;
     int prompt;

{
	char *name;
	FILE *fp;
	char msg[YZ_STR_MAX];
	score_t board[HS_ENTRIES];
	int i, count;
	WINDOW *win;
	int ch;

	if ((name = GrabName(old_win, 0)) == 0)
		return;
	if ((fp = fopen(name, "r")) == 0)
	{
		sprintf(msg, "Cannot read %s.", name);
		PopError(old_win, msg);
		return;
	}
	count = fread((char *)board, sizeof(score_t), HS_ENTRIES, fp);
	(void)fclose(fp);
	if (count == 0)
	{
		PopError(old_win, "High score file empty.");
		return;
	}
	win = GrabWindow(old_win, HS_ENTRIES + 6, 25, 2, 10, PAIR6);
	wmove(win, 1, 4);
#ifdef SYS5_3_COLOR
	if (OptUseColor)
		wattron(win, COLOR_PAIR(PAIR5));
#endif
	wprintw(win, "Top %d High Scores", HS_ENTRIES);
#ifdef SYS5_3_COLOR
	if (OptUseColor)
		wattroff(win, COLOR_PAIR(PAIR5));
#endif
	for (i = 0; i < count; ++i)
	{
		wmove(win, i + 3, 2);
		wprintw(win, "%s", board[i].name);
		wmove(win, i + 3, 15);
		wprintw(win, "%5d", board[i].score);
	}
	if (prompt)
		mvwaddstr(win, HS_ENTRIES + 6 - 2, 1, "--press space--");
	else
	{
		wmove(win, HS_ENTRIES + 4, 1);
		wrefresh(win);
	}
	if (prompt)
	{
#ifndef SYS5_3_CURSES
		wrefresh(win);
#endif
		while ((ch = wgetch(win)) != ' ')
			ExtraCheck(win, ch, HLP_HS);
		mvwaddstr(win, HS_ENTRIES + 6 - 2, 1, "               ");
		wrefresh(win);
		DropWindow(old_win, win);
	}
	else if (OptNonStop)
	{
		if (!OptFast)
			(void)sleep(4);
		else
			sleep(0);
		DropWindow(old_win, win);
	}
}

void
WriteScore(win, players, player_count)
     WINDOW *win;
     players_t players[YZ_PLAYERS_MAX];
     int player_count;

{
	char *hs_name;
	FILE *fp;
	int limit;
	int count, i, player;
	char msg[YZ_STR_MAX];
	score_t board[HS_ENTRIES + YZ_PLAYERS_MAX];

	if (OptCheat)
	{
		PopError(win, "No highscore update with cheat option.");
		return;
	}
	if ((hs_name = GrabName(win, 1)) == 0)
		return;
	if ((fp = fopen(hs_name, "r")) == NULL)
	{
		sprintf(msg, "Cannot read %s.", hs_name);
		PopError(win, msg);
		return;
	}
	count = fread((char *)board, sizeof(score_t), HS_ENTRIES, fp);
	(void)fclose(fp);
	limit = count + player_count;
	for (i = count, player = 0; i < limit; ++i, ++player)
	{
		board[i].score = players[player].total;
		strcpy(board[i].name, players[player].name);
	}
	qsort((char *)board, (unsigned)limit, (unsigned)sizeof(score_t), NCmp);
	if ((fp = fopen(hs_name, "w")) == NULL)
	{
		sprintf(msg, "Cannot write %s.", hs_name);
		PopError(win, msg);
		return;
	}
	if (limit > HS_ENTRIES)
		limit = HS_ENTRIES;
	fwrite((char *)board, (unsigned)sizeof(score_t), (unsigned)limit, fp);
	(void)fclose(fp);
}

static int
NCmp(item1, item2)
     score_t *item1;
     score_t *item2;

{
	if (item1->score < item2->score)
		return 1;
	if (item1->score > item2->score)
		return -1;
	return 0;
}

static char *
GrabName(win, create)
     WINDOW *win;
     int create;

{
	FILE *fp;
	char *env_name;
	struct stat stat_buf;
	char err_buf[YZ_STR_MAX];
	static char hs_name[YZ_FN_MAX] = { 0 };
	extern char *getenv();

	env_name = getenv("YAHTZEE_HS");
	if (env_name)
		strcpy(hs_name, env_name);
	else
		strcpy(hs_name, YZ_HS_FILE);
	if (stat(hs_name, &stat_buf) == -1)
	{
		sprintf(err_buf, "Cannot stat %s: %d.", hs_name, errno);
		if (win)
			PopError(win, err_buf);
		else
			fprintf(stderr, "%s\n", err_buf);
		if (create)
		{
			sprintf(err_buf, "Attempting to create: %s.", hs_name);
			PopError(win, err_buf);
			if ((fp = fopen(hs_name, "w")) == NULL)
			{
				sprintf(err_buf, "Cannot create %s.", hs_name);
				PopError(win, err_buf);
				PopError(win, "Check YAHTZEE_HS and yahtzee makefile.");
				return 0;
			}
			(void)fclose(fp);
		}
		else
			return 0;
	}

	return hs_name;
}

void
DumpScore()
{
	char *name;
	FILE *fp;
	score_t board[HS_ENTRIES];
	int i, count;

	if ((name = GrabName((WINDOW *) 0, 0)) == 0)
		return;
	if ((fp = fopen(name, "r")) == 0)
	{
		fprintf(stderr, "Cannot read %s.\n", name);
		return;
	}
	count = fread((char *)board, sizeof(score_t), HS_ENTRIES, fp);
	(void)fclose(fp);
	if (count == 0)
	{
		fprintf(stderr, "High score file empty.\n");
		return;
	}
	fprintf(stdout, "    Yahtzee Top %d High Scores\n\n", HS_ENTRIES);
	for (i = 0; i < count; ++i)
		fprintf(stdout, "\t%s\t %5d\n", board[i].name, board[i].score);
}
