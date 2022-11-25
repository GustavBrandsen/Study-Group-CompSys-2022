        .text

# Read 32-bit integers from console, one per line.
# The first line read is an integer indicating how many
# integers will follow.
#
# The integers are written consecutively to memory starting at the
# address stored in a0.  It is assumed that there is enough space.
# The number of integers read is returned in a0.
read_ints:
        mv      t0, a0          # first output addr

        li      a7, 5
        ecall                   # read int into a0
        mv      t2, a0          # total number of elements

        mv      t1, zero        # read so far
read_loop:
        beq     t1, t2, read_done
        li      a7, 5
        ecall                   # read int
        sw      a0, 0(t0)
        addi    t0, t0, 4       # next output addr
        addi    t1, t1, 1       # increment count
        jal     zero, read_loop
read_done:
        mv      a0, t2          # return total number of elements
        jalr    zero, ra, 0

# Write 32-bit integers to console, one per line.
# a0 must contain the number of integers to write.
# a1 must contain the address of the first integer.
write_ints:
        mv      t2, a0          # to go
        mv      t1, zero        # printed so far
        mv      t0, a1          # next addr to read from
write_loop:
        beq     t2, zero, write_done
        lw      a0, 0(t0)
        li      a7, 1
        ecall                   # print int
        li      a0, 10
        li      a7, 11
        ecall                   # print newline
        addi    t0, t0, 4
        addi    t1, t1, 1
        addi    t2, t2, -1
        jal     zero, write_loop
write_done:
        jalr    zero, ra, 0
