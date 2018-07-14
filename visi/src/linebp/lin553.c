#include "line.h"
#include "linebp.h"

extern  int cvt_hl();
extern  int cvt_lh();
extern  int cvt_a();
extern  int cvt_sp();
extern  int tst_m();


IN_PORTS in_help[] = {
	{ "tst_m",      tst_m   },
	{ "cvt_sp",     cvt_sp  },
	{ "cvt_a",      cvt_a   },
	{ "cvt_lh",     cvt_lh  },
	{ "cvt_hl",     cvt_hl  },
	{ 0,            0       },
	};

