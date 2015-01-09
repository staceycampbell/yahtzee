#include <signal.h>
#include <stdlib.h>
#include <curses.h>
#include <getopt.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "yz_consts.h"
#include "yz_types.h"
#include "yz_macros.h"
#include "yz_funcs.h"

static void InitSignals();
static void Cleanup();
static void DumpCore();
static int InitGame();
static int GetPlayers();
static int Continue();
static void Randomise();
static void DrawBoard();
static void SidesAndPlayers();
static void ProcessArgs();
static WINDOW *InitScreen();

static char DieStr[YZ_DICE_MAX][YZ_DICE_Y][YZ_DICE_X] = {
	{"       ", "   O   ", "       "},
	{" O     ", "       ", "     O "},
	{" O     ", "   O   ", "     O "},
	{" O   O ", "       ", " O   O "},
	{" O   O ", "   O   ", " O   O "},
	{" O   O ", " O   O ", " O   O "}
};

int OptLearn = 0;
static int OptNoDim = 0;
int OptCheat = 0;
int OptFast = 0;
int OptNonStop = 0;
int OptUseColor = 1;

int
main(argc, argv)
     int argc;
     char *argv[];

{
	int player_count;
	int winner;
	players_t players[YZ_PLAYERS_MAX];
	dice_t dice[YZ_DICE];
	WINDOW *win;

	ProcessArgs(argc, argv);
	win = InitScreen();
	InitSignals();
	InitCosts();
	Randomise((unsigned long)time((long *)0));
	do
	{
		player_count = InitGame(win, players, dice);
		PlayGame(win, players, dice, player_count);
		winner = DetermineWinner(win, players, player_count);
		if (OptLearn)
			AdjustCosts(winner, player_count);
		else
			WriteScore(win, players, player_count);
	}
	while (Continue(win));
	DisplayScore(win, 0);
#ifdef BSD_CURSES
	wmove(win, LINES - 1, 0);
	wrefresh(win);
	nocrmode();
	echo();
#endif
	endwin();

	return 0;
}

static WINDOW *
InitScreen()
{
	initscr();
	cbreak();
	noecho();
#ifndef BSD_CURSES
	OptNoDim = !tparm(enter_dim_mode);
#ifdef SYS5_3_COLOR
	if (OptUseColor)
	{
		if (start_color() != OK)
			OptUseColor = 0;
		else if (has_colors())
		{
			init_pair(PAIR1, COLOR_MAGENTA, COLOR_BLACK);
			init_pair(PAIR2, COLOR_RED, COLOR_WHITE);
			init_pair(PAIR3, COLOR_BLUE, COLOR_BLACK);
			init_pair(PAIR4, COLOR_YELLOW, COLOR_BLACK);
			init_pair(PAIR5, COLOR_RED, COLOR_BLACK);
			init_pair(PAIR6, COLOR_GREEN, COLOR_BLACK);
		}
		else
			OptUseColor = 0;
	}
#endif
#endif
	return newwin(0, 0, 0, 0);
}

static void
ProcessArgs(argc, argv)
     int argc;
     char *argv[];

{
	int opt;
	char *prog;

	prog = argv[0];
	while ((opt = getopt(argc, argv, "snldchfC")) != EOF)
		switch (opt)
		{
		case 's':
			DumpScore();
			exit(0);
			break;
		case 'l':
			OptLearn = 1;
			fprintf(stderr, "learning...");
			break;
		case 'd':
			OptNoDim = 1;
			break;
		case 'c':
			OptCheat = 1;
			break;
		case 'f':
			OptFast = 1;
			break;
		case 'n':
			OptNonStop = 1;
			break;
		case 'C':
			OptUseColor = 0;
			break;
		case 'h':
		case '?':
			fprintf(stderr, "usage: %s -nshfd\n", prog);
			fprintf(stderr, "\t-n\t = non-stop\n");
			fprintf(stderr, "\t-s\t = high score file\n");
			fprintf(stderr, "\t-h\t = display these options\n");
			fprintf(stderr, "\t-f\t = fast computer play\n");
			fprintf(stderr, "\t-d\t = don't use dim mode\n");
			exit(1);
			break;
		}
}

static int
Continue(old_win)
     WINDOW *old_win;

{
	WINDOW *win;
	int ch;
	int yes, no;

	if (OptNonStop)
		return 1;
	win = GrabWindow(old_win, 3, 24, 2, YZ_DICE * (YZ_DICE_X + YZ_DICE_PAD) - 1, PAIR1);
	mvwaddstr(win, 1, 1, "Continue (y) or (n): ");
#ifndef SYS5_3_CURSES
	wrefresh(win);
#endif
	overwrite(win, old_win);
	do
	{
		if (!OptLearn)
			ch = wgetch(win);
		else
			ch = 'y';
		yes = ch == 'Y' || ch == 'y' || ch == ' ';
		if (!yes && ch != 'n' && ch != 'N')
		{
			no = 0;
			ExtraCheck(win, ch, HLP_CONTINUE);
			touchwin(old_win);
			wrefresh(old_win);
			touchwin(win);
			wrefresh(win);
		}
		else
			no = 1;
	}
	while (!no && !yes);
	DropWindow(old_win, win);

	return yes;
}

static int
InitGame(win, players, dice)
     WINDOW *win;
     players_t players[YZ_PLAYERS_MAX];
     dice_t dice[YZ_DICE];

{
	int i, j;
	int player_count;

	player_count = GetPlayers(win, players);
	for (i = 0; i < player_count; ++i)
	{
		players[i].bonus = 0;
		players[i].bonus63 = 0;
		players[i].total63 = 0;
		players[i].total = 0;
		for (j = 0; j < YZ_CAT_COUNT; ++j)
		{
			players[i].score[j] = 0;
			players[i].used[j] = 0;
		}
	}
	CLEAR_HOLD(dice);
	ROLL_DICE(dice);
	DrawBoard(win, players, dice, player_count);

	return player_count;
}

static void
DrawBoard(win, players, dice, player_count)
     WINDOW *win;
     players_t players[YZ_PLAYERS_MAX];
     dice_t dice[YZ_DICE];
     int player_count;

{
	int i, j, die;
	int y, x;

	werase(win);
	SidesAndPlayers(win, players, player_count);
#ifdef SYS5_3_COLOR
	if (OptUseColor)
		wattron(win, COLOR_PAIR(PAIR6));
#endif
	for (die = 0; die < YZ_DICE; ++die)
	{
		y = 0;
		x = die * (YZ_DICE_X + YZ_DICE_PAD);
		for (i = 1; i < YZ_DICE_Y; ++i)
		{
			mvwaddch(win, y + i, x, ACS_VLINE);
			mvwaddch(win, y + i, x + YZ_DICE_X, ACS_VLINE);
		}
		for (j = 1; j < YZ_DICE_X; ++j)
		{
			mvwaddch(win, y, x + j, ACS_HLINE);
			mvwaddch(win, y + YZ_DICE_Y, j + x, ACS_HLINE);
		}
		mvwaddch(win, y, x, ACS_ULCORNER);
		mvwaddch(win, y + YZ_DICE_Y, x, ACS_LLCORNER);
		mvwaddch(win, y + YZ_DICE_Y, x + YZ_DICE_X, ACS_LRCORNER);
		mvwaddch(win, y, x + YZ_DICE_X, ACS_URCORNER);
	}
#ifdef SYS5_3_COLOR
	if (OptUseColor)
		wattroff(win, COLOR_PAIR(PAIR6));
#endif
	DisplayDice(win, dice);
}

void
DisplayDie(win, dice, die)
     WINDOW *win;
     dice_t dice[YZ_DICE];
     int die;

{
	int y, x;
	int i;

	x = die * (YZ_DICE_X + YZ_DICE_PAD) + 1;
	y = 1;
	if (dice[die].hold)
		DICEATTRON(win, OptNoDim);
	for (i = 0; i < YZ_DICE_Y; ++i)
		mvwaddstr(win, y + i, x, DieStr[dice[die].dice][i]);
	if (dice[die].hold)
		DICEATTROFF(win, OptNoDim);
	DieCentre(win, die);
}

void
DieCentre(win, die)
     WINDOW *win;
     int die;

{
	wmove(win, YZ_CEN_Y, die * (YZ_DICE_X + YZ_DICE_PAD) + YZ_CEN_X);
	wrefresh(win);
}

static void
SidesAndPlayers(win, players, player_count)
     WINDOW *win;
     players_t players[YZ_PLAYERS_MAX];
     int player_count;

{
	int i;
	char *name;
	static char *cat_names[YZ_CAT_COUNT] = {
		"one", "two", "three", "four", "five", "six",
		"3 of a kind", "4 of a kind", "full house",
		"small straight", YZ_LG_STRAIGHT, "yahtzee", "chance"
	};

	for (i = 0; i < player_count; ++i)
	{
		name = players[i].name;
		mvwaddstr(win, YZ_TOP - 2, YZ_PLAYER_COLS * i + YZ_PLAYER_PAD - strlen(name), name);
	}
#ifdef SYS5_3_COLOR
	if (OptUseColor)
		wattron(win, COLOR_PAIR(PAIR4));
#endif
	for (i = 0; i < YZ_CAT_COUNT; ++i)
		mvwaddstr(win, i + YZ_TOP - 1, YZ_SIDE(cat_names[i]), cat_names[i]);
	wattron(win, A_UNDERLINE);
	mvwaddstr(win, YZ_63TOTAL_ROW, YZ_SIDE(YZ_63), YZ_63);
	mvwaddstr(win, YZ_63BONUS_ROW, YZ_SIDE(YZ_63BONUS), YZ_63BONUS);
	mvwaddstr(win, YZ_BONUS_ROW, YZ_SIDE(YZ_BSTR), YZ_BSTR);
	mvwaddstr(win, YZ_TOTAL_ROW, YZ_SIDE(YZ_TOTSTR), YZ_TOTSTR);
	wattroff(win, A_UNDERLINE);
	wmove(win, 1, YZ_DICE * (YZ_DICE_X + YZ_DICE_PAD));
	waddstr(win, YZ_BANNER);
	wmove(win, 3, YZ_DICE * (YZ_DICE_X + YZ_DICE_PAD) + 2);
#ifndef BSD_CURSES
	if (!OptNoDim)
		wattron(win, A_DIM);
#endif
#ifdef SYS5_3_COLOR
	if (OptUseColor)
	{
		wattroff(win, COLOR_PAIR(PAIR4));
		wattron(win, COLOR_PAIR(PAIR2));
	}
#endif
	waddstr(win, "uunet!sco!staceyc");
#ifdef SYS5_3_COLOR
	if (OptUseColor)
		wattroff(win, COLOR_PAIR(PAIR2));
#endif
#ifndef BSD_CURSES
	if (!OptNoDim)
		wattroff(win, A_DIM);
#endif
}

void
DisplayDice(win, dice)
     WINDOW *win;
     dice_t dice[YZ_DICE];

{
	int i, die;
	int y, x;

	for (die = 0; die < YZ_DICE; ++die)
	{
		x = die * (YZ_DICE_X + YZ_DICE_PAD) + 1;
		y = 1;
		if (dice[die].hold)
			DICEATTRON(win, OptNoDim);
		for (i = 0; i < YZ_DICE_Y; ++i)
			mvwaddstr(win, y + i, x, DieStr[dice[die].dice][i]);
		if (dice[die].hold)
			DICEATTROFF(win, OptNoDim);
	}
	DieCentre(win, 0);
}

static int
GetPlayers(old_win, players)
     WINDOW *old_win;
     players_t players[YZ_PLAYERS_MAX];

{
	int i;
	int ch;
	int player;
	WINDOW *win;
	static int player_count = 0;
	static int name_change[YZ_PLAYERS_MAX];
	static char *type[] = { "Human   ", "Computer" };
	static char *prefix[] = { "human", "simul" };
	static int names_set = 0;

	if (OptNonStop && names_set)
	{
		DisplayScore(old_win, 0);
		return player_count;
	}
	win = GrabWindow(old_win, 18, 30, 0, 0, PAIR3);
#ifdef SYS5_3_COLOR
	if (OptUseColor)
		wattron(win, COLOR_PAIR(PAIR4));
	else
		wstandout(win);
#else
	wstandout(win);
#endif
	mvwaddstr(win, 1, (30 - sizeof(YZ_BANNER)) / 2, YZ_BANNER);
#ifdef SYS5_3_COLOR
	if (OptUseColor)
		wattroff(win, COLOR_PAIR(PAIR4));
	else
		wstandend(win);
#else
	wstandend(win);
#endif
	mvwaddstr(win, 16, 2, "press '?' for help anytime");
	mvwaddstr(win, 3, 4, "Player Count: ");
	if (OptLearn)
	{
		player_count = YZ_PLAYERS_MAX;
		wprintw(win, "%d", player_count);
	}
	else
		player_count = GrabDigit(win, 1, YZ_PLAYERS_MAX, HLP_PLAYER);
	mvwaddstr(win, 6, 4, "Player information...");
	if (!names_set)
	{
		for (i = 0; i < YZ_PLAYERS_MAX; ++i)
		{
			players[i].computer = 1;
			sprintf(players[i].name, "%s-%d", prefix[players[i].computer], i + 1);
			name_change[i] = 0;
		}
		names_set = 1;
	}
	for (i = 0; i < player_count; ++i)
	{
		mvwaddstr(win, i + 8, 3, players[i].name);
		mvwaddstr(win, i + 8, YZ_NAME_MAX + 4, type[players[i].computer]);
	}
	player = 0;
	wmove(win, 8 + player, YZ_NAME_MAX + 4);
	wrefresh(win);
	while ((ch = OptLearn ? YZ_CR : wgetch(win)) != YZ_CR)
	{
		switch (ch)
		{
		case ' ':
			players[player].computer = !players[player].computer;
			mvwaddstr(win, player + 8, YZ_NAME_MAX + 4, type[players[player].computer]);
			if (!name_change[player])
			{
				(void)sprintf(players[player].name, "%s-%d", prefix[players[player].computer], player + 1);
				mvwaddstr(win, player + 8, 3, players[player].name);
			}
			break;
		case 'j':
			player = (player + 1) % player_count;
			break;
		case 'k':
			--player;
			if (player < 0)
				player = player_count - 1;
			break;
		case 'n':
			wmove(win, player + 8, 3);
			GetString(win, players[player].name, YZ_NAME_MAX - 1, 1);
			if (!players[player].name[0])
			{
				(void)sprintf(players[player].name, "%s-%d", prefix[players[player].computer], player + 1);
				name_change[player] = 0;
			}
			else
				name_change[player] = 1;
			mvwaddstr(win, player + 8, 3, players[player].name);
			player = (player + 1) % player_count;
			break;
		default:
			ExtraCheck(win, ch, HLP_NAMES);
			break;
		}
		wmove(win, 8 + player, YZ_NAME_MAX + 4);
#ifndef SYS5_3_CURSES
		wrefresh(win);
#endif
	}
	if (!OptFast)
		sleep(0);
	DropWindow(old_win, win);

	return player_count;
}

static void
Randomise(seed)
     unsigned long seed;

{
	srand48(seed);
}

static void
InitSignals()
{
	if (OptLearn)
	{
		(void)signal(SIGINT, SIG_IGN);
		(void)signal(SIGHUP, SIG_IGN);
	}
	else
		(void)signal(SIGINT, Cleanup);
	(void)signal(SIGTERM, Cleanup);
	(void)signal(SIGQUIT, DumpCore);
}

static void
Cleanup(sig_no)
     int sig_no;

{
	endwin();
	if (OptLearn)
		DumpCosts();
	if (sig_no != SIGINT)
		fprintf(stderr, "Signal %d received.\n", sig_no);
	exit(1);
}

static void
DumpCore(sig_no)
     int sig_no;

{
	if (sig_no != SIGQUIT)
		Cleanup(sig_no);
	(void)signal(SIGQUIT, SIG_DFL);
	kill(getpid(), SIGQUIT);
	pause();
	exit(1);
}
