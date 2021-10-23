.data 
	msg1: .asciiz"answer "
	msg2: .asciiz"n="
	newline: .asciiz"\n"
	var1: .word 0
.text
.globl main

main:	
	li $v0, 5
	syscall
	add $s0, $v0, $zero
	
	li $v0, 5
	syscall
	add $s1, $v0, $zero
	
	li $v0, 5
	syscall
	add $s2, $v0, $zero
	
	li $v0, 5
	syscall
	add $s3, $v0, $zero
	
	li $v0, 5
	syscall
	add $s4, $v0, $zero
	
	li $v0, 5
	syscall
	add $s5, $v0, $zero
	
	li $v0, 5
	syscall
	add $s6, $v0, $zero
	
	lw $s7, var1
	lw $t0, var1
begin:
	addi $s7, $s7, 1
	
	move $t2, $s0
	beq $s7, 1, loop
	
	move $t2, $s1
	beq $s7, 2, loop
	
	move $t2, $s2
	beq $s7, 3, loop
	
	move $t2, $s3
	beq $s7, 4, loop
	
	move $t2, $s4
	beq $s7, 5, loop
	
	move $t2, $s5
	beq $s7, 6, loop
	
	move $t2, $s6
	beq $s7, 7, loop
	
	li $v0, 1
	move $a0, $t0
	syscall
	
	j end

loop:
	lw $t1, var1
	
	blt $s7, 2, flag
	seq $t1, $s0, $t2
	beq $t1, 1, flag
	
	blt $s7, 3, flag
	seq $t1, $s1, $t2
	beq $t1, 1, flag
	
	blt $s7, 4, flag
	seq $t1, $s2, $t2
	beq $t1, 1, flag
	
	blt $s7, 5, flag
	seq $t1, $s3, $t2
	beq $t1, 1, flag
	
	blt $s7, 6, flag
	seq $t1, $s4, $t2
	beq $t1, 1, flag
	
	blt $s7, 7, flag
	seq $t1, $s5, $t2
	beq $t1, 1, flag
	
	blt $s7, 8, flag
	seq $t1, $s6, $t2
	beq $t1, 1, flag

 	j flag
flag:
	beq $t1, 1, begin
	add $t0, $t0, $t2
	j begin
		
end:

	

	
