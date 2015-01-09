#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include "yz_consts.h"
#include "yz_types.h"
#include "yz_macros.h"
#include "yz_funcs.h"

extern int OptUseColor;

static char *Also[] = {
	"Also press:",
	"   '?'  - for help any time",
	"   's'  - current scores",
	"   'b'  - display rules",
	"   '^L' - redraw current screen",
	"   '!'  - to fork shell",
	"   'q'  - to quit"
};

static char Help[HLP_LAST_VALID + 1][YZ_HLP_LINES][YZ_HLP_COLS] = {
	{"Player Count",
	 "Input the number of players,",
	 "maximum is six, minimum is one."},
	{"Continue",
	 "Press:",
	 "    'y'  - to play another game.",
	 "    'n'  - to exit yahtzee."},
	{"Player Selection",
	 "Select which players will be human",
	 "and which will be simulated.",
	 "Press:",
	 "  space  - to toggle human/computer",
	 "    'j'  - to move cursor down",
	 "    'k'  - to move cursor up",
	 "    'n'  - to change name",
	 " return  - to start game"},
	{"Name Input",
	 "Type in an alphanumeric string,",
	 "then press return."},
	{"Holding Dice",
	 "Stop dice from being rolled.",
	 "Press:",
	 "     ' '  - toggle hold on die",
	 "'1'..'5'  - toggle hold on die N",
	 "     'l'  - move cursor right",
	 "     'h'  - move cursor left",
	 "     'r'  - computer recommendation",
	 "     'a'  - hold all dice",
	 "     'o'  - order dice",
	 "     't'  - toggle all dice",
	 "Press return when finished."},
	{"Category Selection",
	 "Select the desired category by",
	 "moving cursor next to category",
	 "and pressing return.",
	 "Press:",
	 "    'j'  - to move cursor down",
	 "    'k'  - to move cursor up",
	 "    'r'  - computer recommendation",
	 " return  - select current category"},
	{"Rules",
	 "These are the rules for Yahtzee",
	 "(also known as dice poker).",
	 "Press:",
	 "    'm'  - for next page",
	 "    'b'  - for previous page",
	 "    'g'  - exit rule display"},
	{"High Score",
	 "This is the current high score file."},
	{"Help",
	 "Exit the help window by pressing",
	 "space."}
};

void
HelpOut(old_win, hlp_no)
     WINDOW *old_win;
     int hlp_no;

{
	WINDOW *win;
	int ch;
	int len;
	int i, j;
	char msg[YZ_STR_MAX];

	if (hlp_no > HLP_LAST_VALID || hlp_no == HLP_NONE)
	{
		PopError(old_win, "No help available");
		return;
	}
	win = GrabWindow(old_win, YZ_HLP_LINES + 2, YZ_HLP_COLS + 2, 0, 32, PAIR1);
	sprintf(msg, "Help - %s", Help[hlp_no][0]);
	len = strlen(msg);
#ifdef SYS5_3_COLOR
	if (OptUseColor)
		wattron(win, COLOR_PAIR(PAIR2));
	else
		wstandout(win);
#else
	wstandout(win);
#endif
	mvwaddstr(win, 1, (YZ_HLP_COLS + 2 - len) / 2, msg);
#ifdef SYS5_3_COLOR
	if (OptUseColor)
		wattroff(win, COLOR_PAIR(PAIR2));
	else
		wstandend(win);
#else
	wstandend(win);
#endif
	i = 1;
#ifdef SYS5_3_COLOR
	if (OptUseColor)
		wattron(win, COLOR_PAIR(PAIR1));
#endif
	while (Help[hlp_no][i][0])
	{
		mvwaddstr(win, i + 2, 2, Help[hlp_no][i]);
		++i;
	}
	for (j = 0; j < sizeof(Also) / sizeof(Also[0]); ++j)
		mvwaddstr(win, i + 3 + j, 2, Also[j]);
	mvwaddstr(win, YZ_HLP_LINES, 1, "--press space--");
#ifndef SYS5_3_CURSES
	wrefresh(win);
#endif
	while ((ch = wgetch(win)) != ' ')
		ExtraCheck(win, ch, HLP_HLP);
	mvwaddstr(win, YZ_HLP_LINES, 1, "               ");
	wrefresh(win);
	DropWindow(old_win, win);
}
