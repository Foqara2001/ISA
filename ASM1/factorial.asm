    # Load n from memory[0x100] into $a0
    lw $a0, $zero, $imm, 0x100

    # Call factorial
    jal $ra, $imm, $zero, FACTORIAL

    # Store result in memory[0x101]
    sw $v0, $zero, $imm, 0x101

    # End program
    halt $zero, $zero, $zero, 0

# --- Recursive factorial implementation ---
FACTORIAL:
    # if ($a0 == 0) goto BASE
    beq $imm, $zero, $a0, BASE

    # Allocate stack space: $sp = $sp - 2
    sub $sp, $sp, $imm, 1
    sw $a0, $sp, $zero, 0        # save $a0
    sub $sp, $sp, $imm, 1
    sw $ra, $sp, $zero, 0        # save $ra

    # $a0 = $a0 - 1
    sub $a0, $a0, $imm, 1
    jal $ra, $imm, $zero, FACTORIAL   # recursive call

    # Restore $ra
    lw $ra, $sp, $zero, 0
    add $sp, $sp, $imm, 1

    # Restore $a0
    lw $a0, $sp, $zero, 0
    add $sp, $sp, $imm, 1

    # $v0 = $v0 * $a0
    mul $v0, $v0, $a0, 0

    # Return
    beq $ra, $zero, $zero, 0    # simulate jr $ra

BASE:
    # Base case: $v0 = 1
    add $v0, $imm, $zero, 1

    # Return
    beq $ra, $zero, $zero, 0    # simulate jr $ra


.word 0x100 6
