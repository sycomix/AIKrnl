# Quartus / Synthesis Skeleton (Cyclone 10 LP)

This folder contains a skeleton for migrating to Intel Quartus and performing synthesis for the Cyclone 10 LP dev kit.

Notes:
- This skeleton is intentionally minimal. When you install Quartus on your WinTel machine, populate this folder with a `.qpf`/`.qsf` project, constraints, and scripts to run synthesis.

Suggested steps (once Quartus is installed):
1. Create a new Quartus project targeting the Cyclone 10 LP device for your dev kit.
2. Add RTL created from HLS or hand-written Verilog/HDL to the project.
3. Set pin constraints and timing constraints according to your dev kit's documentation.
4. Run Analysis & Synthesis, Fitter, and generate the `.sof` or `.rbf` bitstream.
5. Program the board using Quartus programmer or `usb_blaster` tools.

Example placeholder command (replace with actual paths):
```
# inside WinTel environment, once project is configured
quartus_sh --flow compile my_project
quartus_pgm -c USB-Blaster -m JTAG -o "p;output_files/my_project.sof"
```

We'll add a `make synth-fpga` target that calls a local script which will invoke the HLS/Quartus flow once you have the toolchain available.

Hardware-in-loop test: you can run `make test-fpga-uart` to exercise the host UART program against a connected Cyclone 10 LP dev board. The test is skip-aware and will exit cleanly when no serial device is present.