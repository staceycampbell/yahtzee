#include "port.h"

#define YZ_PLAYERS_MAX 6
#define YZ_NAME_MAX 9
#define YZ_CAT_COUNT 13
#define YZ_DICE 5
#define YZ_DICE_MAX 6
#define YZ_STR_MAX 256
#define YZ_COMMAND 40
#define YZ_FN_MAX 256
#define YZ_CAT63 (YZ_DICE_MAX - 1)
#define YZ_63BONUS_VAL 35
#define YZ_63BONUS_LIMIT 63
#define YZ_CAT 11
#define YZ_LG_CAT 10
#define YZ_SM_CAT 9

#define YZ_BANNER "Y a h t z e e   2 . 1"
#define YZ_DICE_Y 4
#define YZ_DICE_X 8
#define YZ_CEN_Y (YZ_DICE_Y - 1)
#define YZ_CEN_X ((YZ_DICE_X + 2) / 2 - 1)
#define YZ_DICE_PAD 3
#define YZ_PLAYER_COLS (YZ_NAME_MAX + 1)
#define YZ_LG_STRAIGHT "large straight"
#define YZ_63 "63 sub total"
#define YZ_63BONUS "63 bonus"
#define YZ_BSTR "yahtzee bonus"
#define YZ_TOTSTR "total"
#define YZ_TOP (YZ_DICE_Y + 3)
#define YZ_PLAYER_PAD 25
#define YZ_63TOTAL_ROW (YZ_CAT_COUNT + YZ_TOP)
#define YZ_63BONUS_ROW (YZ_CAT_COUNT + YZ_TOP + 1)
#define YZ_BONUS_ROW (YZ_CAT_COUNT + YZ_TOP + 2)
#define YZ_TOTAL_ROW (YZ_CAT_COUNT + YZ_TOP + 3)
#define YZ_HLP_COLS 37
#define YZ_HLP_LINES 22

#define HLP_PLAYER 0
#define HLP_CONTINUE 1
#define HLP_NAMES 2
#define HLP_GETSTR 3
#define HLP_HOLD 4
#define HLP_SELECT 5
#define HLP_ROOLS 6
#define HLP_HS 7
#define HLP_HLP 8
#define HLP_LAST_VALID HLP_HLP
#define HLP_NONE -1

#define SC_OAK_NORMAL 3
#define SC_STRAIGHT_NORMAL 6
#define SC_STR_SCORE_NORM 3

#define SC_YAHTZEE 50
#define SC_YAHTZEE_BONUS 100
#define SC_LARGE_STRAIGHT 40
#define SC_SMALL_STRAIGHT 30
#define SC_FULL_HOUSE 25

#define HS_ENTRIES 15

#define HUER_SAMPLE_SIZE 100
