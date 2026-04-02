/*
 *  svf_sim_host - SVF simulation host callbacks for Lib(X)SVF
 *
 *  Provides a simulated JTAG host that logs all signal
 *  transitions (TCK, TMS, TDI, TDO, TAP state) to a VCD file.
 */

#include "svf_sim_host.h"
#include "vcd_writer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Helper: emit timestamp only when time advances */
static void ctx_set_time(struct sim_ctx *ctx, unsigned long long t)
{
	if (t != ctx->vcd_time) {
		ctx->vcd_time = t;
		vcd_timestamp(ctx->vcd_file, t);
	}
}

/* ======================== libxsvf Callbacks ======================== */

static int sim_setup(struct libxsvf_host *h)
{
	struct sim_ctx *ctx = h->user_data;

	if (ctx->verbose)
		fprintf(stderr, "[SETUP]\n");

	/* Write VCD header and initial signal values */
	vcd_write_header(ctx->vcd_file);
	vcd_timestamp(ctx->vcd_file, 0);

	/* set initial signal values in shared struct sim_ctx object */
	ctx->vcd_time = 0;
	ctx->cur_tck = 0;
	ctx->cur_tms = 0;
	ctx->cur_tdi = 0;
	ctx->cur_tdo = -1;
	ctx->cur_tap = -1;

	return 0;
}

static int sim_shutdown(struct libxsvf_host *h)
{
	struct sim_ctx *ctx = h->user_data;

	if (ctx->verbose)
		fprintf(stderr, "[SHUTDOWN]\n");

	/* Drive TCK low at the end and write final timestamp */
	ctx->vcd_time += 2;
	vcd_timestamp(ctx->vcd_file, ctx->vcd_time);
	vcd_bit(ctx->vcd_file, VCD_ID_TCK, 0);

	return 0;
}

static void sim_udelay(struct libxsvf_host *h, long usecs, int tms,
                        long num_tck)
{
	struct sim_ctx *ctx = h->user_data;

	if (ctx->verbose >= 3)
		fprintf(stderr, "[DELAY:%ld, TMS:%d, NUM_TCK:%ld]\n",
		        usecs, tms, num_tck);

	/* Generate TCK pulses with TMS held at specified value */
	if (num_tck > 0) {
		if (tms != ctx->cur_tms) {
			ctx->cur_tms = tms;
			ctx_set_time(ctx, ctx->vcd_time);
			vcd_bit(ctx->vcd_file, VCD_ID_TMS, tms);
		}
		while (num_tck-- > 0) {
			/* TCK falling edge */
			ctx_set_time(ctx, ctx->vcd_time + 1);
			vcd_bit(ctx->vcd_file, VCD_ID_TCK, 0);

			/* TCK rising edge */
			ctx_set_time(ctx, ctx->vcd_time + 1);
			vcd_bit(ctx->vcd_file, VCD_ID_TCK, 1);

			ctx->clockcount++;
		}
	}

	/* Advance time for the microsecond delay (10ns units -> usecs * 100) */
	if (usecs > 0)
		ctx->vcd_time += (unsigned long long)usecs * 100;
}

static int sim_getbyte(struct libxsvf_host *h)
{
	struct sim_ctx *ctx = h->user_data;
	return fgetc(ctx->svf_file);
}

/*
 * Core JTAG I/O callback.
 *
 * For simulation: TMS and TDI transitions are logged to VCD,
 * TCK is toggled, and TDO is simulated (always matches expected
 * value since there is no real hardware).
 */
static int sim_pulse_tck(struct libxsvf_host *h, int tms, int tdi,
                          int tdo, int rmask, int sync)
{
	struct sim_ctx *ctx = h->user_data;
	int line_tdo;

	/* --- Setup phase: update TMS and TDI before clock edge --- */

	if (tms != ctx->cur_tms) {
		ctx->cur_tms = tms;
		ctx_set_time(ctx, ctx->vcd_time);
		vcd_bit(ctx->vcd_file, VCD_ID_TMS, tms);
	}

	if (tdi >= 0) {
		ctx->bitcount_tdi++;
		if (tdi != ctx->cur_tdi) {
			ctx->cur_tdi = tdi;
			ctx_set_time(ctx, ctx->vcd_time);
			vcd_bit(ctx->vcd_file, VCD_ID_TDI, tdi);
		}
	}

	/* --- TCK falling edge --- */
	ctx_set_time(ctx, ctx->vcd_time + 1);
	vcd_bit(ctx->vcd_file, VCD_ID_TCK, 0);
	ctx->cur_tck = 0;

	/* --- TCK rising edge (target samples TMS/TDI here) --- */
	ctx_set_time(ctx, ctx->vcd_time + 1);
	vcd_bit(ctx->vcd_file, VCD_ID_TCK, 1);
	ctx->cur_tck = 1;

	/* --- Simulated TDO: always return expected value --- */
	line_tdo = (tdo >= 0) ? tdo : 0;

	if (line_tdo != ctx->cur_tdo) {
		ctx->cur_tdo = line_tdo;
		vcd_bit(ctx->vcd_file, VCD_ID_TDO, line_tdo);
	}

	if (tdo >= 0)
		ctx->bitcount_tdo++;

	ctx->clockcount++;

	if (ctx->verbose >= 4)
		fprintf(stderr, "[TMS:%d, TDI:%d, TDO_ARG:%d, TDO_SIM:%d, RMASK:%d]\n",
		        tms, tdi, tdo, line_tdo, rmask);

	/* Simulation always succeeds (no real hardware to mismatch) */
	return line_tdo;
}

static void sim_pulse_sck(struct libxsvf_host *h)
{
	/* SCK not relevant for simulation */
}

static void sim_set_trst(struct libxsvf_host *h, int v)
{
	struct sim_ctx *ctx = h->user_data;
	/* Debug print only. Q:Does TRST signal needs to be simulated?*/
	if (ctx->verbose >= 2)
		fprintf(stderr, "[TRST:%d]\n", v);
}

static int sim_set_frequency(struct libxsvf_host *h, int v)
{
	struct sim_ctx *ctx = h->user_data;
	/* Frequency not relevant for simulation, but log it if verbose
	 * Debug print only, no HW timing is affected since this is a simulation. 
	 */
	if (ctx->verbose)
		fprintf(stderr, "[FREQUENCY: %d Hz]\n", v);
	return 0;
}

static void sim_report_tapstate(struct libxsvf_host *h)
{
	struct sim_ctx *ctx = h->user_data;
	int state = (int)h->tap_state;

	/* Log TAP state as a 5-bit integer in VCD */
	if (state != ctx->cur_tap) {
		ctx->cur_tap = state;
		ctx_set_time(ctx, ctx->vcd_time);
		vcd_vector(ctx->vcd_file, VCD_ID_TAP, state, 5);
	}

	if (ctx->verbose >= 2)
		fprintf(stderr, "[TAP: %s]\n", libxsvf_state2str(h->tap_state));
}

static void sim_report_device(struct libxsvf_host *h, unsigned long idcode)
{
	printf("idcode=0x%08lx, revision=0x%01lx, part=0x%04lx, "
	       "manufacturer=0x%03lx\n",
	       idcode, (idcode >> 28) & 0xf,
	       (idcode >> 12) & 0xffff, (idcode >> 1) & 0x7ff);
}

static void sim_report_status(struct libxsvf_host *h, const char *message)
{
	struct sim_ctx *ctx = h->user_data;
	if (ctx->verbose >= 2)
		fprintf(stderr, "[STATUS] %s\n", message);
}

static void sim_report_error(struct libxsvf_host *h, const char *file,
                              int line, const char *message)
{
	fprintf(stderr, "[ERROR %s:%d] %s\n", file, line, message);
}

static void *sim_realloc(struct libxsvf_host *h, void *ptr, int size,
                          enum libxsvf_mem which)
{
	return realloc(ptr, size);
}

/* ======================== Public API ======================== */

void svf_sim_host_init(struct libxsvf_host *h, struct sim_ctx *ctx)
{
	memset(h, 0, sizeof(*h));
	h->setup           = sim_setup;
	h->shutdown        = sim_shutdown;
	h->udelay          = sim_udelay;
	h->getbyte         = sim_getbyte;
	h->pulse_tck       = sim_pulse_tck;
	h->pulse_sck       = sim_pulse_sck;
	h->set_trst        = sim_set_trst;
	h->set_frequency   = sim_set_frequency;
	h->report_tapstate = sim_report_tapstate;
	h->report_device   = sim_report_device;
	h->report_status   = sim_report_status;
	h->report_error    = sim_report_error;
	h->realloc         = sim_realloc;
	h->user_data       = ctx;
}
