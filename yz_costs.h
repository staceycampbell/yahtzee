static float Que = 0.5;

static float CostList[YZ_CAT_COUNT][SC_YAHTZEE + 1] = {
	/* one */ {1.000000, 0.300000, 1.000000, 1.000000, 1.700000,
		   1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
		   1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
		   1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
		   1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
		   1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
		   1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
		   1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
		   1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
		   1.000000, 1.000000, 1.000000, 1.000000, 1.000000,
		   1.000000},
	/* two */ {-1.000000, -1.000000, -2.000000, -0.714286, -1.000000,
		   -0.428571, 1.000000, -0.142857, 0.142857, 0.142857,
		   0.428571, 0.428571, 0.714286, 0.714286, 1.000000,
		   1.000000, 1.285714, 1.285714, 1.571429, 1.571429,
		   1.857143, 1.857143, 2.142857, 2.142857, 2.428571,
		   2.428571, 2.714286, 2.714286, 3.000000, 3.000000,
		   3.285714, 3.285714, 3.571429, 3.571429, 3.857143,
		   3.857143, 4.142857, 4.142857, 4.428571, 4.428571,
		   -5.000000, 4.714286, 5.000000, 5.000000, 5.285714,
		   5.285714, 5.571429, 5.571429, 5.857143, 5.857143,
		   6.142857},
	/* three */ {-7.000000, -7.000000, -7.000000, -7.000000, -4.000000,
		     -4.000000, -5.000000, -3.500000, -3.000000, 2.000000,
		     2.000000, 2.000000, -1.000000, 5.000000, 5.000000,
		     11.000000, 8.000000, 8.000000, 11.000000, 11.000000,
		     11.000000, 14.000000, 14.000000, 14.000000, 17.000000,
		     17.000000, 17.000000, 20.000000, 20.000000, 20.000000,
		     23.000000, 23.000000, 23.000000, 26.000000, 26.000000,
		     26.000000, 29.000000, 29.000000, 29.000000, 32.000000,
		     32.000000, 32.000000, 35.000000, 35.000000, 35.000000,
		     38.000000, 38.000000, 38.000000, 41.000000, 41.000000,
		     41.000000},
	/* four */ {-27.000000, -27.000000, -27.000000, -27.000000, -19.000000,
		    -19.000000, -19.000000, -19.000000, -11.000000, -5.100000,
		    -5.500000, -11.000000, 9.000000, 9.000000, 9.000000,
		    9.000000, 14.000000, 14.000000, 14.000000, 14.000000,
		    19.000000, 19.000000, 19.000000, 19.000000, 24.000000,
		    24.000000, 24.000000, 24.000000, 29.000000, 29.000000,
		    29.000000, 29.000000, 34.000000, 34.000000, 34.000000,
		    34.000000, 39.000000, 39.000000, 39.000000, 39.000000,
		    44.000000, 44.000000, 44.000000, 44.000000, 49.000000,
		    49.000000, 49.000000, 49.000000, 54.000000, 54.000000,
		    54.000000},
	/* five */ {-31.000000, -31.000000, -31.000000, -31.000000, -31.000000,
		    -22.000000, -22.000000, -22.000000, -22.000000, -22.000000,
		    -13.000000, -13.000000, -9.000000, -10.000000, -13.000000,
		    9.000000, 10.000000, 10.000000, 10.000000, 10.000000,
		    16.000000, 16.000000, 16.000000, 16.000000, 16.000000,
		    22.000000, 22.000000, 22.000000, 22.000000, 22.000000,
		    28.000000, 28.000000, 28.000000, 28.000000, 28.000000,
		    34.000000, 34.000000, 34.000000, 34.000000, 34.000000,
		    40.000000, 40.000000, 40.000000, 40.000000, 40.000000,
		    46.000000, 46.000000, 46.000000, 46.000000, 46.000000,
		    52.000000},
	/* six */ {-35.000000, -35.000000, -35.000000, -35.000000, -35.000000,
		   -35.000000, -25.000000, -25.000000, -25.000000, -25.000000,
		   -25.000000, -25.000000, -18.000000, -10.000000, -8.000000,
		   -9.000000, -10.000000, -15.000000, 6.700000, 11.000000,
		   11.000000, 11.000000, 11.000000, 11.000000, 18.000000,
		   18.000000, 18.000000, 18.000000, 18.000000, 18.000000,
		   19.000000, 25.000000, 25.000000, 25.000000, 25.000000,
		   25.000000, 32.000000, 32.000000, 32.000000, 32.000000,
		   32.000000, 32.000000, 39.000000, 39.000000, 39.000000,
		   39.000000, 39.000000, 39.000000, 46.000000, 46.000000,
		   46.000000},
	/* 3 of a kind */ {-2.000000, 0.100000, 0.200000, 0.300000, 0.400000,
			   -1.000000, -1.100000, -4.500000, -5.500000, -8.000000,
			   -10.000000, -10.000000, -12.000000, -9.900000, -12.000000,
			   -12.000000, -9.700000, -11.500000, -12.000000, -9.900000,
			   -12.000000, -11.000000, -1.000000, -2.000000, 0.400000,
			   -0.500000, 2.600000, 2.700000, 2.800000, 2.900000,
			   3.000000, 3.100000, 3.200000, 3.300000, 3.400000,
			   3.500000, 3.600000, 3.700000, 3.800000, 3.900000,
			   4.000000, 4.100000, 4.200000, 4.300000, 4.400000,
			   4.500000, 4.600000, 4.700000, 4.800000, 4.900000,
			   5.000000},
	/* 4 of a kind */ {-1.500000, 0.111111, 0.222222, 0.333333, 0.444444,
			   0.555556, -1.000000, -3.377778, -4.000000, -7.000000,
			   -9.000000, -4.222220, -10.000000, -5.000000, -6.500000,
			   -5.999999, -5.700000, -8.500000, -8.500000, 2.111111,
			   2.222222, 2.333333, 2.444444, 2.555556, 2.666667,
			   2.777778, 2.888889, 3.000000, 3.111111, 3.222222,
			   3.333333, 3.444444, 3.555556, 3.666667, 3.777778,
			   3.888889, 4.000000, 4.111111, 4.222222, 4.333333,
			   4.444444, 4.555556, 4.666667, 4.777778, 4.888889,
			   5.000000, 5.111111, 5.222222, 5.333333, 5.444444,
			   5.555556},
	/* full house */ {-3.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			  0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			  0.000000},
	/* small straight */ {-3.000000, 0.000000, 0.000000, -1.000000, -6.000000,
			      -7.000000, -7.000000, -7.000000, -8.000000, -8.000000,
			      -8.000000, -10.000000, -8.000000, -7.000000, -9.000000,
			      -9.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			      0.000000},
	/* large straight */ {-5.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
			      0.000000},
	/* yahtzee */ {-1.601000, 4.000000, 5.000000, 13.000000, 0.000000,
		       0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		       0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		       0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		       0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		       0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		       0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		       0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		       0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		       0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		       50.00000},
	/* chance */ {-8.000000, -9.000000, -10.000000, -11.000000, -12.000000,
		      -13.000000, -14.000000, -15.000000, -16.000000, -17.000000,
		      -17.000000, -17.000000, -17.000000, -17.000000, -14.000000,
		      -16.000000, -16.000000, -16.000000, -16.000000, -16.000000,
		      -17.000000, -17.000000, -17.000000, -17.000000, -20.000000,
		      -21.000000, -22.000000, -23.000000, -24.000000, -25.000000,
		      -26.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		      0.000000, 0.000000, 0.000000, 0.000000, 0.000000,
		      0.000000}
};