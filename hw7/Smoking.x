enum type {
     TOBACCO = 0,
     PAPER = 1,
     MATCHES = 2
};

struct data {
       type type;
       int amount;
       int smoker;
};

program SMOKE {
	version SMOKERS {
		int GET_ME_MY_SUPPLY(data) = 1;
	} = 1;
} = 0x20A0FACE;