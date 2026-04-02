/*
 *  vcd_writer - VCD (Value Change Dump) file writer
 *
 *  Writes IEEE 1364-2001 compliant VCD output for waveform viewers
 *  such as GTKWave.
 */

#ifndef VCD_WRITER_H
#define VCD_WRITER_H

#include <stdio.h>

/* VCD signal identifier characters (single printable ASCII) */
#define VCD_ID_TCK  '!'
#define VCD_ID_TMS  '"'
#define VCD_ID_TDI  '#'
#define VCD_ID_TDO  '$'
#define VCD_ID_TAP  '%'

/*
 * Write the VCD file header with JTAG signal definitions
 * and initial values ($dumpvars block).
 */
void vcd_write_header(FILE *f);

/* Write a VCD timestamp marker: #<t> */
void vcd_timestamp(FILE *f, unsigned long long t);

/* Write a single-bit VCD value change: <0|1><id> */
void vcd_bit(FILE *f, char id, int val);

/* Write a multi-bit VCD value change: b<bits> <id> */
void vcd_vector(FILE *f, char id, int val, int width);

#endif /* VCD_WRITER_H */
