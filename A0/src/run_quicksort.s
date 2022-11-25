        .data
array:
        .space 4096 # 4 KiB

        .text
test:
        la      a0, array
        jal     ra, read_ints
        mv      s0, a0

        la      a0, array
        addi    a1, zero, 0
        add     a2, zero, s0
        jal     ra, quicksort

        mv      a0, s0
        la      a1, array
        jal     ra, write_ints

        li      a0, 0
        li      a7, 93
        ecall                   # exit

.include "quicksort.s"
.include "io.s"
