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
	
	move $a1, $s0
	move $a2, $s1
	jal GCDrecur	

	li $v0, 1
	move $a0, $t0
	syscall
	j end
	
GCDrecur:
	addi $sp, $sp, -4
	sw $ra, 0($sp)
	
	beq $a2, $zero, finish
	rem $t1, $a1, $a2
	add $a1, $zero, $a2
	add $a2, $zero, $t1
	jal GCDrecur
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	jr $ra

finish:
	add $t0, $zero, $a1
	lw $ra, 0($sp)
	addi $sp, $sp, 4
	jr $ra	
end:

	

	
