.word 0x100 7
.word 0x101 5
.word 0x102 11
.word 0x103 12
.word 0x104 6
.word 0x105 18
.word 0x106 4
.word 0x0107 55
.word 0x108 15
.word 0x109 8
.word 0x10A 10
.word 0x10B 20
.word 0x10C 14
.word 0x10D 9
.word 0x10E 3
.word 0x10F 2

	add $s0, $zero, $imm, 0x100		
	add $s1, $zero, $zero, 0	
Loop:
	add $s2, $zero, $zero, 0	
InLoop:
	lw $t0, $s0, $s2, 0		
	add $t1, $s2, $imm, 1			
	lw $t2, $s0 ,$t1, 0			
	ble $imm, $t0, $t2, endif		
	sw  $t2, $s0, $s2, 0		
	sw $t0, $s0, $t1, 0
endif:							
	add $s2, $s2, $imm, 1			
	add $t0, $zero, $imm, 15		
	sub $t1, $t0, $s1, 0		
	blt $imm, $s2, $t1, InLoop	
	add $s1, $s1, $imm, 1			
	add $t0, $zero, $imm, 15		
	blt $imm, $s1, $t0, Loop	
	halt $zero, $zero, $zero, 0