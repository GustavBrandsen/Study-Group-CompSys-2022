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
    int imm1;
    int imm2;
    int imm3;
    int imm4;
    int imm;

    char jump_var[3] = "";
    char cond_jump_var[4] = "";

    int insn = memory_rd_w(mem, pc);

    fprintf(log_file, "\n\n\n\n ITERATION LOG: \n\n");
    fflush(log_file);
    char log_output[32];
    char *r_NAMES[32] = { "zero","ra","sp","gp","tp","t0","t1","t2","s0","s1","a0","a1","a2","a3","a4","a5","a6","a7","s2","s3","s4","s5","s6","s7","s8","s9","s10","s11","t3","t4","t5","t6" };
    int i = -1;
    while (1) {
        i += 1;
        pc = pc + 4;
        num_insns += 1;

        // We get the instruction(insn)
        insn = memory_rd_w(mem, pc);

        // We want to decode the instruction (insn) to variables
        int opcode = insn & 127; // Max 127 int

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
                            r[rd] = memory_rd_b(mem, (r[rs1] + imm));
                            sprintf((char *)&log_output, "%s,%d(%s)", r_NAMES[rd], imm, r_NAMES[rs1]);
                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "LB", log_output, cond_jump_var, rd, r[rd]);
                            fflush(log_file);
                            strcpy(jump_var, "");
                            strcpy(cond_jump_var, "");
                            break;

                        case LH:
                            r[rd] = memory_rd_h(mem, (r[rs1] + imm));
                            sprintf((char *)&log_output, "%s,%d(%s)", r_NAMES[rd], imm, r_NAMES[rs1]);
                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "LH", log_output, cond_jump_var, rd, r[rd]);
                            fflush(log_file);
                            strcpy(jump_var, "");
                            strcpy(cond_jump_var, "");
                            break;

                        case LW:
                            r[rd] = memory_rd_w(mem, (r[rs1] + imm));
                            sprintf((char *)&log_output, "%s,%d(%s)", r_NAMES[rd], imm, r_NAMES[rs1]);
                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "LW", log_output, cond_jump_var, rd, r[rd]);
                            fflush(log_file);
                            strcpy(jump_var, "");
                            strcpy(cond_jump_var, "");
                            break;

                        case LBU:
                            r[rd] = memory_rd_b(mem, (r[rs1] + imm));
                            sprintf((char *)&log_output, "%s,%d(%s)", r_NAMES[rd], imm, r_NAMES[rs1]);
                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "LBU", log_output, cond_jump_var, rd, r[rd]);
                            fflush(log_file);
                            strcpy(jump_var, "");
                            strcpy(cond_jump_var, "");
                            break;

                        case LHU:
                            r[rd] = memory_rd_h(mem, (r[rs1] + imm));
                            sprintf((char *)&log_output, "%s,%d(%s)", r_NAMES[rd], imm, r_NAMES[rs1]);
                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "LHU", log_output, cond_jump_var, rd, r[rd]);
                            fflush(log_file);
                            strcpy(jump_var, "");
                            strcpy(cond_jump_var, "");
                            break;

                        default:
                            break;
                    }
                    break;

                case FMT_I19:
                    switch (funct3)
                    {
                        case ADDI:
                            r[rd] = r[rs1] + imm;
                            sprintf((char *)&log_output, "%s = %s + %d", r_NAMES[rd], r_NAMES[rs1], imm);
                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "ADDI", log_output, cond_jump_var, rd, r[rd]);
                            fflush(log_file);
                            strcpy(jump_var, "");
                            strcpy(cond_jump_var, "");
                            break;

                        case SLLI:
                            r[rd] = r[rs1] << imm;
                            sprintf((char *)&log_output, "%s = %s << %d", r_NAMES[rd], r_NAMES[rs1], imm);
                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "SLLI", log_output, cond_jump_var, rd, r[rd]);
                            fflush(log_file);
                            strcpy(jump_var, "");
                            strcpy(cond_jump_var, "");
                            break;

                        case SLTI:
                            r[rd] = (r[rs1] < imm) ? 1 : 0;
                            sprintf((char *)&log_output, "%s = %s < %d", r_NAMES[rd], r_NAMES[rs1], imm);
                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "SLTI", log_output, cond_jump_var, rd, r[rd]);
                            fflush(log_file);
                            strcpy(jump_var, "");
                            strcpy(cond_jump_var, "");
                            break;

                        case SLTIU:
                            r[rd] = (r[rs1] < imm) ? 1 : 0;
                            sprintf((char *)&log_output, "%s = %s < %d", r_NAMES[rd], r_NAMES[rs1], imm);
                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "SLTIU", log_output, cond_jump_var, rd, r[rd]);
                            fflush(log_file);
                            strcpy(jump_var, "");
                            strcpy(cond_jump_var, "");
                            break;

                        case XORI:
                            r[rd] = r[rs1] ^ imm;
                            sprintf((char *)&log_output, "%s = %s ^ %d", r_NAMES[rd], r_NAMES[rs1], imm);
                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "XORI", log_output, cond_jump_var, rd, r[rd]);
                            fflush(log_file);
                            strcpy(jump_var, "");
                            strcpy(cond_jump_var, "");
                            break;

                        case 5:
                            switch (imm)
                            {
                                case SRLI:
                                    r[rd] = r[rs1] >> imm;
                                    sprintf((char *)&log_output, "%s = %s >> %d", r_NAMES[rd], r_NAMES[rs1], imm);
                                    fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "SRLI", log_output, cond_jump_var, rd, r[rd]);
                                    fflush(log_file);
                                    strcpy(jump_var, "");
                                    break;

                                case SRAI:
                                    r[rd] = r[rs1] >> imm;
                                    sprintf((char *)&log_output, "%s >> %s + %d", r_NAMES[rd], r_NAMES[rs1], imm);
                                    fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "SRAI", log_output, cond_jump_var, rd, r[rd]);
                                    fflush(log_file);
                                    strcpy(cond_jump_var, "");
                                    break;

                                default:
                                    break;
                            }
                            break;

                        case ORI:
                            r[rd] = r[rs1] | imm;
                            sprintf((char *)&log_output, "%s | %s + %d", r_NAMES[rd], r_NAMES[rs1], imm);
                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "ORI", log_output, cond_jump_var, rd, r[rd]);
                            fflush(log_file);
                            strcpy(jump_var, "");
                            strcpy(cond_jump_var, "");
                            break;

                        case ANDI:
                            r[rd] = r[rs1] & imm;
                            sprintf((char *)&log_output, "%s & %s + %d", r_NAMES[rd], r_NAMES[rs1], imm);
                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "ANDI", log_output, cond_jump_var, rd, r[rd]);
                            fflush(log_file);
                            strcpy(jump_var, "");
                            strcpy(cond_jump_var, "");
                            break;

                        default:
                            break;
                    }
                    break;

                case FMT_I103: // JALR 
                    sprintf((char *)&log_output, "pc = %s + %d", r_NAMES[rs1], imm);
                    fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s\n", i, jump_var, pc, insn, "JALR", log_output, cond_jump_var);
                    fflush(log_file);
                    strcpy(jump_var, "=>");
                    strcpy(cond_jump_var, "");
                    pc = (r[rs1] + imm) - 4;
                    break;

                case FMT_I115:
                    switch (imm)
                    {
                        case ECALL:
                            switch (r[17])
                            {
                                case 1:
                                    r[17] = getchar();
                                    sprintf((char *)&log_output, "%s = %d", r_NAMES[17], r[17]);
                                    break;

                                case 2:
                                    putchar(r[16]);
                                    if (r[16] == 10) {
                                        sprintf((char *)&log_output, "PRINT: %s", "\\n");
                                    } else {
                                        sprintf((char *)&log_output, "PRINT: %c", (char)r[16]);
                                    }
                                    break;

                                case 3:
                                    sprintf((char *)&log_output, "EXIT");
                                    fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s\n", i, jump_var, pc, insn, "ECALL", log_output, cond_jump_var);
                                    fflush(log_file);
                                    return num_insns;
                                    break;

                                case 93:
                                    sprintf((char *)&log_output, "EXIT");
                                    fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s\n", i, jump_var, pc, insn, "ECALL", log_output, cond_jump_var);
                                    fflush(log_file);
                                    return num_insns;
                                    break;

                                default:
                                    break;
                            }

                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s\n", i, jump_var, pc, insn, "ECALL", log_output, cond_jump_var);
                            fflush(log_file);
                            strcpy(jump_var, "");
                            strcpy(cond_jump_var, "");

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
            imm = (insn >> 12) << 12; // Start from 20th bit, max 4095 int MAYBE "<< 12"

            switch (opcode)
            {
                case FMT_U23: //AUIPC 
                    r[rd] = pc - 4 + imm;
                    sprintf((char *)&log_output, "%s = pc + %d", r_NAMES[rd], imm);
                    fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "AUIPC", log_output, cond_jump_var, rd, r[rd]);
                    fflush(log_file);
                    strcpy(jump_var, "");
                    strcpy(cond_jump_var, "");
                    break;

                case FMT_U55: //LUI 
                    r[rd] = imm;
                    sprintf((char *)&log_output, "%s = %d", r_NAMES[rd], imm);
                    fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "LUI", log_output, cond_jump_var, rd, r[rd]);
                    fflush(log_file);
                    strcpy(jump_var, "");
                    strcpy(cond_jump_var, "");
                    break;

                default:
                    break;
            }
        } else if (opcode == FMT_S35) {
            funct3 = (insn >> 12) & 7; // Start from 12th bit, max 7 int
            rs1 = (insn >> 15) & 31; // Start from 15th bit, max 31 int
            rs2 = (insn >> 20) & 31; // Start from 20th bit, max 31 int
            imm1 = (insn >> 7) & 31; // Start from 7th bit, max 31 int
            imm2 = (insn >> 25) & 4095; // Start from 20th bit, max 4095 int
            imm = (imm2 << 5) | imm1;

            switch (funct3)
            {
                case SB:
                    memory_wr_b(mem, r[rs1] + imm, r[rs2] % (1 << 8));
                    sprintf((char *)&log_output, "M[%d] = %s", r[rs1] + imm, r_NAMES[rs2]);
                    fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "SB", log_output, cond_jump_var, rd, r[rd]);
                    fflush(log_file);
                    strcpy(jump_var, "");
                    strcpy(cond_jump_var, "");
                    break;

                case SH:
                    memory_wr_h(mem, r[rs1] + imm, r[rs2] % (1 << 16));
                    sprintf((char *)&log_output, "M[%d] = %s", r[rs1] + imm, r_NAMES[rs2]);
                    fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "SH", log_output, cond_jump_var, rd, r[rd]);
                    fflush(log_file);
                    strcpy(jump_var, "");
                    strcpy(cond_jump_var, "");
                    break;

                case SW:
                    memory_wr_w(mem, r[rs1] + imm, r[rs2]);
                    sprintf((char *)&log_output, "M[%d] = %s", r[rs1] + imm, r_NAMES[rs2]);
                    fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "SW", log_output, cond_jump_var, rd, r[rd]);
                    fflush(log_file);
                    strcpy(jump_var, "");
                    strcpy(cond_jump_var, "");
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
                                    r[rd] = r[rs1] + r[rs2];
                                    sprintf((char *)&log_output, "%s = %s + %s", r_NAMES[rd], r_NAMES[rs1], r_NAMES[rs2]);
                                    fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "ADD", log_output, cond_jump_var, rd, r[rd]);
                                    fflush(log_file);
                                    strcpy(jump_var, "");
                                    strcpy(cond_jump_var, "");
                                    break;

                                case SUB:
                                    r[rd] = r[rs1] - r[rs2];
                                    sprintf((char *)&log_output, "%s = %s - %s", r_NAMES[rd], r_NAMES[rs1], r_NAMES[rs2]);
                                    fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "SUB", log_output, cond_jump_var, rd, r[rd]);
                                    fflush(log_file);
                                    strcpy(jump_var, "");
                                    strcpy(cond_jump_var, "");
                                    break;

                                default:
                                    break;
                            }
                            break;

                        case 1: //SLL 
                            r[rd] = r[rs1] << r[rs2];
                            sprintf((char *)&log_output, "%s = %s << %s", r_NAMES[rd], r_NAMES[rs1], r_NAMES[rs2]);
                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "SLL", log_output, cond_jump_var, rd, r[rd]);
                            fflush(log_file);
                            strcpy(jump_var, "");
                            strcpy(cond_jump_var, "");
                            break;

                        case 2: //SLT 
                            r[rd] = (r[rs1] < r[rs2]) ? 1 : 0;
                            sprintf((char *)&log_output, "%s = %s < %s", r_NAMES[rd], r_NAMES[rs1], r_NAMES[rs2]);
                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "SLT", log_output, cond_jump_var, rd, r[rd]);
                            fflush(log_file);
                            strcpy(jump_var, "");
                            strcpy(cond_jump_var, "");
                            break;

                        case 3: //SLTU 
                            r[rd] = (r[rs1] < r[rs2]) ? 1 : 0;
                            sprintf((char *)&log_output, "%s = %s < %s", r_NAMES[rd], r_NAMES[rs1], r_NAMES[rs2]);
                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "SLTU", log_output, cond_jump_var, rd, r[rd]);
                            fflush(log_file);
                            strcpy(jump_var, "");
                            strcpy(cond_jump_var, "");
                            break;

                        case 4: //XOR 
                            r[rd] = r[rs1] ^ r[rs2];
                            sprintf((char *)&log_output, "%s = %s ^ %s", r_NAMES[rd], r_NAMES[rs1], r_NAMES[rs2]);
                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "XOR", log_output, cond_jump_var, rd, r[rd]);
                            fflush(log_file);
                            strcpy(jump_var, "");
                            strcpy(cond_jump_var, "");
                            break;

                        case 5:
                            switch (funct7)
                            {
                                case SRL:
                                    r[rd] = r[rs1] >> r[rs2];
                                    sprintf((char *)&log_output, "%s = %s >> %s", r_NAMES[rd], r_NAMES[rs1], r_NAMES[rs2]);
                                    fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "SRL", log_output, cond_jump_var, rd, r[rd]);
                                    fflush(log_file);
                                    strcpy(jump_var, "");
                                    strcpy(cond_jump_var, "");
                                    break;

                                case SRA:
                                    r[rd] = r[rs1] >> r[rs2];
                                    sprintf((char *)&log_output, "%s = %s >> %s", r_NAMES[rd], r_NAMES[rs1], r_NAMES[rs2]);
                                    fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "SRA", log_output, cond_jump_var, rd, r[rd]);
                                    fflush(log_file);
                                    strcpy(jump_var, "");
                                    strcpy(cond_jump_var, "");
                                    break;

                                default:
                                    break;
                            }
                            break;

                        case OR:
                            r[rd] = r[rs1] | r[rs2];
                            sprintf((char *)&log_output, "%s = %s | %s", r_NAMES[rd], r_NAMES[rs1], r_NAMES[rs2]);
                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "OR", log_output, cond_jump_var, rd, r[rd]);
                            fflush(log_file);
                            strcpy(jump_var, "");
                            strcpy(cond_jump_var, "");
                            break;

                        case 7:
                            switch (funct7)
                            {
                                case AND:
                                    r[rd] = r[rs1] & r[rs2];
                                    sprintf((char *)&log_output, "%s = %s & %s", r_NAMES[rd], r_NAMES[rs1], r_NAMES[rs2]);
                                    fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "AND", log_output, cond_jump_var, rd, r[rd]);
                                    fflush(log_file);
                                    strcpy(jump_var, "");
                                    strcpy(cond_jump_var, "");
                                    break;

                                case 1:
                                    switch (funct3)
                                    {
                                        case MUL:
                                            r[rd] = (long int)(r[rs1] * r[rs2]) % ((long int)1 << 32);
                                            sprintf((char *)&log_output, "%s = %s * %s (31:0)", r_NAMES[rd], r_NAMES[rs1], r_NAMES[rs2]);
                                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "MUL", log_output, cond_jump_var, rd, r[rd]);
                                            fflush(log_file);
                                            strcpy(jump_var, "");
                                            strcpy(cond_jump_var, "");
                                            break;

                                        case MULH:
                                            r[rd] = (long int)(r[rs1] * r[rs2]) >> 32;
                                            sprintf((char *)&log_output, "%s = %s * %s (63:32)", r_NAMES[rd], r_NAMES[rs1], r_NAMES[rs2]);
                                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "MULH", log_output, cond_jump_var, rd, r[rd]);
                                            fflush(log_file);
                                            strcpy(jump_var, "");
                                            strcpy(cond_jump_var, "");
                                            break;

                                        case MULHSU:
                                            r[rd] = (long unsigned int)(r[rs1] * r[rs2]) >> 32;
                                            sprintf((char *)&log_output, "%s = %s * %s (63:32)", r_NAMES[rd], r_NAMES[rs1], r_NAMES[rs2]);
                                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "MULHSU", log_output, cond_jump_var, rd, r[rd]);
                                            fflush(log_file);
                                            strcpy(jump_var, "");
                                            strcpy(cond_jump_var, "");
                                            break;

                                        case MULHU:
                                            r[rd] = (long int)(r[rs1] * r[rs2]) >> 32;
                                            sprintf((char *)&log_output, "%s = %s * %s (63:32)", r_NAMES[rd], r_NAMES[rs1], r_NAMES[rs2]);
                                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "MULHU", log_output, cond_jump_var, rd, r[rd]);
                                            fflush(log_file);
                                            strcpy(jump_var, "");
                                            strcpy(cond_jump_var, "");
                                            break;

                                        case DIV:
                                            r[rd] = r[rs1] / r[rs2];
                                            sprintf((char *)&log_output, "%s = %s / %s", r_NAMES[rd], r_NAMES[rs1], r_NAMES[rs2]);
                                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "DIV", log_output, cond_jump_var, rd, r[rd]);
                                            fflush(log_file);
                                            strcpy(jump_var, "");
                                            strcpy(cond_jump_var, "");
                                            break;

                                        case DIVU:
                                            r[rd] = r[rs1] / r[rs2];
                                            sprintf((char *)&log_output, "%s = %s / %s", r_NAMES[rd], r_NAMES[rs1], r_NAMES[rs2]);
                                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "DIVU", log_output, cond_jump_var, rd, r[rd]);
                                            fflush(log_file);
                                            strcpy(jump_var, "");
                                            strcpy(cond_jump_var, "");
                                            break;

                                        case REM:
                                            r[rd] = r[rs1] % r[rs2];
                                            sprintf((char *)&log_output, "%s = %s %% %s", r_NAMES[rd], r_NAMES[rs1], r_NAMES[rs2]);
                                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "REM", log_output, cond_jump_var, rd, r[rd]);
                                            fflush(log_file);
                                            strcpy(jump_var, "");
                                            strcpy(cond_jump_var, "");
                                            break;

                                        case REMU:
                                            r[rd] = r[rs1] % r[rs2];
                                            sprintf((char *)&log_output, "%s = %s %% %s", r_NAMES[rd], r_NAMES[rs1], r_NAMES[rs2]);
                                            fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s R[%2d] <- %x\n", i, jump_var, pc, insn, "REMU", log_output, cond_jump_var, rd, r[rd]);
                                            fflush(log_file);
                                            strcpy(jump_var, "");
                                            strcpy(cond_jump_var, "");
                                            break;

                                        default:
                                            break;
                                    }
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
            imm1 = (insn >> 7) & 1;
            imm2 = (insn >> 8) & 15;
            imm3 = (insn >> 25) & 63;
            imm4 = (insn >> 31) & 1;
            imm = ((((imm1)) << 6 | imm3) << 4 | imm2) << 1;

            if (imm4)
            {
                imm = imm | -(imm4 << 12);
            } else {
                imm = imm | (imm4 << 12);
            }

            switch (funct3)
            {
                case BEQ:
                    if (r[rs1] == r[rs2])
                    {
                        sprintf((char *)&log_output, "pc = pc + %d", imm);
                        fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s\n", i, jump_var, pc, insn, "BEQ", log_output, cond_jump_var);
                        fflush(log_file);
                        strcpy(jump_var, "");
                        strcpy(cond_jump_var, "{T}");
                        pc = pc + imm - 4;
                    } else {
                        sprintf((char *)&log_output, "pc = pc");
                        fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s\n", i, jump_var, pc, insn, "BEQ", log_output, cond_jump_var);
                        fflush(log_file);
                        strcpy(jump_var, "");
                        strcpy(cond_jump_var, "");
                    }
                    break;

                case BNE:
                    if (r[rs1] != r[rs2])
                    {
                        sprintf((char *)&log_output, "pc = pc + %d", imm);
                        fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s\n", i, jump_var, pc, insn, "BNE", log_output, cond_jump_var);
                        fflush(log_file);
                        strcpy(jump_var, "");
                        strcpy(cond_jump_var, "{T}");
                        pc = pc + imm - 4;
                    } else {
                        sprintf((char *)&log_output, "pc = pc");
                        fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s\n", i, jump_var, pc, insn, "BNE", log_output, cond_jump_var);
                        fflush(log_file);
                        strcpy(jump_var, "");
                        strcpy(cond_jump_var, "");
                    }
                    break;

                case BLT:
                    if (r[rs1] < r[rs2])
                    {
                        sprintf((char *)&log_output, "pc = pc + %d", imm);
                        fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s\n", i, jump_var, pc, insn, "BLT", log_output, cond_jump_var);
                        fflush(log_file);
                        strcpy(jump_var, "");
                        strcpy(cond_jump_var, "{T}");
                        pc = pc + imm - 4;
                    } else {
                        sprintf((char *)&log_output, "pc = pc");
                        fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s\n", i, jump_var, pc, insn, "BLT", log_output, cond_jump_var);
                        fflush(log_file);
                        strcpy(jump_var, "");
                        strcpy(cond_jump_var, "");
                    }
                    break;

                case BGE:
                    if (r[rs1] >= r[rs2])
                    {
                        sprintf((char *)&log_output, "pc = pc + %d", imm);
                        fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s\n", i, jump_var, pc, insn, "BGE", log_output, cond_jump_var);
                        fflush(log_file);
                        strcpy(jump_var, "");
                        strcpy(cond_jump_var, "{T}");
                        pc = pc + imm - 4;
                    } else {
                        sprintf((char *)&log_output, "pc = pc");
                        fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s\n", i, jump_var, pc, insn, "BGE", log_output, cond_jump_var);
                        fflush(log_file);
                        strcpy(jump_var, "");
                        strcpy(cond_jump_var, "");
                    }
                    break;

                case BLTU:
                    if (r[rs1] < r[rs2])
                    {
                        sprintf((char *)&log_output, "pc = pc + %d", imm);
                        fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s\n", i, jump_var, pc, insn, "BLTU", log_output, cond_jump_var);
                        fflush(log_file);
                        strcpy(jump_var, "");
                        strcpy(cond_jump_var, "{T}");
                        pc = pc + imm - 4;
                    } else {
                        sprintf((char *)&log_output, "pc = pc");
                        fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s\n", i, jump_var, pc, insn, "BLTU", log_output, cond_jump_var);
                        fflush(log_file);
                        strcpy(jump_var, "");
                        strcpy(cond_jump_var, "");
                    }
                    break;

                case BGEU:
                    if (r[rs1] >= r[rs2])
                    {
                        sprintf((char *)&log_output, "pc = pc + %d", imm);
                        fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s\n", i, jump_var, pc, insn, "BGEU", log_output, cond_jump_var);
                        fflush(log_file);
                        strcpy(jump_var, "");
                        strcpy(cond_jump_var, "{T}");
                        pc = pc + imm - 4;
                    } else {
                        sprintf((char *)&log_output, "pc = pc");
                        fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s\n", i, jump_var, pc, insn, "BGEU", log_output, cond_jump_var);
                        fflush(log_file);
                        strcpy(jump_var, "");
                        strcpy(cond_jump_var, "");
                    }
                    break;

                default:
                    break;
            }
        } else if (opcode == FMT_UJ111) {
            rd = (insn >> 7) & 31; // Start from 7th bit, max 31 int
            imm1 = ((insn >> 12) & 255); // Start from 20th bit, max 4095 int
            imm2 = ((insn >> 20) & 1);
            imm3 = ((insn >> 21) & 1023);
            imm4 = (insn >> 31) & 1;
            imm = ((imm1 << 12) | (imm2 << 11) | (imm3 << 1) | 0);

            if (imm4)
            {
                imm = imm | -(imm4 << 20);
            } else {
                imm = imm | (imm4 << 20);
            }

            switch (opcode)
            {
                case FMT_UJ111: //JAL 
                    r[rd] = pc + 4;
                    sprintf((char *)&log_output, "pc = pc + %d", imm);
                    fprintf(log_file, "%-3d %2s %5x : %8x    %-8s %-20s %-4s\n", i, jump_var, pc, insn, "JAL", log_output, cond_jump_var);
                    fflush(log_file);
                    strcpy(jump_var, "=>");
                    strcpy(cond_jump_var, "");
                    pc = pc + imm - 4;
                    break;

                default:
                    break;
            }
        }
        // We don't use as, so here we just remove the warning
        struct assembly *as2 = as;
        as = as2;
    }
    return num_insns;
}
