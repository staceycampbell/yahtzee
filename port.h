/* Establish curses version and random number generator */

#ifndef ACS_PLUS		/* must not be System 5.3 curses */
#ifdef A_DIM			/* probably System 5.[12] */
#define SYS5_2_CURSES
#else /* probably BSD curses */
#define BSD_CURSES
#endif
#else
#define SYS5_3_CURSES
#ifdef COLOR_RED
#define SYS5_3_COLOR
#endif
#endif

#define PAIR1 1
#define PAIR2 2
#define PAIR3 3
#define PAIR4 4
#define PAIR5 5
#define PAIR6 6

#ifndef BSD_CURSES
#include <term.h>
#endif

#ifdef BSD_CURSES
#define A_DIM A_STANDOUT	/* this works for BSD curses on Xenix 2.3 */
#define flushinp()
#define resetterm() system("stty intr '^c' erase '^?' echo")
#define flash() putchar(7)
#ifdef sun
#define wattron(win, attr) wstandout(win)
#define wattroff(win, attr) wstandend(win)
#else
#define cbreak crmode
#endif
#endif

#ifndef SYS5_3_CURSES
#define ACS_HLINE '-'
#define ACS_VLINE '|'
#define ACS_URCORNER '.'
#define ACS_ULCORNER '.'
#define ACS_LLCORNER '`'
#define ACS_LRCORNER '\''
#endif

#if defined(SYS5_2_CURSES) || defined(BSD_CURSES)
#define box(win, a, b) BSDbox(win)
#endif

#if defined(BSD_CURSES) && ! defined(sun)
#define YZ_CR '\r'
#else
#define YZ_CR '\n'
#endif

#if defined(BSD_RANDOM) || (defined(BSD_CURSES) && ! defined(M_XENIX))
#define lrand48 random
#define srand48 srandom
#endif
