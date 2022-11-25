.include "partition.s"

        .text
quicksort:
        # a0 stores array
        # a1 stores start
        # a2 stores end

        # ADD CODE HERE
        
        addi sp, sp, -20 #make room for 5 saves
        sw a0, 0(sp) #save a0 in 0(sp)
        sw a1, 4(sp) #save a1 in 4(sp)
        sw a2, 8(sp) #save a2 in 8(sp)
        sw ra, 12(sp) #save ra in 12(sp)
        
	sub t0, a2, a1 # t0 == end-start
	li t1, 2 # t1 == 2
	blt t0, t1, restore # if end-start (t0) < 2
        
qs:
	jal ra, partition #run partition
	sw a0, 16(sp) #the result from partition is saved in a0, so we save a0 in 16(sp)

	lw a0, 0(sp) #insert the array into a0 again
	lw a1, 4(sp) #insert start value into a1 again
	lw a2, 16(sp) #we load the result from partition (q) into a2 (end value)
	jal ra, quicksort #we run quicksort with quicksort(array, start, q)
	
	lw a1, 16(sp) #we load the result from partition (q) into a1 (start)
	lw a0, 0(sp)
	lw a2, 8(sp) #we load the old a2 into a2
	jal ra, quicksort #we run quicksort with quicksort(array, q, end)
	
restore:
	lw ra, 12(sp)
	addi sp, sp, 20
        jalr zero, ra, 0
        
#void quicksort(int *array, int start, int end) {
#	if (end-start < 2) {
#		return;
#	}
#	int q = partition(array, start, end);
#	quicksort(array, start, q);
#	quicksort(array, q, end);
#}