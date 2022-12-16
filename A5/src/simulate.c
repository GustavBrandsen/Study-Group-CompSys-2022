//mangler sllw
#include "simulate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


long int simulate(struct memory *mem, struct assembly *as, int start_addr, FILE *log_file)
{
    int pc = start_addr - 4;
    int r[32] = { 0 };
    long int num_insns = 0;

    int rd;
    int funct3;
    int funct7;
    int rs1;
    int rs2;
    unsigned int imm1;
    unsigned int imm2;
    unsigned int imm3;
    unsigned int imm4;
    unsigned int imm;

    int insn = memory_rd_w(mem, pc);

    int i = 0;
    while (i < 30) {
        pc = pc + 4;
        printf("\nSTART PC : %x\n", pc);
        i += 1;
        // We get the instruction(insn)
        insn = memory_rd_w(mem, pc);
        // printf("insn: %d \n", memory_rd_w(mem, pc));


        // We want to decode the instruction (insn) to variables
        int opcode = insn & 127; // Max 127 int
        //

        if (opcode == FMT_I3 || opcode == FMT_I19 || opcode == FMT_I27 || opcode == FMT_I103 || opcode == FMT_I115) {
            rd = (insn >> 7) & 31; // Start from 7th bit, max 31 int
            funct3 = (insn >> 12) & 7; // Start from 12th bit, max 7 int
            rs1 = (insn >> 15) & 31; // Start from 15th bit, max 31 int
            imm = (insn >> 20) & 4095; // Start from 20th bit, max 4095 int

            switch (opcode)
            {
                case FMT_I3:
                    switch (funct3)
                    {
                        case LB:
                            // check if address is within bounds of memory array
                            printf("LB\n");
                            r[rd] = 0b00000000000000000000000000000000 | memory_rd_b(mem, r[rs1] + imm);
                            break;

                        case LH:
                            printf("LH\n");
                            r[rd] = 0b00000000000000000000000000000000 | memory_rd_h(mem, r[rs1] + imm);
                            break;

                        case LW:
                            printf("LW\n");
                            r[rd] = memory_rd_w(mem, r[rs1] + imm);
                            break;

                        case LBU:
                            printf("LBU\n");
                            r[rd] = 0b00000000000000000000000000000000 | memory_rd_b(mem, r[rs1] + imm);
                            break;

                        case LHU:
                            printf("LHU\n");
                            r[rd] = 0b00000000000000000000000000000000 | memory_rd_h(mem, r[rs1] + imm);
                            break;

                        default:
                            break;
                    }
                    break;

                case FMT_I19:
                    switch (funct3)
                    {
                        case ADDI:
                            printf("ADDI\n");
                            r[rd] = r[rs1] + imm;
                            break;

                        case SLLI:
                            printf("SLLI\n");
                            r[rd] = r[rs1] << imm;
                            break;

                        case SLTI:
                            printf("SLTI\n");
                            r[rd] = (r[rs1] < imm) ? 1 : 0;
                            break;

                        case SLTIU:
                            printf("SLTIU\n");
                            r[rd] = (r[rs1] < imm) ? 1 : 0;
                            break;

                        case XORI:
                            printf("XORI\n");
                            r[rd] = r[rs1] ^ imm;
                            break;

                        case 5:
                            switch (imm)
                            {
                                case SRLI:
                                    printf("SRLI\n");
                                    r[rd] = r[rs1] >> imm;
                                    break;

                                case SRAI:
                                    printf("SRAI\n");
                                    r[rd] = r[rs1] >> imm;
                                    break;

                                default:
                                    break;
                            }
                            break;

                        case ORI:
                            printf("ORI\n");
                            r[rd] = r[rs1] || imm;
                            break;

                        case ANDI:
                            printf("ANDI\n");
                            r[rd] = r[rs1] && imm;
                            break;

                        default:
                            break;
                    }
                    break;

                case FMT_I103: // JALR 
                    printf("JALR\n");
                    r[rd] = pc + 4;
                    pc = (r[rs1] + imm) - 4;
                    //r[rd] = r[rs1] - 4;
                    printf("PC: %d \n", pc);
                    printf("r[rs1]: %d\n", r[rs1]);
                    printf("r[rs2]: %d\n", r[rs2]);
                    printf("IMM: %d\n", imm);
                    break;

                case FMT_I115:
                    switch (imm)
                    {
                        case ECALL:
                            printf("r[17]: %d\n", r[17]);
                            switch (r[17])
                            {
                                case 1:
                                    r[17] = getchar();
                                    break;

                                case 2:
                                    putchar(r[16]);
                                    break;

                                case 3:
                                    return num_insns;
                                    break;

                                case 93:
                                    return num_insns;
                                    break;

                                default:
                                    break;
                            }
                            printf("\nECALL\n");
                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }

        } else if (opcode == FMT_U23 || opcode == FMT_U55) {
            rd = (insn >> 7) & 31; // Start from 7th bit, max 31 int
            imm = (insn >> 12); // Start from 20th bit, max 4095 int MAYBE "<< 12"

            switch (opcode)
            {
                case FMT_U23: //AUIPC 
                    printf("AUIPC\n");
                    r[rd] = pc + imm;
                    break;

                case FMT_U55: //LUI 
                    printf("LUI\n");
                    r[rd] = imm;
                    break;

                default:
                    break;
            }
        } else if (opcode == FMT_S35) {
            imm1 = (insn >> 7) & 31; // Start from 7th bit, max 31 int
            funct3 = (insn >> 12) & 7; // Start from 12th bit, max 7 int
            rs1 = (insn >> 15) & 31; // Start from 15th bit, max 31 int
            rs2 = (insn >> 20) & 31; // Start from 20th bit, max 31 int
            imm2 = (insn >> 25) & 4095; // Start from 20th bit, max 4095 int
            imm = (imm2 << 5) | imm1;

            switch (funct3)
            {
                case SB:
                    printf("SB\n");
                    memory_wr_b(mem, r[rs1] + imm, r[rs2]);
                    break;

                case SH:
                    printf("SH\n");
                    memory_wr_h(mem, r[rs1] + imm, r[rs2]);
                    break;

                case SW:
                    printf("SW\n");
                    memory_wr_w(mem, r[rs1] + imm, r[rs2]);
                    break;

                default:
                    break;
            }
        } else if (opcode == FMT_R51 || opcode == FMT_R59) {
            rd = (insn >> 7) & 31; // Start from 7th bit, max 31 int
            funct3 = (insn >> 12) & 7; // Start from 12th bit, max 7 int
            rs1 = (insn >> 15) & 31; // Start from 15th bit, max 31 int
            rs2 = (insn >> 20) & 31; // Start from 20th bit, max 31 int
            funct7 = (insn >> 25) & 127; // Start from 25th bit, max 127 int

            switch (opcode)
            {
                case FMT_R51:
                    switch (funct3)
                    {
                        case 0:
                            switch (funct7)
                            {
                                case ADD:
                                    printf("ADD\n");
                                    r[rd] = r[rs1] + r[rs2];
                                    break;

                                case SUB:
                                    printf("SUB\n");
                                    r[rd] = r[rs1] - r[rs2];
                                    break;

                                default:
                                    break;
                            }
                            break;

                        case 1: //SLL 
                            printf("SLL\n");
                            r[rd] = r[rs1] << r[rs2];
                            break;

                        case 2: //SLT 
                            printf("SLT\n");
                            r[rd] = (r[rs1] < r[rs2]) ? 1 : 0;
                            break;

                        case 3: //SLTU 
                            printf("SLTU\n");
                            r[rd] = (r[rs1] < r[rs2]) ? 1 : 0;
                            break;

                        case 4: //XOR 
                            printf("XOR\n");
                            r[rd] = r[rs1] ^ r[rs2];
                            break;

                        case 5:
                            switch (funct7)
                            {
                                case SRL:
                                    printf("SRL\n");
                                    r[rd] = r[rs1] >> r[rs2];
                                    break;

                                case SRA:
                                    printf("SRA\n");
                                    r[rd] = r[rs1] >> r[rs2];
                                    break;

                                default:
                                    break;
                            }
                            break;

                        case OR:
                            printf("OR\n");
                            r[rd] = r[rs1] || r[rs2];
                            break;

                        case 7:
                            switch (funct7)
                            {
                                case AND:
                                    printf("AND\n");
                                    r[rd] = r[rs1] && r[rs2];
                                    break;

                                case REMU:
                                    printf("REMU\n");
                                    r[rd] = r[rs1] % r[rs2];
                                    break;

                                default:
                                    break;
                            }
                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
        } else if (opcode == FMT_SB99) {
            funct3 = (insn >> 12) & 7; // Start from 12th bit, max 7 int
            rs1 = (insn >> 15) & 31; // Start from 15th bit, max 31 int
            rs2 = (insn >> 20) & 31; // Start from 20th bit, max 31 int
            // imm1 = (insn >> 7) & 31; // Start from 7th bit, max 31 int
            // imm2 = (insn >> 25) & 63; // Start from 20th bit, max 4095 int
            imm1 = (insn >> 7) & 1;
            imm2 = (insn >> 8) & 15;
            imm3 = (insn >> 25) & 63;
            imm4 = (insn >> 31) & 1;
            // imm = ((imm4 << 12) | (imm1 << 11) | (imm3 << 5) | (imm2 << 1) | 0);
            printf("INSN: %d\n", insn);
            imm = ((((imm4 << 1) | (imm1)) << 6 | imm3) << 4 | imm2) << 1;
            printf("SB IMM: %d\n", imm);

            switch (funct3)
            {
                case BEQ:
                    printf("BEQ\n");
                    if (r[rs1] == r[rs2])
                    {
                        pc = pc + imm;
                    }
                    break;

                case BNE:
                    printf("BNE\n");
                    if (r[rs1] != r[rs2])
                    {
                        pc = pc + imm;
                    }
                    break;

                case BLT:
                    printf("BLT\n");
                    if (r[rs1] < r[rs2])
                    {
                        pc = pc + imm;
                    }
                    break;

                case BGE:
                    printf("BGE\n");
                    if (r[rs1] >= r[rs2])
                    {
                        pc = pc + imm;
                    }
                    break;

                case BLTU:
                    printf("BLTU\n");
                    if (r[rs1] < r[rs2])
                    {
                        pc = pc + imm;
                    }
                    break;

                case BGEU:
                    printf("BGEU\n");
                    if (r[rs1] >= r[rs2])
                    {
                        pc = pc + imm;
                    }
                    break;

                default:
                    break;
            }
        } else if (opcode == FMT_UJ111) {
            rd = (insn >> 7) & 31; // Start from 7th bit, max 31 int
            imm1 = ((insn >> 12) & 0b011111111); // Start from 20th bit, max 4095 int
            imm2 = ((insn >> 20) & 0b01);
            imm3 = ((insn >> 21) & 0b01111111111);
            imm4 = (insn >> 31) & 0b1;
            imm = ((imm1 << 12) | (imm2 << 11) | (imm3 << 1) | 0);

            if (imm4)
            {
                imm = imm | -(imm4 << 20);
            } else {
                imm = imm | (imm4 << 20);
            }


            // printf("[10:1]: %d\n", imm3);
            // printf("[11]: %d\n", imm2);
            // printf("[19:12]: %d\n", imm1);
            // printf("[20]: %d\n", imm4);

            switch (opcode)
            {
                case FMT_UJ111: //JAL 
                    printf("JAL\n");
                    r[rd] = pc + 4;
                    pc = pc + imm - 4;
                    printf("PC: %d\n", pc);
                    break;

                default:
                    break;
            }
        } else {
            //printf("WE SHIT \n\n");
        }
    }
}