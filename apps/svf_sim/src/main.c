/*
 *  svf_sim - JTAG Waveform Simulator using Lib(X)SVF
 *
 *  Reads an SVF file and outputs a VCD (Value Change Dump) file
 *  showing the exact JTAG signals: TCK, TMS, TDI, TDO, TAP state.
 *
 *  Open the VCD in GTKWave or any VCD-compatible waveform viewer.
 *
 *  Usage: svf_sim -s <input.svf> [-o <output.vcd>] [-v]
 */

#include "svf_sim_host.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ======================== Main ======================== */

static void usage(const char *progname)
{
	fprintf(stderr, "\n");
	fprintf(stderr, "svf_sim - JTAG Waveform Simulator using Lib(X)SVF\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Reads an SVF file and outputs a VCD waveform file showing\n");
	fprintf(stderr, "the exact JTAG signals (TCK, TMS, TDI, TDO, TAP state).\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage: %s -s <input.svf> [-o <output.vcd>] [-v ...]\n",
	        progname);
	fprintf(stderr, "\n");
	fprintf(stderr, "  -s <file>   Input SVF file (required)\n");
	fprintf(stderr, "  -o <file>   Output VCD file (default: output.vcd)\n");
	fprintf(stderr, "  -v          Verbose (repeat for more detail)\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Open the VCD file in GTKWave to view waveforms.\n");
	fprintf(stderr, "\n");
	exit(1);
}

int main(int argc, char **argv)
{
	const char *svf_path = NULL;
	const char *vcd_path = "output.vcd";
	struct sim_ctx ctx;
	struct libxsvf_host h;
	int rc, i;

	memset(&ctx, 0, sizeof(ctx));

	/* Parse command-line arguments (portable, no getopt dependency) */
	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-s") && i + 1 < argc) {
			svf_path = argv[++i];
		} else if (!strcmp(argv[i], "-o") && i + 1 < argc) {
			vcd_path = argv[++i];
		} else if (!strcmp(argv[i], "-v")) {
			ctx.verbose++;
		} else if (!strcmp(argv[i], "-vv")) {
			ctx.verbose += 2;
		} else if (!strcmp(argv[i], "-vvv")) {
			ctx.verbose += 3;
		} else if (!strcmp(argv[i], "-vvvv")) {
			ctx.verbose += 4;
		} else {
			usage(argc >= 1 ? argv[0] : "svf_sim");
		}
	}

	if (!svf_path)
		usage(argc >= 1 ? argv[0] : "svf_sim");

	/* Open SVF input file */
	if (!strcmp(svf_path, "-")) {
		ctx.svf_file = stdin;
	} else {
		ctx.svf_file = fopen(svf_path, "rb");
		if (!ctx.svf_file) {
			fprintf(stderr, "Error: Cannot open SVF file '%s'\n", svf_path);
			return 1;
		}
	}

	/* Open VCD output file */
	ctx.vcd_file = fopen(vcd_path, "w");
	if (!ctx.vcd_file) {
		fprintf(stderr, "Error: Cannot create VCD file '%s'\n", vcd_path);
		if (ctx.svf_file != stdin)
			fclose(ctx.svf_file);
		return 1;
	}

	/* Wire up the libxsvf host struct with simulation callbacks */
	svf_sim_host_init(&h, &ctx);

	if (ctx.verbose)
		fprintf(stderr, "Simulating SVF file: %s -> %s\n", svf_path, vcd_path);

	/* Run the SVF player (simulation mode) */
	rc = libxsvf_play(&h, LIBXSVF_MODE_SVF);

	/* Print summary */
	fprintf(stderr, "\n--- Simulation Summary ---\n");
	fprintf(stderr, "Total TCK clock cycles: %d\n", ctx.clockcount);
	fprintf(stderr, "Significant TDI bits:   %d\n", ctx.bitcount_tdi);
	fprintf(stderr, "Significant TDO bits:   %d\n", ctx.bitcount_tdo);
	fprintf(stderr, "VCD output written to:  %s\n", vcd_path);

	if (rc < 0)
		fprintf(stderr, "Result: FAILED (rc=%d)\n", rc);
	else
		fprintf(stderr, "Result: OK\n");

	/* Cleanup */
	fclose(ctx.vcd_file);
	if (ctx.svf_file != stdin)
		fclose(ctx.svf_file);

	return rc < 0 ? 1 : 0;
}
