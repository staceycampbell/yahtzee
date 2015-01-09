#define ROLL_DICE(dice_struct) \
	{ \
	int i; \
	extern long lrand48(); \
	for (i = 0; i < YZ_DICE; ++i) \
		if (! (dice_struct)[i].hold) \
			(dice_struct)[i].dice = lrand48() % YZ_DICE_MAX; \
	}

#define CLEAR_HOLD(dice) \
	{ \
	int i; \
	for (i = 0; i < YZ_DICE; ++i) \
		(dice)[i].hold = 0; \
	}

#define DICE_SORT(dice) GEN_SORT(dice, >)
#define RDICE_SORT(dice) GEN_SORT(dice, <)
#define SWAP(a, b, tmp) ((tmp) = (a), (a) = (b), (b) = (tmp))

#define GEN_SORT(dice, order) \
	{ \
	int i, j; \
	int tmp; \
	for (i = 0; i < YZ_DICE; ++i) \
		for (j = i; j < YZ_DICE; ++j) \
			if ((dice)[i] order (dice)[j]) \
				SWAP((dice)[i], (dice)[j], tmp); \
	}

#define DSTRUCT_SORT(d) \
	{ \
	int i, j; \
	dice_t tmp; \
	for (i = 0; i < YZ_DICE; ++i) \
		for (j = i; j < YZ_DICE; ++j) \
			if ((d)[i].dice > (d)[j].dice) \
				SWAP((d)[i], (d)[j], tmp); \
	}

#define YZ_SIDE(str) (sizeof(YZ_LG_STRAIGHT) - strlen(str) - 1)

#define BDICEATTRON(win, dim) ((dim) ? wattron(win, A_STANDOUT) : \
	wattron(win, A_DIM))
#define BDICEATTROFF(win, dim) ((dim) ? wattroff(win, A_STANDOUT) : \
	wattroff(win, A_DIM))

#ifndef SYS5_3_COLOR
#define DICEATTRON(win, dim) BDICEATTRON(win, dim)
#define DICEATTROFF(win, dim) BDICEATTROFF(win, dim)
#else
#define DICEATTRON(win, dim) (OptUseColor ? wattron(win, COLOR_PAIR(PAIR3)) : \
	BDICEATTRON(win, dim))
#define DICEATTROFF(win, dim) (OptUseColor ? wattroff(win, COLOR_PAIR(PAIR3)) \
	: BDICEATTROFF(win, dim))
#endif
