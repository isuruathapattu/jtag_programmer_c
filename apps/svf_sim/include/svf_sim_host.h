/*
 *  svf_sim_host - SVF simulation host callbacks for Lib(X)SVF
 *
 *  Provides a simulated JTAG host that logs all signal
 *  transitions (TCK, TMS, TDI, TDO, TAP state) to a VCD file.
 */

#ifndef SVF_SIM_HOST_H
#define SVF_SIM_HOST_H

#include "libxsvf.h"
#include <stdio.h>

struct sim_ctx {
	FILE *svf_file;              /* SVF input file handle */
	FILE *vcd_file;              /* VCD output file handle */
	unsigned long long vcd_time; /* current VCD timestamp (in 10ns units) */
	int verbose;
	int clockcount;
	int bitcount_tdi;
	int bitcount_tdo;
	/* Track current signal values to avoid redundant VCD entries */
	int cur_tck;
	int cur_tms;
	int cur_tdi;
	int cur_tdo;
	int cur_tap;
};

/*
 * Initialize a libxsvf_host with simulation callbacks.
 * The caller must have populated ctx->svf_file and ctx->vcd_file
 * before calling libxsvf_play().
 */
void svf_sim_host_init(struct libxsvf_host *h, struct sim_ctx *ctx);

#endif /* SVF_SIM_HOST_H */
