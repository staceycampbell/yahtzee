typedef struct players_t
{
	int computer;
	int score[YZ_CAT_COUNT];
	int bonus;
	int bonus63;
	int total63;
	int total;
	int used[YZ_CAT_COUNT];
	char name[YZ_NAME_MAX];
} players_t;

typedef struct dice_t
{
	int dice;
	int hold;
} dice_t;

typedef struct score_t
{
	char name[YZ_NAME_MAX];
	int score;
} score_t;
