#ifndef __SIMULATE_H__
#define __SIMULATE_H__

#include "memory.h"
#include "assembly.h"
#include <stdio.h>

#define FMT_I3  3
#define FMT_I19  19
#define FMT_I27 27
#define FMT_I103 103
#define FMT_I115  115

#define FMT_U23  23
#define FMT_U55  55

#define FMT_S35  35

#define FMT_R51  51
#define FMT_R59  59

#define FMT_SB99  99

#define FMT_UJ111  111

#define LB 0
#define LH 1
#define LW 2
#define LD 3
#define LBU 4
#define LHU 5
#define LWU 6

#define ADDI 0
#define SLLI 1
#define SLTI 2
#define SLTIU 3
#define XORI 4
#define SRLI 0
#define SRAI 32
#define ORI 6
#define ANDI 7

#define ADDIW 0
#define SLLIW 1
#define SRLIW 0
#define SRAIW 32

#define ECALL 0
#define EBREAK 1

#define SB 0
#define SH 1
#define SW 2
#define SD 3

#define ADD 0
#define SUB 32
#define SLL 1
#define SLT 2
#define SLTU 3
#define XOR 4
#define SRL 0
#define SRA 32
#define OR 6
#define AND 0


#define ADDW 0
#define SUBW 32
#define SSLW 1
#define SRLW 0
#define SRAW 32


#define BEQ 0
#define BNE 1
#define BLT 4
#define BGE 5
#define BLTU 6
#define BGEU 7

// MUL IMPLEMENTATION
#define REMU 1

// Simuler RISC-V program i givet lager og fra given start adresse
long int simulate(struct memory *mem, struct assembly *as, int start_addr, FILE *log_file);

#endif
