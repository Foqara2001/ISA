.word 0x100 4           # מקום לאחסן את n
.word 0x101 4           # מקום לאחסן את k
.word 0x102 0           # מקום לאחסן את התוצאה

   main:
    # טוען את הערכים מהכתובות של n ו-k
    lw   $t0, $zero, $imm, 0x100      # Load n into $t0
    lw   $t1, $zero, $imm, 0x101      # Load k into $t1
    
    # טוען את הערכים לתוך הרגיסטרים המתאימים לפונקציה
    add  $a0, $t0, $zero, 0 # Set $a0 = n
    add  $a1, $t1, $zero, 0 # Set $a1 = k
    
    # קריאה לפונקציית binom
    jal $ra, $imm, $zero, binom
    
    # אחסון התוצאה
    sw $v0, $zero, $imm, 0x102      # Store result in memory location 0x102
    
    # סיום התוכנית
   halt $zero, $zero, $zero, 0

# פונקציה לחישוב

# binomial coefficient
# Arguments: $a0 = n, $a1 = k
# Returns: $v0 = result

binom:
    # Base case: if k == 0 or n == k, return 1
    beq  $imm, $a1, $zero, base_case   # if k == 0
    beq  $imm, $a0, $a1, base_case     # if n == k

    # Recursive case: binom(n-1, k-1) + binom(n-1, k)
    # Save registers
    add $sp, $sp, $imm, -12            # Make room on stack for $ra, $s0, $s1
    sw $ra, $sp, $imm, 8             # Save return address
    sw $s0, $sp, $imm, 4             # Save $s0 (n)
    sw   $s1, $sp, $imm, 0             # Save $s1 (k)

    # binom(n-1, k-1)
    add $a0, $a0, $imm, -1            # n = n - 1
    add $a1, $a1, $imm, -1            # k = k - 1
    beq $imm, $zero, $zero, binom                    # Call binom(n-1, k-1)
    add  $s0, $v0, $zero , 0       # Store result in $s0

    # Restore $a0, $a1
    add $a0, $a0, $imm, 1             # n = n + 1
    add $a1, $a1, $imm, 1             # k = k + 1

    # binom(n-1, k)
    add $a0, $a0, $imm, -1            # n = n - 1
    beq $imm, $zero, $zero, binom                    # Call binom(n-1, k)
    add  $v0, $v0, $s0, 0         # Add result of binom(n-1, k-1) to binom(n-1, k)

    # Restore registers
    lw $ra, $sp, $zero, 8             # Restore return address
    lw $s0, $sp, $imm, 4             # Restore $s0
    lw $s1, $sp, $imm, 0             # Restore $s1
    add $sp, $sp, $imm, 12            # Deallocate stack space
    
    jal $ra, $imm, $zero, 0                     # Return to caller

base_case:
    # שימוש ב-addi כדי להטמיע את הערך 1 (המקרה הבסיסי)
    add $v0, $zero, $imm, 1          # Set $v0 to 1
  jal $ra, $imm, $zero, 0                    # Return to caller
