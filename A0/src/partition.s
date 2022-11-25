        .text
partition:
        # array in a0
        # p in a1
        # r in a2

        # ADD CODE HERE
        
        slli t0, a1, 2 # p*2^2 (p*4) to get the address)
        add t0, t0, a0 # array address + p addres (array[p])
        lw t0, 0(t0) # // t0 == POVIT
        addi t1, a1, -1 # insert p in a1 into t1 (t1 = i = p-1) // t1 == i == p-1
        addi t2, a2, 0 # insert r in a2 into t2 (t2 = j = r)	// t2 == j == r

while1:

dowhile1: #first do-while loop
	addi t2, t2, -1 # j--
	slli t3, t2, 2 # j*2^2 (j*4) to get the address)
        add t3, t3, a0 # array address + j address (array[j])
        lw t5, 0(t3) # array[j] value
	blt t0, t5, dowhile1 # if pivot < array[j] go to dowhile1 again
	
dowhile2:
	addi t1, t1, 1 # i++
	slli t4, t1, 2 # i*2^2 (i*4) to get the address)
        add t4, t4, a0 # array address + i addres (array[i])
        lw t6, 0(t4) # array[i] value
	blt t6, t0, dowhile2 # if array[i] < pivot go to dowhile2 again


bge t1, t2, else # if i >= j go to else

ifless: # this should set i = j and j = i (swap values)
	sw t5, 0(t4) # array[i] address == array[j] value
	sw t6, 0(t3) # array [j] address == temp (array[i]) value
	
jal zero, while1 # go back to original while loop

else:
	addi t2, t2, 1
	mv a0, t2
	jalr zero, ra, 0


# int partition(int *array, int p, int r) {
#	int pivot = array[p];
#	int i=p-1;
#	int j=r;

#	while (1) {
#		do { j--; } while (pivot < array[j]);
#		do { i++; } while (array[i] < pivot);
#		if (i < j) {
#			int tmp = array[i];
#			array[i] = array[j];
#			array[j] = tmp;
#		} else {
#			return j+1;
#		}
#	}
#}
