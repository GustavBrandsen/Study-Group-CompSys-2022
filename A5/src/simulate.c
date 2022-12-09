#include "simulate.h"
#include <stdio.h>
long int simulate(struct memory *mem, struct assembly *as, int start_addr, FILE *log_file)
{
    int p = start_addr;
    int insn;
    while (1) {
        // We get the instruction (insn)
        insn = memory_rd_w(mem, p);
        p += 4; // We increment the pointer as we start at "_start"

        // We want to decode the instruction (insn) to variables
        int opcode = insn & 127; // Max 127 int


        // Decode opcode into FMT
        //int FMT = decode_opcode(opcode);

        if (opcode == (FMT_I3 || FMT_I19 || FMT_I27 || FMT_I103 || FMT_I115))
        {
            int rd = (insn >> 7) & 31; // Start from 7th bit, max 31 int
            int funct3 = (insn >> 12) & 7; // Start from 12th bit, max 7 int
            int rs1 = (insn >> 15) & 31; // Start from 15th bit, max 31 int
            int imm = (insn >> 20) & 4095; // Start from 20th bit, max 4095 int

            FMT_I(opcode, rd, funct3, rs1, imm);

        } else if (opcode == (FMT_U23 || FMT_U55))
        {
            int rd = (insn >> 7) & 31; // Start from 7th bit, max 31 int
            int imm = (insn >> 12) & 1048576; // Start from 20th bit, max 4095 int

            FMT_U(opcode, rd, imm);

        } else if (opcode == FMT_S35) //VEND TILBAGE
        {
            int imm1 = (insn >> 7) & 31; // Start from 7th bit, max 31 int
            int funct3 = (insn >> 12) & 7; // Start from 12th bit, max 7 int
            int rs1 = (insn >> 15) & 31; // Start from 15th bit, max 31 int
            int rs2 = (insn >> 20) & 31; // Start from 20th bit, max 31 int
            int imm2 = (insn >> 25) & 4095; // Start from 20th bit, max 4095 int
            int imm = (imm2 << 5) + imm1;

            FMT_S(opcode, funct3, rs1, rs2, imm);

        } else if (opcode == (FMT_R51 || FMT_R59))
        {
            int rd = (insn >> 7) & 31; // Start from 7th bit, max 31 int
            int funct3 = (insn >> 12) & 7; // Start from 12th bit, max 7 int
            int rs1 = (insn >> 15) & 31; // Start from 15th bit, max 31 int
            int rs2 = (insn >> 20) & 31; // Start from 20th bit, max 31 int
            int funct7 = (insn >> 25) & 127; // Start from 25th bit, max 127 int

            FMT_R(opcode, rd, funct3, rs1, rs2, funct7);

        } else if (opcode == FMT_SB99)
        {
            int imm1 = (insn >> 7) & 31; // Start from 7th bit, max 31 int
            int funct3 = (insn >> 12) & 7; // Start from 12th bit, max 7 int
            int rs1 = (insn >> 15) & 31; // Start from 15th bit, max 31 int
            int rs2 = (insn >> 20) & 31; // Start from 20th bit, max 31 int
            int imm2 = (insn >> 25) & 4095; // Start from 20th bit, max 4095 int
            int imm = (imm2 << 5) + imm1;

            FMT_SB(opcode, funct3, rs1, rs2, imm);

        } else if (opcode == FMT_UJ111)
        {
            int rd = (insn >> 7) & 31; // Start from 7th bit, max 31 int
            int imm = (insn >> 12) & 1048576; // Start from 20th bit, max 4095 int

            FMT_UJ(opcode, rd, imm);

        } else
        {
            printf("FMT cannot be decoded");
        }
    }

}

// char *decode_opcode(opcode)
// {
//     int FMT;
//     switch (opcode)
//     {
//         case 0b0000011:
//             FMT = FMT_I3;
//             break;

//         case 0010011:
//             FMT = FMT_I19;
//             break;

//         case 0011011:
//             FMT = FMT_I27;
//             break;

//         case 1100111:
//             FMT = FMT_I103;
//             break;

//         case 1110011:
//             FMT = FMT_I115;
//             break;

//         case 0010111:
//             FMT = FMT_U23;
//             break;

//         case 0110111:
//             FMT = FMT_U55;
//             break;

//         case 0100011:
//             FMT = FMT_S35;
//             break;

//         case 0110011:
//             FMT = FMT_R51;
//             break;

//         case 0111011:
//             FMT = FMT_R59;
//             break;

//         case 1100011:
//             FMT = FMT_SB99;
//             break;

//         case 1101111:
//             FMT = FMT_UJ111;
//             break;

//         default:
//             break;
//     }
//     return FMT;
// }



int FMT_I(opcode, rd, funct3, rs1, imm)
{
    switch (opcode)
    {
        case FMT_I3:
            switch (funct3)
            {
                case LB:
                    break;

                case LH:
                    break;

                case LW:
                    break;

                case LD:
                    break;

                case LBU:
                    break;

                case LHU:
                    break;

                case LWU:
                    break;

                default:
                    break;
            }
            break;

        case FMT_I19:
            switch (funct3)
            {
                case ADDI:
                    break;

                case SLLI:
                    break;

                case SLTI:
                    break;

                case SLTIU:
                    break;

                case XORI:
                    break;

                case 5:
                    switch (imm)
                    {
                        case SRLI:
                            break;

                        case SRAI:
                            break;

                        default:
                            break;
                    }
                    break;

                case ORI:
                    break;

                case ANDI:
                    break;

                default:
                    break;
            }
            break;

        case FMT_I27:
            switch (funct3)
            {
                case ADDIW:
                    break;
                case SLLIW:
                    break;
                case 5:
                    switch (imm)
                    {
                        case SRLIW:
                            break;

                        case SRAIW:
                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
            break;

        case FMT_I103: // JALR
            break;

        case FMT_I115:
            switch (imm)
            {
                case ECALL:
                    break;

                case EBREAK:
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }
    return 1;
}
int FMT_U(opcode, rd, imm)
{
    switch (opcode)
    {
        case FMT_U23: //AUIPC
            break;

        case FMT_U55: //LUI
            break;

        default:
            break;
    }
    return 1;
}
int FMT_S(opcode, funct3, rs1, rs2, imm)
{
    switch (funct3)
    {
        case SB:
            break;

        case SH:
            break;

        case SW:
            break;

        case SD:
            break;

        default:
            break;
    }
    return 1;
}
int FMT_R(opcode, rd, funct3, rs1, rs2, funct7)
{
    switch (opcode)
    {
        case FMT_R51:
            switch (funct3)
            {
                case 0:
                    switch (funct7)
                    {
                        case ADD:
                            break;

                        case SUB:
                            break;

                        default:
                            break;
                    }
                    break;

                case 1: //SLL
                    break;

                case 2: //SLT
                    break;

                case 3: //SLTU
                    break;

                case 4: //XOR
                    break;

                case 5:
                    switch (funct7)
                    {
                        case SRL:
                            break;

                        case SRA:
                            break;

                        default:
                            break;
                    }
                    break;

                case OR:
                    break;

                case AND:
                    break;

                default:
                    break;
            }
            break;

        case FMT_R59:
            switch (funct3)
            {
                case 0:
                    switch (funct7)
                    {
                        case ADDW:
                            break;

                        case SUBW:
                            break;

                        default:
                            break;
                    }
                    break;

                case SSLW:
                    break;

                case 5:
                    switch (funct7)
                    {
                        case SRLW:
                            break;

                        case SRAW:
                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }
    return 1;
}
int FMT_SB(opcode, funct3, rs1, rs2, imm)
{
    switch (funct3)
    {
        case BEQ:
            break;

        case BNE:
            break;

        case BLT:
            break;

        case BGE:
            break;

        case BLTU:
            break;

        case BGEU:
            break;

        default:
            break;
    }
    return 1;
}
int FMT_UJ(opcode, rd, imm)
{
    switch (opcode)
    {
        case FMT_UJ111: //JAL
            break;

        default:
            break;
    }
    return 1;
}

