#include <curses.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "yz_consts.h"
#include "yz_types.h"
#include "yz_macros.h"
#include "yz_funcs.h"

extern int OptFast;
extern int OptUseColor;

void
DropWindow(WINDOW *old_win, WINDOW *win)
{
	delwin(win);
	touchwin(old_win);
	wrefresh(old_win);
}

void
GetString(WINDOW *win, char *str, int max_len, int alphanum)
{
	int y, x;
	int i;
	int len = 0;
	int ch;

	getyx(win, y, x);
	for (i = 0; i < max_len; ++i)
		waddch(win, ' ');
	wmove(win, y, x);
	wrefresh(win);
	while ((ch = wgetch(win)) != YZ_CR)
	{
		switch (ch)
		{
		case 8:
		case 127:
			if (len > 0)
			{
				--len;
				mvwaddch(win, y, x + len, ' ');
			}
			else
				flash();
			break;
		default:
			if (len < max_len && (!alphanum || isalnum(ch)))
			{
				str[len] = ch;
				mvwaddch(win, y, x + len, ch);
				++len;
			}
			else if (alphanum)
				ExtraCheck(win, ch, HLP_GETSTR);
			else
				flash();
			break;
		}
		wmove(win, y, x + len);
		wrefresh(win);
	}
	str[len] = 0;
}

static void
ForkShell(WINDOW *old_win)
{
	WINDOW *win;
	char *shell;
	char command[YZ_COMMAND];
	static char prompt[] = "Command: ";
	extern char *getenv();

	win = GrabWindow(old_win, 3, YZ_COMMAND + sizeof(prompt) + 1, LINES - 4, 0, 0);
	mvwaddstr(win, 1, 1, prompt);
	GetString(win, command, YZ_COMMAND - 1, 0);
	DropWindow(old_win, win);
	endwin();
	if (!command[0])
	{
		if (!(shell = getenv("SHELL")))
			shell = "/bin/sh";
		system(shell);
	}
	else
	{
		system(command);
		sleep(3);
	}
	wrefresh(curscr);
	touchwin(old_win);
	wrefresh(old_win);
}

static int
AbsolutelySure(WINDOW *old_win)
{
	WINDOW *win;
	int ch;
	int quit;
	int done = 0;
	static char prompt[] = "Quit Yahtzee - are you absolutely sure? ";

	win = GrabWindow(old_win, 3, sizeof(prompt) + 3, 12, (COLS - sizeof(prompt)) / 2, PAIR6);
	mvwaddstr(win, 1, 1, prompt);
#ifndef SYS5_3_CURSES
	wrefresh(win);
#endif
	do
	{
		ch = wgetch(win);
		switch (ch)
		{
		case 'y':
		case 'Y':
		case ' ':
			quit = 1;
			done = 1;
			break;
		case 'n':
		case 'N':
			quit = 0;
			done = 1;
			break;
		default:
			ExtraCheck(win, ch, HLP_NONE);
			break;
		}
	}
	while (!done);
	DropWindow(old_win, win);

	return quit;
}

void
ExtraCheck(WINDOW *win, int ch, int hlp_no)
{
	switch (ch)
	{
	case '?':
		HelpOut(win, hlp_no);
		break;
	case '\014':
/*		touchwin(curscr); */
		wrefresh(curscr);
		touchwin(win);
		wrefresh(win);
		break;
	case 'b':
		Rools(win);
		break;
	case 'F':
		OptFast = 1;
		break;
	case 's':
		DisplayScore(win, 1);
		break;
	case '!':
		ForkShell(win);
		break;
	case 'q':
		if (!AbsolutelySure(win))
			return;
		DisplayScore(win, 0);
		nocbreak();
		echo();
		endwin();
		exit(1);
		break;
	default:
		flash();
		break;
	}
}

void
PutNumber(WINDOW *win, int y, int x, int val)
{
	char buf[YZ_STR_MAX];

	(void)sprintf(buf, "%d", val);
	mvwaddstr(win, y, x - strlen(buf), buf);
}

void
PopError(WINDOW *old_win, char *str)
{
	WINDOW *win;
	int len;

	len = strlen(str) + 2 + 2;
	win = GrabWindow(old_win, 3, len, 12, (COLS - len) / 2, PAIR5);
	mvwaddstr(win, 1, 2, str);
	wrefresh(win);
	sleep(3);
	DropWindow(old_win, win);
}

WINDOW *
GrabWindow(WINDOW *old_win, int y, int x, int beg_y, int beg_x, int color_pair)
{
	WINDOW *win;

	win = newwin(y, x, beg_y, beg_x);
#ifndef BSD_CURSES
#ifdef SYS5_3_COLOR
	if (OptUseColor)
		wattron(win, COLOR_PAIR(color_pair));
	else
		wattron(win, A_DIM);
#else
	wattron(win, A_DIM);
#endif
#endif
	box(win, 0, 0);
#ifndef BSD_CURSES
#ifdef SYS5_3_COLOR
	if (OptUseColor)
		wattroff(win, COLOR_PAIR(color_pair));
	else
		wattroff(win, A_DIM);
#else
	wattroff(win, A_DIM);
#endif
#endif
	touchwin(win);

	return win;
}

int
GrabDigit(WINDOW *win, int min_valid, int max_valid, int hlp_no)
{
	int y, x;
	int ch;
	char dig_str[YZ_STR_MAX];
	int len = 0;
	int max_len;
	int ret_val = 0;

	getyx(win, y, x);
	(void)sprintf(dig_str, "%d", max_valid);
	max_len = strlen(dig_str);
#ifndef SYS5_3_CURSES
	wrefresh(win);
#endif
	while ((ch = wgetch(win)) != YZ_CR || (ch == YZ_CR && len == 0))
	{
		switch (ch)
		{
		case 8:
		case 127:
			if (len == 0)
				flash();
			else
			{
				--len;
				mvwaddch(win, y, x + len, ' ');
				wmove(win, y, x + len);
			}
			break;
		default:
			if (isdigit(ch))
				if (len == max_len)
					flash();
				else
				{
					mvwaddch(win, y, x + len, ch);
					dig_str[len++] = ch;
					dig_str[len] = '\0';
				}
			else
				ExtraCheck(win, ch, hlp_no);
		}
		wrefresh(win);
	}
	(void)sscanf(dig_str, "%d", &ret_val);
	if (ret_val < min_valid || ret_val > max_valid)
	{
		flash();
		for (len = 0; len < max_len; ++len)
			mvwaddch(win, y, x + len, ' ');
		wmove(win, y, x);
		wrefresh(win);
		ret_val = GrabDigit(win, min_valid, max_valid, hlp_no);
	}

	return ret_val;
}

#if defined(BSD_CURSES) || defined (SYS5_2_CURSES)

#undef box

int
BSDbox(WINDOW *win)
{
	wstandout(win);
	box(win, ':', '-');
	wstandend(win);
}

#endif
