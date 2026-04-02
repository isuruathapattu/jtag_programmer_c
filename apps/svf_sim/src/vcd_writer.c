/*
 *  vcd_writer - VCD (Value Change Dump) file writer
 *
 *  Writes IEEE 1364-2001 compliant VCD output for waveform viewers
 *  such as GTKWave.
 */

#include "vcd_writer.h"

void vcd_write_header(FILE *f)
{
	fprintf(f, "$date 2026 $end\n");
	fprintf(f, "$version svf_sim 1.0 $end\n");
	fprintf(f, "$timescale 10ns $end\n");
	fprintf(f, "$scope module jtag $end\n");
	fprintf(f, "$var wire 1 %c TCK $end\n", VCD_ID_TCK);
	fprintf(f, "$var wire 1 %c TMS $end\n", VCD_ID_TMS);
	fprintf(f, "$var wire 1 %c TDI $end\n", VCD_ID_TDI);
	fprintf(f, "$var wire 1 %c TDO $end\n", VCD_ID_TDO);
	fprintf(f, "$var integer 5 %c TAP_STATE $end\n", VCD_ID_TAP);
	fprintf(f, "$upscope $end\n");
	fprintf(f, "$enddefinitions $end\n");
	fprintf(f, "$dumpvars\n");
	fprintf(f, "0%c\n", VCD_ID_TCK);
	fprintf(f, "0%c\n", VCD_ID_TMS);
	fprintf(f, "0%c\n", VCD_ID_TDI);
	fprintf(f, "x%c\n", VCD_ID_TDO);
	fprintf(f, "b00000 %c\n", VCD_ID_TAP);
	fprintf(f, "$end\n");
}

void vcd_timestamp(FILE *f, unsigned long long t)
{
	fprintf(f, "#%llu\n", t);
}

void vcd_bit(FILE *f, char id, int val)
{
	fprintf(f, "%c%c\n", val ? '1' : '0', id);
}

void vcd_vector(FILE *f, char id, int val, int width)
{
	int i;
	fprintf(f, "b");
	for (i = width - 1; i >= 0; i--)
		fprintf(f, "%c", (val >> i) & 1 ? '1' : '0');
	fprintf(f, " %c\n", id);
}
