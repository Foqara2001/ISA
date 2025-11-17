# ISA
instruction set architecture for mips

# Project Overview
This project simulates a simplified RISC processor named SIMP. The system includes an assembler written in C (ASM1.c), a cycle-accurate simulator (SIM1.c), and multiple assembly programs. These programs demonstrate core processor functionality such as recursion, memory manipulation, disk I/O, interrupt handling, and graphical operations on a simulated monitor.
# 1. Assembler (ASM1.c)
The assembler reads assembly source files, identifies labels, instructions, and data directives, and generates a hexadecimal instruction memory file (memin.txt). It handles the parsing and encoding of each instruction, including differentiating between short (imm8) and extended (imm32) immediates.
Features include:
- Parsing word directives to hex data
- Label resolution and back-patching
- Instruction format encoding based on opcode and operands
- Supporting branching with symbolic labels
- Distinguishing between immediate sizes (8-bit - 32-bit)

  # 2. Simulator (SIM1.c)
The simulator emulates the SIMP processor using instruction and hardware logic defined in C. It performs fetch-decode-execute cycles, handles interrupts, manages I/O devices (disk, timer, LEDs, 7-seg, monitor), and writes the output to several trace and state files.
Key Features:
- Instruction decoding and execution using dispatch tables
- Full interrupt support (irq0, irq1, irq2) and reti
- Hardware register I/O through in/out instructions
- Cycle counter and PC management with support for branching
- Output file generation (trace.txt, regout.txt, monitor.txt, etc.)

