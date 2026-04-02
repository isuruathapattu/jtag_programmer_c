# svf_sim — JTAG Waveform Simulator

`svf_sim` reads an SVF (Serial Vector Format) file and produces a VCD (Value Change Dump) file showing JTAG signals: **TCK**, **TMS**, **TDI**, **TDO**, and **TAP state**. Open the VCD in GTKWave or any VCD-compatible waveform viewer.

## Prerequisites

- C compiler (GCC or compatible)
- CMake >= 3.16 (for CMake build) **or** GNU Make (for standalone build)

## Building

### Option 1: CMake (recommended)

```bash
mkdir build && cd build
cmake ..
make
```

The executable is produced at `build/apps/svf_sim/svf_sim`.

### Option 2: Standalone Makefile
```bash
cmake --build . --target svf_sim
```
or
```bash
cd apps/svf_sim
make
```

The executable is produced at `apps/svf_sim/svf_sim`.

### Option 3: Manual single-command build

```bash
cd apps/svf_sim
gcc -o svf_sim src/main.c src/svf_sim_host.c src/vcd_writer.c \
    ../../external/libxsvf/svf.c \
    ../../external/libxsvf/tap.c \
    ../../external/libxsvf/play.c \
    ../../external/libxsvf/statename.c \
    ../../external/libxsvf/memname.c \
    -Iinclude -I../../external/libxsvf \
    -DLIBXSVF_WITHOUT_XSVF -DLIBXSVF_WITHOUT_SCAN -Wall -O2
```

## Usage

```
svf_sim -s <input.svf> [-o <output.vcd>] [-v ...]
```

| Flag | Description |
|------|-------------|
| `-s <file>` | Input SVF file (**required**) |
| `-o <file>` | Output VCD file (default: `output.vcd`) |
| `-v` | Increase verbosity (repeat for more: `-v`, `-vv`, `-vvv`, `-vvvv`) |

### Examples

Generate a VCD from an SVF file:

```bash
./svf_sim -s design.svf -o waveform.vcd
```

Run with verbose output:

```bash
./svf_sim -s design.svf -o waveform.vcd -vv
```

### Viewing the output

Open the resulting VCD file in a waveform viewer:

```bash
gtkwave output.vcd
```

## Project Structure

```
jtag_programmer_c/
├── CMakeLists.txt              # Top-level CMake build
├── apps/
│   └── svf_sim/
│       ├── CMakeLists.txt      # svf_sim build target
│       ├── Makefile            # Standalone build
│       ├── include/
│       │   ├── svf_sim_host.h  # Simulation host API (struct sim_ctx, init)
│       │   └── vcd_writer.h    # VCD file output API
│       └── src/
│           ├── main.c          # CLI entry point (arg parsing, file I/O)
│           ├── svf_sim_host.c  # libxsvf host callbacks (JTAG simulation)
│           └── vcd_writer.c    # VCD writer implementation
└── external/
    └── libxsvf/                # SVF/XSVF parsing library
        ├── CMakeLists.txt
        ├── libxsvf.h
        ├── svf.c
        ├── tap.c
        ├── play.c
        ├── statename.c
        └── memname.c
```